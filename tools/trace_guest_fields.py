"""Conservative intra-function pointer provenance over a private instruction index.

Tracks explicitly named global objects and receivers through registers and CFG
joins. Unknown calls clobber SysV caller-saved registers. Stack spills, arbitrary
heap aliases and unconfigured indirect jumps remain unresolved, never guessed.
The public report contains technical addresses/fields, not disassembly.
"""
import argparse
from collections import deque
import hashlib
import json
from pathlib import Path
import re
import sqlite3
import struct

from inspect_frame_flow import program_headers, mapped_offset

CALLER_SAVED = ('rax', 'rcx', 'rdx', 'rsi', 'rdi', 'r8', 'r9', 'r10', 'r11')
ARGUMENTS = ('rdi', 'rsi', 'rdx', 'rcx', 'r8', 'r9')
MEMORY = re.compile(r'^(?:(?:byte|word|dword|qword|xmmword|ymmword) ptr )?\[(r[a-z0-9]+)(?: \+ 0x([0-9a-f]+))?\]$')
REG64 = re.compile(r'^(?:r(?:ax|bx|cx|dx|si|di|bp|sp|[89]|1[0-5]))$')


def canonical(register):
    if REG64.fullmatch(register):
        return register
    if re.fullmatch(r'e(?:ax|bx|cx|dx|si|di|bp|sp)', register):
        return 'r'+register[1:]
    if re.fullmatch(r'r(?:[89]|1[0-5])[dwb]', register):
        return register[:-1]
    for full, aliases in {'rax': ('ax', 'al', 'ah'), 'rbx': ('bx', 'bl', 'bh'),
                          'rcx': ('cx', 'cl', 'ch'), 'rdx': ('dx', 'dl', 'dh'),
                          'rsi': ('si', 'sil'), 'rdi': ('di', 'dil'),
                          'rbp': ('bp', 'bpl'), 'rsp': ('sp', 'spl')}.items():
        if register in aliases:
            return full
    return None


def parts(operands):
    return [part.strip() for part in operands.split('#', 1)[0].split(',')]


def normalize(row):
    address, size, opcode, operands, rip, direct = row
    while opcode in ('lock', 'rep', 'repe', 'repz', 'repne', 'repnz', 'data16'):
        match = re.match(r'([a-z][a-z0-9.]*)\s*(.*)', operands)
        if not match:
            break
        opcode, operands = match.groups()
    return address, size, opcode, operands, rip, direct


def memory_tag(operand, state):
    match = MEMORY.fullmatch(operand)
    if not match or match[1] not in state:
        return None
    kind, name, offset = state[match[1]]
    return kind, name, offset+int(match[2] or '0', 16)


def transfer(row, state, globals_):
    address, size, opcode, operands, rip, direct = row
    result = dict(state)
    args = parts(operands)
    if opcode in ('stosb', 'stosw', 'stosd', 'stosq', 'movsb', 'movsw', 'movsd', 'movsq', 'lodsb', 'lodsw', 'lodsd', 'lodsq'):
        for register in ('rax', 'rcx', 'rsi', 'rdi'):
            result.pop(register, None)
        return result
    if opcode in ('mul', 'div', 'idiv', 'cpuid', 'rdtsc', 'rdtscp', 'syscall', 'sysenter', 'cmpxchg') or (opcode == 'imul' and len(args) == 1):
        for register in ('rax', 'rdx', 'rcx'):
            result.pop(register, None)
        return result
    if opcode in ('xchg', 'xadd'):
        for operand in args:
            register = canonical(operand)
            if register:
                result.pop(register, None)
        return result
    if opcode == 'call':
        for register in CALLER_SAVED:
            result.pop(register, None)
        return result
    if opcode.startswith('j') or opcode in ('cmp', 'test', 'push', 'ret', 'nop', 'int'):
        return result
    destination = canonical(args[0]) if args else None
    if not destination:
        return result
    previous = result.pop(destination, None)
    if len(args) != 2 or not REG64.fullmatch(args[0]):
        return result
    source = args[1]
    if opcode == 'mov' and source in state and REG64.fullmatch(source):
        result[destination] = state[source]
    elif opcode in ('mov', 'lea') and rip in globals_:
        result[destination] = ('slot' if opcode == 'lea' else 'object', globals_[rip], 0)
    elif opcode == 'lea':
        tag = memory_tag(source, state)
        if tag:
            result[destination] = tag
    elif opcode == 'mov':
        tag = memory_tag(source, state)
        if tag and tag[0] == 'slot' and tag[2] == 0:
            result[destination] = ('object', tag[1], 0)
        elif tag and tag[0] == 'object' and tag[2] == 0:
            result[destination] = ('vtable', tag[1], 0)
    elif opcode in ('add', 'sub') and previous and re.fullmatch(r'0x[0-9a-f]+', source):
        amount = int(source, 16)*(1 if opcode == 'add' else -1)
        result[destination] = previous[:2]+(previous[2]+amount,)
    return result


def successors(rows, switches):
    addresses = {row[0] for row in rows}
    edges = {}
    unresolved = []
    for i, row in enumerate(rows):
        address, size, opcode, operands, rip, direct = row
        following = [rows[i+1][0]] if i+1 < len(rows) else []
        target = re.match(r'0x([0-9a-f]+)\b', operands)
        target = int(target[1], 16) if target else None
        if opcode in ('ret', 'ud2', 'hlt'):
            outgoing = []
        elif opcode == 'jmp':
            outgoing = [target] if target is not None else switches.get(address, [])
            if target is None and address not in switches:
                unresolved.append(address)
        elif opcode.startswith('j'):
            outgoing = following+([target] if target is not None else [])
        else:
            outgoing = following
        edges[address] = [a for a in outgoing if a in addresses]
    return edges, unresolved


