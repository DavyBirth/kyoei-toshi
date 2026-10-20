"""Swap face-button artwork from the owner's archive; keep derivatives private."""
from PIL import Image, ImageDraw, ImageFilter, ImageChops
from collections import Counter
import struct
import zlib
from tex_preview import preview, unpack_tex
from make_keyboard_icons import encode_rgba
from localize_textures import encode_tex

PAIRS = (
    ('font/icon/IC_maru_1.tex', 'font/icon/IC_batu_2.tex'),
    ('texture/treasure_change/TC_maru_01.tex', 'texture/treasure_change/TC_batu_01.tex'),
    ('texture/intermission/IM_next_maru_button.tex', 'texture/intermission/IM_next_batu_button.tex'),
    ('texture/intermission/IM_dialog_o_btn.tex', 'texture/intermission/IM_dialog_x_btn.tex'),
)


def encode_patch(original, image):
    """Keep untouched native mip blocks byte-identical, including tall stems."""
    info,body=unpack_tex(original)
    encoded=encode_tex(original,image,info) if info['storage_family']==13 else encode_rgba(original,image)
    _,candidate=unpack_tex(encoded)
    result=bytearray(body)
    before=preview(original)[0].convert('RGBA');after=image.convert('RGBA')
    at=0;block=4 if info['storage_family']==13 else 1
    unit=16 if block==4 else 4
    for _ in range(info['levels']):
        difference=ImageChops.difference(before,after)
        mask=difference.getchannel('R')
        for channel in ('G','B','A'): mask=ImageChops.lighter(mask,difference.getchannel(channel))
        box=mask.getbbox()
        columns=(before.width+block-1)//block
        rows=(before.height+block-1)//block
        if box:
            x0,y0,x1,y1=box
            for y in range(y0//block,(y1+block-1)//block):
                start=at+(y*columns+x0//block)*unit
                end=at+(y*columns+(x1+block-1)//block)*unit
                result[start:end]=candidate[start:end]
        at+=columns*rows*unit
        size=(max(1,before.width//2),max(1,before.height//2))
        before=before.resize(size,Image.Resampling.LANCZOS);after=after.resize(size,Image.Resampling.LANCZOS)
    if at!=len(body) or len(result)!=len(body): raise ValueError('Native mip layout differs')
    raw=bytes.fromhex(info['descriptor'])+result
    compressed=zlib.compress(raw,9);head=bytearray(original[:144])
    magic,_,_,flags=struct.unpack_from('>4sIII',head,128)
    struct.pack_into('>4sIII',head,128,magic,len(raw),len(compressed),flags)
    return bytes(head)+compressed


def glyph(image, reference, circle=False, glow=False):
    """Transplant the native glyph, preserving the opaque pin/button interior."""
    image = image.convert('RGBA')
    pixels = list(image.getdata())
    colored = [i for i, (r,g,b,a) in enumerate(pixels)
               if a > 8 and max(r,g,b)-min(r,g,b) > 18]
    if not colored:
        raise ValueError('Expected a colored face-button glyph')
    w,h = image.size
    maximum=max(pixels[i][3] for i in colored)
    solid = [i for i in colored if pixels[i][3] >= maximum*.8]
    left,right = min(i%w for i in solid),max(i%w for i in solid)
    top,bottom = min(i//w for i in solid),max(i//w for i in solid)
    reference=reference.convert('RGBA')
    ref_pixels=list(reference.getdata())
    # Font faces have a neutral shared button body. Their colored glyph's
    # chroma gives its antialiased coverage without copying the button body.
    chroma=lambda p:max(0,p[2]-p[0]) if circle else max(0,p[0]-p[2])
    peak=max(chroma(p) for p in ref_pixels if p[3]>160)
    mask=Image.new('L',reference.size)
    mask.putdata([round(max(0,min(1,(chroma(p)-2)/(peak-2)))*p[3]) for p in ref_pixels])
    bounds=mask.point(lambda a:255 if a>100 else 0).getbbox()
    if not bounds: raise ValueError('Native face glyph missing')
    bounds=(max(0,bounds[0]-1),max(0,bounds[1]-1),min(reference.width,bounds[2]+1),min(reference.height,bounds[3]+1))
    mask=mask.crop(bounds)
    cx,cy=(left+right)/2,(top+bottom)/2
    size=max(right-left+1,bottom-top+1)
    if circle:
        color=Counter(pixels[i] for i in solid).most_common(1)[0][0]
        image=Image.new('RGBA',image.size)
    else:
        # The original center is the real opaque backing, not transparent.
        backing=image.getpixel((round(cx),round(cy)))
        radius=size/2+2
        ImageDraw.Draw(image).ellipse((cx-radius,cy-radius,cx+radius,cy+radius),fill=backing)
        color=Counter(p for p in ref_pixels if p[3]>160 and chroma(p)>=peak-2).most_common(1)[0][0]
        size=round(size*.8) # Native cross is smaller than the circle.
    mask=mask.resize((size,size),Image.Resampling.LANCZOS)
    if color[3]!=255: mask=mask.point(lambda a:round(a*color[3]/255))
    face=Image.new('RGBA',mask.size,color[:3]+(255,));face.putalpha(mask)
    layer=Image.new('RGBA',(w,h))
    layer.alpha_composite(face,(round(cx-(size-1)/2),round(cy-(size-1)/2)))
    if glow:
        image.alpha_composite(layer.filter(ImageFilter.GaussianBlur(3)))
    image.alpha_composite(layer)
    return image


def build(originals):
    out={}
    cross=preview(originals['font/icon/IC_batu_2.tex'])[0]
    circle=preview(originals['font/icon/IC_maru_1.tex'])[0]
    for a,b in PAIRS:
        ia,pa=unpack_tex(originals[a]);ib,pb=unpack_tex(originals[b])
        layout=lambda i:tuple(i[k] for k in ('width','height','levels','storage_family','channel_layout'))
        if layout(ia)!=layout(ib) or len(pa)!=len(pb):
            raise ValueError('Face-button layouts differ: '+a)
        out[a]=originals[b];out[b]=originals[a]
    for name,original in originals.items():
        if name=='texture/hud/maru_button.tex' or name.startswith('texture/text marker/talk_icon_'):
            image=glyph(preview(original)[0],cross)
        elif name.startswith('texture/hud/action_kaihi_batu_'):
            image=glyph(preview(original)[0],circle,circle=True,glow=name.endswith('_2.tex'))
        else:
            continue
        out[name]=encode_patch(original,image)
    return out
