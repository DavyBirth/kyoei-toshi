"""Apply the private en-16 -> en-17 translation patch without touching saves.

Python 3.11+, Windows or Linux/Wine. Outputs are staged and checked before
installation. The old archive is kept only until the transaction succeeds.
"""
import sys
# --check-only must not create __pycache__ inside the extracted update folder.
sys.dont_write_bytecode = True

import argparse
from contextlib import contextmanager
import json
import os
from pathlib import Path
import shutil
import tempfile

from translation_patch import apply as patch_archive, digest, header

ARCHIVE = 'languages/en/common.psarc'
FILES = {
    'City Shrouded in Shadow.exe',
    'languages/en/prompts.kypack',
    'languages/en/ui-corrections.kydelta',
    'languages/en/eboot-text.kytext',
    'assets/ui-art.kyart',
    'assets/title-exit.kyart',
}
RELEASE = 'translation-en17-20261024'


def inside(root, name):
    relative = Path(name)
    if relative.is_absolute() or '..' in relative.parts or '\\' in name or ':' in name:
        raise ValueError('Invalid update path')
    target = root / relative
    if not target.resolve().is_relative_to(root.resolve()):
        raise ValueError('Update path escapes game folder')
    for parent in (target, *target.parents):
        if parent == root:
            break
        if parent.is_symlink() or (hasattr(parent, 'is_junction') and parent.is_junction()):
            raise ValueError('Update paths cannot use links or junctions')
    return target


def save_journal(path, state):
    temporary = path.with_suffix('.tmp')
    with temporary.open('w', encoding='utf-8') as stream:
        json.dump(state, stream, indent=2)
        stream.flush()
        os.fsync(stream.fileno())
    os.replace(temporary, path)


def require_closed(root):
    if os.name == 'nt':
        import ctypes
        from ctypes import wintypes
        create = ctypes.WinDLL('kernel32', use_last_error=True).CreateFileW
        create.argtypes = (wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD,
                           wintypes.LPVOID, wintypes.DWORD, wintypes.DWORD, wintypes.HANDLE)
        create.restype = wintypes.HANDLE
        close = ctypes.WinDLL('kernel32', use_last_error=True).CloseHandle
        close.argtypes = (wintypes.HANDLE,)
        for name in ('runtime/kyoei.exe', 'City Shrouded in Shadow.exe'):
            target = inside(root, name)
            if not target.exists():
                continue
            handle = create(str(target), 0xC0000000, 0, None, 3, 0, None)
            if handle == wintypes.HANDLE(-1).value:
                raise RuntimeError('Feche o jogo e o launcher antes de atualizar.')
            close(handle)
    else:
        from apply_runtime_update_linux import require_closed as linux_closed
        linux_closed(root)


@contextmanager
def session_guard(root):
    lock = inside(root, 'data/session.lock')
    lock.parent.mkdir(parents=True, exist_ok=True)
    if os.name == 'nt':
        import ctypes
        from ctypes import wintypes
        kernel = ctypes.WinDLL('kernel32', use_last_error=True)
        create = kernel.CreateFileW
        create.argtypes = (wintypes.LPCWSTR, wintypes.DWORD, wintypes.DWORD,
                           wintypes.LPVOID, wintypes.DWORD, wintypes.DWORD, wintypes.HANDLE)
        create.restype = wintypes.HANDLE
        kernel.CloseHandle.argtypes = (wintypes.HANDLE,)
        handle = create(str(lock), 0xC0000000, 0, None, 4, 0, None)
        if handle == wintypes.HANDLE(-1).value:
            raise RuntimeError('Feche o launcher ou outra atualização.')
        try:
            yield
        finally:
            kernel.CloseHandle(handle)
    else:
        import fcntl
        with lock.open('a+b') as stream:
            fcntl.flock(stream, fcntl.LOCK_EX | fcntl.LOCK_NB)
            yield


