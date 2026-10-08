"""Fixa fontes e bibliotecas locais do Godzilla, sem alterar o projeto oficial."""
import hashlib
import io
import json
from pathlib import Path
import shlex
import subprocess
import tarfile

REPO = Path(__file__).resolve().parents[1]
OFFICIAL = Path('G:/godzilla-port-codex')
DEST = REPO / 'third_party/godzilla-core'


def git(root, *args):
    return subprocess.check_output(['git', '-c', f'safe.directory={root.as_posix()}', '-C', str(root), *args])


def archive(root, destination, prefixes=None):
    revision = git(root, 'rev-parse', 'HEAD').decode().strip()
    records = []
    with tarfile.open(fileobj=io.BytesIO(git(root, 'archive', revision))) as files:
        for member in files:
            if not member.isfile() or (prefixes and not any(member.name == p or member.name.startswith(p+'/') for p in prefixes)):
                continue
            relative = Path(member.name)
            if relative.is_absolute() or '..' in relative.parts:
                raise ValueError('Caminho inseguro no Git')
            data = files.extractfile(member).read()
            output = destination / relative
            output.parent.mkdir(parents=True, exist_ok=True)
            output.write_bytes(data)
            records.append(dict(path=output.relative_to(DEST).as_posix(), sha256=hashlib.sha256(data).hexdigest()))
    return dict(commit=revision, files=records)


def main():
    if DEST.exists():
        raise ValueError('Cópia já existe; não sobrescrever a base fixa')
    # Runtime/GPU must match the committed source; unrelated capture tools may be dirty.
    dirty = git(OFFICIAL, 'status', '--porcelain', '--', 'src', 'gpu', 'scripts', 'CMakeLists.txt').decode().strip()
    if dirty:
        raise ValueError('Fontes compartilhadas oficiais têm mudanças sem commit')
    report = {'official': str(OFFICIAL), 'core': archive(OFFICIAL, DEST, ['src', 'gpu', 'scripts', 'LICENSE', 'CMakeLists.txt'])}
    for name in ('shadPS4', 'bloodborne_pc'):
        root = OFFICIAL / 'third_party' / name
        report[name] = archive(root, DEST / 'third_party' / name)
    ninja = (OFFICIAL / 'build/build.ninja').read_text(encoding='utf8')
    block = ninja.split('build gzport.exe:', 1)[1].split('\nbuild ', 1)[0]
    line = next(s.strip().split(' = ', 1)[1] for s in block.splitlines() if s.strip().startswith('LINK_LIBRARIES = '))
    libs = []
    system = []
    binaries = []
    for token in shlex.split(line):
        if token.startswith('-l'):
            name = token[2:].removesuffix('.lib')
            if name not in system:
                system.append(name)
            continue
        source = OFFICIAL / 'build' / token
        if not source.is_file():
            raise ValueError(f'Biblioteca ausente: {source}')
        data = source.read_bytes()
        output = DEST / 'prebuilt' / token
        output.parent.mkdir(parents=True, exist_ok=True)
        output.write_bytes(data)
        libs.append(token)
        binaries.append(dict(path=output.relative_to(DEST).as_posix(), sha256=hashlib.sha256(data).hexdigest()))
    for source in (OFFICIAL/'build').glob('*.dll'):
        data = source.read_bytes()
        output = DEST / 'prebuilt' / source.name
        output.write_bytes(data)
        binaries.append(dict(path=output.relative_to(DEST).as_posix(), sha256=hashlib.sha256(data).hexdigest()))
    report['prebuilt'] = dict(origin=str(OFFICIAL/'build'), libraries=libs, system_libraries=system, files=binaries)
    (REPO/'docs/godzilla-core.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf8')
    cmake = 'set(KYOEI_GPU_LIBRARIES\n'+''.join(f'  "${{KYOEI_CORE}}/prebuilt/{p}"\n' for p in libs)+')\n'
    cmake += 'set(KYOEI_SYSTEM_LIBRARIES '+' '.join(system)+')\n'
    (DEST/'prebuilt/libraries.cmake').write_text(cmake, encoding='utf8')
    print('BASE FIXADA:', report['core']['commit'], '; GPU local com hashes; sem arquivos de jogo')


if __name__ == '__main__':
    main()
