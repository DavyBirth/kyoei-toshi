"""Prepare the owned game from explicit paths; no private inventory is required.

Uses the fixed Godzilla preparer without editing it. No game/firmware is
downloaded and no guest instructions are executed.
"""
import argparse
import types
import importlib
import json
from pathlib import Path
import sys
from native_module_metadata import records,write_tls_sites
from native_module_metadata import normalize_imports

ROOT=Path(__file__).resolve().parents[1]
TITLE='CUSA03562'
VERSION='01.03'
IMAGE_SHA256='2ae0f7932c9e6f8c2f16b2d1a92a604cdfa707bb0412b045659fb5a461833c1f'
CORE_COMMIT='e976b59a3f056c5ea5d0ddfe1c5ef566cce2f45e'
STAGE_NAMES=tuple(f'stage{n}' for n in range(201,219) if n!=217)+('stage210_2','stage210_3')


def stage_paths(game):
    paths=tuple(f'module/MasterORBIS/{name}/StageModule.prx' for name in sorted(STAGE_NAMES))
    missing=[name for name in paths if not (Path(game)/name).is_file()]
    if missing:raise ValueError('Missing stage modules: '+', '.join(missing))
    return paths


def validate_prepared_stages(out):
    """Reject old stage201-only images before copying them to an installation."""
    rows=(Path(out)/'deferred.tsv').read_text(encoding='utf8').splitlines()
    found=[row.split('\t')[1] for row in rows if row.startswith('M\t') and 'MasterORBIS/' in row]
    expected={'/app0/module/MasterORBIS/'+name+'/StageModule.prx' for name in STAGE_NAMES}
    if set(found)!=expected or len(found)!=len(expected):
        raise ValueError('Prepared image lacks the complete stage set; rebuild with tools/prepare_portable_image.py')
    return len(found)

def validate_output(game,firmware,out):
    game,firmware,out=(Path(p).resolve() for p in (game,firmware,out))
    if out.exists() or any(out==p or out.is_relative_to(p) for p in (game,firmware)):
        raise ValueError('Choose a new output outside the game and firmware folders')
    if out.is_relative_to(ROOT) and out.relative_to(ROOT).parts[0] not in ('build','runtime','data'):
        raise ValueError('Generated data inside the checkout must stay under build/, runtime/ or data/')
    return game,firmware,out

def stage_linker(linker):
    """Fixed-core linker adapted for one StageModule.prx per stage. Every stage exports
    the same names; the game finds them per module (deferred.tsv E records), so later
    duplicates stay out of the global import table instead of stopping the link."""
    source=Path(linker.__file__).read_text(encoding='utf8')
    old=("                if s['identity'] in exports:\n"
         "                    raise ValueError(f'{filename}: ambiguous export')\n")
    new=("                if s['identity'] in exports:\n"
         "                    if Path(filename).name == 'StageModule.prx' and Path(exports[s['identity']][2]).name == 'StageModule.prx': continue\n"
         "                    raise ValueError(f'{filename}: ambiguous export')\n")
    if source.count(old)!=1:raise ValueError('Fixed core linker differs; stage modules not adapted')
    adapted=types.ModuleType('kyoei_link_modules');adapted.__file__=linker.__file__
    exec(compile(source.replace(old,new),linker.__file__,'exec'),adapted.__dict__)
    return adapted

def prepare_image(game,firmware,out,core_scripts=None):
    game,firmware,out=validate_output(game,firmware,out)
    scripts=Path(core_scripts or ROOT/'third_party/godzilla-core/scripts').resolve()
    for name in ('prepare.py','link_modules.py','game_check.py'):
        if not (scripts/name).is_file():raise ValueError('Missing fixed core preparation tools')
    sys.path.insert(0,str(scripts))
    prepare=importlib.import_module('prepare');linker=stage_linker(importlib.import_module('link_modules'))
    original_check=importlib.import_module('game_check')
    info=prepare.sfo((game/'sce_sys/param.sfo').read_bytes())
    if (info.get('TITLE_ID'),info.get('APP_VER'))!=(TITLE,VERSION):
        raise ValueError('Only Kyoei Toshi CUSA03562 version 01.03 is supported')
    image_hash=original_check.image_sha256(game)
    if image_hash!=IMAGE_SHA256:raise ValueError('The loaded executable image differs from the supported game')
    # Every stage loads its own StageModule.prx when the story reaches it.
    deferred=('module/prx/libSceSecure.prx',)+stage_paths(game)
    bundled=list(linker.DEFAULT_MODULES)+['libSceNpToolkit2.prx']
    required=[game/'sce_module'/name for name in bundled]+[game/p for p in deferred]
    required += [firmware/name for name in linker.FIRMWARE_MODULES]
    missing=[p.name for p in required if not p.is_file()]
    if missing:raise ValueError('Required modules are missing: '+', '.join(missing))
    class KyoeiCheck:
        @staticmethod
        def problem(source,loaded_hash):
            if Path(source).resolve()!=game or loaded_hash!=IMAGE_SHA256:
                raise ValueError('Unexpected game executable')
            return None
    previous_check=prepare.game_check
    try:
        prepare.game_check=KyoeiCheck
        prepare.prepare(game,out)
    finally:prepare.game_check=previous_check
    firmware_modules=[str(firmware/name) for name in linker.FIRMWARE_MODULES]
    linker.link(game,out,bundled+firmware_modules+[str(game/p) for p in deferred])
    linked=json.loads((out/'link.json').read_text(encoding='utf8'));metadata=[]
    for relative in deferred:
        path=game/relative
        record=next(r for r in linked['modules'] if Path(r['file']).resolve()==path)
        base=int(record['base'],16)
        metadata.append(f'M\t/app0/{relative}\t{base:x}\t{int(record["init"],16):x}')
        module=linker.module(path);metadata.extend(records(base,module))
        for symbol in module['symbols']:
            if symbol['section'] and symbol['binding'] in (1,2) and symbol['identity']:
                metadata.append(f'E\t{symbol["identity"][0]}\t{base+symbol["value"]:x}')
    (out/'deferred.tsv').write_text('\n'.join(metadata)+'\n',encoding='utf8')
    stage_count=validate_prepared_stages(out)
    write_tls_sites(out);unnamed=normalize_imports(out)
    proof=dict(title=TITLE,version=VERSION,image_sha256=image_hash,core_commit=CORE_COMMIT,
               unnamed_imports_normalized=unnamed,shared_sources_modified=False,guest_executed=False,
               stage_modules=stage_count)
    (out/'prepared.json').write_text(json.dumps(proof,indent=2)+'\n',encoding='utf8')
    return proof

def main():
    sys.stdout.reconfigure(encoding='utf8')
    sys.stderr.reconfigure(encoding='utf8')
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--game',type=Path,required=True)
    parser.add_argument('--firmware',type=Path,required=True)
    parser.add_argument('--out',type=Path,required=True)
    parser.add_argument('--core-scripts',type=Path)
    args=parser.parse_args()
    print(json.dumps(prepare_image(args.game,args.firmware,args.out,args.core_scripts)),flush=True)

if __name__=='__main__':main()