def rollback(root, backup):
    root, backup = root.resolve(), backup.resolve()
    allowed = inside(root, 'data/update-backups')
    if backup.parent != allowed or not backup.name.startswith(RELEASE + '-'):
        raise ValueError('Choose this update transaction inside data/update-backups')
    journal = backup / 'transaction.json'
    state = json.loads(journal.read_text(encoding='utf-8'))
    if state['release'] != RELEASE or state['root'] != str(root):
        raise ValueError('Backup belongs to another installation')
    if state.get('archive_discarded'):
        raise ValueError('A atualização terminou e a tradução antiga já foi excluída.')
    # Validate everything before restoring anything. Never overwrite unique new work.
    for row in state['files']:
        if row['path'] not in FILES | {ARCHIVE}:
            raise ValueError('Invalid backup path')
        target = inside(root, row['path'])
        saved = inside(backup, 'original/' + row['path'])
        if saved.exists() and digest(saved) != row['before']:
            raise ValueError('Backup checksum mismatch')
        if target.exists() and digest(target) not in {row['before'], row['after']}:
            raise ValueError('Installed file was edited after update; preserve it before restoring')
        if row['before'] and not saved.exists() and (not target.exists() or digest(target) != row['before']):
            raise ValueError('Original file not found in backup or installation')
    for row in reversed(state['files']):
        target = inside(root, row['path'])
        saved = inside(backup, 'original/' + row['path'])
        if saved.exists():
            target.parent.mkdir(parents=True, exist_ok=True)
            os.replace(saved, target)
        elif row['before'] is None:
            target.unlink(missing_ok=True)
    state['status'] = 'rolled-back'
    save_journal(journal, state)
    clean_stage(backup)


def clean_stage(backup):
    stage = backup / 'stage'
    if stage.exists():
        if stage.is_symlink() or stage.resolve().parent != backup.resolve():
            raise ValueError('Unexpected staging path')
        shutil.rmtree(stage)


def discard_old_archive(root, backup, state):
    """Davy requested no 6.35 GB backup after a verified successful install."""
    archive = next((r for r in state['files'] if r['path'] == ARCHIVE), None)
    if not archive or state.get('archive_discarded'):
        return
    saved = inside(backup, 'original/' + ARCHIVE)
    if saved.exists():
        if digest(inside(root, ARCHIVE)) != archive['after'] or digest(saved) != archive['before']:
            raise ValueError('Archive cleanup checksum mismatch; both files preserved')
        saved.unlink()
    state['archive_discarded'] = True
    save_journal(backup / 'transaction.json', state)


def preflight(package, root):
    metadata = json.loads((package / 'translation-update.json').read_text(encoding='utf-8'))
    manifest = json.loads((root / 'manifest.json').read_text(encoding='utf-8'))
    if (metadata['schema'], metadata['release'], metadata['title'], metadata['version']) != (
            1, RELEASE, 'CUSA03562', '01.03'):
        raise ValueError('Pacote de tradução incompatível.')
    if (manifest['title'], manifest['version']) != (metadata['title'], metadata['version']):
        raise ValueError('Versão do jogo incompatível.')
    if digest(inside(root, 'runtime/kyoei.exe')) != metadata['required_runtime_sha256']:
        raise ValueError('Instale primeiro o update GoW3 de 24/10/2026.')
    rows = metadata['files']
    if len(rows) != len(FILES) or {r['path'] for r in rows} != FILES:
        raise ValueError('Unexpected translation payload paths')
    patch = inside(package, 'common.patch')
    if digest(patch) != metadata['patch_sha256']:
        raise ValueError('Patch danificado.')
    with patch.open('rb') as stream:
        info = header(stream)
    if info != metadata['archive']:
        raise ValueError('Patch header differs from manifest')
    archive = inside(root, ARCHIVE)
    current = digest(archive)
    if current not in (info['base_sha256'], info['target_sha256']):
        raise ValueError('Tradução instalada diferente da en-16 esperada. Nenhum arquivo alterado.')
    changes = []
    if current != info['target_sha256']:
        changes.append(dict(path=ARCHIVE, before=current, after=info['target_sha256'], bytes=info['target_bytes']))
    installed = {r['path']: r['sha256'] for r in manifest.get('files', [])}
    for row in rows:
        payload = inside(package, 'payload/' + row['path'])
        if payload.stat().st_size != row['bytes'] or digest(payload) != row['sha256']:
            raise ValueError('Arquivo do pacote danificado: ' + row['path'])
        target = inside(root, row['path'])
        before = digest(target) if target.exists() else None
        if before == row['sha256']:
            continue
        if before is not None and installed.get(row['path']) != before:
            raise ValueError('Arquivo instalado foi modificado: ' + row['path'])
        changes.append(dict(path=row['path'], before=before, after=row['sha256'], bytes=row['bytes']))
    require_closed(root)
    return info, changes


