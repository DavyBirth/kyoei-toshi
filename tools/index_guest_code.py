"""Build a private, hash-bound SQLite instruction index without executing guest code.

Only unwind-delimited functions are retained. Instruction byte lengths must
cover each function contiguously; gaps/unknowns are retried from its own entry.
The generated database contains private disassembly: never commit/distribute it.
"""
import argparse
import hashlib
import json
from pathlib import Path
import re
import shutil
import sqlite3
import subprocess

from inspect_frame_flow import unwind_functions

LINE = re.compile(r'^\s*([0-9a-f]+):\s+((?:[0-9a-f]{2}\s+)+)([a-z][a-z0-9.]*|<unknown>)\s*(.*)$')


def instruction(line):
    match = LINE.match(line)
    if not match:
        return None
    address, raw, opcode, operands = match.groups()
    comment = re.search(r'#\s+0x([0-9a-f]+)', operands)
    direct = re.match(r'0x([0-9a-f]+)\b', operands)
    return (int(address, 16), len(raw.split()), opcode, operands,
            int(comment[1], 16) if comment else None,
            int(direct[1], 16) if direct and opcode in ('call', 'jmp') else None)


def complete(start, end, rows):
    at = start
    for address, size, opcode, *rest in rows:
        if address != at or size < 1 or opcode == '<unknown>':
            return False
        at += size
    return at == end


def command(objdump, elf, start, end):
    return [objdump, '-d', '--x86-asm-syntax=intel', f'--start-address={start:#x}',
            f'--stop-address={end:#x}', str(elf)]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('elf', type=Path)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--objdump', default=shutil.which('llvm-objdump'))
    args = parser.parse_args()
    if not args.objdump:
        parser.error('LLVM objdump is required')
    if args.out.exists():
        parser.error('Output already exists; choose a new path to preserve the current index')
    data = args.elf.read_bytes()
    functions = unwind_functions(data)
    args.out.parent.mkdir(parents=True, exist_ok=True)
    temporary = args.out.with_suffix('.building.sqlite')
    if temporary.exists():
        parser.error('A previous incomplete build exists; inspect it before retrying')
    connection = sqlite3.connect(temporary)
    connection.executescript('''
        PRAGMA journal_mode=OFF;
        CREATE TABLE metadata(key TEXT PRIMARY KEY, value TEXT);
        CREATE TABLE functions(start INTEGER PRIMARY KEY, end INTEGER, valid INTEGER, fallback INTEGER, instructions INTEGER);
        CREATE TABLE instructions(address INTEGER PRIMARY KEY, function INTEGER, size INTEGER, opcode TEXT, operands TEXT, rip_target INTEGER, direct_target INTEGER);
    ''')
    failures, fallbacks, count = [], 0, 0

    def save(function, rows):
        nonlocal fallbacks, count
        start, end = function['start'], function['end']
        fallback = not complete(start, end, rows)
        if fallback:
            fallbacks += 1
            result = subprocess.run(command(args.objdump, args.elf, start, end),
                                    capture_output=True, text=True, check=True)
            rows = [parsed for line in result.stdout.splitlines() if (parsed := instruction(line))
                    and start <= parsed[0] < end]
        valid = complete(start, end, rows)
        if not valid:
            failures.append(hex(start))
            rows = []  # Never expose partial decoding as verified instruction data.
        connection.execute('INSERT INTO functions VALUES(?,?,?,?,?)',
                           (start, end, int(valid), int(fallback), len(rows)))
        connection.executemany('INSERT INTO instructions VALUES(?,?,?,?,?,?,?)',
                               [(a, start, size, op, operands, rip, direct)
                                for a, size, op, operands, rip, direct in rows])
        count += len(rows)

    # Stream once, retaining at most one function's decoded instructions.
    with subprocess.Popen(command(args.objdump, args.elf, functions[0]['start'], functions[-1]['end']),
                          stdout=subprocess.PIPE, stderr=subprocess.DEVNULL, text=True) as process:
        index, rows = 0, []
        for line in process.stdout:
            parsed = instruction(line)
            if parsed is None:
                continue
            while index < len(functions) and parsed[0] >= functions[index]['end']:
                save(functions[index], rows)
                rows = []
                index += 1
                if index % 5000 == 0:
                    print(f'Indexed {index}/{len(functions)} function ranges', flush=True)
            if index < len(functions) and functions[index]['start'] <= parsed[0] < functions[index]['end']:
                rows.append(parsed)
        while index < len(functions):
            save(functions[index], rows)
            rows = []
            index += 1
        if process.wait():
            raise RuntimeError('LLVM objdump failed; incomplete index retained for inspection')
    summary = dict(source_sha256=hashlib.sha256(data).hexdigest(), functions=len(functions),
                   indexed_instructions=count, individually_redecoded=fallbacks,
                   decode_failures=failures, runtime_verified=False,
                   note='Structural instruction coverage, not complete semantic reverse engineering. Private database contains disassembly.')
    connection.executemany('INSERT INTO metadata VALUES(?,?)',
                           [(key, json.dumps(value)) for key, value in summary.items()])
    connection.executescript('''
        CREATE INDEX instruction_function ON instructions(function);
        CREATE INDEX instruction_rip ON instructions(rip_target);
        CREATE INDEX instruction_call ON instructions(direct_target);
    ''')
    connection.commit()
    connection.close()
    temporary.replace(args.out)
    print(json.dumps(summary, indent=2))


if __name__ == '__main__':
    main()
