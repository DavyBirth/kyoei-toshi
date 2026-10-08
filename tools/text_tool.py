"""Extrai/reinsere strings de um recurso .luc/.csh sem executá-lo.

Textos originais e saídas devem ficar fora do repositório e do dump.
"""
import argparse
import hashlib
import json
from pathlib import Path
from game_resource import Resource
from text_formats import Csh, JAPANESE, Lua52

REPO = Path(__file__).resolve().parent.parent


def decode(data):
    resource = Resource(data)
    table = Lua52(resource.payload) if data[:4] == b'\0cul' else Csh(resource.payload)
    return resource, table


def export(data):
    resource, table = decode(data)
    records = []
    for string in table.strings:
        try:
            text = string.text()
        except UnicodeDecodeError:
            continue
        records.append(dict(id=string.id, original=text, translation=None,
                            japanese_candidate=bool(JAPANESE.search(text))))
    return dict(format='kyoei-text-v1', resource_kind='lua52' if isinstance(table, Lua52) else 'csh',
                source_sha256=hashlib.sha256(data).hexdigest(), strings=records)


def insert(data, document):
    canonical = export(data)
    if any(document.get(key) != canonical[key] for key in ('format', 'resource_kind', 'source_sha256')):
        raise ValueError('Documento não corresponde ao recurso original')
    expected = {r['id']: r for r in canonical['strings']}
    changes = {}
    for record in document['strings']:
        id = record['id']
        if id in changes or id not in expected or record['original'] != expected[id]['original']:
            raise ValueError('ID duplicado/desconhecido ou texto original alterado')
        text = record.get('translation')
        if text is None:
            text = record['original']
        elif not isinstance(text, str) or '\0' in text:
            raise ValueError('Tradução deve ser texto sem byte nulo')
        changes[id] = text.encode('utf8')
    if changes.keys() != expected.keys():
        raise ValueError('Documento com registros ausentes')
    resource, table = decode(data)
    result = resource.rebuild(table.rebuild(changes))
    if len(result) > 64 * 1024 * 1024:
        raise ValueError('Recurso reconstruído excessivamente grande')
    return result


def output_path(path):
    output = path.resolve()
    inventory = json.loads((REPO / 'docs/m0/inventory.json').read_text(encoding='utf8'))
    catalogue = json.loads((REPO / 'work/m1/catalog.json').read_text(encoding='utf8'))
    for protected in (REPO, Path(inventory['source']).resolve(), Path(catalogue['extraction']).resolve()):
        if output == protected or output.is_relative_to(protected):
            raise ValueError('Saída deve ficar fora do repositório, dump e extração M1')
    if output.exists():
        raise ValueError('Saída já existe; não sobrescrevemos arquivos')
    return output


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest='action', required=True)
    extract = sub.add_parser('extract')
    extract.add_argument('source', type=Path)
    extract.add_argument('output', type=Path)
    reinsert = sub.add_parser('insert')
    reinsert.add_argument('source', type=Path)
    reinsert.add_argument('document', type=Path)
    reinsert.add_argument('output', type=Path)
    args = parser.parse_args()
    output = output_path(args.output)
    data = args.source.read_bytes()
    if args.action == 'extract':
        document = export(data)
        result = (json.dumps(document, indent=2, ensure_ascii=False)+'\n').encode('utf8')
        print(f'Strings: {len(document["strings"])}; candidatas japonesas: {sum(r["japanese_candidate"] for r in document["strings"])}')
    else:
        document = json.loads(args.document.read_text(encoding='utf8'))
        result = insert(data, document)
        print(f'Reinserido: {len(result)} bytes; idêntico ao original={result == data}')
    output.parent.mkdir(parents=True, exist_ok=True)
    with output.open('xb') as stream:
        stream.write(result)
    if output.read_bytes() != result:
        raise ValueError('Gravação não confere')
    print(f'Salvo em: {output}')


if __name__ == '__main__':
    main()
