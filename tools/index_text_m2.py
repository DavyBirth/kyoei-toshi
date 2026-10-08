"""Índice local de candidatos japoneses; relatório público sem textos originais."""
import argparse
import collections
import hashlib
import json
from pathlib import Path
from text_tool import REPO, decode, output_path
from text_formats import JAPANESE


def category(path):
    name = path.lower()
    for needle, label in (
        ('bannedwords', 'filtro_de_nomes'), ('staffroll', 'creditos'),
        ('loading', 'carregamento'), ('item_data', 'itens'),
        ('reading_material', 'materiais_de_leitura'), ('store_text', 'loja'),
        ('operation_text', 'instrucoes'), ('textdata', 'interface_compartilhada'),
        ('news', 'noticias'), ('mission', 'objetivos'), ('stage', 'cenas'),
    ):
        if needle in name:
            return label
    return 'a_classificar'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output', type=Path)
    args = parser.parse_args()
    output = output_path(args.output)
    audit = json.loads((REPO / 'docs/m2/audit.json').read_text(encoding='utf8'))
    root = Path(audit['source'])
    counts = collections.Counter()
    files = []
    invalid = []
    duplicate_counts = collections.Counter()
    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open('x', encoding='utf8', newline='\n') as stream:
        for row in audit['records']:
            data = (root / row['path']).read_bytes()
            if hashlib.sha256(data).hexdigest() != row['sha256']:
                raise ValueError('Recurso diferente da auditoria M2')
            _, table = decode(data)
            total = 0
            group = category(row['path'])
            for string in table.strings:
                try:
                    text = string.text()
                except UnicodeDecodeError:
                    invalid.append(dict(path=row['path'], id=string.id, bytes=len(string.raw)))
                    continue
                if not JAPANESE.search(text):
                    continue
                record = dict(path=row['path'], id=string.id, original=text,
                              translation=None, category_hint=group, source_sha256=row['sha256'])
                stream.write(json.dumps(record, ensure_ascii=False)+'\n')
                duplicate_counts[text] += 1
                counts[group] += 1
                total += 1
            if total:
                files.append(dict(path=row['path'], category_hint=group, candidates=total))
    expected = audit['japanese_strings']
    if sum(counts.values()) != expected:
        raise ValueError('Índice não corresponde à auditoria')
    with output.open('rb') as stream:
        digest = hashlib.file_digest(stream, 'sha256').hexdigest()
    report = dict(local_index=str(output), local_index_sha256=digest,
                  japanese_occurrences=expected, unique_japanese_strings=len(duplicate_counts),
                  resources_with_candidates=len(files), category_hints=dict(counts),
                  classification_verified=False, files=files,
                  non_utf8_strings=invalid, all_visible_text_coverage_verified=False)
    (REPO / 'docs/m2/text-map.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf8')
    print(f'ÍNDICE: {expected} ocorrências; {len(duplicate_counts)} strings únicas; {len(files)} recursos; {len(invalid)} não UTF-8')
    print(json.dumps(dict(counts), ensure_ascii=False))


if __name__ == '__main__':
    main()
