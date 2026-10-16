"""Map selected static call paths in the owner's prepared executable, without running it.

Uses the ELF unwind index for function boundaries and LLVM to validate candidate
CALL/JMP instructions. Reports addresses, hashes and technical labels only.
Does not export executable bytes, disassembly, dialogue or game resources.
"""
import argparse
from bisect import bisect_right
from collections import defaultdict
import hashlib
import json
from pathlib import Path
import re
import shutil
import struct
import subprocess

from inspect_elf_references import load_segments


def program_headers(data):
    load_segments(data)  # Validate ELF64/x86-64 and load ranges first.
    header = struct.unpack_from('<16sHHIQQQIHHHHHH', data)
    return [struct.unpack_from('<IIQQQQQQ', data, header[5]+i*header[9])
            for i in range(header[10])]


def mapped_offset(headers, address, size=1):
    for kind, flags, offset, base, _, stored, memory, alignment in headers:
        if kind in (1, 0x61000010) and base <= address and address+size <= base+stored:
            return offset+address-base
    raise ValueError(f'Unmapped stored address {address:#x}+{size:#x}')


def unwind_functions(data):
    headers = program_headers(data)
    unwind = [p for p in headers if p[0] == 0x6474e550]
    if len(unwind) != 1:
        raise ValueError('Expected one GNU unwind index')
    _, _, offset, base, _, stored, _, _ = unwind[0]
    if stored < 12 or offset+stored > len(data) or data[offset:offset+4] != bytes.fromhex('011b033b'):
        raise ValueError('Unsupported/truncated unwind index encoding')
    count = struct.unpack_from('<I', data, offset+8)[0]
    if 12+8*count > stored:
        raise ValueError('Truncated unwind table')
    rows = []
    for i in range(count):
        relative_start, relative_fde = struct.unpack_from('<ii', data, offset+12+i*8)
        start, fde = base+relative_start, base+relative_fde
        pos = mapped_offset(headers, fde, 16)
        length, cie_back, relative_pc, size = struct.unpack_from('<IIiI', data, pos)
        mapped_offset(headers, fde, 4+length)
        cie = fde+4-cie_back
        cpos = mapped_offset(headers, cie, 8)
        clen, cid = struct.unpack_from('<II', data, cpos)
        mapped_offset(headers, cie, 4+clen)
        # This executable's zR CIE: version 1, code alignment 1, data -8,
        # return register 16, one augmentation byte, signed pcrel32 pointer.
        if clen < 13 or cid or data[cpos+8:cpos+16] != bytes.fromhex('017a520001781001') or data[cpos+16] != 0x1b:
            raise ValueError(f'Unsupported CIE at {cie:#x}; review before interpreting ranges')
        if length < 12 or start != fde+8+relative_pc or not size:
            raise ValueError('Unwind table/FDE disagreement')
        if not any(p[0] == 1 and p[1] & 1 and p[3] <= start and start+size <= p[3]+p[5]
                   for p in headers):
            raise ValueError('Function outside executable segment')
        if rows and rows[-1]['end'] > start:
            raise ValueError('Unsorted/overlapping unwind ranges')
        rows.append(dict(start=start, end=start+size, fde=fde))
    return rows


def prepared_imports(data):
    if len(data) < 56:
        raise ValueError('Truncated prepared image')
    magic, size, entry, ns, nr, ni, flags = struct.unpack_from('<8s6Q', data)
    if magic != b'BBPROBE2':
        raise ValueError('Use original boot.bin (BBPROBE2), not linked/patched image')
    at = 56+ns*24
    if at+ni*128+nr*32+size != len(data):
        raise ValueError('Invalid prepared image sizes')
    names = [data[at+i*128:at+(i+1)*128].split(b'\0', 1)[0].decode('ascii') for i in range(ni)]
    at += ni*128
    imports = {}
    for i in range(nr):
        target, kind, symbol, addend = struct.unpack_from('<QQqq', data, at+i*32)
        if kind in (1, 2):
            if not 0 <= symbol < ni or target+8 > size:
                raise ValueError('Invalid imported relocation')
            imports[target] = names[symbol]
    return imports


def direct_candidates(data, functions):
    headers = program_headers(data)
    starts = [row['start'] for row in functions]
    callers = defaultdict(list)
    for kind, flags, offset, base, _, size, _, _ in headers:
        if kind != 1 or not flags & 1:
            continue
        code = data[offset:offset+size]
        for match in re.finditer(rb'[\xe8\xe9]', code):
            at = match.start()
            if at+5 > size:
                continue
            address = base+at
            index = bisect_right(starts, address)-1
            if index < 0 or address+5 > functions[index]['end']:
                continue
            target = address+5+struct.unpack_from('<i', code, at+1)[0]
            callers[target].append((starts[index], address))
    return callers


