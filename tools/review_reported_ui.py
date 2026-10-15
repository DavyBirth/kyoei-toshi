"""Private contact sheet for the owner's reported mail/pause UI defects."""
from pathlib import Path
from PIL import Image, ImageDraw
from tex_preview import preview

if __name__ == '__main__':
    root = Path('G:/kyoei-toshi-work/m1/common')
    files = list((root/'texture/menu_mail').glob('*.tex'))
    files += list((root/'texture/hud').glob('status*.tex'))
    files += [p for p in (root/'texture/intermission').glob('*.tex')
              if any(n in p.name for n in ('header', 'time', 'weather', 'money', 'coin'))]
    sheet = Image.new('RGB', (1500, ((len(files)+2)//3)*260), '#404044')
    draw = ImageDraw.Draw(sheet)
    for n, path in enumerate(files):
        im, _ = preview(path.read_bytes()); im = im.convert('RGBA'); im.thumbnail((480,225))
        x, y = (n%3)*500, (n//3)*260
        sheet.paste(im, (x,y+25), im)
        draw.text((x+4,y+3), path.name, fill='white')
    sheet.save('build/reported-ui-originals.png')
