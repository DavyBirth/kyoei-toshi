"""Confere os leitores de textos nos recursos reais, sem alterar o dump."""
import collections
import hashlib
import json
from pathlib import Path
from game_resource import Resource
from text_formats import Csh, JAPANESE, Lua52
from text_tool import export, insert


def main():
    repo = Path(__file__).resolve().parent.parent
    catalogue = json.loads((repo / 'work/m1/catalog.json').read_text(encoding='utf8'))
    root = Path(catalogue['extraction'])
    records = []
    failures = []
    for archive in catalogue['archives']:
        for entry in archive['files']:
            suffix = Path(entry['path']).suffix.lower()
            if suffix not in ('.csh', '.luc'):
                continue
            name = f'{Path(archive["archive"]).stem}/{entry["path"]}'
            try:
                data = (root / name).read_bytes()
                digest = hashlib.sha256(data).hexdigest()
                if digest != entry['sha256']:
                    raise ValueError('Recurso diferente da extração M1')
                resource = Resource(data)
                table = Lua52(resource.payload) if suffix == '.luc' else Csh(resource.payload)
                # Passa pelos valores extraídos e pelo reinseridor, não copia simplesmente o arquivo.
                rebuilt = resource.rebuild(table.rebuild({s.id: s.raw for s in table.strings}))
                if rebuilt != data:
                    raise ValueError('Ida e volta sem mudanças alterou bytes')
                document = json.loads(json.dumps(export(data), ensure_ascii=False))
                if insert(data, document) != data:
                    raise ValueError('Ida e volta via JSON alterou bytes')
                jp = 0
                invalid_utf8 = 0
                for s in table.strings:
                    try:
                        jp += bool(JAPANESE.search(s.text()))
                    except UnicodeDecodeError:
                        invalid_utf8 += 1
                row = dict(path=name, sha256=digest, bytes=len(data), payload_bytes=len(resource.payload),
                           strings=len(table.strings), japanese_strings=jp, non_utf8_strings=invalid_utf8,
                           identical_roundtrip=True, identical_json_roundtrip=True)
                if isinstance(table, Csh):
                    row.update(rows=table.rows, columns=table.columns, cell_types=table.types)
                else:
                    row['prototypes'] = table.prototypes
                records.append(row)
            except (ValueError, OSError) as error:
                failures.append(dict(path=name, error=str(error)))
            if (len(records) + len(failures)) % 200 == 0:
                print(f'Conferidos: {len(records)}; falhas: {len(failures)}', flush=True)
    summary = dict(source=str(root), files=len(records), strings=sum(r['strings'] for r in records),
                   japanese_strings=sum(r['japanese_strings'] for r in records), failures=failures,
                   by_extension=dict(collections.Counter(Path(r['path']).suffix for r in records)),
                   records=records)
    out = repo / 'docs/m2'
    out.mkdir(parents=True, exist_ok=True)
    (out / 'audit.json').write_text(json.dumps(summary, indent=2)+'\n', encoding='utf8')
    print(f'IDA E VOLTA: {len(records)} recursos idênticos; {summary["japanese_strings"]} strings japonesas; {len(failures)} falhas', flush=True)
    if failures:
        print(json.dumps(failures[:10], ensure_ascii=True), flush=True)
        raise SystemExit(1)


if __name__ == '__main__':
    main()