def import_thunks(data, imports):
    thunks = {}
    for kind, flags, offset, base, _, size, _, _ in program_headers(data):
        if kind != 1 or not flags & 1:
            continue
        code = data[offset:offset+size]
        for match in re.finditer(b'\xff\x25', code):
            at = match.start()
            if at+6 <= size:
                slot = base+at+6+struct.unpack_from('<i', code, at+2)[0]
                if slot in imports:
                    thunks[base+at] = dict(slot=slot, symbol=imports[slot])
    return thunks


def parse_instructions(text):
    rows = []
    for line in text.splitlines():
        match = re.match(r'^\s*([0-9a-f]+):\s+([a-z][a-z0-9]*)\s*(.*)$', line)
        if match:
            address, opcode, operands = match.groups()
            direct = re.match(r'0x([0-9a-f]+)\b', operands)
            rows.append(dict(address=int(address, 16), opcode=opcode, operands=operands,
                             target=int(direct[1], 16) if direct and opcode in ('call', 'jmp') else None))
    return rows


def assignment(value):
    label, address = value.rsplit('=', 1)
    if not re.fullmatch(r'[A-Za-z0-9_.:-]+', label):
        raise argparse.ArgumentTypeError('Use a technical ASCII label=0xaddress')
    return label, int(address, 0)


def verify_facts(data, facts):
    if hashlib.sha256(data).hexdigest() != facts['source_sha256']:
        raise ValueError('Static facts belong to a different ELF')
    headers = program_headers(data)
    verified = []
    formats = dict(u64='Q', u32='I', i32='i', f32='f')
    for row in facts.get('reads', []):
        code = formats[row['type']]
        expected = row['expected']
        expected = [int(v, 0) if isinstance(v, str) else v for v in expected]
        size = struct.calcsize('<'+code)*len(expected)
        address = int(row['address'], 0)
        actual = list(struct.unpack_from('<'+code*len(expected), data,
                                         mapped_offset(headers, address, size)))
        if actual != expected:
            raise ValueError(f'Static fact mismatch: {row["label"]}')
        verified.append(row)
    return verified


