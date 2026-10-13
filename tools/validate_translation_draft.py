"""Validate draft IDs/tokens and prepare private per-language resource previews."""
import argparse
import hashlib
import json
from pathlib import Path
from prepare_translation_catalog import TOKENS
from text_tool import REPO,decode,export,insert,output_path
from translation_names import validate_names
def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('draft',type=Path); parser.add_argument('--output',type=Path)
    args=parser.parse_args(); draft=json.loads(args.draft.read_text(encoding='utf8'))
    glossary=json.loads((REPO/'translations/glossary.json').read_text(encoding='utf8'))
    if draft['format']!='kyoei-translation-draft-v1' or draft['title']!='CUSA03562' or draft['version']!='01.03':
        raise ValueError('Wrong draft identity')
    audit=json.loads((REPO/'docs/m2/audit.json').read_text(encoding='utf8'))
    expected={r['path']:r for r in audit['records']}; root=Path(audit['source'])
    output=output_path(args.output) if args.output else None
    if output and output.exists(): raise ValueError('Choose a new private output')
    records=[]
    for resource in draft['resources']:
        row=expected[resource['path']]; data=(root/row['path']).read_bytes()
        if hashlib.sha256(data).hexdigest()!=row['sha256']: raise ValueError('Source differs from M2')
        canonical=export(data); originals={r['id']:r['original'] for r in canonical['strings']}
        entries={r['id']:r for r in resource['entries']}
        if len(entries)!=len(resource['entries']) or not entries.keys()<=originals.keys(): raise ValueError('Duplicate/unknown IDs')
        for language in ('en','pt-BR'):
            document=export(data)
            for string in document['strings']:
                if string['id'] not in entries: continue
                value=entries[string['id']][language]
                if not isinstance(value,str) or '\0' in value or TOKENS.findall(value)!=TOKENS.findall(string['original']):
                    raise ValueError('Invalid translated text/tokens')
                validate_names(string['original'],value,language,glossary,
                               entries[string['id']].get('protected_literals',[]))
                string['translation']=value
            rebuilt=insert(data,document); _,parsed=decode(rebuilt)
            decoded={r.id:r.text() for r in parsed.strings}
            if any(decoded[id]!=entry[language] for id,entry in entries.items()): raise ValueError('Translated roundtrip differs')
            if any(decoded[id]!=text for id,text in originals.items() if id not in entries): raise ValueError('Untouched text changed')
            if output:
                target=output/language/row['path']; target.parent.mkdir(parents=True,exist_ok=True)
                target.write_bytes(rebuilt); target.with_suffix(target.suffix+'.json').write_text(json.dumps(document,ensure_ascii=False,indent=2)+'\n',encoding='utf8')
            records.append(dict(resource=row['path'],language=language,translated_entries=len(entries),
                                source_sha256=row['sha256'],rebuilt_sha256=hashlib.sha256(rebuilt).hexdigest()))
    report=dict(draft=str(args.draft),status=draft['status'],runtime_verified=False,private_output=str(output),records=records)
    (REPO/'docs/m4/pilot-validation.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print(json.dumps(report,indent=2))
if __name__=='__main__': main()
