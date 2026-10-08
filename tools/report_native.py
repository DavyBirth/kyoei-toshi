"""Registra a prova nativa e verifica que a base fixa permaneceu intacta."""
import hashlib
import json
import argparse
import re
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]


def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--current', action='store_true')
    args = parser.parse_args()
    if args.current:
        prepare = json.loads((REPO/'docs/native-prepare.json').read_text())
        out = Path(prepare['prepared_out'])
        log = max((out.parent/'logs').glob('20*.log'), key=lambda p: p.name)
        text = log.read_text(encoding='utf8')
        stop = re.findall(r'^STOP: (.+)$', text, re.MULTILINE)
        codes = re.findall(r'^Exit code (\d+)', text, re.MULTILINE)
        report = dict(core_commit=prepare['core_commit'], base_integrity_reference='native-proof.json',
                      executable_sha256=digest(REPO/'build/kyoei.exe'),
                      log=str(log), log_sha256=digest(log),
                      exit_code=int(codes[-1]) if codes else None,
                      latest_blocker=stop[-1] if stop else None,
                      main_tls_configured='Kyoei TLS: 40 bytes per thread' in text,
                      graphics_window_created='GPU: SDL window created' in text,
                      eboot_entered="Entering the game's code" in text,
                      rendered_scene_verified=False, game_ready=False)
        (REPO/'docs/native-current.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf8')
        print(json.dumps(report, indent=2))
        return
    if (REPO/'docs/native-proof.json').exists():
        raise ValueError('Prova histórica já existe; use --current para a versão atual')
    snapshot = json.loads((REPO/'docs/godzilla-core.json').read_text())
    core = REPO/'third_party/godzilla-core'
    count = 0
    for section in ('core', 'shadPS4', 'bloodborne_pc', 'prebuilt'):
        for row in snapshot[section]['files']:
            if digest(core/row['path']) != row['sha256']:
                raise ValueError('Base fixa alterada: '+row['path'])
            count += 1
    prepare = json.loads((REPO/'docs/native-prepare.json').read_text())
    output = Path(prepare['prepared_out'])
    link = json.loads((output/'link.json').read_text())
    log = output.parent/'logs/boot-01.log'
    text = log.read_text(encoding='utf8')
    reason = 'this eboot has static TLS; the Windows loader does not support it yet'
    if reason not in text:
        raise ValueError('O log não contém o bloqueio observado')
    report = dict(core_commit=snapshot['core']['commit'], fixed_files_verified=count,
                  executable=str(REPO/'build/kyoei.exe'), executable_sha256=digest(REPO/'build/kyoei.exe'),
                  boot_image_sha256=digest(output/'boot-linked.bin'),
                  imports=link['imports'], native_bindings=link['bindings'], main_tls=link['main_tls'],
                  log=str(log), log_sha256=digest(log), first_execution_exit_code=1,
                  first_execution_blocker=reason, game_window_opened=False,
                  shared_sources_modified=False, graphics_library_reused=True)
    (REPO/'docs/native-proof.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf8')
    print(json.dumps(report, indent=2))


if __name__ == '__main__':
    main()
