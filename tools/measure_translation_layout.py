"""Compare static glyph advances, without claiming runtime widths or automatic layout fixes."""
import argparse
import hashlib
import json
from pathlib import Path
from check_game_font_tables import glyph_rows
from prepare_translation_catalog import TOKENS
from text_tool import REPO,export


def measure(text,glyphs):
    controls=TOKENS.findall(text)
    dynamic=[token for token in controls if not (token.startswith('[#') or token=='[/#]')]
    plain=TOKENS.sub('',text)
    missing=sorted(set(plain)-set(glyphs)-{'\n','\r'})
    advances=[sum(glyphs[character]['advance'] for character in line if character in glyphs)
              for line in plain.replace('\r\n','\n').replace('\r','\n').split('\n')]
    return dict(lines=len(advances),known_line_advances=advances,
        max_static_advance=max(advances,default=0),missing_glyphs=missing,
        unresolved_controls=dynamic,complete_static_measurement=not missing and not dynamic)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('drafts',nargs='+',type=Path)
    parser.add_argument('--language',choices=('en','pt-BR'),default='en')
    parser.add_argument('--report',type=Path,required=True)
    args=parser.parse_args()
    audit=json.loads((REPO/'docs/m2/audit.json').read_text(encoding='utf8'))
    rows={row['path']:row for row in audit['records']}; root=Path(audit['source'])
    tables={}
    for name in ('common/font/utf8all_table.csh','common/font/utf8all_IM_table.csh'):
        data=(root/name).read_bytes()
        if hashlib.sha256(data).hexdigest()!=rows[name]['sha256']: raise ValueError('Font source changed')
        tables[name]=glyph_rows(data)[0]
    reports=[];seen=set()
    for path in args.drafts:
        draft=json.loads(path.read_text(encoding='utf8'))
        if (draft.get('format'),draft.get('title'),draft.get('version'))!=('kyoei-translation-draft-v1','CUSA03562','01.03'):
            raise ValueError('Wrong draft identity')
        for resource in draft['resources']:
            name=resource['path']; data=(root/name).read_bytes()
            if resource['source_sha256']!=rows[name]['sha256'] or hashlib.sha256(data).hexdigest()!=rows[name]['sha256']:
                raise ValueError('Text source changed')
            originals={row['id']:row['original'] for row in export(data)['strings']}
            for entry in resource['entries']:
                key=(name,entry['id'])
                if key in seen: raise ValueError('Duplicate translation entry')
                seen.add(key)
                translated=entry[args.language]; original=originals[entry['id']]
                measurements={}
                for font,glyphs in tables.items():
                    source=measure(original,glyphs); target=measure(translated,glyphs)
                    ratio=None
                    if source['complete_static_measurement'] and target['complete_static_measurement'] and source['max_static_advance']:
                        ratio=round(target['max_static_advance']/source['max_static_advance'],3)
                    # No original text is copied into the repository report.
                    measurements[font]=dict(source=source,translation=target,growth_ratio=ratio)
                reports.append(dict(resource=name,id=entry['id'],language=args.language,measurements=measurements))
    report=dict(scope='Sum of inferred native advances; no kerning, scale, wrapping, clipping or engine measurement',
        renderer_font_verified=False,runtime_verified=False,records=reports)
    args.report.write_text(json.dumps(report,ensure_ascii=True,indent=2)+'\n',encoding='utf8')
    print(f'{len(reports)} entries measured against two font tables; estimates only; report: {args.report}')


if __name__=='__main__':main()
