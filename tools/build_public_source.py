"""Assemble the runtime's source review kit from fixed, hash-checked snapshots.

No game/dump/profile paths are inputs. Local source caches are explicit inputs.
The archive includes fixed prebuilt libraries for developer convenience; source
and upstream build recipes are retained separately. A clean build remains a
required release check, never inferred from successful ZIP assembly.
"""
import argparse
import hashlib
import json
import re
from pathlib import Path
import subprocess
import zipfile
from build_public_candidate import ROOT,SEEDS,TRANSLATIONS,tool_closure

SKIP={'.git','__pycache__','.cache','.vs'}
BINARY={'.exe','.dll','.lib','.obj','.o','.a','.pdb','.pyc','.zip','.7z','.tar','.gz','.xz','.bin','.psarc','.prx','.sprx','.elf'}


def source_files(folder):
    for path in sorted(folder.rglob('*')):
        relative=path.relative_to(folder)
        if any(part in SKIP for part in relative.parts) or path.suffix.lower() in BINARY:continue
        if path.is_file() and not path.is_symlink():yield path,relative


def assemble(output,cache):
    output=output.resolve();cache=cache.resolve()
    if not output.is_relative_to(ROOT/'build') or output.exists():raise ValueError('Choose a new ZIP under build/')
    if not cache.is_dir():raise ValueError('Missing explicit dependency source cache')
    records=[];seen=set()
    with zipfile.ZipFile(output,'x',zipfile.ZIP_DEFLATED,compresslevel=3) as archive:
        def add(path,name,expected=None):
            name=Path(name).as_posix()
            if name in seen:return
            if Path(name).is_absolute() or '..' in Path(name).parts:raise ValueError('Invalid source path')
            path=Path(path)
            if path.is_symlink():raise ValueError('Source symlink not allowed')
            data=path.read_bytes();digest=hashlib.sha256(data).hexdigest()
            if expected and digest!=expected:raise ValueError('Fixed snapshot changed: '+name)
            archive.writestr(name,data);seen.add(name)
            records.append(dict(path=name,bytes=len(data),sha256=digest))
        for name in ('CMakeLists.txt','LICENSE','README.md'):add(ROOT/name,name)
        for folder in ('src','gpu-gow3','cmake','third_party/imgui'):
            for path,relative in source_files(ROOT/folder):add(path,Path(folder)/relative)
        cmake_sources=[ROOT/'CMakeLists.txt',*sorted((ROOT/'cmake').glob('*.cmake'))]
        cmake_tools=set()
        for cmake in cmake_sources:
            cmake_tools.update(re.findall(r'tools/([A-Za-z0-9_]+\.py)',cmake.read_text(encoding='utf8')))
        tools=tool_closure(ROOT,(*SEEDS,*(Path(name).stem for name in cmake_tools)))
        for pattern in ('*.c','*.cpp','*.h'):
            for path in sorted((ROOT/'tools').glob(pattern)):add(path,'tools/'+path.name)
        for name in tools+['build_public_candidate','build_public_source','test_public_candidate']:
            add(ROOT/'tools'/(name+'.py'),'tools/'+name+'.py')
        add(ROOT/'tools/requirements-images.txt','tools/requirements-images.txt')
        add(ROOT/'tools/start-on-linux.sh','tools/start-on-linux.sh')
        for name in TRANSLATIONS:add(ROOT/'translations'/name,'translations/'+name)
        for path in sorted((ROOT/'translations').glob('localization-en*.json')):add(path,'translations/'+path.name)
        for path,relative in source_files(ROOT/'docs/release'):add(path,Path('docs/release')/relative)
        for name,prefix in (('godzilla-core','third_party/godzilla-core'),('gow3-gpu','third_party/gow3-gpu')):
            metadata=json.loads((ROOT/'docs'/(name+'.json')).read_text(encoding='utf8'))
            if name=='godzilla-core':
                metadata.pop('official',None)
                metadata.get('prebuilt',{}).pop('origin',None)
                groups=[value['files'] for value in metadata.values() if isinstance(value,dict) and 'files' in value]
            else:groups=[metadata['files']]
            metadata_name='docs/'+name+'.json'
            metadata_data=(json.dumps(metadata,indent=2)+'\n').encode('utf8')
            archive.writestr(metadata_name,metadata_data);seen.add(metadata_name)
            records.append(dict(path=metadata_name,bytes=len(metadata_data),sha256=hashlib.sha256(metadata_data).hexdigest()))
            for rows in groups:
                for row in rows:
                    path=Path(prefix)/row['path']
                    if path.suffix.lower() in {'.bin','.prx','.sprx','.psarc','.elf'}:raise ValueError('Game-like file in snapshot')
                    add(ROOT/path,path,row['sha256'])
        add(ROOT/'third_party/godzilla-core/prebuilt/libraries.cmake',
            'third_party/godzilla-core/prebuilt/libraries.cmake')
        # Preserve the source trees and the generated headers used by the build.
        for folder in sorted(cache.glob('*-src')):
            for path,relative in source_files(folder):add(path,Path('build/_deps')/folder.name/relative)
        for folder in sorted(cache.glob('*-build')):
            for path in sorted(folder.rglob('*.h')):
                if '.git' not in path.parts:add(path,Path('build/_deps')/folder.name/path.relative_to(folder))
        for path,relative in source_files(ROOT/'build/fsr-vulkan-src'):
            add(path,Path('build/fsr-vulkan-src')/relative)
        inputs=ROOT/'build/public-source-inputs'
        import tarfile
        expected_ffmpeg='6b9a5ee501be41d6abc7579a106263b31f787321cbc45dedee97abf992bf8236cdb2394571dd256a74154f4a20018d429ae7e7f0409611ddc4d6f529d924d175'
        if hashlib.sha512((inputs/'ffmpeg-n7.1.1.tar.gz').read_bytes()).hexdigest()!=expected_ffmpeg:
            raise ValueError('FFmpeg source checksum differs from pinned port')
        for name in ('ffmpeg-n7.1.1.tar.gz','vcpkg-df8bfe5.tar.gz'):
            with tarfile.open(inputs/name) as upstream:upstream.getmembers()
            add(inputs/name,'upstream-source/'+name)
        try:head=subprocess.check_output(['git','-C',str(ROOT),'rev-parse','HEAD'],text=True,stderr=subprocess.DEVNULL).strip()
        except subprocess.CalledProcessError:
            head=json.loads((ROOT/'source-manifest.json').read_text())['reference_commit']
        archive.writestr('source-manifest.json',json.dumps(dict(format='kyoei-source-review-v1',
            reference_commit=head,game_data_included=False,clean_build_verified=False,
            publication_ready=False,files=records),indent=2)+'\n')
    with zipfile.ZipFile(output) as archive:
        if archive.testzip():raise ValueError('Source archive verification failed')
    checksum=hashlib.sha256(output.read_bytes()).hexdigest()
    output.with_suffix('.zip.sha256').write_text(checksum+'  '+output.name+'\n')
    print(json.dumps(dict(archive=str(output),sha256=checksum,files=len(records),clean_build_verified=False)))


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output',type=Path);parser.add_argument('--dependency-cache',type=Path,required=True)
    args=parser.parse_args();assemble(args.output,args.dependency_cache)