def analyze(rows, globals_, initial=None, switches=None):
    rows = [normalize(row) for row in rows]
    if not rows:
        return dict(fields=[], calls=[], virtual_calls=[], reached=0, unresolved_jumps=[])
    initial = initial or {}
    edges, unresolved = successors(rows, switches or {})
    by_address = {row[0]: row for row in rows}
    states = {rows[0][0]: initial}
    pending = deque([rows[0][0]])
    while pending:
        address = pending.popleft()
        after = transfer(by_address[address], states[address], globals_)
        for target in edges[address]:
            previous = states.get(target)
            merged = after if previous is None else {k: v for k, v in previous.items() if after.get(k) == v}
            if previous is None or merged != previous:
                states[target] = dict(merged)
                pending.append(target)
    fields, calls, virtuals = [], [], []
    for row in rows:
        address, size, opcode, operands, rip, direct = row
        state = states.get(address)
        if state is None:
            continue
        args = parts(operands)
        for index, operand in enumerate(args):
            tag = memory_tag(operand, state)
            if not tag or tag[0] != 'object':
                continue
            kind = 'address' if opcode == 'lea' else 'read'
            if index == 0 and opcode not in ('cmp', 'test', 'push', 'call') and not opcode.startswith('j'):
                kind = 'write' if opcode.startswith(('mov', 'vmov')) else 'read_write'
            record = dict(site=hex(address), object=tag[1], offset=hex(tag[2]), access=kind, opcode=opcode)
            if kind == 'write' and len(args) == 2 and re.fullmatch(r'-?0x[0-9a-f]+', args[1]):
                record['immediate'] = args[1]
            fields.append(record)
        if opcode == 'call':
            values = {register: dict(object=tag[1], offset=hex(tag[2]), kind=tag[0])
                      for register in ARGUMENTS if (tag := state.get(register))}
            if values:
                calls.append(dict(site=hex(address), target=hex(direct) if direct is not None else None,
                                  arguments=values))
            if args:
                tag = memory_tag(args[0], state)
                if tag and tag[0] == 'vtable':
                    virtuals.append(dict(site=hex(address), object=tag[1], slot=hex(tag[2]),
                                         receiver_runtime_verified=False))
    return dict(fields=fields, calls=calls, virtual_calls=virtuals,
                reached=len(states), unresolved_jumps=[hex(a) for a in unresolved])


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('index', type=Path)
    parser.add_argument('--elf', required=True, type=Path)
    parser.add_argument('--facts', required=True, type=Path)
    parser.add_argument('--out', required=True, type=Path)
    args = parser.parse_args()
    facts = json.loads(args.facts.read_text(encoding='utf8'))
    data = args.elf.read_bytes()
    digest = hashlib.sha256(data).hexdigest()
    connection = sqlite3.connect(args.index.resolve().as_uri()+'?mode=ro', uri=True)
    metadata = {key: json.loads(value) for key, value in connection.execute('SELECT key,value FROM metadata')}
    if digest != facts['source_sha256'] or digest != metadata['source_sha256']:
        raise ValueError('ELF, index and field facts must have the same source hash')
    globals_ = {int(address, 0): name for name, address in facts['globals'].items()}
    selected = set()
    for address in globals_:
        selected.update(r[0] for r in connection.execute('SELECT DISTINCT function FROM instructions WHERE rip_target=?', (address,)))
    receivers = {int(key, 0): {register: ('object', name, 0) for register, name in value.items()}
                 for key, value in facts.get('receivers', {}).items()}
    selected.update(receivers)
    switches = {}
    headers = program_headers(data)
    for switch in facts.get('switches', []):
        base = int(switch['table'], 0)
        relative = struct.unpack_from('<'+'i'*switch['count'], data,
                                      mapped_offset(headers, base, 4*switch['count']))
        targets = [base+offset for offset in relative]
        if any(connection.execute('SELECT 1 FROM instructions WHERE address=? AND function=?',
                                  (target, int(switch['function'], 0))).fetchone() is None for target in targets):
            raise ValueError('Switch target is not a decoded instruction in the stated function')
        switches[int(switch['site'], 0)] = targets
    reports = []
    for function in sorted(selected):
        rows = list(connection.execute('SELECT address,size,opcode,operands,rip_target,direct_target FROM instructions WHERE function=? ORDER BY address', (function,)))
        report = analyze(rows, globals_, receivers.get(function), switches)
        report.update(function=hex(function), indexed_instructions=len(rows))
        reports.append(report)
    result = dict(source_sha256=digest, functions=reports, runtime_verified=False,
                  limitations='Register provenance on reachable intraprocedural CFG paths only. Explicit receiver/switch assumptions in facts. Unmodeled spills/heap aliases/indirect jumps and interprocedural effects limit coverage. No claim of all runtime writes.')
    args.out.write_text(json.dumps(result, indent=2)+'\n', encoding='utf8')
    print(json.dumps(dict(functions=len(reports), fields=sum(len(r['fields']) for r in reports),
                          object_calls=sum(len(r['calls']) for r in reports), virtual_calls=sum(len(r['virtual_calls']) for r in reports),
                          unresolved_jumps=sum(len(r['unresolved_jumps']) for r in reports), out=str(args.out)), indent=2))


if __name__ == '__main__':
    main()
