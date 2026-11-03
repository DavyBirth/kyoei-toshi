"""Build a verified private language delta directly from the owned archive.

Avoids a temporary 6 GB translated archive. The public inputs are translation
JSON and tools; generated resources/deltas contain game data and stay private.
"""
import argparse
import hashlib
import json
from pathlib import Path
from psarc import Psarc
from psarc_delta import build,verify
from text_tool import export,insert,decode
from prepare_translation_catalog import functional_tokens
from translation_names import validate_names
from english_typography import native_english
from build_ui_corrections import corrections
from check_game_font_tables import glyph_rows
from prewrap_text import PREWRAP,prewrap

ROOT=Path(__file__).resolve().parents[1]
SOURCE_SHA256='3a54e70d40b57562cbf350028ab8b75ee09c8143f8f1e766cc04bd81ce133966'

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('original',type=Path);p.add_argument('output',type=Path)
    p.add_argument('drafts',nargs='+',type=Path)
    p.add_argument('--language',choices=('en','pt-BR'),required=True)
    p.add_argument('--textures',type=Path)
    a=p.parse_args();output=a.output.resolve()
    if not output.is_relative_to(ROOT/'build') or output.exists():raise ValueError('Choose a new private output under build/')
    with a.original.open('rb') as f:
        if hashlib.file_digest(f,'sha256').hexdigest()!=SOURCE_SHA256:raise ValueError('Archive differs from CUSA03562 01.03')
    resources={}
    for path in a.drafts:
        draft=json.loads(path.read_text(encoding='utf8'))
        if (draft['format'],draft['title'],draft['version'])!=('kyoei-translation-draft-v1','CUSA03562','01.03'):
            raise ValueError('Draft identity differs')
        for r in draft['resources']:
            dest=resources.setdefault(r['path'],dict(source_sha256=r['source_sha256'],entries={}))
            if dest['source_sha256']!=r['source_sha256']:raise ValueError('Resource revisions differ')
            for e in r['entries']:
                if e['id'] in dest['entries']:raise ValueError('Duplicate entry')
                dest['entries'][e['id']]=e
    names={name.removeprefix('common/') for name in resources}
    changed,counts=corrections(a.original,a.language,names)
    glossary=json.loads((ROOT/'translations/glossary.json').read_text(encoding='utf8'))
    with Psarc(a.original) as archive:
        entries={e.name:e for e in archive.entries[1:]}
        glyphs=glyph_rows(b''.join(archive.chunks(entries['font/utf8all_table.csh'])))[0]
        for name,r in resources.items():
            if not name.startswith('common/'):raise ValueError('Expected a common.psarc resource')
            key=name.split('/',1)[1];data=b''.join(archive.chunks(entries[key]))
            if hashlib.sha256(data).hexdigest()!=r['source_sha256']:raise ValueError('Resource source differs: '+name)
            doc=export(data);originals={s['id'] for s in doc['strings']}
            if not r['entries'].keys()<=originals:raise ValueError('Unknown string ID')
            expected={}
            for s in doc['strings']:
                if s['id'] not in r['entries']:continue
                e=r['entries'][s['id']];value=e[a.language]
                if not isinstance(value,str) or '\0' in value or (not value and s['original'] and not e.get('allow_empty')):
                    raise ValueError('Invalid text: '+name+' '+s['id'])
                if a.language=='en':value=native_english(value)
                if sorted(functional_tokens(value,name))!=sorted(functional_tokens(s['original'],name)):
                    raise ValueError('Control tokens differ: '+name+' '+s['id'])
                if e.get('source_text_sha256') and hashlib.sha256(s['original'].encode()).hexdigest()!=e['source_text_sha256']:
                    raise ValueError('Source text differs: '+name+' '+s['id'])
                validate_names(s['original'],value,a.language,glossary,e.get('protected_literals',[]))
                if name in PREWRAP:value=prewrap(value,glyphs,PREWRAP[name])
                s['translation']=value;expected[s['id']]=value
            rebuilt=insert(data,doc);actual={s.id:s.text() for s in decode(rebuilt)[1].strings}
            if any(actual[k]!=v for k,v in expected.items()):raise ValueError('Text roundtrip differs')
            changed[key]=rebuilt
    if a.textures:
        files=sorted(a.textures.rglob('*.tex'))
        if not files:raise ValueError('No localized textures found')
        for path in files:changed[path.relative_to(a.textures).as_posix()]=path.read_bytes()
    delta,number=build(a.original,changed);verify(a.original,delta,changed)
    output.parent.mkdir(parents=True,exist_ok=True);output.write_bytes(delta)
    report=dict(language=a.language,source_archive_sha256=SOURCE_SHA256,
                sha256=hashlib.sha256(delta).hexdigest(),bytes=len(delta),resources=number,
                translated_entries=sum(len(r['entries']) for r in resources.values()),
                corrections=counts,roundtrip_verified=True,runtime_verified=False,game_data_included=True)
    output.with_suffix('.proof.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print(json.dumps(report),flush=True)

if __name__=='__main__':main()
