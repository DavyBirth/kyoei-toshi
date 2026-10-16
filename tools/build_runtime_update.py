"""Build a Windows runtime-only update; never package game data or saves."""
import argparse
import json
from pathlib import Path
import tempfile
import zipfile

from build_portable import ROOT, clean_pe, digest


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--baseline', type=Path, required=True)
    parser.add_argument('--out', type=Path, required=True)
    args = parser.parse_args()
    baseline = args.baseline.resolve()
    manifest = json.loads((baseline/'manifest.json').read_text(encoding='utf8'))
    if (manifest['title'], manifest['version']) != ('CUSA03562', '01.03'):
        raise ValueError('Wrong game revision')
    previous_hash = digest(baseline/'runtime/kyoei.exe')
    entry = next(row for row in manifest['files'] if row['path']=='runtime/kyoei.exe')
    if entry['sha256'] != previous_hash:
        raise ValueError('Baseline runtime differs from the distributed manifest')
    output = args.out.resolve()
    if not output.is_relative_to(ROOT/'build') or output.exists():
        raise ValueError('Choose a new ZIP under ignored build/')
    with tempfile.TemporaryDirectory(dir=ROOT/'build') as temporary:
        runtime = Path(temporary)/'kyoei.exe'
        clean_pe(ROOT/'build/kyoei.exe', runtime)
        metadata = dict(schema=1, platform='windows-x64', title=manifest['title'],
                        version=manifest['version'], sha256=digest(runtime),
                        accepted_sha256=[previous_hash])
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
        command = ('@echo off\r\npowershell -NoProfile -ExecutionPolicy Bypass -File '
                   '"%~dp0apply_runtime_update.ps1" -GamePath "%~dp0.."\r\npause\r\n')
        prefix = 'Atualizacao-Kyoei/'
        with zipfile.ZipFile(output, 'w', compression=zipfile.ZIP_DEFLATED) as archive:
            archive.write(runtime, prefix+'kyoei.exe')
            archive.write(ROOT/'scripts/apply_runtime_update.ps1', prefix+'apply_runtime_update.ps1')
            archive.writestr(prefix+'update.json', json.dumps(metadata, indent=2)+'\n')
            archive.writestr(prefix+'LEIA-ME.txt', readme.encode('utf-8-sig'))
            archive.writestr(prefix+'Aplicar atualizacao.cmd', command)
        print(json.dumps(dict(zip=str(output), bytes=output.stat().st_size, **metadata), indent=2))


if __name__ == '__main__':
    main()
