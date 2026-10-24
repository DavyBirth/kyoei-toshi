"""Small translation update on synthetic installs (Windows PowerShell and WSL).

Usage: python tools/test_translation_update.py build/<update>.zip
"""
import json, os, shutil, subprocess, sys, tempfile, zipfile
from pathlib import Path
from build_portable import ROOT, digest

PREFIX = 'Atualizacao-Traducao/'
KEPT = {'data/user/save.bin': b'save', 'data/settings.txt': b'en', 'game/eboot.bin': b'eboot',
        'game/DAT/common.psarc': b'DISC', 'languages/ja/other.bin': b'ja'}


def install(root, update):
    with zipfile.ZipFile(update) as archive:
        archive.extractall(root)
    package = root/PREFIX
    metadata = json.loads((package/'translation-update.json').read_text(encoding='utf8'))
    # The fixture disc archive is 4 bytes, not 6 GB; the old runtime is fake.
    metadata['disc_archive_bytes'] = 4
    old_runtime = b'old runtime'
    (root/'runtime').mkdir()
    (root/'runtime/kyoei.exe').write_bytes(old_runtime)
    metadata['accepted_runtime_sha256'].append(digest(root/'runtime/kyoei.exe'))
    (package/'translation-update.json').write_text(json.dumps(metadata), encoding='utf8')
    (root/'manifest.json').write_text(json.dumps(dict(title='CUSA03562', version='01.03')), encoding='utf8')
    for name, data in {**KEPT, 'City Shrouded in Shadow.exe': b'old launcher',
                       'assets/ui-art.kyart': b'old art', 'languages/en/common.psarc': b'old 6 GB copy',
                       'languages/en/ui-corrections.kydelta': b'old delta'}.items():
        (root/name).parent.mkdir(parents=True, exist_ok=True)
        (root/name).write_bytes(data)
    return metadata


def runner(platform, root):
    if platform == 'windows':
        base = ['powershell', '-NoProfile', '-ExecutionPolicy', 'Bypass', '-File', str(root/PREFIX/'apply_translation_update.ps1'), '-GamePath', str(root)]
        return lambda *extra: subprocess.run(base+[{'--check-only': '-CheckOnly'}[e] for e in extra], capture_output=True, text=True)
    linux = '/mnt/'+root.drive[0].lower()+root.as_posix()[2:]
    base = ['wsl', '-d', 'Ubuntu', '--', 'python3', linux+'/'+PREFIX+'apply_translation_update.py', '--game', linux]
    return lambda *extra: subprocess.run(base+list(extra), capture_output=True, text=True)


def check(platform, update):
    with tempfile.TemporaryDirectory(dir=ROOT/'build') as temporary:
        root = Path(temporary).resolve()
        metadata = install(root, update)
        run = runner(platform, root)
        before = {p: p.read_bytes() for p in root.rglob('*') if p.is_file() and PREFIX[:-1] not in p.parts}
        result = run('--check-only')
        assert result.returncode == 0 and 'Nenhum arquivo alterado' in result.stdout, result
        assert before == {p: p.read_bytes() for p in before}, 'check-only changed files'
        assert not list(root.rglob('__pycache__'))
        result = run()
        assert result.returncode == 0, result
        for row in metadata['files']:
            assert digest(root/row['path']) == row['sha256'], row['path']
        for name in metadata['remove']:
            assert not (root/name).exists(), name
        for name, data in KEPT.items():
            assert (root/name).read_bytes() == data, name
        backups = list((root/'data/update-backups').iterdir())
        assert len(backups) == 1 and (backups[0]/'runtime/kyoei.exe').read_bytes() == b'old runtime'
        assert not list(root.rglob('*.update-tmp'))
        result = run()
        assert result.returncode == 0 and 'instalada' in result.stdout, result

    with tempfile.TemporaryDirectory(dir=ROOT/'build') as temporary:
        root = Path(temporary).resolve()
        install(root, update)
        (root/'runtime/kyoei.exe').write_bytes(b'unknown runtime')
        result = runner(platform, root)()
        assert result.returncode != 0 and (root/'languages/en/common.psarc').exists(), result

    if platform == 'windows':
        # A file that cannot be replaced midway: everything already moved comes back.
        with tempfile.TemporaryDirectory(dir=ROOT/'build') as temporary:
            root = Path(temporary).resolve()
            install(root, update)
            originals = {p: p.read_bytes() for p in root.rglob('*') if p.is_file() and PREFIX[:-1] not in p.parts}
            locker = subprocess.Popen(['powershell', '-NoProfile', '-Command',
                "$f=[IO.File]::Open('%s','Open','Read','Read'); [Console]::Out.WriteLine('locked'); [Console]::In.ReadLine() | Out-Null; $f.Dispose()"
                % (root/'assets/ui-art.kyart')], stdin=subprocess.PIPE, stdout=subprocess.PIPE, text=True)
            assert locker.stdout.readline().strip() == 'locked'
            result = runner(platform, root)()
            locker.communicate('\n')
            assert result.returncode != 0 and 'desfeita' in (result.stdout+result.stderr), result
            current = {p: p.read_bytes() for p in root.rglob('*') if p.is_file() and PREFIX[:-1] not in p.parts
                       and 'update-backups' not in p.parts and p.name != 'session.lock'}
            assert current == {p: d for p, d in originals.items()}, 'rollback incomplete'
    print(platform, 'TRANSLATION UPDATE PASS')


if __name__ == '__main__':
    sys.stdout.reconfigure(encoding='utf-8')
    update = Path(sys.argv[1]).resolve()
    for platform in ('windows', 'linux'):
        check(platform, update)
