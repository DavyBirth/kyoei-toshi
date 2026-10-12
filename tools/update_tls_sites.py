"""Export existing prepared instruction metadata after checking the own eboot."""
from pathlib import Path
import hashlib
import json
from native_module_metadata import write_tls_sites
root=Path(__file__).resolve().parents[1]
proof=json.loads((root/'docs/native-prepare.json').read_text())
inventory=json.loads((root/'docs/m0/inventory.json').read_text())
game=Path(proof['source']).resolve()
assert game==Path(inventory['source']).resolve()
expected=next(row['sha256'] for row in inventory['files'] if row['path']=='eboot.bin')
assert hashlib.sha256((game/'eboot.bin').read_bytes()).hexdigest()==expected
out=Path(proof['prepared_out']).resolve()
assert not out.is_relative_to(root)
print('TLS sites exported:',write_tls_sites(out))
