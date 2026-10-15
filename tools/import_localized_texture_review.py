"""Validate the owner's exported review and preserve the complete pre-edit textures."""
import argparse,hashlib,json,shutil
from collections import Counter
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

def digest(file):return hashlib.sha256(file.read_bytes()).hexdigest()

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('review',type=Path);a=p.parse_args()
    data=a.review.read_bytes();doc=json.loads(data)
    inventory=json.loads((ROOT/'build/texture-review/inventory.json').read_text(encoding='utf-8'))
    expected={row['id']:row for row in inventory};seen=set()
    if doc.get('format')!='kyoei-localized-texture-review-v1' or len(doc.get('images',[]))!=len(expected):raise ValueError('Expected the complete texture review')
    for row in doc['images']:
        base=expected.get(row.get('id'))
        if not base or row['id'] in seen:raise ValueError('Unknown or repeated image')
        if any(row.get(k)!=base[k] for k in ('resource','original_sha256','localized_sha256','width','height')):raise ValueError('Review targets another texture revision')
        if row.get('decision') not in {'pending','accept','revise','defer'} or not isinstance(row.get('notes'),str):raise ValueError('Invalid review decision/notes')
        if digest(ROOT/'build/overnight/textures-en'/base['resource'])!=base['localized_sha256']:raise ValueError('Current TEX differs; preserve and review manually')
        seen.add(row['id'])
    review_hash=hashlib.sha256(data).hexdigest();out=ROOT/'build/texture-review-backups'/review_hash[:16]
    if not out.exists():
        out.mkdir(parents=True)
        shutil.copytree(ROOT/'build/texture-review',out/'gallery')
        shutil.copytree(ROOT/'build/overnight/textures-en',out/'textures-en')
        (out/'review.json').write_bytes(data)
        records=[dict(path=f.relative_to(out).as_posix(),bytes=f.stat().st_size,sha256=digest(f)) for f in sorted(out.rglob('*')) if f.is_file()]
        (out/'manifest.json').write_text(json.dumps(dict(review_sha256=review_hash,files=records),indent=2)+'\n',encoding='utf-8')
    manifest=json.loads((out/'manifest.json').read_text(encoding='utf-8'))
    if manifest['review_sha256']!=review_hash:raise ValueError('Backup identity differs')
    for record in manifest['files']:
        if digest(out/record['path'])!=record['sha256']:raise ValueError('Backup verification failed')
    counts=dict(Counter(row['decision'] for row in doc['images']))
    print(json.dumps(dict(backup=str(out),review_sha256=review_hash,decisions=counts,backup_verified=True,texture_changes_applied=False),ensure_ascii=False))
    for row in doc['images']:
        if row['notes']:print(json.dumps(dict(resource=row['resource'],decision=row['decision'],notes=row['notes']),ensure_ascii=True))

if __name__=='__main__':main()
