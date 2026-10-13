"""Audit native font tables/pages directly in the owned PSARC; never edit/extract pixels."""
import argparse
import collections
import hashlib
import json
import math
from pathlib import Path
import re
from check_game_font_tables import glyph_rows
from psarc import Psarc
from tex_preview import unpack_tex
from text_tool import REPO


def audit(archive_path):
    textures={};tables=[]
    with Psarc(archive_path) as archive:
        selected=[entry for entry in archive.entries if
                  re.fullmatch(r'font/utf8all(?:_IM)?_\d+\.tex',entry.name) or
                  re.fullmatch(r'font/(?:[^/]+/)?utf8all(?:_IM)?_table\.csh',entry.name)]
        for entry in selected:
            data=b''.join(archive.chunks(entry))
            digest=hashlib.sha256(data).hexdigest()
            if entry.name.endswith('.tex'):
                info,_=unpack_tex(data)
                textures[entry.name]=dict(resource=entry.name,sha256=digest,bytes=len(data),**info)
            else:
                glyphs,invalid,rows,duplicates=glyph_rows(data)
                counts=collections.Counter(glyph['page'] for glyph in glyphs.values())
                prefix=entry.name.removesuffix('_table.csh')
                page_rows=[]
                for page,count in sorted(counts.items()):
                    gs=[g for g in glyphs.values() if g['page']==page]
                    page_rows.append(dict(page=page,glyphs=count,expected_resource=f'{prefix}_{page}.tex',
                        max_right=max(g['x']+math.ceil(g['width']) for g in gs),
                        max_bottom=max(g['y']+math.ceil(g['height']) for g in gs),
                        invalid_rectangles=sum(g['x']<0 or g['y']<0 or g['width']<0 or g['height']<0 for g in gs)))
                tables.append(dict(resource=entry.name,sha256=digest,rows=rows,invalid_keys=invalid,
                                   duplicate_keys=duplicates,pages=page_rows))
    for table in tables:
        for page in table['pages']:
            texture=textures.get(page['expected_resource'])
            page['same_prefix_present']=texture is not None
            page['bounds_verified']=bool(texture and not page['invalid_rectangles'] and
                page['max_right']<=texture['width'] and page['max_bottom']<=texture['height'])
    if not tables or not textures:raise ValueError('No measured native font resources found')
    return dict(scope='Read-only font page inventory. Same-prefix mapping only; variant sharing/runtime selection unverified.',
                archive=str(archive_path),runtime_verified=False,tables=sorted(tables,key=lambda t:t['resource']),
                textures=sorted(textures.values(),key=lambda t:t['resource']))


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    output=args.output.resolve()
    if output.exists() or not output.is_relative_to((REPO/'build').resolve()):
        raise ValueError('Choose a new metadata file under build/')
    inventory=json.loads((REPO/'docs/m0/inventory.json').read_text(encoding='utf8'))
    report=audit(Path(inventory['source'])/'DAT/common.psarc')
    output.parent.mkdir(parents=True,exist_ok=True)
    output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print(f'{len(report["tables"])} tables; {len(report["textures"])} texture pages; {output}')
    for table in report['tables']:
        verified=sum(p['bounds_verified'] for p in table['pages'])
        print(f'{table["resource"]}: {table["rows"]} glyphs, {len(table["pages"])} pages; {verified} same-prefix pages bounded')


if __name__=='__main__':main()
