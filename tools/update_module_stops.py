"""Add verified finalizer metadata to the existing prepared local output."""
from pathlib import Path
import hashlib
import json
import sys

root = Path(__file__).resolve().parents[1]
sys.path.insert(0, str(root/'third_party/godzilla-core/scripts'))
import link_modules
from native_module_metadata import records

proof = json.loads((root/'docs/native-prepare.json').read_text())
inventory = json.loads((root/'docs/m0/inventory.json').read_text())
game = Path(proof['source']).resolve()
assert game == Path(inventory['source']).resolve()
out = Path(proof['prepared_out']).resolve()
assert not out.is_relative_to(root)
hashes = {f['path']: f['sha256'] for f in inventory['files']}
table = out/'deferred.tsv'
rows = []
for line in table.read_text().splitlines():
    if line[:2] in ('F\t','S\t','H\t'):
        continue
    rows.append(line)
    if not line.startswith('M\t'):
        continue
    _, guest, base, init = line.split('\t')
    relative = guest.removeprefix('/app0/')
    path = (game/relative).resolve()
    assert path.is_relative_to(game)
    assert hashlib.sha256(path.read_bytes()).hexdigest() == hashes[relative]
    module = link_modules.module(path)
    rows.extend(records(int(base,16), module))
    print(relative, 'verified finalizer/segments/unwind metadata')
table.write_text('\n'.join(rows)+'\n', encoding='utf8')
