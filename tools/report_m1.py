"""Resume metadados do catálogo local sem distribuir dados do jogo."""
import collections
import hashlib
import json
from pathlib import Path

repo = Path(__file__).resolve().parent.parent
path = repo / 'work/m1/catalog.json'
data = json.loads(path.read_text(encoding='utf8'))
if not data['extracted_and_verified']:
    raise ValueError('Extração ainda não foi verificada')
extensions = collections.defaultdict(lambda: dict(files=0, bytes=0, prefixes=collections.Counter()))
roots = collections.Counter()
for archive in data['archives']:
    for row in archive['files']:
        ext = Path(row['path']).suffix.lower() or '(sem extensão)'
        group = extensions[ext]
        group['files'] += 1
        group['bytes'] += row['bytes']
        group['prefixes'][row['prefix_hex'][:8]] += 1
        roots[row['path'].split('/')[0]] += 1
report = dict(catalog_sha256=hashlib.sha256(path.read_bytes()).hexdigest(),
              extraction=data['extraction'], verified_files=sum(g['files'] for g in extensions.values()),
              verified_bytes=data['total_bytes'], extensions=dict(extensions), root_components=dict(roots))
out = repo / 'docs/m1'
(out / 'formats.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf8')
lines = ['# Catálogo M1 — contagens verificadas', '',
         f'Arquivos: {report["verified_files"]}. Bytes: {report["verified_bytes"]}.', '',
         '## Arquivos de origem', '', '| PSARC | Arquivos extraídos | Bytes extraídos |', '|---|---:|---:|']
for archive in data['archives']:
    lines.append(f'| {archive["archive"]} | {len(archive["files"])} | {sum(r["bytes"] for r in archive["files"])} |')
lines += ['', '## Extensões', '', '| Extensão | Arquivos | Bytes | Prefixos de 4 bytes (hexadecimal: contagem) |', '|---|---:|---:|---|']
for ext, group in sorted(extensions.items(), key=lambda item: (-item[1]['files'], item[0])):
    prefixes = '; '.join(f'{p}: {n}' for p, n in group['prefixes'].most_common(5))
    lines.append(f'| {ext} | {group["files"]} | {group["bytes"]} | {prefixes} |')
lines += ['', 'São mostrados até cinco prefixos mais frequentes por extensão; `formats.json` guarda todas as contagens.',
          'Prefixos são indícios de formato. Extensão e diretório não comprovam o conteúdo sem um leitor próprio.', '',
          'O catálogo detalhado está em `work/m1/catalog.json` (ignorado pelo Git).',
          'Seu SHA-256 está em `formats.json`; inclui caminhos, offsets, tamanhos e hashes de todas as cópias verificadas.']
(out / 'CATALOGO.md').write_text('\n'.join(lines)+'\n', encoding='utf8')
print(f'Relatório gerado: {report["verified_files"]} arquivos verificados')
