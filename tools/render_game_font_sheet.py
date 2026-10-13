"""Inspect native atlas glyphs at inferred table coordinates; no modifications."""
import argparse
import hashlib
import json
import math
from pathlib import Path
from PIL import Image,ImageDraw,ImageFont
from text_tool import REPO,output_path

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('atlas',type=Path); parser.add_argument('output',type=Path)
    args=parser.parse_args(); output=output_path(args.output)
    coverage=json.loads((REPO/'docs/m3/game-font-coverage.json').read_text(encoding='utf8'))
    table=next(row for row in coverage['tables'] if row['resource']=='common/font/utf8all_table.csh')
    font_path=REPO/'third_party/godzilla-core/third_party/shadPS4/src/imgui/renderer/fonts/NotoSans-Regular.ttf'
    label_font=ImageFont.truetype(str(font_path),16)
    characters=list(coverage['required_characters']); width,height=800,math.ceil(len(characters)/10)*100
    sheet=Image.new('RGB',(width,height),'#303845'); draw=ImageDraw.Draw(sheet); verified=[]
    with Image.open(args.atlas) as atlas:
        for index,char in enumerate(characters):
            glyph=table['required_glyphs'][char]; x=index%10*80; y=index//10*100
            draw.text((x+6,y+76),f'{char} {ord(char):04X}',fill='white',font=label_font)
            if glyph['page']!=0: draw.text((x+5,y+8),'Other page',fill='orange'); continue
            left,top=int(glyph['x']),int(glyph['y']); w,h=math.ceil(glyph['width']),math.ceil(glyph['height'])
            if left<0 or top<0 or w<0 or h<0 or left+w>atlas.width or top+h>atlas.height:
                raise ValueError('Glyph rectangle outside atlas')
            crop=atlas.crop((left,top,left+w,top+h)).convert('RGBA'); crop.thumbnail((72,68))
            sheet.paste(crop,(x+(80-crop.width)//2,y+6+(68-crop.height)//2),crop)
            verified.append(dict(character=char,codepoint=f'U+{ord(char):04X}',glyph_row=glyph['row'],
                                 rectangle=[left,top,w,h],rgba_extrema=crop.getextrema()))
    output.parent.mkdir(parents=True,exist_ok=True); sheet.save(output)
    report=dict(private_sheet=str(output),private_atlas=str(args.atlas),
        atlas_png_sha256=hashlib.sha256(args.atlas.read_bytes()).hexdigest(),table_source_sha256=table['source_sha256'],
        checked_rectangles=verified,visual_review_pending=True,runtime_verified=False)
    (REPO/'docs/m3/native-glyph-sheet.json').write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf8')
    print(f'{len(verified)} glyph rectangles within atlas bounds; private sheet: {output}')

if __name__=='__main__': main()
