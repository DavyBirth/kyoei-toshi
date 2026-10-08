"""Confere o dump contra o inventário M0, sem escrever nele."""
import hashlib
import json
import struct
import sys
from pathlib import Path

reports = Path(__file__).resolve().parent.parent / 'docs' / 'm0'
inventory = json.loads((reports / 'inventory.json').read_text(encoding='utf8'))
root = Path(sys.argv[1] if len(sys.argv) > 1 else inventory['source']).resolve(strict=True)
expected = {r['path']: r for r in inventory['files']}
actual = {p.relative_to(root).as_posix(): p for p in root.rglob('*') if p.is_file()}
if actual.keys() != expected.keys():
    raise ValueError(f'Arquivos diferentes: ausentes={expected.keys()-actual.keys()}, novos={actual.keys()-expected.keys()}')
for index, (name, record) in enumerate(expected.items(), 1):
    p = actual[name]
    before = p.stat()
    h = hashlib.sha256()
    with p.open('rb') as f:
        while block := f.read(8 * 1024 * 1024):
            h.update(block)
    after = p.stat()
    if (before.st_size, before.st_mtime_ns) != (after.st_size, after.st_mtime_ns):
        raise ValueError(f'Arquivo mudou durante a leitura: {name}')
    if before.st_size != record['bytes'] or h.hexdigest() != record['sha256']:
        raise ValueError(f'Tamanho ou SHA-256 diferente: {name}')
    print(f'{index}/{len(expected)} OK {name}', flush=True)
sums = ''.join(f'{r["sha256"]}  {r["path"]}\n' for r in inventory['files'])
assert (reports / 'SHA256SUMS.txt').read_text(encoding='utf8') == sums
data = (root / 'sce_sys' / 'param.sfo').read_bytes()
magic, version, keys, values, count = struct.unpack_from('<5I', data)
assert magic == 0x46535000
sfo = {}
for i in range(count):
    key, fmt, length, capacity, offset = struct.unpack_from('<HHIII', data, 20 + i * 16)
    name = data[keys + key:data.index(0, keys + key)].decode('utf8')
    value = data[values + offset:values + offset + length]
    sfo[name] = struct.unpack('<I', value)[0] if fmt == 0x404 else value.rstrip(b'\0').decode('utf8')
assert sfo == json.loads((reports / 'param-sfo.json').read_text(encoding='utf8'))
for record in json.loads((reports / 'tool-provenance.json').read_text(encoding='utf8')):
    assert hashlib.sha256(Path(record['path']).read_bytes()).hexdigest() == record['sha256'], record['path']
print(f'VALIDADO: {len(expected)} arquivos; {sum(r["bytes"] for r in expected.values())} bytes; {sfo["TITLE_ID"]}; APP_VER={sfo["APP_VER"]}; VERSION={sfo["VERSION"]}', flush=True)
