"""Audit a full private localization against the original handoff ZIP.

Reports IDs and error classes, never source prose. Only entry en/notes may vary.
"""
import argparse
from collections import Counter
import hashlib
import json
from pathlib import Path
import re
from zipfile import ZipFile
from prepare_translation_catalog import TOKENS


def load(path):
    if path.is_dir():
        return {p.name:json.loads(p.read_text(encoding='utf-8-sig'))
                for p in sorted(path.glob('translate-en-*.json'))}
    with ZipFile(path) as archive:
        names=[n for n in archive.namelist() if re.fullmatch(r'translate-en-\d{4}\.json',n)]
        if len(names)!=len(set(names)): raise ValueError('Duplicate ZIP members')
        return {n:json.loads(archive.read(n)) for n in names}


def protected(document):
    # Remove only the two permitted keys at the precise entry level.
    result=json.loads(json.dumps(document))
    for resource in result['resources']:
        for entry in resource['entries']:
            entry.pop('en',None); entry.pop('notes',None)
    return result


def audit(original,working):
    issues=[]; files=[]; total=Counter(); seen=set()
    if set(original)!=set(working):
        issues.append(dict(kind='file_set',missing=sorted(set(original)-set(working)),extra=sorted(set(working)-set(original))))
    for name in sorted(set(original)&set(working)):
        source=original[name]; target=working[name]
        if protected(source)!=protected(target): issues.append(dict(file=name,kind='protected_fields'))
        counts=Counter()
        for resource in target['resources']:
            for entry in resource['entries']:
                key=(resource['path'],entry['id']); counts['entries']+=1
                location=dict(file=name,resource=resource['path'],id=entry['id'])
                if key in seen: issues.append(dict(**location,kind='duplicate_id'))
                seen.add(key)
                text=entry['original']; en=entry.get('en'); notes=entry.get('notes')
                if hashlib.sha256(text.encode('utf-8')).hexdigest()!=entry['source_text_sha256']:
                    issues.append(dict(**location,kind='source_text_hash'))
                if not isinstance(en,str) or not isinstance(notes,str):
                    issues.append(dict(**location,kind='value_type')); continue
                counts['filled' if en.strip() else 'empty']+=1
                if notes.strip(): counts['with_notes']+=1
                if not en.strip():
                    if not notes.strip(): counts['empty_without_notes']+=1
                    continue
                if '\0' in en: issues.append(dict(**location,kind='nul'))
                if TOKENS.findall(text)!=TOKENS.findall(en): issues.append(dict(**location,kind='tokens'))
                # Contents of Japanese editorial brackets may be translated;
                # preserve their delimiters, not the source Japanese prose.
                if [text.count(c) for c in '【】']!=[en.count(c) for c in '【】']:
                    issues.append(dict(**location,kind='japanese_bracket_delimiters'))
                if text.count('\n')!=en.count('\n'): issues.append(dict(**location,kind='newlines'))
        total.update(counts); files.append(dict(file=name,**counts))
    return dict(files=len(working),totals=dict(total),issues=issues,by_file=files,
                structural_pass=not issues,editorial_approved=False,runtime_verified=False)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('original',type=Path); parser.add_argument('working',type=Path)
    parser.add_argument('--out',type=Path,required=True)
    args=parser.parse_args(); report=audit(load(args.original),load(args.working))
    args.out.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({k:v for k,v in report.items() if k!='by_file'},indent=2))


if __name__=='__main__': main()
