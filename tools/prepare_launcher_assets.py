"""Convert the owner's PS4 application artwork into local Windows icon assets."""
import hashlib
import json
from pathlib import Path
import shutil
from PIL import Image
ROOT=Path(__file__).resolve().parents[1]

def main():
    inventory=json.loads((ROOT/'docs/m0/inventory.json').read_text(encoding='utf8'))
    source=Path(inventory['source'])/'sce_sys/icon0.png'
    destination=ROOT/'build/launcher-assets';destination.mkdir(parents=True,exist_ok=True)
    with Image.open(source) as image:
        if image.format!='PNG' or image.size!=(512,512):raise ValueError('Unexpected PS4 icon')
        image.convert('RGBA').save(destination/'game.ico',sizes=[(n,n) for n in (16,24,32,48,64,128,256)])
    shutil.copyfile(source,destination/'artwork.png')
    report=dict(title='CUSA03562',source='sce_sys/icon0.png',source_sha256=hashlib.sha256(source.read_bytes()).hexdigest(),
                local_assets='build/launcher-assets',original_art_distributed=False,artwork_modified=False)
    (ROOT/'docs/launcher-artwork.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print('Local launcher artwork and multi-size Windows icon prepared.')

if __name__=='__main__':main()
