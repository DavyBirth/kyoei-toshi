"""Prepara o Kyoei com o preparador fixo do Godzilla e validação própria M0."""
import argparse
import hashlib
import importlib
import json
from pathlib import Path
import sys
from native_module_metadata import records

REPO = Path(__file__).resolve().parents[1]
CORE = REPO/'third_party/godzilla-core'


def normalize_imports(out):
    path = out/'imports.tsv'
    rows = []
    unnamed = []
    for index, line in enumerate(path.read_text(encoding='utf8').splitlines()):
        fields = line.split('\t')
        if len(fields) != 4 or fields[0] != str(index) or not fields[1] or not fields[2]:
            raise ValueError('Tabela de importações inválida')
        if not fields[3]:
            fields[3] = '?'
            unnamed.append(index)
        if fields[1].split('#')[0] == 'fJgP+wqifno' and fields[2] == 'libSceDiscMap':
            fields[3] = 'sceDiscMap_7C980FFB0AA27E7A'
        rows.append('\t'.join(fields))
    path.write_text('\n'.join(rows)+'\n', encoding='utf8')
    return unnamed


def main():
    sys.stdout.reconfigure(encoding='utf8')
    sys.stderr.reconfigure(encoding='utf8')
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--out', type=Path, default=Path('G:/kyoei-toshi-work/native-04/out'))
    parser.add_argument('--firmware', type=Path, default=Path('G:/GodzillaShadPS4PC/user/sys_modules'))
    parser.add_argument('--repair-imports', action='store_true', help='normalize only the already prepared local TSV')
    args = parser.parse_args()
    out = args.out.resolve()
    if args.repair_imports:
        proof = json.loads((REPO/'docs/native-prepare.json').read_text(encoding='utf8'))
        if out != Path(proof['prepared_out']).resolve() or out.is_relative_to(REPO):
            raise ValueError('Reparo limitado à saída preparada registrada')
        print('Importações sem nome normalizadas:', normalize_imports(out))
        return
    inventory = json.loads((REPO/'docs/m0/inventory.json').read_text(encoding='utf8'))
    game = Path(inventory['source']).resolve()
    if out.exists() or out.is_relative_to(REPO) or out.is_relative_to(game):
        raise ValueError('Saída deve ser nova e ficar fora do dump e do repositório')
    sys.path.insert(0, str(CORE/'scripts'))
    prepare = importlib.import_module('prepare')
    linker = importlib.import_module('link_modules')
    expected = {r['path']: r['sha256'] for r in inventory['files']}
    deferred = ['module/prx/libSceSecure.prx',
                'module/MasterORBIS/stage201/StageModule.prx']
    bundled = list(linker.DEFAULT_MODULES) + ['libSceNpToolkit2.prx']
    for name in ['eboot.bin', 'sce_sys/param.sfo'] + ['sce_module/'+n for n in bundled] + deferred:
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
    linker.link(game, out, bundled+firmware+[str(game/p) for p in deferred])
    linked = json.loads((out/'link.json').read_text(encoding='utf8'))
    metadata = []
    for relative in deferred:
        module_path = game/relative
        record = next(r for r in linked['modules'] if Path(r['file']) == module_path)
        base = int(record['base'], 16)
        metadata.append(f'M\t/app0/{relative}\t{base:x}\t{int(record["init"],16):x}')
        module = linker.module(module_path)
        metadata.extend(records(base, module))
        for symbol in module['symbols']:
            if symbol['section'] and symbol['binding'] in (1,2) and symbol['identity']:
                metadata.append(f'E\t{symbol["identity"][0]}\t{base+symbol["value"]:x}')
    (out/'deferred.tsv').write_text('\n'.join(metadata)+'\n', encoding='utf8')
    unnamed = normalize_imports(out)
    report = dict(source=str(game), title='CUSA03562', version='01.03',
                  image_sha256=image_hash, core_commit=json.loads((REPO/'docs/godzilla-core.json').read_text())['core']['commit'],
                  prepared_out=str(out), unnamed_imports_normalized=unnamed, shared_sources_modified=False)
    (REPO/'docs/native-prepare.json').write_text(json.dumps(report, indent=2)+'\n', encoding='utf8')
    print('KYOEI PREPARADO:', out)


if __name__ == '__main__':
    main()
