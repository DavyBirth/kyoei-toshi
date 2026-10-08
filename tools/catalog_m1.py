"""Lista PSARC e, opcionalmente, extrai para uma pasta externa nova.

Uso: python tools/catalog_m1.py [--extract G:/kyoei-toshi-work/m1]
"""
import argparse
import collections
import hashlib
import json
import shutil
from pathlib import Path
from psarc import Psarc


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--extract', type=Path)
    args = parser.parse_args()
    repo = Path(__file__).resolve().parent.parent
    inventory = json.loads((repo / 'docs/m0/inventory.json').read_text(encoding='utf8'))
    source = Path(inventory['source']).resolve(strict=True)
    destination = args.extract.resolve() if args.extract else None
    if destination:
        if destination == repo or repo in destination.parents or destination == source or source in destination.parents:
            raise ValueError('Extração deve ficar fora do repositório e do dump')
        if destination.exists():
            raise ValueError('Escolha uma pasta de extração nova; não sobrescrevemos arquivos')
    records = []
    for record in inventory['files']:
        if not record['path'].endswith('.psarc'):
            continue
        path = source / record['path']
        with Psarc(path) as archive:
            entries = [dict(path=e.name, bytes=e.size, offset=e.offset, block_index=e.block,
                            path_md5=e.digest.hex()) for e in archive.entries[1:]]
            records.append(dict(archive=path.name, archive_sha256=record['sha256'],
                                header=archive.header, files=entries))
            print(f'{path.name}: {len(entries)} arquivos; {sum(e["bytes"] for e in entries)} bytes', flush=True)
    total = sum(e['bytes'] for r in records for e in r['files'])
    if destination:
        existing_parent = destination.parent
        while not existing_parent.exists():
            existing_parent = existing_parent.parent
        if shutil.disk_usage(existing_parent).free < total + 1024 ** 3:
            raise ValueError('Espaço insuficiente para extração mais 1 GiB de margem')
        destination.mkdir(parents=True)
        for record in records:
            path = source / 'DAT' / record['archive']
            before = path.stat()
            # O conteúdo extraído deve corresponder exatamente ao dump validado no M0.
            digest = hashlib.sha256()
            with path.open('rb') as stream:
                while block := stream.read(8 * 1024 * 1024):
                    digest.update(block)
            if digest.hexdigest() != record['archive_sha256']:
                raise ValueError(f'PSARC mudou desde M0: {path.name}')
            with Psarc(path) as archive:
                for index, (entry, row) in enumerate(zip(archive.entries[1:], record['files'], strict=True), 1):
                    output = destination / path.stem / entry.name
                    output.parent.mkdir(parents=True, exist_ok=True)
                    if not output.resolve().is_relative_to(destination):
                        raise ValueError('Destino fora da pasta de extração')
                    digest = hashlib.sha256()
                    prefix = bytearray()
                    with output.open('xb') as stream:
                        for chunk in archive.chunks(entry):
                            stream.write(chunk)
                            digest.update(chunk)
                            if len(prefix) < 64:
                                prefix.extend(chunk[:64-len(prefix)])
                    row['sha256'] = digest.hexdigest()
                    row['prefix_hex'] = prefix.hex()
                    if index % 2000 == 0:
                        print(f'{path.name}: extraídos {index}/{len(record["files"])}', flush=True)
            print(f'EXTRAÍDO {path.name}', flush=True)
            after = path.stat()
            if (before.st_size, before.st_mtime_ns) != (after.st_size, after.st_mtime_ns):
                raise ValueError(f'PSARC mudou durante a leitura: {path.name}')
        # Relê todos os resultados para comprovar a escrita.
        for record in records:
            for index, row in enumerate(record['files'], 1):
                path = destination / Path(record['archive']).stem / row['path']
                with path.open('rb') as stream:
                    digest = hashlib.file_digest(stream, 'sha256')
                if path.stat().st_size != row['bytes'] or digest.hexdigest() != row['sha256']:
                    raise ValueError(f'Extração não confere: {path}')
                if index % 2000 == 0:
                    print(f'{record["archive"]}: conferidos {index}/{len(record["files"])}', flush=True)
            print(f'VERIFICADO {record["archive"]}', flush=True)
    report = dict(source=str(source), extraction=str(destination) if destination else None,
                  extracted_and_verified=bool(destination), total_bytes=total, archives=records)
    out = repo / ('docs/m1' if destination else 'work/m1')
    out.mkdir(parents=True, exist_ok=True)
    # Prefixos e nomes detalhados ficam no workspace ignorado; Git recebe apenas metadados.
    local = repo / 'work/m1'
    local.mkdir(parents=True, exist_ok=True)
    local_name = 'catalog.json' if destination else 'listing.json'
    (local / local_name).write_text(json.dumps(report, indent=2, ensure_ascii=False)+'\n', encoding='utf8')
    summary = []
    for r in records:
        counts = collections.Counter(Path(e['path']).suffix.lower() or '(sem extensão)' for e in r['files'])
        summary.append(dict(archive=r['archive'], sha256=r['archive_sha256'], header=r['header'],
                            files=len(r['files']), bytes=sum(e['bytes'] for e in r['files']), extensions=dict(counts)))
    summary_name = 'summary.json' if destination else 'listing-summary.json'
    (out / summary_name).write_text(json.dumps(dict(source=str(source), extraction=report['extraction'],
        extracted_and_verified=bool(destination), total_bytes=total, archives=summary), indent=2)+'\n', encoding='utf8')
    print(f'TOTAL: {sum(r["files"] for r in summary)} arquivos; {total} bytes; extração verificada={bool(destination)}', flush=True)


if __name__ == '__main__':
    main()
