"""Build the small translation update for existing friend installations.

The whole translation is one read-time delta over the disc archive
(build_translation_delta.py), so the update only carries small files: the
runtime that reads the delta, the launcher, the delta and the HUD/prompt
files. The applicator deletes the old 6 GB English archive. Windows needs no
Python; Linux/Wine uses Python 3.11+. Never packages game data or saves.
"""
import argparse
import json
from pathlib import Path
import zipfile

from build_portable import ROOT, digest, personal_text_hits

RELEASE = 'translation-en17-delta-20261029'
DISC_ARCHIVE_BYTES = 6346525059
FILES = (
    'runtime/kyoei.exe',
    'City Shrouded in Shadow.exe',
    'languages/en/translation.kydelta',
    'languages/en/prompts.kypack',
    'languages/ja/prompts.kypack',
    'languages/en/eboot-text.kytext',
    'assets/ui-art.kyart',
    'assets/title-exit.kyart',
)
REMOVE = ('languages/en/common.psarc', 'languages/en/ui-corrections.kydelta')
PREFIX = 'Atualizacao-Traducao/'

README = '''Tradução inglesa natural (en-17) — atualização pequena, 29/10/2026

A tradução agora é um arquivo pequeno lido por cima do jogo original.
Este pacote troca o runtime, o launcher e os arquivos da tradução e EXCLUI
a tradução antiga de 6 GB (languages/en/common.psarc), liberando espaço.
Não contém arquivos do jogo. Saves, configurações e game/ ficam intactos.
Já inclui o núcleo GoW3; serve para qualquer versão distribuída antes.

1. Feche o jogo e o launcher.
2. Extraia este ZIP na pasta que contém City Shrouded in Shadow.exe.
3. Windows: abra Atualizacao-Traducao e execute Aplicar traducao.cmd.
   Não precisa instalar nada.
   Linux/Wine: num terminal na pasta do jogo, execute
   sh Atualizacao-Traducao/Aplicar-no-Linux.sh   (Python 3.11+, sem sudo)
4. Abra pelo launcher habitual. O inglês deve aparecer com a revisão
   natural, novos menus e dicas.

Se algo falhar no meio, o aplicador desfaz e restaura os arquivos
anteriores. Os arquivos pequenos substituídos ficam em data/update-backups.
A tradução ainda está em revisão; alguns textos podem seguir em japonês.
'''


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--portable', type=Path, default=ROOT/'build/City-Shrouded-in-Shadow-Portable')
    parser.add_argument('--runtime-update', type=Path, action='append', required=True,
                        help='Distributed runtime update ZIP whose runtime and accepted runtimes are accepted')
    parser.add_argument('--translation-update', type=Path, action='append', default=[],
                        help='Distributed translation update ZIP whose runtime is also accepted')
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    output = args.out.resolve()
    if output.exists() or not output.is_relative_to(ROOT/'build'):
        raise ValueError('Choose a new ZIP inside ignored build/')
    source = args.portable.resolve()
    accepted = []
    for path in args.runtime_update:
        with zipfile.ZipFile(path) as archive:
            update = json.loads(archive.read('Atualizacao-Kyoei/update.json'))
        if (update['title'], update['version']) != ('CUSA03562', '01.03'):
            raise ValueError('Runtime update for another game')
        accepted += [h for h in (update['sha256'], *update['accepted_sha256']) if h not in accepted]
    for path in args.translation_update:
        with zipfile.ZipFile(path) as archive:
            update = json.loads(archive.read(PREFIX+'translation-update.json'))
        if (update['title'], update['version']) != ('CUSA03562', '01.03'):
            raise ValueError('Translation update for another game')
        runtime = next(row['sha256'] for row in update['files'] if row['path'] == 'runtime/kyoei.exe')
        accepted += [h for h in (runtime, *update['accepted_runtime_sha256']) if h not in accepted]
    if (source/'game/DAT/common.psarc').stat().st_size != DISC_ARCHIVE_BYTES:
        raise ValueError('Portable disc archive differs from the original')
    rows = []
    for name in FILES:
        path = source/name
        hits, _ = personal_text_hits(path.read_bytes(), [b'davyd', b'codex', b'claude', b'chatgpt', b'kyoei-toshi-work'])
        if hits:
            raise ValueError('Personal metadata in payload: '+name)
        rows.append(dict(path=name, bytes=path.stat().st_size, sha256=digest(path)))
    metadata = dict(schema=2, release=RELEASE, title='CUSA03562', version='01.03',
                    disc_archive_bytes=DISC_ARCHIVE_BYTES, accepted_runtime_sha256=accepted,
                    files=rows, remove=list(REMOVE))
    with zipfile.ZipFile(output, 'x', compression=zipfile.ZIP_DEFLATED, compresslevel=9) as archive:
        for row in rows:
            archive.write(source/row['path'], PREFIX+'payload/'+row['path'])
        # BOM: Windows PowerShell 5.1 reads BOM-less scripts as ANSI and garbles accents.
        archive.writestr(PREFIX+'apply_translation_update.ps1',
                         (ROOT/'scripts/apply_translation_update.ps1').read_text(encoding='utf8').encode('utf-8-sig'))
        for name in ('apply_translation_update.py', 'apply_runtime_update_linux.py'):
            archive.writestr(PREFIX+name, (ROOT/'tools'/name).read_text(encoding='utf8').replace('\r\n', '\n'))
        archive.writestr(PREFIX+'translation-update.json', json.dumps(metadata, indent=2)+'\n')
        archive.writestr(PREFIX+'LEIA-ME.txt', README.encode('utf-8-sig'))
        archive.writestr(PREFIX+'Aplicar-no-Linux.sh',
                         '#!/bin/sh\nset -eu\ncd "$(dirname "$0")"\nexec python3 ./apply_translation_update.py "$@"\n')
        archive.writestr(PREFIX+'Aplicar traducao.cmd',
                         '@echo off\r\npowershell -NoProfile -ExecutionPolicy Bypass -File '
                         '"%~dp0apply_translation_update.ps1" -GamePath "%~dp0.."\r\npause\r\n')
    report = dict(zip=output.name, bytes=output.stat().st_size, sha256=digest(output), **metadata)
    output.with_suffix('.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf-8')
    print(json.dumps(dict(zip=output.name, bytes=report['bytes'], sha256=report['sha256']), indent=2))


if __name__ == '__main__':
    main()
