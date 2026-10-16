"""Build a Windows runtime-only update; never package game data or saves."""
import argparse
import hashlib
import json
from pathlib import Path
import tempfile
import zipfile

from build_portable import ROOT, clean_pe, digest


def accepted_update_hash(path, title, version):
    with zipfile.ZipFile(path) as archive:
        metadata = json.loads(archive.read('Atualizacao-Kyoei/update.json'))
        if (metadata['schema'],metadata['platform'],metadata['title'],metadata['version']) != (1,'windows-x64',title,version):
            raise ValueError('Incompatible previous update')
        payload = archive.read('Atualizacao-Kyoei/kyoei.exe')
        actual = hashlib.sha256(payload).hexdigest()
        if actual != metadata['sha256']:
            raise ValueError('Previous update payload differs from its manifest')
        return actual


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    parser.add_argument('--linux-wine', action='store_true', help='Linux applicator for the shipped Wine/Proton version')
    parser.add_argument('--accept-update', type=Path, action='append', default=[], help='Previously distributed ZIP whose installed runtime is also accepted')
    args = parser.parse_args()
    baseline = args.baseline.resolve()
    manifest = json.loads((baseline/'manifest.json').read_text(encoding='utf8'))
    if (manifest['title'], manifest['version']) != ('CUSA03562', '01.03'):
        raise ValueError('Wrong game revision')
    previous_hash = digest(baseline/'runtime/kyoei.exe')
    entry = next(row for row in manifest['files'] if row['path']=='runtime/kyoei.exe')
    if entry['sha256'] != previous_hash:
        raise ValueError('Baseline runtime differs from the distributed manifest')
    accepted = [previous_hash]
    for previous in args.accept_update:
        value = accepted_update_hash(previous, manifest['title'], manifest['version'])
        if value not in accepted:
            accepted.append(value)
    output = args.out.resolve()
    if not output.is_relative_to(ROOT/'build') or output.exists():
        raise ValueError('Choose a new ZIP under ignored build/')
    with tempfile.TemporaryDirectory(dir=ROOT/'build') as temporary:
        runtime = Path(temporary)/'kyoei.exe'
        clean_pe(ROOT/'build/kyoei.exe', runtime)
        metadata = dict(schema=1, platform='windows-x64', title=manifest['title'],
                        version=manifest['version'], sha256=digest(runtime),
                        accepted_sha256=accepted)
        readme = ('WINDOWS x64 — atualização somente do runtime\n\n'
                  '1. Feche o jogo e o launcher.\n'
                  '2. Extraia este ZIP dentro da pasta onde está City Shrouded in Shadow.exe.\n'
                  '3. Abra Atualizacao-Kyoei e execute Aplicar atualizacao.cmd.\n'
                  '4. Inicie o jogo pelo launcher normal.\n\n'
                  'O aplicador confere hashes e guarda o runtime anterior em data/update-backups.\n'
                  'Não altera game/, languages/, saves ou cache. Não é uma versão Linux.\n'
                  'Esta atualização inclui melhorias de diagnóstico; ganho de FPS não comprovado.\n'
                  'Para desfazer: com jogo/launcher fechados, copie o backup indicado sobre\n'
                  'runtime/kyoei.exe. Preserve o backup.\n')
        if args.linux_wine:
            readme = ('LINUX x86-64 / WINE OU PROTON — atualização do pacote já distribuído\n\n'
                      '1. Feche o jogo e o launcher; mantenha-os fechados durante a atualização.\n'
                      '2. Extraia este ZIP dentro da pasta do jogo.\n'
                      '3. Abra um terminal nessa pasta e execute:\n'
                      '   sh Atualizacao-Kyoei/Aplicar-no-Linux.sh\n'
                      '4. Inicie pelo Start on Linux.sh habitual.\n\n'
                      'Requer Python 3.11 ou mais recente. Não use sudo.\n'
                      'Verifica hashes e guarda backup em data/update-backups.\n'
                      'Preserva saves, arquivos do jogo, cache e prefixo Wine.\n'
                      'É o runtime Windows usado pelo Wine/Proton; não é um port Linux nativo.\n'
                      'Ganho de FPS ainda não comprovado. Para desfazer: com jogo/launcher\n'
                      'fechados, copie o backup indicado sobre runtime/kyoei.exe.\n')
        command = ('@echo off\r\npowershell -NoProfile -ExecutionPolicy Bypass -File '
                   '"%~dp0apply_runtime_update.ps1" -GamePath "%~dp0.."\r\npause\r\n')
        readme += ('\nMudanças desta revisão: menos consultas GPU quando não há callbacks pendentes;\n'
                   'diagnóstico limitado para investigar imagens de 1x1 e esperas longas.\n'
                   'Otimização experimental: não há ganho de FPS comprovado.\n'
                   'Aceita a distribuição original e os updates anteriores listados no manifest.\n'
                   'Se já existe Atualizacao-Kyoei, substitua seus arquivos ao extrair.\n'
                   'O pacote troca apenas o runtime; não inclui mudanças nos arquivos do jogo.\n')
        prefix = 'Atualizacao-Kyoei/'
        with zipfile.ZipFile(output, 'w', compression=zipfile.ZIP_DEFLATED) as archive:
            archive.write(runtime, prefix+'kyoei.exe')
            if args.linux_wine:
                archive.writestr(prefix+'apply_runtime_update_linux.py',
                    (ROOT/'tools/apply_runtime_update_linux.py').read_text(encoding='utf8').replace('\r\n','\n'))
                archive.writestr(prefix+'Aplicar-no-Linux.sh',
                    '#!/bin/sh\nset -eu\ncd "$(dirname "$0")"\nexec python3 ./apply_runtime_update_linux.py "$@"\n')
            else:
                archive.write(ROOT/'scripts/apply_runtime_update.ps1', prefix+'apply_runtime_update.ps1')
            archive.writestr(prefix+'update.json', json.dumps(metadata, indent=2)+'\n')
            archive.writestr(prefix+'LEIA-ME.txt', readme.encode('utf-8-sig'))
            if not args.linux_wine:
                archive.writestr(prefix+'Aplicar atualizacao.cmd', command)
        print(json.dumps(dict(zip=str(output), bytes=output.stat().st_size, **metadata), indent=2))


if __name__ == '__main__':
    main()
