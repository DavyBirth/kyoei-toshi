"""Assemble a review candidate from public inputs only; never copy a game installation.

Builds a neutral launcher (no disc icon), cleans runtime debug metadata and
bundles Python plus Pillow/NumPy. No game, firmware or generated patch is read.
Publication still requires corresponding source and the visual release checks.
"""
import argparse
import ast
import hashlib
import importlib.util
import json
import os
from pathlib import Path
import shutil
import subprocess
import sys
import zipfile
from build_portable import clean_pe

ROOT=Path(__file__).resolve().parents[1]
SEEDS=('bootstrap_portable','localize_textures','merge_language_fallback',
       'build_localized_delta','build_eboot_text','make_title_exit_art','build_prompt_pack')
TRANSLATIONS=('binary-en.json','binary-labels-pt-BR.json','glossary.json',
              'localization-pt-BR.json','texture-labels-pt-BR.json','textures-en.json')
FORBIDDEN={'.psarc','.bin','.prx','.sprx','.elf','.kydelta','.kypack','.kytext','.kyart','.pdb','.pyc'}


def tool_closure(root,seeds=SEEDS):
    pending=list(seeds);found=set()
    while pending:
        name=pending.pop()
        if name in found:continue
        path=root/'tools'/(name+'.py')
        if not path.is_file():raise ValueError('Missing preparation tool: '+name)
        found.add(name)
        tree=ast.parse(path.read_text(encoding='utf8'))
        for node in ast.walk(tree):
            modules=[]
            if isinstance(node,ast.Import):modules=[item.name for item in node.names]
            elif isinstance(node,ast.ImportFrom) and node.module:modules=[node.module]
            for module in modules:
                local=module.split('.')[0]
                if (root/'tools'/(local+'.py')).is_file():pending.append(local)
    return sorted(found)


def audit(folder):
    records=[]
    for path in sorted(folder.rglob('*')):
        if path.is_symlink():raise ValueError('Symlink in public payload')
        if not path.is_file():continue
        relative=path.relative_to(folder).as_posix()
        if relative.split('/')[0] in {'game','languages','data','user','photos','work'} or path.suffix.lower() in FORBIDDEN:
            raise ValueError('Private/generated file in public payload: '+relative)
        if path.name in {'eboot.bin','param.sfo','pyvenv.cfg'}:raise ValueError('Nonportable/private input: '+relative)
        records.append(dict(path=relative,bytes=path.stat().st_size,
                            sha256=hashlib.sha256(path.read_bytes()).hexdigest()))
    return records


def copy(source,target):
    if not source.is_file():raise ValueError('Required public input missing: '+str(source))
    target.parent.mkdir(parents=True,exist_ok=True);shutil.copyfile(source,target)


def copy_tree(source,target):
    if not source.is_dir():raise ValueError('Required public directory missing: '+str(source))
    shutil.copytree(source,target,ignore=shutil.ignore_patterns('__pycache__','*.pyc','*.pdb'))


def python_runtime(target):
    base=Path(sys.base_prefix)
    # Copy the actual interpreter, never a venv redirector or user site configuration.
    for path in base.iterdir():
        if path.is_file() and (path.suffix.lower() in {'.dll','.exe'} or path.name=='LICENSE.txt'):
            copy(path,target/path.name)
    copy_tree(base/'DLLs',target/'DLLs')
    shutil.copytree(base/'Lib',target/'Lib',ignore=shutil.ignore_patterns(
        '__pycache__','*.pyc','site-packages','test','tests','idlelib','tkinter','ensurepip','venv'))
    site=target/'Lib/site-packages';site.mkdir()
    import importlib.metadata
    for name,module in (('Pillow','PIL'),('numpy','numpy')):
        distribution=importlib.metadata.distribution(name)
        for record in distribution.files or []:
            parts=Path(record).parts
            if not parts or '..' in parts or '__pycache__' in parts or record.suffix=='.pyc':continue
            if parts[0] not in {module,module+'.libs'} and not parts[0].endswith('.dist-info'):continue
            copy(Path(distribution.locate_file(record)),site/record)
    version=f'{sys.version_info.major}{sys.version_info.minor}'
    # Isolated CPython search path ignores PYTHONHOME, user site and registry.
    (target/('python'+version+'._pth')).write_text('.\nDLLs\nLib\nLib/site-packages\n../../tools\n',encoding='ascii')


