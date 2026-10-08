"""Gera uma cópia independente do dump após a ida e volta dos textos."""
import argparse
import hashlib
import json
import shutil
from pathlib import Path
from psarc_patch import patch
from text_tool import REPO, export, insert, output_path


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('destination', type=Path)
    args = parser.parse_args()
    destination = output_path(args.destination)
    inventory = json.loads((REPO / 'docs/m0/inventory.json').read_text(encoding='utf8'))
    catalogue = json.loads((REPO / 'work/m1/catalog.json').read_text(encoding='utf8'))
    source = Path(inventory['source'])
    extracted = Path(catalogue['extraction'])
    required = sum(r['bytes'] for r in inventory['files'])
    parent = destination.parent
    while not parent.exists():
        parent = parent.parent
    if shutil.disk_usage(parent).free < required + 1024 ** 3:
        raise ValueError('Espaço insuficiente para a cópia mais 1 GiB de margem')
    archives = {r['archive']: r for r in catalogue['archives']}
    destination.mkdir(parents=True)
    proofs = []
    resource_count = 0
    for row in inventory['files']:
        file = source / row['path']
        output = destination / row['path']
        print(f'Preparando: {row["path"]}', flush=True)
        if file.suffix == '.psarc':
            archive = archives[file.name]
            replacements = {}
            for entry in archive['files']:
                if Path(entry['path']).suffix.lower() not in ('.csh', '.luc'):
                    continue
                data = (extracted / file.stem / entry['path']).read_bytes()
                if hashlib.sha256(data).hexdigest() != entry['sha256']:
                    raise ValueError('Recurso de entrada diferente do M1')
                document = json.loads(json.dumps(export(data), ensure_ascii=False))
                rebuilt = insert(data, document)
                if rebuilt != data:
                    raise ValueError('Reinserção sem tradução alterou um recurso')
                replacements[entry['path']] = rebuilt
                resource_count += 1
            proof = patch(file, output, replacements, row['sha256'])
            if not proof['identical_archive']:
                raise ValueError('Arquivo PSARC de prova deveria ser idêntico')
            proofs.append(dict(path=row['path'], **proof))
        else:
            before = file.stat()
            digest = hashlib.sha256()
            output.parent.mkdir(parents=True, exist_ok=True)
            with file.open('rb') as read, output.open('xb') as write:
                while block := read.read(1024 * 1024):
                    digest.update(block)
                    write.write(block)
            after = file.stat()
            if (before.st_size, before.st_mtime_ns) != (after.st_size, after.st_mtime_ns):
                raise ValueError('Arquivo mudou durante a cópia')
            with output.open('rb') as stream:
                copied = hashlib.file_digest(stream, 'sha256').hexdigest()
            if digest.hexdigest() != row['sha256'] or copied != row['sha256']:
                raise ValueError('Arquivo copiado diferente do M0')
            proofs.append(dict(path=row['path'], source_sha256=digest.hexdigest(), output_sha256=copied, verified=True))
        if output.stat().st_size != row['bytes']:
            raise ValueError('Tamanho da cópia diferente do M0')
        print(f'IDÊNTICO: {row["path"]}', flush=True)
    names = {p.relative_to(destination).as_posix() for p in destination.rglob('*') if p.is_file()}
    if names != {r['path'] for r in inventory['files']}:
        raise ValueError('Lista final de arquivos não confere')
    report = dict(source=str(source), destination=str(destination), resources_roundtripped=resource_count,
                  identical_files=len(proofs), identical_bytes=required, files=proofs,
                  emulator_test_completed=False)
    (REPO / 'docs/m2/dump-proof.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf8')
    print(f'DUMP IDÊNTICO: {len(proofs)} arquivos; {required} bytes; {resource_count} recursos reinseridos', flush=True)


if __name__ == '__main__':
    main()
