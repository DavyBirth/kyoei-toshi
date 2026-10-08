"""Repete a prova do catálogo M1 sem extrair ou sobrescrever arquivos."""
import hashlib
import json
from pathlib import Path
from psarc import safe_name


def main():
    repo = Path(__file__).resolve().parent.parent
    path = repo / 'work/m1/catalog.json'
    expected = json.loads((repo / 'docs/m1/formats.json').read_text(encoding='utf8'))
    raw = path.read_bytes()
    if hashlib.sha256(raw).hexdigest() != expected['catalog_sha256']:
        raise ValueError('Catálogo local diferente da prova registrada')
    catalog = json.loads(raw)
    if not catalog['extracted_and_verified']:
        raise ValueError('Catálogo sem extração comprovada')
    root = Path(catalog['extraction']).resolve(strict=True)
    files = 0
    total = 0
    for archive in catalog['archives']:
        safe_name(archive['archive'])
        if '/' in archive['archive'] or not archive['archive'].endswith('.psarc'):
            raise ValueError('Nome de PSARC inválido')
        folder = root / Path(archive['archive']).stem
        names = {safe_name(row['path']) for row in archive['files']}
        actual = {p.relative_to(folder).as_posix() for p in folder.rglob('*') if p.is_file()}
        if actual != names:
            raise ValueError(f'Lista de arquivos diferente: {folder}')
        for row in archive['files']:
            p = folder / row['path']
            if not p.resolve().is_relative_to(root):
                raise ValueError('Arquivo fora da extração')
            before = p.stat()
            with p.open('rb') as stream:
                digest = hashlib.file_digest(stream, 'sha256').hexdigest()
            after = p.stat()
            if (before.st_size, before.st_mtime_ns) != (after.st_size, after.st_mtime_ns):
                raise ValueError(f'Arquivo mudou durante a leitura: {p}')
            if before.st_size != row['bytes'] or digest != row['sha256']:
                raise ValueError(f'Tamanho ou hash diferente: {p}')
            files += 1
            total += before.st_size
            if files % 5000 == 0:
                print(f'Conferidos: {files}', flush=True)
        print(f'VERIFICADO {archive["archive"]}', flush=True)
    if files != expected['verified_files'] or total != expected['verified_bytes']:
        raise ValueError('Totais diferentes da prova registrada')
    print(f'VALIDADO M1: {files} arquivos; {total} bytes', flush=True)


if __name__ == '__main__':
    main()
