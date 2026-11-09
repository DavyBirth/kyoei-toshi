"""Build local game-dependent files for a portable application from owned inputs.

No downloads, private M0/M2 workspace, SDK or guest execution. Everything
derived from the game is created locally and excluded from the public payload.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import uuid

ROOT=Path(__file__).resolve().parents[1]

def digest(path):
    with path.open('rb') as stream:return hashlib.file_digest(stream,'sha256').hexdigest()

def step(portuguese,english):print('SETUP: '+portuguese+'|'+english,flush=True)

def run(tool,*args):
    subprocess.run([sys.executable,'-X','utf8','-u',str(ROOT/'tools'/tool),*[str(v) for v in args]],cwd=ROOT,check=True)

def copy_owned_game(source,target,metadata,link_for_check=False):
    source,target=Path(source).resolve(),Path(target).resolve()
    if target.exists():raise ValueError('Game destination must be new')
    if source==target or target.is_relative_to(source):raise ValueError('Never write into the input dump')
    if link_for_check and not source.is_relative_to(ROOT/'build'):
        marker=source.parent/'manifest.json'
        identity=json.loads(marker.read_text(encoding='utf8')) if marker.is_file() else {}
        if identity.get('format')!='kyoei-private-portable-v1' or identity.get('title')!='CUSA03562':
            raise ValueError('Developer hardlink checks require an existing private portable copy, never the input disc dump')
    for row in metadata['files']:
        relative=Path(row['path'])
        if relative.is_absolute() or '..' in relative.parts:raise ValueError('Invalid compatibility path')
        path=source/relative
        if not path.is_file() or path.stat().st_size!=row['bytes'] or digest(path)!=row['sha256']:
            raise ValueError('Owned dump differs from supported CUSA03562 01.03: '+relative.as_posix())
    target.mkdir(parents=True)
    for row in metadata['files']:
        relative=Path(row['path']);before=source/relative;after=target/relative
        after.parent.mkdir(parents=True,exist_ok=True)
        if link_for_check:os.link(before,after)
        else:shutil.copyfile(before,after)
        if digest(after)!=row['sha256']:raise ValueError('Game copy differs: '+relative.as_posix())

def texture_sources(table):
    result={item['resource'] for item in table['textures']}
    for item in table['textures']:
        for region in item.get('regions',[]):
            if isinstance(region.get('plate'),str):result.add(region['plate'])
            for key in ('graft','paste'):
                if region.get(key,{}).get('res'):result.add(region[key]['res'])
    return result

def commit(stage,root,assets,proof,compatibility):
    # Move results into the application; on any failure undo exactly what was
    # added here so a later attempt starts clean. Saves and settings are never touched.
    added,previous=[],{}
    def keep(path):
        if path.is_file():previous[path]=path.read_bytes()
        added.append(path);return path
    try:
        root.mkdir(parents=True,exist_ok=True)
        (root/'runtime').mkdir(exist_ok=True)
        (stage/'game').replace(keep(root/'game'))
        (root/'languages').mkdir(exist_ok=True)
        for language in ('en','pt-BR','ja'):
            target=keep(root/'languages'/language);target.mkdir()
            for path in (stage/'languages'/language).iterdir():
                if path.suffix in ('.kydelta','.kypack','.kytext','.kyart'):path.replace(target/path.name)
        runtime_files=('boot-linked.bin','imports.tsv','deferred.tsv','tls-sites.tsv','prepared.json')
        keep(root/'runtime/out').mkdir()
        for name in runtime_files:(stage/'runtime/out'/name).replace(root/'runtime/out'/name)
        (root/'assets').mkdir(exist_ok=True)
        for path in assets.iterdir():path.replace(keep(root/'assets'/path.name))
        proof.update(languages=['en','pt-BR','ja'],owned_game_files=len(compatibility['files']),
                     owned_game_verified=True,private_derivatives_generated=True)
        keep(root/'setup-manifest.json').write_text(json.dumps(proof,indent=2)+'\n',encoding='utf8')
        records=[dict(row,path='game/'+row['path']) for row in compatibility['files']]
        for folder in ('runtime','languages','assets'):
            for path in (root/folder).rglob('*'):
                if path.is_file():records.append(dict(path=path.relative_to(root).as_posix(),bytes=path.stat().st_size,sha256=digest(path)))
        keep(root/'manifest.json').write_text(json.dumps(dict(format='kyoei-private-portable-v1',title='CUSA03562',version='01.03',
            pc_version='1.0',languages=['ja','en','pt-BR'],files=records,
            privacy_scan_passed=False,personal_saves_copied=False),indent=2)+'\n',encoding='utf8')
        launcher=root/'City Shrouded in Shadow.exe'
        if launcher.is_file():subprocess.run([str(launcher),'--check'],cwd=root,check=True)
    except BaseException:
        for path in reversed(added):
            if path.is_dir():shutil.rmtree(path,ignore_errors=True)
            elif path.exists():path.unlink()
            if path in previous:path.write_bytes(previous[path])
        raise

def bootstrap(game,firmware,root,link_for_check=False):
    from psarc import Psarc,safe_name
    from prepare_portable_image import prepare_image
    from prepare_hud_metrics import native_metrics
    from make_ui_art import build as ui_art,write as write_ui_art
    from PIL import Image
    game,firmware,root=(Path(p).resolve() for p in (game,firmware,root))
    if any(root==p or root.is_relative_to(p) for p in (game,firmware)):
        raise ValueError('Choose an application folder outside your dump and firmware')
    if root!=ROOT and not root.is_relative_to(ROOT/'build'):
        raise ValueError('Development output must be under build/; installed applications use their own folder')
    for folder in ('game','runtime/out','languages/en','languages/pt-BR','languages/ja'):
        if (root/folder).exists():raise ValueError('Use a fresh application folder; existing game/profile data is preserved')
    compatibility=json.loads((ROOT/'docs/release/supported-game.json').read_text(encoding='utf8'))
    stage=ROOT/'build'/('setup-'+uuid.uuid4().hex);stage.mkdir(parents=True)
    try:
        proof=dict(format='kyoei-private-setup-v1',title='CUSA03562',version='01.03',guest_executed=False)
        (stage/'setup-state.json').write_text(json.dumps(proof)+'\n',encoding='utf8')
        step('Preparando o executável com os módulos do seu PS4','Preparing the executable with your PS4 modules')
        prepare_image(game,firmware,stage/'runtime/out')
        step('Conferindo e preparando a cópia do seu jogo','Checking and preparing your game copy')
        copy_owned_game(game,stage/'game',compatibility,link_for_check)
        base=stage/'game/DAT/common.psarc'
        step('Preparando fontes e recursos da interface','Preparing fonts and interface resources')
        table=json.loads((ROOT/'translations/textures-en.json').read_text(encoding='utf8'))
        assets=stage/'assets';assets.mkdir()
        with Psarc(base) as archive:
            entries={e.name:e for e in archive.entries[1:]}
            needed=texture_sources(table)
            for name in needed:
                safe_name(name);target=stage/'source/common'/name
                target.parent.mkdir(parents=True,exist_ok=True)
                target.write_bytes(b''.join(archive.chunks(entries[name])))
            tables=[b''.join(archive.chunks(entries[name])) for name in ('font/utf8all_table.csh','font/utf8all_IM_table.csh')]
            (assets/'hud-metrics.bin').write_bytes(native_metrics(tables))
        write_ui_art(ui_art(base),assets/'ui-art.kyart')
        icon=stage/'game/sce_sys/icon0.png'
        if icon.is_file():
            with Image.open(icon) as original:
                original.convert('RGBA').save(assets/'game.ico',sizes=[(16,16),(32,32),(48,48),(64,64),(128,128),(256,256)])
                original.convert('RGBA').save(assets/'artwork.png')
        for language in ('en','pt-BR'):
            step('Preparando a interface em '+('português' if language=='pt-BR' else 'inglês'),
                 'Preparing the '+('Portuguese' if language=='pt-BR' else 'English')+' interface')
            textures=stage/('textures-'+language)
            run('localize_textures.py',ROOT/'translations/textures-en.json',textures,
                '--source',stage/'source/common','--language',language)
            drafts=sorted((ROOT/'translations').glob('localization-en*.json'))
            if language=='pt-BR':
                merged=stage/'merged-pt-BR.json'
                run('merge_language_fallback.py',ROOT/'translations/localization-pt-BR.json',merged)
                drafts=[merged]
            delta=stage/'languages'/language/'translation.kydelta'
            run('build_localized_delta.py',base,delta,*drafts,'--language',language,'--textures',textures)
            run('build_eboot_text.py',ROOT/'translations/binary-en.json',stage/'languages'/language/'eboot-text.kytext',
                '--language',language,'--eboot',stage/'game/eboot.bin','--elf',stage/'runtime/out/eboot.elf')
            run('make_title_exit_art.py',base,stage/'languages'/language,'--language',language)
            step('Preparando os comandos em '+('português' if language=='pt-BR' else 'inglês'),
                 'Preparing '+('Portuguese' if language=='pt-BR' else 'English')+' button prompts')
            run('build_prompt_pack.py',base,stage/'languages'/language/'prompts.kypack','--delta',delta,
                '--diagrams','--legend-diagrams','--language',language)
        step('Preparando os comandos do idioma original','Preparing original-language button prompts')
        run('build_prompt_pack.py',base,stage/'languages/ja/prompts.kypack','--language','ja')
        shutil.copyfile(stage/'languages/en/title-exit.kyart',assets/'title-exit.kyart')
        # Keep the existing fonts from the application payload; development checks
        # can obtain the identical OFL fonts from the fixed core snapshot.
        for name in ('NotoSans-Regular.ttf','NotoSansCJK-Regular.ttc'):
            target=root/'assets/fonts'/name
            if not target.exists():
                source=ROOT/'assets/fonts'/name
                if not source.exists():source=ROOT/'third_party/godzilla-core/third_party/shadPS4/src/imgui/renderer/fonts'/name
                target.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(source,target)
        step('Conferindo e concluindo a preparação','Checking and finishing')
        commit(stage,root,assets,proof,compatibility)
    finally:
        # Only this run's generated staging folder, never the dump or a profile.
        if stage.parent.resolve()==ROOT/'build' and (stage/'setup-state.json').is_file():
            shutil.rmtree(stage,ignore_errors=True)
    step('Pronto. Seus saves e configurações ficam nesta pasta.','Done. Your saves and settings stay in this folder.')
    return proof

def main():
    sys.stdout.reconfigure(encoding='utf8');sys.stderr.reconfigure(encoding='utf8')
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--game',type=Path,required=True);p.add_argument('--firmware',type=Path,required=True)
    p.add_argument('--root',type=Path,default=ROOT)
    p.add_argument('--link-owned-data-for-check',action='store_true',help='Private developer check only; never links the input disc dump')
    a=p.parse_args()
    try:print(json.dumps(bootstrap(a.game,a.firmware,a.root,a.link_owned_data_for_check)),flush=True)
    except Exception as error:
        step('O preparo falhou e nada foi alterado: '+str(error),'Preparation failed and nothing was changed: '+str(error))
        raise

if __name__=='__main__':main()
