"""Generate a private ASCII metric cache from both audited native font tables."""
import argparse
import hashlib
import json
import math
import struct
from pathlib import Path
from check_game_font_tables import glyph_rows
from text_tool import REPO


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output',type=Path)
    args=parser.parse_args()
    # Font-derived metadata is private and never goes in a tracked patch.
    target=args.output.resolve()
    if not target.is_relative_to((REPO/'build').resolve()) or target.exists():
        raise ValueError('Choose a new private file under build/')
    audit=json.loads((REPO/'docs/m2/audit.json').read_text(encoding='utf8'))
    records={r['path']:r for r in audit['records']}
    tables=[]
    for name in ['common/font/utf8all_table.csh','common/font/utf8all_IM_table.csh']:
        data=(Path(audit['source'])/name).read_bytes()
        if hashlib.sha256(data).hexdigest()!=records[name]['sha256']:
            raise ValueError('Native font table changed')
        glyphs=glyph_rows(data)[0]
        nominal=glyphs['\u3042']['advance']
        if not math.isfinite(nominal) or nominal<=0: raise ValueError('Invalid nominal advance')
        tables.append([glyphs[chr(ch)]['advance']/nominal for ch in range(32,127)])
    advances=[max(pair) for pair in zip(*tables)]
    if any(not math.isfinite(v) or v<=0 or v>4 for v in advances):
        raise ValueError('Invalid normalized native advances')
    target.parent.mkdir(parents=True,exist_ok=True)
    target.write_bytes(b'KHF1'+struct.pack('<I95f',95,*advances))
    print(f'Private native metrics: 95 ASCII glyphs, max of two fonts; {target}')


if __name__=='__main__': main()
