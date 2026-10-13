"""Build a metadata-only coverage queue for every inventoried asset family."""
import collections
import hashlib
import json
from pathlib import Path
ROOT=Path(__file__).resolve().parents[1]

def main():
    ledger=ROOT/'work/m1/catalog.json'
    proof=json.loads((ROOT/'docs/m1/formats.json').read_text(encoding='utf8'))
    if hashlib.sha256(ledger.read_bytes()).hexdigest()!=proof['catalog_sha256']:
        raise ValueError('M1 catalog changed')
    catalog=json.loads(ledger.read_text(encoding='utf8'))
    audit=json.loads((ROOT/'docs/m2/audit.json').read_text(encoding='utf8'))
    audited={row['path'] for row in audit['records']}
    supplementary=json.loads((ROOT/'docs/m4/supplementary-scan.json').read_text(encoding='utf8'))
    hit_resources={(r['archive'],r['resource']) for r in supplementary['hits']}
    counters=collections.Counter(); families={}; special=[]
    for archive in catalog['archives']:
        stem=Path(archive['archive']).stem
        for asset in archive['files']:
            path=asset['path']; extension=Path(path).suffix.lower() or '(none)'
            if f'{stem}/{path}' in audited:
                scope='structured_text_extraction'; next_step='context_and_human_review'
            elif extension=='.tex':
                scope='texture_inventory'; next_step='interface_image_review'
            elif (archive['archive'],path) in hit_resources:
                scope='supplementary_text_hits'; next_step='decode_structure_before_reinsertion'
            elif extension in ('.usm','.mp4'):
                scope='movie_review_pending'; next_step='check_burned_in_text_and_subtitle_tracks'
            elif extension in ('.acb','.awb','.acf','.acb_tmp'):
                scope='audio_preserved'; next_step='check_story_subtitle_coverage_without_dubbing'
            else:
                scope='unverified_asset_family'; next_step='determine_whether_visible_text_exists'
            counters[scope]+=1
            key=(extension,scope)
            row=families.setdefault(key,dict(extension=extension,scope=scope,files=0,bytes=0,next_step=next_step))
            row['files']+=1; row['bytes']+=asset['bytes']
            if scope in ('movie_review_pending','supplementary_text_hits'):
                special.append(dict(archive=archive['archive'],resource=path,bytes=asset['bytes'],
                                    sha256=asset['sha256'],scope=scope,next_step=next_step))
    if sum(counters.values())!=proof['verified_files']: raise ValueError('Coverage lost inventoried files')
    report=dict(catalog_sha256=proof['catalog_sha256'],inventoried_files=proof['verified_files'],
                scope_counts=dict(counters),all_visible_text_identified=False,
                families=sorted(families.values(),key=lambda r:(r['scope'],r['extension'])),
                focused_review_queue=special)
    target=ROOT/'docs/m4/coverage-queue.json'
    target.write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print(json.dumps(report['scope_counts'],indent=2))

if __name__=='__main__': main()