def install(package, root, check_only=False):
    package, root = package.resolve(), root.resolve()
    info, changes = preflight(package, root)
    backups = inside(root, 'data/update-backups')
    if backups.exists():
        for path in backups.glob(RELEASE + '-*/transaction.json'):
            state = json.loads(path.read_text(encoding='utf-8'))
            if state['status'] in ('staging', 'installing'):
                raise ValueError('Atualização interrompida. Use --rollback com a pasta: ' + str(path.parent))
            if state['status'] == 'complete' and not check_only:
                with session_guard(root):
                    discard_old_archive(root, path.parent, state)
    if not changes:
        print('Tradução en-17 já instalada.')
        return None
    needed = sum(r['bytes'] for r in changes) + (32 << 20)
    if shutil.disk_usage(root).free < needed:
        raise ValueError(f'Espaço livre insuficiente; necessário {needed / 1e9:.2f} GB.')
    if check_only:
        print(f'Compatível; {len(changes)} arquivos; {needed / 1e9:.2f} GB livres necessários. Nenhum arquivo alterado.')
        return None
    with session_guard(root):
        # Catch concurrent installs or edits during the first preflight.
        locked_info, locked_changes = preflight(package, root)
        if locked_info != info or locked_changes != changes:
            raise ValueError('Installation changed during preflight; retry')
        backups.mkdir(parents=True, exist_ok=True)
        backup = Path(tempfile.mkdtemp(prefix=RELEASE + '-', dir=backups))
        stage = backup / 'stage'
        stage.mkdir()
        journal = backup / 'transaction.json'
        state = dict(release=RELEASE, root=str(root), status='staging', files=changes)
        save_journal(journal, state)
        print('Recuperação em caso de falha:', backup, flush=True)
        try:
            for row in changes:
                staged = inside(stage, row['path'])
                staged.parent.mkdir(parents=True, exist_ok=True)
                print('Preparando:', row['path'], flush=True)
                if row['path'] == ARCHIVE:
                    patch_archive(inside(root, ARCHIVE), package / 'common.patch', staged)
                else:
                    shutil.copyfile(inside(package, 'payload/' + row['path']), staged)
                if digest(staged) != row['after']:
                    raise ValueError('Staged output checksum mismatch')
            require_closed(root)
            # Verify originals again before renaming any of them.
            for row in changes:
                target = inside(root, row['path'])
                if (digest(target) if target.exists() else None) != row['before']:
                    raise ValueError('Installed file changed during staging')
            state['status'] = 'installing'
            save_journal(journal, state)
            for row in changes:
                target = inside(root, row['path'])
                target.parent.mkdir(parents=True, exist_ok=True)
                if row['before'] is not None:
                    saved = inside(backup, 'original/' + row['path'])
                    saved.parent.mkdir(parents=True, exist_ok=True)
                    os.replace(target, saved)
                os.replace(inside(stage, row['path']), target)
            state['status'] = 'complete'
            save_journal(journal, state)
        except BaseException:
            rollback(root, backup)
            raise
        clean_stage(backup)
        discard_old_archive(root, backup, state)
        print('Tradução en-17 aplicada; antiga excluída. Saves preservados. Abra pelo launcher habitual.')
        return backup


def main():
    if sys.version_info < (3, 11):
        raise SystemExit('Esta atualização requer Python 3.11 ou mais recente.')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game', type=Path)
    parser.add_argument('--check-only', action='store_true')
    parser.add_argument('--rollback', type=Path)
    args = parser.parse_args()
    package = Path(__file__).resolve().parent
    root = (args.game or package.parent).resolve()
    try:
        if args.rollback:
            require_closed(root)
            with session_guard(root):
                rollback(root, args.rollback)
            print('Versão anterior restaurada. Saves preservados.')
        else:
            install(package, root, args.check_only)
    except (OSError, ValueError, RuntimeError, KeyError) as error:
        parser.exit(1, 'Atualização não aplicada: ' + str(error) + '\n')


if __name__ == '__main__':
    main()
