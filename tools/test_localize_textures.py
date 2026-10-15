"""Regression for the reported option-button scars and inconsistent label sizes."""
from PIL import Image,ImageDraw,ImageFont
from localize_textures import FONT,label,relabel

for selected in (False,True):
    original=Image.new('RGBA',(208,44));pixels=original.load()
    for y in range(44):
        v=(180 if selected else 18)+y//3
        for x in range(208):pixels[x,y]=(v,v,v,255)
    draw=ImageDraw.Draw(original);draw.rectangle((1,1,206,42),outline=(240,240,240,255),width=2)
    font=ImageFont.truetype(str(FONT),21)
    draw.text((42,5),'XXXXXXXXXX',font=font,fill=(0,0,0,255) if selected else (255,255,255,255))
    translated=relabel(original,'On','center',12,21)
    # All frame/corner pixels remain exact, with no solid-fill replacement.
    for y in range(44):
        for x in range(208):
            if x<12 or x>=196 or y<4 or y>=40:assert translated.getpixel((x,y))==original.getpixel((x,y))
    # Removed old glyphs at the sides must reveal the original row gradient.
    for y in range(4,40):
        for x in range(42,75):assert translated.getpixel((x,y))==original.getpixel((12,y))

try:
    label(Image.new('RGBA',(24,36),(255,255,255,255)),'Too long for this label',19)
except ValueError:pass
else:raise AssertionError('Long labels must fail instead of silently changing font size')
print('OPTION TEXTURES PASS: original frames and row gradients preserved; fixed-size overflow rejected')
