"""Gera o relatório legível a partir das importações verificadas."""
import collections
import json
from pathlib import Path

folder = Path(__file__).resolve().parent.parent / 'docs' / 'm0'
data = json.loads((folder / 'imports.json').read_text(encoding='utf8'))
imports = data['imports']
lines = ['# Importações do eboot — M0', '', f'Total: {len(imports)} importações.', '',
         f'NIDs sem nome na aerolib: {sum(i["name"] == "?" for i in imports)}.', '',
         '## Módulos declarados', '']
lines += [f'- ID {key}: `{value}`' for key, value in data['mods'].items()]
lines += ['', '## Bibliotecas e contagens', '', '| Biblioteca | Importações |', '|---|---:|']
lines += [f'| {name} | {count} |' for name, count in collections.Counter(i['lib'] for i in imports).most_common()]
lines += ['', '## Importações completas', '', 'Tipo ELF: 1 = objeto; 2 = função. `?` indica NID sem nome conhecido nesta aerolib.', '',
          '| Módulo | Biblioteca | Nome | NID | Tipo |', '|---|---|---|---|---:|']
lines += [f'| {i["module"]} | {i["lib"]} | {i["name"]} | {i["nid"]} | {i["type"]} |' for i in imports]
(folder / 'IMPORTACOES.md').write_text('\n'.join(lines) + '\n', encoding='utf8')
