"""Linux/Wine applicator of the small translation update (same steps as the
Windows apply_translation_update.ps1): copy the listed files, then delete the
old 6 GB English archive. Saves, game/ and settings are never touched.
"""
import sys
sys.dont_write_bytecode = True  # keep the extracted update folder clean

import argparse
import fcntl
import hashlib
import json
import os
from pathlib import Path
import shutil
import time


def digest(path):
    with path.open('rb') as source:
        return hashlib.file_digest(source, 'sha256').hexdigest()


def inside(root, name):
    relative = Path(name)
    if relative.is_absolute() or '..' in relative.parts or '\\' in name or ':' in name:
        raise ValueError('Caminho inválido no pacote.')
    target = root/relative
    if not target.resolve().is_relative_to(root) or any(p.is_symlink() for p in (target, *target.parents) if p != root and p.is_relative_to(root)):
        raise ValueError('Caminho fora da pasta do jogo.')
    return target


def plan(package, root):
    metadata = json.loads((package/'translation-update.json').read_text(encoding='utf8'))
    manifest = json.loads((root/'manifest.json').read_text(encoding='utf8'))
    if metadata['schema'] != 2:
        raise ValueError('Pacote incompatível.')
    if (manifest['title'], manifest['version']) != (metadata['title'], metadata['version']):
        raise ValueError('Versão do jogo incompatível.')
    disc = inside(root, 'game/DAT/common.psarc')
    if not disc.is_file() or disc.stat().st_size != metadata['disc_archive_bytes']:
        raise ValueError('game/DAT/common.psarc não é o original do disco. Nenhum arquivo alterado.')
    rows = {row['path']: row for row in metadata['files']}
    runtime = digest(inside(root, 'runtime/kyoei.exe'))
    if runtime != rows['runtime/kyoei.exe']['sha256'] and runtime not in metadata['accepted_runtime_sha256']:
        raise ValueError('Runtime diferente das versões esperadas. Nenhum arquivo alterado.')
    changes = []
    for row in metadata['files']:
        if digest(package/'payload'/row['path']) != row['sha256']:
            raise ValueError('Pacote danificado: '+row['path'])
        target = inside(root, row['path'])
        if not target.exists() or digest(target) != row['sha256']:
            changes.append(row)
    removals = [name for name in metadata['remove'] if inside(root, name).exists()]
    return metadata, changes, removals


def apply(package, root, check_only=False):
    from apply_runtime_update_linux import require_closed
    root = root.resolve()
    metadata, changes, removals = plan(package, root)
    if not changes and not removals:
        print('Esta atualização já está instalada.')
        return
    require_closed(root)
    if check_only:
        print(f'Atualização compatível; {len(changes)} arquivos novos, {len(removals)} antigos a excluir. Nenhum arquivo alterado.')
        return
    data = root/'data'
    data.mkdir(exist_ok=True)
    with (data/'runtime-update.lock').open('a+b') as guard:
        fcntl.flock(guard, fcntl.LOCK_EX | fcntl.LOCK_NB)
        require_closed(root)
        backup = data/'update-backups'/(metadata['release']+'-'+time.strftime('%Y%m%d-%H%M%S'))
        done = []
        try:
            for row in changes:
                target = inside(root, row['path'])
                target.parent.mkdir(parents=True, exist_ok=True)
                temporary = target.with_name(target.name+'.update-tmp')
                shutil.copyfile(package/'payload'/row['path'], temporary)
                if target.exists():
                    temporary.chmod(target.stat().st_mode & 0o777)
                if digest(temporary) != row['sha256']:
                    raise ValueError('Falha na cópia: '+row['path'])
            for row in changes:
                target = inside(root, row['path'])
                if target.exists():
                    saved = backup/row['path']
                    saved.parent.mkdir(parents=True, exist_ok=True)
                    os.replace(target, saved)
                os.replace(target.with_name(target.name+'.update-tmp'), target)
                done.append(row)
        except BaseException:
            # Put back every file already replaced; nothing old has been deleted yet.
            for row in done:
                target, saved = inside(root, row['path']), backup/row['path']
                target.unlink()
                if saved.exists():
                    os.replace(saved, target)
            for row in changes:
                target = inside(root, row['path'])
                target.with_name(target.name+'.update-tmp').unlink(missing_ok=True)
            raise
        # Only after every new file is in place: the old copy is no longer read.
        for name in removals:
            inside(root, name).unlink()
    print('Tradução atualizada. Saves, jogo e prefixo Wine preservados.')
    if removals:
        print('Tradução antiga de 6 GB excluída.')
    if backup.exists():
        print('Arquivos pequenos anteriores guardados em:', backup)


def main():
    if sys.version_info < (3, 11):
        raise SystemExit('Esta atualização requer Python 3.11 ou mais recente.')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game', type=Path)
    parser.add_argument('--check-only', action='store_true')
    args = parser.parse_args()
    package = Path(__file__).resolve().parent
    try:
        apply(package, args.game or package.parent, args.check_only)
    except (OSError, ValueError, RuntimeError, KeyError) as error:
        parser.exit(1, 'Atualização não aplicada: '+str(error)+'\n')


if __name__ == '__main__':
    main()
