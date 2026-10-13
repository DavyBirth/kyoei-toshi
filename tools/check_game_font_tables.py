"""Measure glyph-key coverage in native CSH atlas tables, without execution."""
import hashlib
import json
import math
from pathlib import Path
import struct
from text_tool import REPO,decode

def glyph_rows(data):
    resource,table=decode(data)
    if table.columns!=9: raise ValueError('Unexpected glyph table columns')
    result={}; invalid=0; duplicates=0
    for row in range(table.rows):
        values=[]; raw_key=None
        for column in range(9):
            pointer,tag=struct.unpack_from('>II',resource.payload,12+(row*9+column)*8)
            if tag!=(0x40000001 if column<4 else 0x80000001):
                raise ValueError('Unexpected glyph cell type')
            raw=resource.payload[pointer:pointer+4]
            if column==0: raw_key=raw
            value=struct.unpack('>I' if column<4 else '>f',raw)[0]
            if column>=4 and not math.isfinite(value): raise ValueError('Nonfinite glyph metric')
            values.append(value)
        try: character=raw_key.rstrip(b'\0').decode('utf8')
        except UnicodeDecodeError: invalid+=1; continue
        if len(character)!=1: invalid+=1; continue
        if character in result:
            duplicates+=1
            continue
        result[character]=dict(row=row,packed_utf8=raw_key.hex(),page=values[1],x=values[2],y=values[3],
            width=values[4],height=values[5],offset_x=values[6],offset_y=values[7],advance=values[8])
    return result,invalid,table.rows,duplicates

def main():
    required=json.loads((REPO/'docs/m3/fallback-coverage.json').read_text(encoding='utf8'))['required_characters']
    audit=json.loads((REPO/'docs/m2/audit.json').read_text(encoding='utf8'))
    reports=[]
    for row in audit['records']:
        if '/font/' not in row['path'] and '/texture/font' not in row['path']: continue
        if not row['path'].endswith('_table.csh') or 'icon_table' in row['path']: continue
        data=(Path(audit['source'])/row['path']).read_bytes()
        if hashlib.sha256(data).hexdigest()!=row['sha256']: raise ValueError('Audited source differs')
        glyphs,invalid,count,duplicates=glyph_rows(data)
        reports.append(dict(resource=row['path'],source_sha256=row['sha256'],rows=count,
            decoded_glyph_keys=len(glyphs),invalid_keys=invalid,duplicate_keys=duplicates,
            missing_required=sorted(set(required)-glyphs.keys()),
            required_glyphs={char:glyphs[char] for char in required if char in glyphs}))
        print(row['path'],f'keys={len(glyphs)} required={len(required)} missing={len(set(required)-glyphs.keys())}')
    if not reports: raise ValueError('No native font tables audited')
    report=dict(scope='Native glyph-key tables; metrics inferred from measured cells',
        key_encoding='UTF-8 bytes packed into four bytes with trailing zero padding',
        required_characters=required,atlas_pixels_verified=False,runtime_verified=False,tables=reports)
    (REPO/'docs/m3/game-font-coverage.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf8')

if __name__=='__main__': main()
