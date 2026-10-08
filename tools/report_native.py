"""Registra a prova nativa e verifica que a base fixa permaneceu intacta."""
import hashlib
import json
from pathlib import Path

REPO = Path(__file__).resolve().parents[1]


def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def main():
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
