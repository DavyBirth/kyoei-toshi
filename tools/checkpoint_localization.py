"""Create an audited private checkpoint; optionally publish the cumulative ZIP."""
import argparse
from datetime import datetime
import hashlib
import json
from pathlib import Path
import subprocess
from zipfile import ZipFile, ZIP_DEFLATED
from audit_localization_return import audit,load


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('workspace',type=Path)
    parser.add_argument('--publish',action='store_true')
    parser.add_argument('--shutdown-local',default=None,help='Optional explicitly scheduled shutdown, for metadata only')
    args=parser.parse_args();root=args.workspace.resolve();repo=Path(__file__).resolve().parents[1]
    originals=load(root/'original');working=load(root/'working')
    report=audit(originals,working)
    if len(working)!=173 or not report['structural_pass']:raise ValueError('Checkpoint audit failed; prior publication preserved')
    now=datetime.now().astimezone();stamp=now.strftime('%Y%m%d-%H%M%S-%f')
    ledgers=sorted((root/'reports').glob('*ledger.json'),key=lambda p:p.stat().st_mtime)
    summary=dict(timestamp=now.isoformat(),status='checkpoint',files=173,**report['totals'],
        new_english_since_received=report['totals']['filled']-4124,
        latest_ledger=ledgers[-1].name if ledgers else None,structural_pass=True,
        editorial_approved=False,runtime_verified=False,shutdown_local=args.shutdown_local)
    reports=root/'reports';(reports/'AUTO-CHECKPOINT.json').write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
    (reports/'audit-current.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    destination=root/'checkpoints'/f'auto-{stamp}.zip';destination.parent.mkdir(exist_ok=True)
    with ZipFile(destination,'x',ZIP_DEFLATED) as archive:
        for path in sorted((root/'working').rglob('*')):
            if path.is_file():archive.write(path,path.relative_to(root/'working').as_posix())
        for path in sorted(reports.rglob('*')):
            if path.is_file():archive.write(path,'CODEX_REVIEW/'+path.relative_to(reports).as_posix())
        for name in ['CONTINUE-HERE.md','OVERNIGHT.md']:
            path=root/name
            if path.exists():archive.write(path,'CODEX_REVIEW/'+name)
        patch=subprocess.check_output(['git','-c',f'safe.directory={repo.as_posix()}','diff','HEAD'],cwd=repo)
        archive.writestr('CODEX_REVIEW/repository-wip.patch',patch)
    if args.publish:
        target=root/'published/Kyoei_English_Consolidated_Current.zip'
        temporary=target.with_suffix('.zip.tmp')
        with temporary.open('wb') as output:output.write(destination.read_bytes())
        with ZipFile(temporary) as verification:
            if verification.testzip() is not None:raise ValueError('Published ZIP integrity check failed')
            names=[name for name in verification.namelist() if name.startswith('translate-en-') and name.endswith('.json') and '/' not in name]
            if len(names)!=173:raise ValueError('Published ZIP does not contain all translation files')
            for name in names:
                if json.loads(verification.read(name).decode('utf-8'))!=working[name]:
                    raise ValueError('Published JSON differs from audited working document')
        temporary.replace(target)
        summary['published_sha256']=hashlib.sha256(target.read_bytes()).hexdigest()
        publication=dict(summary,archive=target.name,zip_integrity_pass=True,
            all_173_documents_match_working=True)
        metadata=target.parent/'publication.json'
        metadata_temporary=metadata.with_suffix('.json.tmp')
        metadata_temporary.write_text(json.dumps(publication,indent=2)+'\n',encoding='utf-8')
        metadata_temporary.replace(metadata)
    (reports/'AUTO-CHECKPOINT.json').write_text(json.dumps(summary,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(summary))


if __name__=='__main__':main()
