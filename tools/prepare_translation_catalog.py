"""Prepare private bilingual review files and a public coverage report.

Never writes originals into the repository. Covers every audited UTF-8 string;
unrecognized encodings and texture text remain explicit review queues.
"""
import argparse
import collections
import csv
import hashlib
import json
from pathlib import Path
import re
from text_tool import REPO, decode, export, output_path
from text_formats import JAPANESE

TOKENS=re.compile(r'%[-+0-9.*]*[sdfiu]|\{[^{}\n]+\}|<[^<>\n]+>|\[[A-Z_0-9]+\]|\\[nrt]')
def classify(path,text):
    name=path.lower()
    if 'bannedwords' in name: return 'name_filter','excluded'
    if 'staffroll' in name: return 'credits','optional'
    if not text: return 'empty','excluded'
    if not JAPANESE.search(text):
        if re.fullmatch(r'[A-Za-z0-9_./:\\#@+%\-]+',text): return 'engine_or_ascii_label','review_ascii'
        return 'non_japanese','review_ascii'
    for needle,category in [('map_goal','objectives'),('mission','objectives'),
        ('operation','tutorials'),('loading','loading'),('item_data','items'),
        ('reading','lore'),('news','story_news'),('store','shop'),
        ('text_stage','dialogue'),('textdata','shared_ui')]:
        if needle in name: return category,'candidate'
    return 'unclassified_japanese','candidate'

def spreadsheet_cell(text):
    # Review CSV is not an application format. Preserve authoritative source
    # in JSON and display formula-like cells as literals in spreadsheet apps.
    return "'"+text if text[:1] in ('=','+','-','@') else text

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output',type=Path)
    args=parser.parse_args(); output=output_path(args.output)
    if output.exists(): raise ValueError('Use a new private output directory')
    output.mkdir(parents=True)
    audit=json.loads((REPO/'docs/m2/audit.json').read_text(encoding='utf8'))
    root=Path(audit['source']); counts=collections.Counter(); unique=set(); reports=[]
    queues={}; csv_files={}; csv_writers={}
    (output/'resources').mkdir(); (output/'review').mkdir()
    with (output/'all-strings.jsonl').open('w',encoding='utf8') as all_stream, \
         (output/'encoding-review.jsonl').open('w',encoding='utf8') as encoding_stream:
        try:
            for number,row in enumerate(audit['records'],1):
                data=(root/row['path']).read_bytes()
                if hashlib.sha256(data).hexdigest()!=row['sha256']: raise ValueError('Audited resource changed')
                _,table=decode(data); canonical=export(data)
                resource_out=output/'resources'/Path(row['path']+'.json')
                resource_out.parent.mkdir(parents=True,exist_ok=True)
                resource_out.write_text(json.dumps(canonical,ensure_ascii=False,indent=2)+'\n',encoding='utf8')
                local_counts=collections.Counter()
                for string in table.strings:
                    counts['all_occurrences']+=1
                    try: text=string.text()
                    except UnicodeDecodeError:
                        probe=None
                        try:
                            candidate=string.raw.decode('cp932')
                            if len(candidate)>=2 and JAPANESE.search(candidate) and candidate.encode('cp932')==string.raw:
                                probe=candidate
                        except (UnicodeDecodeError,UnicodeEncodeError): pass
                        encoding_stream.write(json.dumps(dict(resource=row['path'],id=string.id,bytes=len(string.raw),
                            raw_sha256=hashlib.sha256(string.raw).hexdigest(),cp932_candidate=probe,
                            status='encoding_unverified'),ensure_ascii=False)+'\n')
                        counts['non_utf8']+=1; counts['cp932_candidates']+=probe is not None; continue
                    category,scope=classify(row['path'],text); counts['utf8']+=1
                    counts[scope]+=1; local_counts[category]+=1
                    if JAPANESE.search(text): counts['japanese_occurrences']+=1; unique.add(text)
                    record=dict(resource=row['path'],id=string.id,source_sha256=row['sha256'],source_encoding='utf8',
                        original=text,text_sha256=hashlib.sha256(string.raw).hexdigest(),
                        category_hint=category,scope_hint=scope,classification_verified=False,
                        english=None,portuguese_brazil=None,status='unreviewed',
                        tokens=TOKENS.findall(text),notes='')
                    all_stream.write(json.dumps(record,ensure_ascii=False)+'\n')
                    if scope in ('candidate','optional'):
                        if category not in queues:
                            queues[category]=(output/'review'/f'{category}.jsonl').open('w',encoding='utf8')
                            csv_files[category]=(output/'review'/f'{category}.csv').open('w',encoding='utf-8-sig',newline='')
                            writer=csv.writer(csv_files[category]); csv_writers[category]=writer
                            writer.writerow(['resource','id','source_sha256','japanese','english','portuguese_brazil','tokens','status','notes'])
                        queues[category].write(json.dumps(record,ensure_ascii=False)+'\n')
                        csv_writers[category].writerow([row['path'],string.id,row['sha256'],spreadsheet_cell(text),
                                                       '','',json.dumps(record['tokens'],ensure_ascii=False),'unreviewed',''])
                reports.append(dict(path=row['path'],source_sha256=row['sha256'],string_categories=dict(local_counts)))
                if number%300==0: print(f'Resources prepared: {number}/{len(audit["records"])}',flush=True)
        finally:
            for file in list(queues.values())+list(csv_files.values()): file.close()
    if counts['all_occurrences']!=audit['strings'] or counts['japanese_occurrences']!=audit['japanese_strings']:
        raise ValueError('Coverage differs from audited M2')
    report=dict(private_output=str(output),resources=len(reports),counts=dict(counts),
                unique_japanese_strings=len(unique),all_visible_text_coverage_verified=False,
                original_distribution_allowed=False,records=reports)
    public=REPO/'docs/m4'; public.mkdir(exist_ok=True)
    (public/'translation-inventory.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    (output/'README.txt').write_text('Authoritative originals: resources/*.json and all-strings.jsonl.\n'
        'Review EN/PT-BR per occurrence. Category and eligibility hints require review.\n'
        'Repeated text hashes are reuse suggestions, not globally shared translations.\n'
        'CSV formula-like originals have a display apostrophe; do not reinsert from CSV without validation.\n'
        'Unknown encodings and image text are not certified by this export.\n',encoding='utf8')
    print(json.dumps({k:v for k,v in report.items() if k!='records'},indent=2))

if __name__=='__main__': main()