def build(output,dependency_cache,runtime=None):
    dependency_cache=dependency_cache.resolve()
    if not dependency_cache.is_dir():raise ValueError("Provide the dependency source cache for current notices")
    output=output.resolve()
    if not output.is_relative_to(ROOT/'build') or output==ROOT/'build' or output.exists():
        raise ValueError('Choose a NEW candidate directory below build/')
    output.mkdir(parents=True)
    try:
        for name in tool_closure(ROOT):copy(ROOT/'tools'/(name+'.py'),output/'tools'/(name+'.py'))
        for name in TRANSLATIONS:copy(ROOT/'translations'/name,output/'translations'/name)
        for path in sorted((ROOT/'translations').glob('localization-en*.json')):copy(path,output/'translations'/path.name)
        copy(ROOT/'docs/release/supported-game.json',output/'docs/release/supported-game.json')
        core=Path('third_party/godzilla-core')
        for name in ('prepare.py','link_modules.py','game_check.py'):
            copy(ROOT/core/'scripts'/name,output/core/'scripts'/name)
        copy(ROOT/core/'LICENSE',output/core/'LICENSE')
        aerolib=core/'third_party/shadPS4/src/core/aerolib/aerolib.inl'
        copy(ROOT/aerolib,output/aerolib)
        for name in ('NotoSans-Regular.ttf','NotoSansCJK-Regular.ttc'):
            copy(ROOT/core/'third_party/shadPS4/src/imgui/renderer/fonts'/name,output/'assets/fonts'/name)
        (output/'runtime').mkdir()
        clean_pe(runtime or ROOT/'build/kyoei.exe',output/'runtime/kyoei.exe')
        for name in ('msvcp140.dll','msvcp140_atomic_wait.dll','vcruntime140.dll','vcruntime140_1.dll'):
            copy(Path(os.environ['WINDIR'])/'System32'/name,output/'runtime'/name)
        csc=Path(os.environ['WINDIR'])/'Microsoft.NET/Framework64/v4.0.30319/csc.exe'
        subprocess.run([str(csc),'/nologo','/codepage:65001','/target:winexe','/platform:x64','/optimize+',
                        '/r:System.Windows.Forms.dll','/r:System.Drawing.dll',
                        '/out:'+str(output/'City Shrouded in Shadow.exe'),str(ROOT/'src/kyoei_portable_launcher.cs')],check=True)
        python_runtime(output/'support/python')
        copy(ROOT/'LICENSE',output/'licenses/GPL-2.0.txt')
        # Read license texts directly from public source inputs, never a private installation.
        for name in ('NotoSans-OFL.txt','NotoSansCJK-OFL.txt'):
            copy(ROOT/'docs/release/licenses'/name,output/'licenses'/name)
        copy(ROOT/core/'third_party/shadPS4/LICENSE',output/'licenses/shadPS4/LICENSE')
        for folder in sorted(dependency_cache.glob('*-src')):
            for path in folder.iterdir():
                if path.is_file() and path.name.upper().startswith(('LICENSE','COPYING','COPYRIGHT','OFL','NOTICE')):
                    copy(path,output/'licenses/dependencies'/folder.name/path.name)
        fsr=ROOT/'build/fsr-vulkan-src'
        for name in ('LICENSE.txt','NOTICE.md','UPSTREAM.md','upstream/ffx-1.1.4/sdk/LICENSE.txt'):
            copy(fsr/name,output/'licenses/FSR-Vulkan'/name)
        copy(ROOT/'third_party/imgui/LICENSE.txt',output/'licenses/imgui/LICENSE.txt')
        copy(ROOT/'tools/start-on-linux.sh',output/'Start on Linux.sh')
        copy(ROOT/'docs/release/INSTALL.txt',output/'README.txt')
        subprocess.run([str(output/'support/python/python.exe'),'-I','-B','-c',
                        'import PIL.Image, numpy, sys; assert sys.flags.isolated; assert not any("AppData" in p for p in sys.path); import bootstrap_portable, psarc, prepare_portable_image, build_prompt_pack; print("Bundled isolated tools/Python/Pillow/NumPy OK")'],
                       cwd=output,check=True)
        subprocess.run([str(output/'support/python/python.exe'),'-B','tools/bootstrap_portable.py','--help'],cwd=output,check=True)
        records=audit(output)
        report=dict(format='kyoei-public-candidate-v1',pc_version='1.0',
                    game_data_included=False,publication_ready=False,
                    pending=['visual NPC/loading review','corresponding source and dependency license audit'],files=records)
        (output/'public-manifest.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
        archive=Path(str(output)+'.zip')
        if archive.exists():raise ValueError('Archive already exists')
        with zipfile.ZipFile(archive,'x',zipfile.ZIP_DEFLATED,compresslevel=6) as pack:
            for path in sorted(output.rglob('*')):
                if path.is_file():pack.write(path,path.relative_to(output).as_posix())
        with zipfile.ZipFile(archive) as pack:
            if pack.testzip():raise ValueError('Archive verification failed')
        checksum=hashlib.sha256(archive.read_bytes()).hexdigest()
        archive.with_suffix('.zip.sha256').write_text(checksum+'  '+archive.name+'\n',encoding='ascii')
        print(json.dumps(dict(archive=str(archive),sha256=checksum,files=len(records),publication_ready=False)))
    except BaseException:
        # Only the fresh candidate created by this invocation, verified above.
        shutil.rmtree(output)
        raise


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__);parser.add_argument('output',type=Path)
    parser.add_argument('--dependency-cache',type=Path,required=True)
    parser.add_argument('--runtime',type=Path,help='Executable from the verified source-kit build')
    args=parser.parse_args();build(args.output,args.dependency_cache,args.runtime)
