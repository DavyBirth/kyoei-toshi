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


def glyph(image, reference, circle=False, glow=False, blue=False):
    """Transplant the native glyph, preserving the opaque pin/button interior."""
    image = image.convert('RGBA')
    pixels = list(image.getdata())
    # Only the face symbol's own hue: a green glow around the button is not the glyph.
    colored = [i for i, (r,g,b,a) in enumerate(pixels)
               if a > 8 and max(r,g,b)-min(r,g,b) > 18 and not (g > r+12 and g >= b)]
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
    chroma=lambda p:max(0,p[2]-p[0]) if circle or blue else max(0,p[0]-p[2])
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
        # Remove only the native symbol's pixels and let the surrounding button body,
        # edge and glow flow back in (a flat disc left a hard cut-out edge).
        holes={(i%w+dx,i//w+dy) for i in colored for dx in (-1,0,1) for dy in (-1,0,1)
               if 0<=i%w+dx<w and 0<=i//w+dy<h}
        image=refill(image,holes,(left-2,top-2,right+3,bottom+3))
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


import numpy as np


def refill(im, holes, box, iters=600):
    """Diffuse premultiplied colour into cleared pixels so soft halos continue under removed glyphs."""
    x0, y0, x1, y1 = box
    # Work on a slightly larger window so real pixels around the box seed the fill.
    x0, y0 = max(0, x0 - 4), max(0, y0 - 4); x1, y1 = min(im.width, x1 + 4), min(im.height, y1 + 4)
    a = np.asarray(im, dtype=np.float32).copy()
    sub = a[y0:y1, x0:x1]; pm = sub.copy(); pm[..., :3] *= pm[..., 3:4] / 255
    mask = np.zeros(sub.shape[:2], bool)
    for x, y in holes: mask[y - y0, x - x0] = True
    # Solid leader lines must not bleed into the fill; treat them as unknown, then put them back.
    g, mx = sub[..., 1], np.maximum(sub[..., 0], sub[..., 2])
    strong = (sub[..., 3] > 150) & (g > 90) & (g > 1.4 * mx)
    near = strong.copy()
    for _ in range(3):
        n = near.copy(); n[1:] |= near[:-1]; n[:-1] |= near[1:]; n[:, 1:] |= near[:, :-1]; n[:, :-1] |= near[:, 1:]; near = n
    # Leader lines and their anti-aliased edges, plus coloured button icons, are kept but never seed the fill.
    icon = (sub[..., 3] > 60) & (sub[..., :3].max(axis=2) - sub[..., :3].min(axis=2) > 35) & ~(g > 1.2 * mx)
    line = ((near & (sub[..., 3] > 20) & (g > 1.15 * mx)) | icon) & ~mask
    keep = sub.copy(); mask |= line; pm[mask] = 0
    for _ in range(iters):
        p = np.pad(pm, ((1, 1), (1, 1), (0, 0)), mode='edge')
        avg = (p[1:-1, :-2] + p[1:-1, 2:] + p[:-2, 1:-1] + p[2:, 1:-1]) / 4
        pm[mask] = avg[mask]
    al = pm[..., 3:4]; rgb = np.where(al > 0.5, pm[..., :3] * 255 / np.maximum(al, 1e-3), 0)
    sub[mask] = np.concatenate([rgb, al], axis=2)[mask]
    sub[line] = keep[line]
    a[y0:y1, x0:x1] = sub
    return Image.fromarray(np.clip(a + 0.5, 0, 255).astype(np.uint8), 'RGBA').copy()


def ring_cross(image, center, ring, stroke):
    """'Press ◎ Button': remove only the thin outer ring (rebuilt from the striped banner
    around it), keep the bold inner ring and draw the white cross inside it: the ⊗ used
    by the control screens."""
    cx, cy = center
    holes = [(x, y) for y in range(int(cy - 17), int(cy + 18)) for x in range(int(cx - 17), int(cx + 18))
             if 12.5 ** 2 <= (x - cx) ** 2 + (y - cy) ** 2 <= 16.5 ** 2]
    out = image.convert('RGBA').copy(); px = out.load(); src = image.convert('RGBA').load()
    # The banner's stripes run diagonally (constant along x+t, y-t): follow the stripe in both
    # directions to clean pixels outside the glyph and blend them by distance.
    for x, y in holes:
        ends = []
        for sgn in (1, -1):
            t = 1
            while (x + sgn * t - cx) ** 2 + (y - sgn * t - cy) ** 2 <= 18.5 ** 2: t += 1
            ends.append((t, src[x + sgn * t, y - sgn * t]))
        (t1, a), (t2, b) = ends; w1 = t2 / (t1 + t2)
        px[x, y] = tuple(round(a[i] * w1 + b[i] * (1 - w1)) for i in range(4))
    S = 4; arm = ring * 0.5
    layer = Image.new('RGBA', (out.width * S, out.height * S)); d = ImageDraw.Draw(layer)
    for sx in (-1, 1):
        d.line([((cx - sx * arm) * S, (cy - arm) * S), ((cx + sx * arm) * S, (cy + arm) * S)], fill=(255, 255, 255, 255), width=round(stroke * S))
    out.alpha_composite(layer.resize(out.size, Image.Resampling.LANCZOS))
    return out


def build(originals):
    out={}
    cross=preview(originals['font/icon/IC_batu_2.tex'])[0]
    circle=preview(originals['font/icon/IC_maru_1.tex'])[0]
    big_cross=preview(originals['texture/intermission/IM_next_batu_button.tex'])[0]
    for a,b in PAIRS:
        ia,pa=unpack_tex(originals[a]);ib,pb=unpack_tex(originals[b])
        layout=lambda i:tuple(i[k] for k in ('width','height','levels','storage_family','channel_layout'))
        if layout(ia)!=layout(ib) or len(pa)!=len(pb):
            raise ValueError('Face-button layouts differ: '+a)
        out[a]=originals[b];out[b]=originals[a]
    from prompt_footers import FOOTERS,draw as footer
    for name,original in originals.items():
        if name in FOOTERS:
            image=footer(name,preview(original)[0].convert('RGBA'),'american',dict(maru=circle,batu=cross))
        elif name=='texture/hud/maru_button.tex' or name.startswith('texture/text marker/talk_icon_'):
            # The large native blue cross (same family as the swapped dialog buttons) keeps the
            # stroke weight and the game's cross color; the 13 px font glyph came out thin and salmon.
            image=glyph(preview(original)[0],big_cross,blue=True)
        elif name in ('texture/hud/cursol_nextchat.tex', 'texture/stage_clear/sc_next.tex'):
            image=glyph(preview(original)[0],big_cross,blue=True)
        elif name=='texture/hud/resalt_next.tex':
            image=ring_cross(preview(original)[0],(251.5,62.5),8.5,2.4)
        elif name=='texture/intermission/x_btn.tex':
            image=circle.resize(preview(original)[0].size,Image.Resampling.LANCZOS)
        elif name.startswith('texture/hud/action_kaihi_batu_'):
            image=glyph(preview(original)[0],circle,circle=True,glow=name.endswith('_2.tex'))
        else:
            continue
        out[name]=encode_patch(original,image)
    return out
