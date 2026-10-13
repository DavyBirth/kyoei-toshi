"""Generate private diagnostic sheets from decoded previews; no asset edits."""
import argparse
import json
from pathlib import Path
from PIL import Image,ImageDraw
from text_tool import output_path

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('inventory',type=Path); parser.add_argument('output',type=Path)
    parser.add_argument('--group',action='append',required=True)
    args=parser.parse_args(); output=output_path(args.output)
    if output.exists(): raise ValueError('Choose a new private output')
    assets=json.loads(args.inventory.read_text(encoding='utf8'))
    selected=[a for a in assets if len(a['resource'].split('/'))>2 and a['resource'].split('/')[1] in args.group and 'preview' in a]
    output.mkdir(parents=True); manifests=[]
    for start in range(0,len(selected),36):
        page=selected[start:start+36]; sheet=Image.new('RGB',(1440,1320),'#252d39'); draw=ImageDraw.Draw(sheet)
        for index,asset in enumerate(page):
            column=index%6; row=index//6; x=column*240; y=row*220
            with Image.open(args.inventory.parent/asset['preview']) as original:
                image=original.convert('RGBA'); image.thumbnail((224,172))
                sheet.paste(image,(x+(240-image.width)//2,y+8+(172-image.height)//2),image)
            label=f'{start+index:04d} '+Path(asset['resource']).stem
            draw.text((x+6,y+182),label[:31],fill='white')
            draw.text((x+6,y+198),asset['id'],fill='#b5c7db')
        filename=f'sheet-{start//36:03d}.png'; sheet.save(output/filename)
        manifests.append(dict(sheet=filename,assets=[dict(index=start+i,id=a['id'],resource=a['resource'],source_sha256=a['source_sha256']) for i,a in enumerate(page)]))
    (output/'manifest.json').write_text(json.dumps(manifests,indent=2)+'\n',encoding='utf8')
    print(f'{len(selected)} previews in {len(manifests)} sheets: {output}')

if __name__=='__main__': main()
