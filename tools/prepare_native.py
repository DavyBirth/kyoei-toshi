"""Prepara o Kyoei com o preparador fixo do Godzilla e validação própria M0."""
import argparse
import hashlib
import importlib
import json
from pathlib import Path
import sys

REPO = Path(__file__).resolve().parents[1]
CORE = REPO/'third_party/godzilla-core'


def main():
    sys.stdout.reconfigure(encoding='utf8')
    sys.stderr.reconfigure(encoding='utf8')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=Path('G:/kyoei-toshi-work/native-02/out'))
    parser.add_argument('--firmware', type=Path, default=Path('G:/GodzillaShadPS4PC/user/sys_modules'))
    args = parser.parse_args()
    out = args.out.resolve()
    inventory = json.loads((REPO/'docs/m0/inventory.json').read_text(encoding='utf8'))
    game = Path(inventory['source']).resolve()
    if out.exists() or out.is_relative_to(REPO) or out.is_relative_to(game):
        raise ValueError('Saída deve ser nova e ficar fora do dump e do repositório')
    sys.path.insert(0, str(CORE/'scripts'))
    prepare = importlib.import_module('prepare')
    linker = importlib.import_module('link_modules')
    expected = {r['path']: r['sha256'] for r in inventory['files']}
    for name in ['eboot.bin', 'sce_sys/param.sfo'] + ['sce_module/'+n for n in linker.DEFAULT_MODULES]:
        with (game/name).open('rb') as stream:
            if hashlib.file_digest(stream, 'sha256').hexdigest() != expected[name]:
                raise ValueError('Arquivo executável diferente do M0: '+name)
    info = prepare.sfo((game/'sce_sys/param.sfo').read_bytes())
    if info.get('TITLE_ID') != 'CUSA03562' or info.get('APP_VER') != '01.03':
        raise ValueError('Somente o Kyoei CUSA03562 01.03 inventariado é aceito')
    image_hash = prepare.game_check.image_sha256(game)

    class KyoeiCheck:
        @staticmethod
        def problem(source, loaded_hash):
            if Path(source).resolve() != game or loaded_hash != image_hash:
                raise ValueError('Imagem carregada não corresponde ao Kyoei validado')
            return None

    # Application-specific policy adapter; the fixed source on disk is untouched.
    prepare.game_check = KyoeiCheck
    prepare.prepare(game, out)
    firmware = [str(args.firmware/name) for name in linker.FIRMWARE_MODULES]
    linker.link(game, out, list(linker.DEFAULT_MODULES)+firmware)
    report = dict(source=str(game), title='CUSA03562', version='01.03',
                  image_sha256=image_hash, core_commit=json.loads((REPO/'docs/godzilla-core.json').read_text())['core']['commit'],
                  prepared_out=str(out), shared_sources_modified=False)
    (REPO/'docs/native-prepare.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf8')
    print('KYOEI PREPARADO:', out)


if __name__ == '__main__':
    main()
