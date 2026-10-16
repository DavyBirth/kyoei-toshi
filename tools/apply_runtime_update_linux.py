"""Apply the shared Windows runtime to the Linux Wine/Proton distribution."""
import argparse
import fcntl
import hashlib
import json
import os
from pathlib import Path
import shutil
import tempfile


def digest(path):
    with path.open('rb') as source:
        return hashlib.file_digest(source, 'sha256').hexdigest()


def require_closed(root):
    targets = [str((root/name).resolve()) for name in
               ('runtime/kyoei.exe', 'City Shrouded in Shadow.exe')]
    for entry in Path('/proc').iterdir():
        if not entry.name.isdigit():
            continue
        try:
            if entry.stat().st_uid != os.getuid():
                continue
            mappings = (entry/'maps').read_text(errors='replace')
        except FileNotFoundError:
            continue  # process exited during enumeration
        except PermissionError:
            raise RuntimeError('Não foi possível verificar processos. Feche jogo/launcher e tente novamente.')
        if any(target in mappings for target in targets):
            raise RuntimeError('Feche o jogo e o launcher antes de atualizar.')


def apply(package, root, check_only=False):
    root = root.resolve()
    metadata = json.loads((package/'update.json').read_text(encoding='utf8'))
    manifest = json.loads((root/'manifest.json').read_text(encoding='utf8'))
    if metadata['schema'] != 1 or metadata['platform'] != 'windows-x64':
        raise ValueError('Pacote incompatível com Wine/Proton.')
    if (manifest['title'], manifest['version']) != (metadata['title'], metadata['version']):
        raise ValueError('Versão do jogo incompatível.')
    target = root/'runtime/kyoei.exe'
    if target.is_symlink() or not target.resolve().is_relative_to(root):
        raise ValueError('Runtime deve estar dentro da pasta do jogo.')
    payload = package/'kyoei.exe'
    expected = metadata['sha256']
    if digest(payload) != expected:
        raise ValueError('Pacote danificado; nenhum arquivo alterado.')
    installed = digest(target)
    if installed == expected:
        print('Esta atualização já está instalada.')
        return
    if installed not in metadata['accepted_sha256']:
        raise ValueError('Runtime diferente da versão esperada; nenhum arquivo alterado.')
    require_closed(root)
    if check_only:
        print('Atualização compatível; nenhum arquivo alterado.')
        return
    data = root/'data'
    data.mkdir(exist_ok=True)
    # flock serializes Linux updaters. Wine's Windows file locks are separate:
    # require_closed checks loaded binaries; user must keep the launcher closed.
    with (data/'runtime-update.lock').open('a+b') as guard:
        fcntl.flock(guard, fcntl.LOCK_EX | fcntl.LOCK_NB)
        require_closed(root)
        if digest(target) != installed:
            raise ValueError('Runtime mudou durante a verificação.')
        backup_dir = data/'update-backups'
        backup_dir.mkdir(exist_ok=True)
        backup = backup_dir/(installed+'.exe')
        with backup.open('xb') as saved, target.open('rb') as current:
            shutil.copyfileobj(current, saved)
            saved.flush()
            os.fsync(saved.fileno())
        if digest(backup) != installed:
            raise ValueError('Falha no backup; runtime anterior preservado.')
        temporary = None
        try:
            with tempfile.NamedTemporaryFile(dir=target.parent, prefix='kyoei-update-', delete=False) as staged:
                temporary = Path(staged.name)
                with payload.open('rb') as source:
                    shutil.copyfileobj(source, staged)
                staged.flush()
                os.fsync(staged.fileno())
            if digest(temporary) != expected:
                raise ValueError('Falha na cópia; runtime anterior preservado.')
            temporary.chmod(target.stat().st_mode & 0o777)
            require_closed(root)
            os.replace(temporary, target)
        finally:
            if temporary is not None:
                temporary.unlink(missing_ok=True)
        print('Atualização aplicada. Saves, jogo e prefixo Wine preservados.')
        print('Backup do runtime anterior:', backup)


def main():
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
