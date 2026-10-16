"""Recover selected reflection-registration metadata without executing the game.

Only explicitly requested ASCII technical type names are emitted. Constructor,
descriptor and parent are constant-flow candidates, not runtime class instances.
Generated reports contain no dialogue, executable bytes or disassembly.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import sqlite3

from inspect_frame_flow import program_headers, mapped_offset
from trace_guest_fields import CALLER_SAVED, canonical, parts, normalize

STACK = re.compile(r'qword ptr \[rsp(?: \+ 0x([0-9a-f]+))?\]')
TECHNICAL = re.compile(rb'[A-Za-z_][A-Za-z0-9_:<>.* -]{1,127}')


def technical_name(data, headers, address):
    try:
        at = mapped_offset(headers, address)
        raw = data[at:at+129].split(b'\0', 1)[0]
        return raw.decode('ascii') if TECHNICAL.fullmatch(raw) else None
    except ValueError:
        return None


def registrations(rows, data, headers, known_functions):
    values, stack, name = {}, {}, None
    output = []
    # A linear constant window never crosses a branch or a branch destination.
    targets = set()
    for a, size, op, operands, rip, direct in rows:
        if op.startswith('j'):
            match = re.match(r'0x([0-9a-f]+)', operands)
            if match:
                targets.add(int(match[1], 16))
    for row in rows:
        address, size, opcode, operands, rip, direct = normalize(row)
        if address in targets:
            values, stack, name = {}, {}, None
        args = parts(operands)
        if opcode == 'call':
            if direct == 0x302ff0:
                source = values.get('rdi')
                label = technical_name(data, headers, source) if source is not None else None
                name = (label, source, address) if label else None
            elif direct == 0x311570:
                constructor = stack.get(0)
                if name and constructor in known_functions and 'rdi' in values:
                    output.append(dict(name=name[0], name_address=hex(name[1]), name_call=hex(name[2]),
                                       registration=hex(address), descriptor=hex(values['rdi']),
                                       constructor=hex(constructor), size=values.get('rdx'),
                                       parent=hex(values['r9']) if 'r9' in values else None,
                                       constructor_runtime_verified=False))
                name, stack = None, {}
            for register in CALLER_SAVED:
                values.pop(register, None)
            continue
        if opcode.startswith('j') or opcode in ('ret', 'ud2'):
            values, stack, name = {}, {}, None
            continue
        if opcode not in ('lea', 'mov', 'movabs', 'add', 'sub', 'cmp', 'test', 'push', 'pop', 'nop'):
            # Unknown implicit writes and overlapping stack stores must never
            # preserve a supposedly constant callback or receiver.
            values, stack, name = {}, {}, None
            continue
        if opcode in ('add', 'sub') and args and args[0] == 'rsp':
            stack = {}
        destination = canonical(args[0]) if args else None
        value = None
        if opcode == 'lea' and rip is not None:
            value = rip
        elif opcode in ('mov', 'movabs') and len(args) == 2:
            source = canonical(args[1])
            if source in values:
                value = values[source]
            elif re.fullmatch(r'-?0x[0-9a-f]+', args[1]):
                value = int(args[1], 16)
        elif opcode in ('add', 'sub') and destination in values and len(args) == 2 and re.fullmatch(r'0x[0-9a-f]+', args[1]):
            value = values[destination]+int(args[1], 16)*(1 if opcode == 'add' else -1)
        if opcode in ('push', 'pop'):
            stack = {}
        if opcode in ('cmp', 'test', 'push', 'nop'):
            continue
        if destination and args[0] != destination:
            # Only 32-bit destinations zero-extend. Partial writes are unknown.
            if args[0].startswith('e') or re.fullmatch(r'r(?:[89]|1[0-5])d', args[0]):
                value = value & 0xffffffff if value is not None else None
            else:
                value = None
        if destination:
            values.pop(destination, None)
            if value is not None:
                values[destination] = value
        elif args and (match := STACK.fullmatch(args[0])):
            offset = int(match[1] or '0', 16)
            stack.pop(offset, None)
            if value is not None:
                stack[offset] = value
        elif args and '[rsp' in args[0]:
            stack = {}
    return output


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('index', type=Path)
    parser.add_argument('--elf', type=Path, required=True)
    parser.add_argument('--match', required=True, help='Regex selecting technical class names')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    data = args.elf.read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    connection = sqlite3.connect(args.index.resolve().as_uri()+'?mode=ro', uri=True)
    metadata = {key: json.loads(value) for key, value in connection.execute('SELECT key,value FROM metadata')}
    if metadata['source_sha256'] != digest:
        raise ValueError('Index and executable disagree')
    headers = program_headers(data)
    functions = {row[0] for row in connection.execute('SELECT start FROM functions WHERE valid=1')}
    registration_functions = [row[0] for row in connection.execute('SELECT DISTINCT function FROM instructions WHERE direct_target=?', (0x311570,))]
    result, recovered = [], 0
    for function in registration_functions:
        rows = list(connection.execute('SELECT address,size,opcode,operands,rip_target,direct_target FROM instructions WHERE function=? ORDER BY address', (function,)))
        for record in registrations(rows, data, headers, functions):
            recovered += 1
            if re.search(args.match, record['name']):
                record['registration_function'] = hex(function)
                result.append(record)
    report = dict(source_sha256=digest, registration_functions=len(registration_functions),
                  registrations_recovered=recovered, selection=args.match, types=result,
                  runtime_verified=False,
                  limitations='Linear constant windows only, reset at branches/targets. Recovery can omit registrations. Constructors/parents are static candidates; no instance or performance claim.')
    args.out.write_text(json.dumps(report, indent=2)+'\n', encoding='utf8')
    print(json.dumps(dict(functions_scanned=len(registration_functions), registrations_recovered=recovered,
                          selected=len(result), names=[r['name'] for r in result]), indent=2))


if __name__ == '__main__':
    main()
