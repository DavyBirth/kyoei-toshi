"""Prepare local experimental translated archives from translation-only drafts.

Does not modify the dump. Generated game archives remain private and may not
be distributed. Requires --preview while editorial/runtime review is pending.
"""
import argparse
import hashlib
import json
from pathlib import Path
from prepare_translation_catalog import functional_tokens
from psarc_patch import patch
from text_tool import REPO,decode,export,insert,output_path
from translation_names import validate_names
from english_typography import native_english

def published_manifest(report, previous=None):
    """Retain independently built languages without claiming equal coverage."""
    combined=json.loads(json.dumps(report))
    if previous is not None:
        for key in ('format','title','version','source_archive_sha256'):
            if previous.get(key)!=report.get(key):
                raise ValueError('Existing preview manifest targets a different source')
        retained={}
        for language,record in previous['languages'].items():
            retained[language]=dict(record)
            retained[language].setdefault('resources',previous.get('resources',[]))
            retained[language].setdefault('translated_entries',previous.get('entries_per_language'))
        retained.update(combined['languages']); combined['languages']=retained
    for language in report['languages']:
        combined['languages'][language]['resources']=report['resources']
        combined['languages'][language]['translated_entries']=report['entries_per_language']
    counts={language:record.get('translated_entries') for language,record in combined['languages'].items()}
    combined['entries_by_language']=counts
    if len(set(counts.values()))>1:
        combined['entries_per_language']=None
    return combined

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output',type=Path);parser.add_argument('drafts',nargs='+',type=Path)
    parser.add_argument('--preview',action='store_true',required=True)
    parser.add_argument('--language',action='append',choices=('en','pt-BR'),
                        help='Build only the selected language; repeat to select both')
    parser.add_argument('--textures-en',type=Path,
                        help='Directory of localized TEX files (from tools/localize_textures.py) mirroring archive paths')
    parser.add_argument('--validate-only',action='store_true',
                        help='Verify resource roundtrips without creating archives or publishing a manifest')
    args=parser.parse_args();output=output_path(args.output)
    if args.textures_en and (not args.textures_en.is_dir() or not any(args.textures_en.rglob('*.tex'))):
        raise ValueError('Localized texture directory is missing or contains no TEX files')
    selected=tuple(dict.fromkeys(args.language or ('en','pt-BR')))
    audit=json.loads((REPO/'docs/m2/audit.json').read_text(encoding='utf8'))
    inventory=json.loads((REPO/'docs/m0/inventory.json').read_text(encoding='utf8'))
    glossary=json.loads((REPO/'translations/glossary.json').read_text(encoding='utf8'))
    audited={r['path']:r for r in audit['records']}; resources={}
    for path in args.drafts:
        draft=json.loads(path.read_text(encoding='utf8'))
        if (draft.get('format'),draft.get('title'),draft.get('version'))!=('kyoei-translation-draft-v1','CUSA03562','01.03'):
            raise ValueError('Draft identity differs')
        for resource in draft['resources']:
            name=resource['path'];row=audited[name]
            if not name.startswith('common/') or resource.get('source_sha256')!=row['sha256']:
                raise ValueError('Unsupported archive or resource revision')
            target=resources.setdefault(name,dict(source_sha256=row['sha256'],entries={}))
            for entry in resource['entries']:
                if entry['id'] in target['entries']: raise ValueError('Duplicate translation ID')
                target['entries'][entry['id']]=entry
    compiled={language:{} for language in selected}; reports=[]
    for name,resource in resources.items():
        data=(Path(audit['source'])/name).read_bytes()
        if hashlib.sha256(data).hexdigest()!=resource['source_sha256']:raise ValueError('M2 source differs')
        canonical=export(data);originals={r['id']:r['original'] for r in canonical['strings']}
        if not resource['entries'].keys()<=originals.keys():raise ValueError('Unknown string ID')
        for language in compiled:
            document=export(data)
            for string in document['strings']:
                if string['id'] not in resource['entries']:continue
                entry=resource['entries'][string['id']];value=entry[language]
                if language=='en' and isinstance(value,str):value=native_english(value)
                if not isinstance(value,str) or '\0' in value or (not value and string['original'] and not entry.get('allow_empty')):
                    raise ValueError('Invalid/empty translation')
                if sorted(functional_tokens(value,name))!=sorted(functional_tokens(string['original'],name)):
                    raise ValueError(f'Control tokens differ: {name} {string["id"]}')
                validate_names(string['original'],value,language,glossary,entry.get('protected_literals',[]))
                if entry.get('source_text_sha256') and entry['source_text_sha256']!=hashlib.sha256(string['original'].encode('utf8')).hexdigest():
                    raise ValueError('Original text hash differs')
                string['translation']=value
            rebuilt=insert(data,document);_,parsed=decode(rebuilt)
            decoded={s.id:s.text() for s in parsed.strings}
            for id,original in originals.items():
                expected=resource['entries'][id][language] if id in resource['entries'] else original
                if language=='en' and id in resource['entries']:expected=native_english(expected)
                if decoded[id]!=expected:raise ValueError('Roundtrip differs')
            compiled[language][name.split('/',1)[1]]=rebuilt
        reports.append(dict(resource=name,source_sha256=resource['source_sha256'],entries=len(resource['entries'])))
    if args.validate_only:
        print(json.dumps(dict(languages=list(selected),resources=reports,
            entries_per_language=sum(r['entries'] for r in reports),
            resource_roundtrip_verified=True,archives_created=False,
            manifest_published=False,runtime_verified=False),indent=2))
        return
    source=Path(inventory['source'])/'DAT/common.psarc'
    expected=next(r['sha256'] for r in inventory['files'] if r['path']=='DAT/common.psarc')
    output.mkdir(parents=True);languages={}
    if args.textures_en:
        # pt-BR reuses the English art until its own textures exist.
        for language in compiled:
            for path in sorted(args.textures_en.rglob('*.tex')):
                compiled[language][path.relative_to(args.textures_en).as_posix()]=path.read_bytes()
    for language,replacements in compiled.items():
        target=output/language/'common.psarc';print(f'Building private preview: {language}',flush=True)
        proof=patch(source,target,replacements,expected)
        (target.parent/'proof.json').write_text(json.dumps(proof,indent=2)+'\n',encoding='utf8')
        languages[language]=dict(archive=str(target),sha256=proof['output_sha256'],bytes=target.stat().st_size,
                                 textures=sum(name.endswith('.tex') for name in replacements),
                                 status='experimental_partial_translation',runtime_verified=False)
    report=dict(format='kyoei-runtime-preview-v1',title='CUSA03562',version='01.03',
        source_archive_sha256=expected,languages=languages,resources=reports,
        entries_per_language=sum(r['entries'] for r in reports),human_review_complete=False,
        runtime_verified=False,original_dump_modified=False)
    (output/'manifest.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    manifest_path=REPO/'docs/m4/runtime-preview.json'
    previous=json.loads(manifest_path.read_text(encoding='utf8')) if manifest_path.exists() else None
    publication=published_manifest(report,previous)
    temporary=manifest_path.with_suffix('.json.tmp')
    temporary.write_text(json.dumps(publication,indent=2)+'\n',encoding='utf8')
    temporary.replace(manifest_path)
    print(f'Local previews ready: {report["entries_per_language"]} entries per language',flush=True)

if __name__=='__main__':main()
