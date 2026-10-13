"""Reextrai somente recursos ausentes do dump validado, sem duplicar o jogo."""
import argparse
import hashlib
import json
from pathlib import Path
from psarc import Psarc, safe_name

REPO = Path(__file__).resolve().parents[1]


def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('archive', help='Nome no catalogo, por exemplo system.psarc')
    parser.add_argument('--entry', action='append', help='Caminho exato; pode repetir para recuperar varios recursos')
    parser.add_argument('--entries-file', type=Path, help='JSON com lista de caminhos exatos, para uma recuperacao seletiva')
    args = parser.parse_args()
    proof = json.loads((REPO/'docs/m1/formats.json').read_text(encoding='utf8'))
    ledger = REPO/'work/m1/catalog.json'
    if digest(ledger) != proof['catalog_sha256']:
        raise ValueError('Catalogo diferente da prova M1')
    catalog = json.loads(ledger.read_text(encoding='utf8'))
    record = next((r for r in catalog['archives'] if r['archive'] == args.archive), None)
    if not record:
        raise ValueError('Pacote fora do catalogo')
    source = Path(catalog['source']).resolve(strict=True)
    root = Path(catalog['extraction']).resolve(strict=True)
    if root.is_relative_to(REPO) or root.is_relative_to(source) or source.is_relative_to(root):
        raise ValueError('Extracao sobrepoe dados protegidos')
    archive_path = source/'DAT'/safe_name(args.archive)
    if not archive_path.resolve(strict=True).is_relative_to(source):
        raise ValueError('Pacote fora do dump')
    if digest(archive_path) != record['archive_sha256']:
        raise ValueError('PSARC mudou desde M0')
    expected = {row['path']:row for row in record['files']}
    selected=set(args.entry or [])
    if args.entries_file:
        entries=json.loads(args.entries_file.read_text(encoding='utf8'))
        if not isinstance(entries,list) or not all(isinstance(name,str) for name in entries):
            raise ValueError('Lista de recursos invalida')
        selected.update(entries)
    selective=args.entry is not None or args.entries_file is not None
    if any(name not in expected for name in selected):
        raise ValueError('Recurso fora do catalogo')
    restored = 0
    with Psarc(archive_path) as archive:
        for entry in archive.entries[1:]:
            if selective and entry.name not in selected:
                continue
            row = expected[entry.name]
            output = root/Path(args.archive).stem/safe_name(entry.name)
            if not output.resolve().is_relative_to(root):
                raise ValueError('Saida fora da extracao')
            if output.exists():
                if output.stat().st_size != row['bytes'] or digest(output) != row['sha256']:
                    raise ValueError(f'Arquivo existente alterado: {output}')
                continue
            output.parent.mkdir(parents=True, exist_ok=True)
            hasher = hashlib.sha256()
            with output.open('xb') as stream:
                for chunk in archive.chunks(entry):
                    stream.write(chunk)
                    hasher.update(chunk)
            if output.stat().st_size != row['bytes'] or hasher.hexdigest() != row['sha256']:
                raise ValueError(f'Reextracao nao confere: {output}')
            restored += 1
    print(f'Recursos restaurados e verificados: {restored}')


if __name__ == '__main__':
    main()