def verify_virtual_slot(data, row, instructions):
    """Validate an instruction and a proposed class table, not its runtime receiver."""
    site, table, slot = (int(row[key], 0) for key in ('site', 'table', 'slot'))
    expected = int(row['target'], 0)
    instruction = next((r for r in instructions if r['address'] == site), None)
    if not instruction or instruction['opcode'] != 'call':
        raise ValueError('Expected virtual call instruction')
    operand = re.fullmatch(r'qword ptr \[r[a-z0-9]+ \+ 0x([0-9a-f]+)\]', instruction['operands'])
    if not operand or int(operand[1], 16) != slot:
        raise ValueError('Virtual call slot disagreement')
    target = struct.unpack_from('<Q', data, mapped_offset(program_headers(data), table+slot, 8))[0]
    if target != expected:
        raise ValueError('Virtual table target disagreement')
    return dict(**row, receiver_runtime_verified=False)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('prepared', type=Path)
    parser.add_argument('--seed', action='append', type=assignment, default=[])
    parser.add_argument('--import-name', action='append', default=[])
    parser.add_argument('--facts', type=Path, help='Optional hash-bound technical facts and seeds')
    parser.add_argument('--depth', type=int, choices=range(5), default=2)
    parser.add_argument('--max-functions', type=int, default=180)
    parser.add_argument('--objdump', default=shutil.which('llvm-objdump'))
    parser.add_argument('--out', required=True, type=Path)
    args = parser.parse_args()
    facts = json.loads(args.facts.read_text(encoding='utf8')) if args.facts else None
    if facts:
        args.seed += [(label, int(address, 0)) for label, address in facts.get('seeds', {}).items()]
        args.import_name += facts.get('import_names', [])
    if not args.objdump or not (args.seed or args.import_name) or args.max_functions < 1:
        parser.error('Select seeds/imports and provide LLVM objdump and a positive limit')
    data = (args.prepared/'eboot.elf').read_bytes()
    verified_facts = verify_facts(data, facts) if facts else []
    functions = unwind_functions(data)
    function_by_start = {row['start']: row for row in functions}
    imports = prepared_imports((args.prepared/'boot.bin').read_bytes())
    hints = json.loads((args.prepared/'analysis.json').read_text(encoding='utf8'))['import_name_hints']
    for line in (args.prepared/'imports.tsv').read_text(encoding='utf8').splitlines():
        index, symbol, library, name = line.split('\t')
        if name != '?':
            hints[symbol] = name
    thunks = import_thunks(data, imports)
    roots = dict((address, label) for label, address in args.seed)
    if any(address not in function_by_start for label, address in args.seed):
        raise ValueError('Every explicit seed must be an unwind function entry')
    for address, thunk in thunks.items():
        label = hints.get(thunk['symbol'], thunk['symbol'])
        if label in args.import_name:
            roots[address] = label
    missing = sorted(set(args.import_name)-set(roots.values()))
    if missing:
        raise ValueError(f'Imports not found: {missing}')
    callers = direct_candidates(data, functions)
    decoded, failures = {}, []

    def decode(start):
        if start in decoded:
            return decoded[start]
        if start not in function_by_start or len(decoded) >= args.max_functions:
            return None
        function = function_by_start[start]
        command = [args.objdump, '-d', '--no-show-raw-insn', '--x86-asm-syntax=intel',
                   f'--start-address={start:#x}', f'--stop-address={function["end"]:#x}',
                   str(args.prepared/'eboot.elf')]
        result = subprocess.run(command, check=True, capture_output=True, text=True)
        rows = parse_instructions(result.stdout)
        if not rows or rows[0]['address'] != start or any(r['opcode'] in ('unknown',) for r in rows) or '<unknown>' in result.stdout:
            failures.append(hex(start))
            decoded[start] = []
        else:
            decoded[start] = rows
        return decoded[start]

    frontier = set(roots)
    visited, edges, limited = set(), {}, False
    for level in range(args.depth+1):
        next_frontier = set()
        for target in sorted(frontier-visited):
            visited.add(target)
            if target in function_by_start:
                decode(target)
            for start, address in callers.get(target, []):
                rows = decode(start)
                if rows is None:
                    limited = True
                    continue
                instruction = next((r for r in rows if r['address'] == address and r['target'] == target), None)
                if instruction:
                    edges[(address, target)] = dict(caller=hex(start), site=hex(address),
                                                   target=hex(target), kind=instruction['opcode'])
                    if level < args.depth:
                        next_frontier.add(start)
        frontier = next_frontier
    virtual_edges = []
    for row in (facts or {}).get('conditional_virtual_edges', []):
        instructions = decode(int(row['caller'], 0))
        if instructions is None:
            raise ValueError('Function limit prevents virtual-slot verification')
        virtual_edges.append(verify_virtual_slot(data, row, instructions))
    # Outgoing calls from every decoded function are useful for ordering siblings.
    outgoing = []
    indirect = []
    for start, rows in sorted(decoded.items()):
        for row in rows:
            if row['target'] is not None and not (row['opcode'] == 'jmp' and start <= row['target'] < function_by_start[start]['end']):
                outgoing.append(dict(caller=hex(start), site=hex(row['address']), target=hex(row['target']), kind=row['opcode']))
            elif row['opcode'] == 'call':
                indirect.append(dict(caller=hex(start), site=hex(row['address']), kind='unresolved_indirect_call'))
    headers = program_headers(data)
    report = dict(source_sha256=hashlib.sha256(data).hexdigest(),
                  llvm_version=subprocess.run([args.objdump, '--version'], check=True, capture_output=True, text=True).stdout.splitlines()[0],
                  unwind_functions=len(functions), roots={hex(k): v for k, v in sorted(roots.items())},
                  verified_static_reads=verified_facts,
                  functions=[dict(start=hex(k), end=hex(function_by_start[k]['end']),
                                  code_sha256=hashlib.sha256(data[mapped_offset(headers, k):mapped_offset(headers, k)+function_by_start[k]['end']-k]).hexdigest(),
                                  decoded_instructions=len(v)) for k, v in sorted(decoded.items())],
                  caller_edges=list(edges.values()), outgoing_edges=outgoing, indirect_calls=indirect,
                  conditional_virtual_edges=virtual_edges,
                  import_thunks={hex(k):dict(slot=hex(v['slot']), name=hints.get(v['symbol'], v['symbol'])) for k, v in sorted(thunks.items())},
                  decode_failures=failures, limit_reached=limited,
                  runtime_verified=False,
                  limitations='Static direct calls/tail jumps only. Indirect calls are unresolved; reachability does not prove execution, per-frame frequency or runtime cost. Unwind coverage can omit functions.')
    args.out.write_text(json.dumps(report, indent=2)+'\n', encoding='utf8')
    print(json.dumps(dict(unwind_functions=len(functions), decoded_functions=len(decoded), caller_edges=len(edges),
                          indirect_calls=len(indirect), limit_reached=limited, decode_failures=failures, out=str(args.out)), indent=2))


if __name__ == '__main__':
    main()
