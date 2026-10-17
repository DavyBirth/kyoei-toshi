"""Save changed project files and a binary Git diff to an ignored local checkpoint."""
from datetime import datetime,timezone
import hashlib
import json
import os
from pathlib import Path
import subprocess
import zipfile

ROOT=Path(__file__).resolve().parents[1]

def git(*args):
    return subprocess.check_output(['git','-C',str(ROOT),*args],stderr=subprocess.PIPE)

def checkpoint():
    folder=ROOT/'build/overnight'
    folder.mkdir(parents=True,exist_ok=True)
    stamp=datetime.now(timezone.utc).strftime('%Y%m%dT%H%M%S%fZ')
    target=folder/f'checkpoint-{stamp}.zip'
    changed=git('diff','--name-only','-z','HEAD').split(b'\0')
    changed+=git('ls-files','--others','--exclude-standard','-z').split(b'\0')
    paths=sorted({os.fsdecode(name) for name in changed if name})
    manifest=dict(created_at=stamp,head=git('rev-parse','HEAD').decode().strip(),files={},deleted=[])
    temporary=target.with_suffix('.tmp')
    with zipfile.ZipFile(temporary,'w',zipfile.ZIP_DEFLATED) as archive:
        archive.writestr('changes.patch',git('diff','--binary','HEAD'))
        for relative in paths:
            path=ROOT/relative
            if not path.resolve().is_relative_to(ROOT):raise ValueError('Checkpoint path outside project')
            if not path.exists():manifest['deleted'].append(relative);continue
            if not path.is_file():raise ValueError('Unexpected non-file in changed paths')
            data=path.read_bytes()
            archive.writestr('files/'+relative.replace('\\','/'),data)
            manifest['files'][relative]=hashlib.sha256(data).hexdigest()
        archive.writestr('manifest.json',json.dumps(manifest,indent=2))
    with temporary.open('r+b') as source:os.fsync(source.fileno())
    with zipfile.ZipFile(temporary) as archive:
        if archive.testzip() is not None:raise ValueError('Invalid checkpoint archive')
        for relative,expected in manifest['files'].items():
            if hashlib.sha256(archive.read('files/'+relative.replace('\\','/'))).hexdigest()!=expected:
                raise ValueError('Checkpoint verification failed')
    temporary.replace(target)
    print(target)
    return target

if __name__=='__main__':checkpoint()
