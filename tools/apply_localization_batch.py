"""Apply explicit private English/notes edits, checkpoint once and audit once."""
import argparse
import json
from pathlib import Path
from zipfile import ZIP_DEFLATED, ZipFile
from audit_localization_return import audit,load


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('workspace',type=Path);parser.add_argument('edits',type=Path)
    parser.add_argument('--batch',required=True)
    args=parser.parse_args(); root=args.workspace.resolve()
    if Path(args.batch).name!=args.batch:raise ValueError('Invalid batch name')
    working=root/'working'; documents=load(working); originals=load(root/'original')
    edits=json.loads(args.edits.read_text(encoding='utf-8')); ledger=[]; modified=set();seen=set()
    for change in edits:
        name=change['file']; key=(name,change['id'])
        if key in seen:raise ValueError('Duplicate edit')
        seen.add(key)
        found=[(r,e) for r in documents[name]['resources'] for e in r['entries'] if e['id']==change['id']]
        if len(found)!=1:raise ValueError('Ambiguous entry')
        resource,entry=found[0]; before=dict(en=entry['en'],notes=entry['notes'])
        if 'en' in change:
            if entry['en'] and (change.get('expected_en')!=entry['en'] or not change.get('reason')):
                raise ValueError('Existing English requires exact expected value and reason')
            entry['en']=change['en']
        if 'notes' in change:
            entry['notes']=(entry['notes']+' | ' if entry['notes'] else '')+change['notes']
        modified.add(name)
        ledger.append(dict(file=name,id=entry['id'],resource=resource['path'],
            source_text_sha256=entry['source_text_sha256'],before=before,
            after=dict(en=entry['en'],notes=entry['notes']),reason=change.get('reason','New draft or evidence note')))
    report=audit(originals,documents)
    if report['issues']:raise ValueError(json.dumps(report['issues']))
    checkpoint=root/'checkpoints'/f'before-{args.batch}.zip';checkpoint.parent.mkdir(exist_ok=True)
    with ZipFile(checkpoint,'x',ZIP_DEFLATED) as z:
        for path in working.rglob('*'):
            if path.is_file():z.write(path,path.relative_to(working).as_posix())
    report_dir=root/'reports'; report_dir.mkdir(exist_ok=True)
    for name in modified:
        path=working/name;temp=path.with_suffix('.json.tmp')
        with temp.open('x',encoding='utf-8') as f:f.write(json.dumps(documents[name],ensure_ascii=False,indent=2)+'\n')
        temp.replace(path)
    (report_dir/f'{args.batch}-ledger.json').write_text(json.dumps(ledger,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    (report_dir/f'{args.batch}-audit.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(dict(batch=args.batch,edits=len(edits),new_english=sum(not row['before']['en'] and bool(row['after']['en']) for row in ledger),
        modified_files=len(modified),totals=report['totals'],structural_pass=report['structural_pass'])))


if __name__=='__main__':main()
