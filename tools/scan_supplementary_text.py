"""Read-only reconnaissance of small non-CSH/LUC resources, not a reinserter."""
import hashlib
import json
from pathlib import Path
import re
import struct
import zlib
ROOT=Path(__file__).resolve().parents[1]
EXTENSIONS={'.hud','.tbl','.nam','.lst','.rsc','.rss','.tip','.csd','.msi','.hfp','.mik','.pln','.cht'}
def main():
    catalog=json.loads((ROOT/'work/m1/catalog.json').read_text(encoding='utf8'))
    root=Path(catalog['extraction']); private=Path('G:/kyoei-toshi-work/translation/catalog-01/supplementary')
    private.mkdir(parents=True,exist_ok=True); hits=[]; missing=[]; scanned=0
    for archive in catalog['archives']:
        for row in archive['files']:
            if Path(row['path']).suffix.lower() not in EXTENSIONS: continue
            path=root/Path(archive['archive']).stem/row['path']
            if not path.exists(): missing.append(dict(archive=archive['archive'],path=row['path'])); continue
            data=path.read_bytes()
            if hashlib.sha256(data).hexdigest()!=row['sha256']: raise ValueError('Resource changed from M1')
            payload=data; location='file'
            if len(data)>144 and data[128:132]==b'BILZ':
                raw_size,packed_size=struct.unpack_from('>II',data,132)
                if raw_size>64*1024*1024 or packed_size!=len(data)-144: raise ValueError('Invalid BILZ sizes')
                decoder=zlib.decompressobj()
                payload=decoder.decompress(data[144:],raw_size+1); location='decompressed_payload'
                if (not decoder.eof or decoder.unused_data or decoder.unconsumed_tail
                    or len(payload)!=raw_size): raise ValueError('Invalid BILZ payload')
            scanned+=1
            for match in re.finditer(rb'[^\x00]{3,2048}',payload):
                try: text=match.group().decode('utf8')
                except UnicodeDecodeError: continue
                if re.search(r'[\u3040-\u30ff\u3400-\u9fff]',text):
                    hits.append(dict(archive=archive['archive'],resource=row['path'],offset=match.start(),
                        offset_space=location,original=text,source_sha256=row['sha256'],
                        encoding='utf8',kind='heuristic_run',status='structure_unverified'))
    with (private/'utf8-hits.jsonl').open('w',encoding='utf8') as file:
        for row in hits: file.write(json.dumps(row,ensure_ascii=False)+'\n')
    report=dict(scanned_resources=scanned,missing_resources=missing,
                heuristic_japanese_runs=len(hits),private_output=str(private),
                hits=[{k:v for k,v in row.items() if k!='original'} for row in hits],
                visible_text_coverage_verified=False,reinsertion_supported=False)
    (ROOT/'docs/m4/supplementary-scan.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print(json.dumps({k:v for k,v in report.items() if k not in ('hits','missing_resources')},indent=2))
    print('Missing small resources:',len(missing))
if __name__=='__main__': main()
