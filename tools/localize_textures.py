"""Build English versions of text-bearing TEX images (BC3 + mip chain) from the user's own dump.

Replaces the Japanese label with an English one drawn over the original background.
Output files mirror archive paths (e.g. OUT/texture/menu_option/M_option_01.tex).
Generated textures are private derivatives of the dump and must never be distributed;
only this tool and the label table are part of the patch.
"""
import argparse, io, json, os, struct, sys, zlib
import numpy as np
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont, ImageFilter, ImageChops
sys.path.insert(0, str(Path(__file__).parent))
from tex_preview import unpack_tex, dds_bc

REPO = Path(__file__).resolve().parents[1]
_NOTO = 'third_party/godzilla-core/third_party/shadPS4/src/imgui/renderer/fonts/NotoSans-Regular.ttf'
FONT = next((p for p in (REPO / _NOTO,REPO/'assets/fonts/NotoSans-Regular.ttf') if p.exists()), REPO / _NOTO)
FONTS = {'sans': FONT,
         'bold': next((p for p in (Path('C:/Windows/Fonts/segoeuib.ttf'),FONT) if p.exists()),FONT),
         'serif': next((p for p in (Path('C:/Windows/Fonts/cambriab.ttf'),FONT) if p.exists()),FONT),
         # Title-menu art is drawn in a Mincho face; Noto Serif CJK's Latin matches it.
         'mincho': next((p for p in (Path('C:/Windows/Fonts/NotoSerifSC-VF.ttf'),Path('C:/Windows/Fonts/cambria.ttc'),FONT) if p.exists()),FONT)}


def decode(data):
    info, body = unpack_tex(data)
    w, h = info['width'], info['height']
    if info['storage_family'] == 1 and info['channel_layout'] in (0, 0x10):
        info['rgba8'] = True
        return info, Image.frombytes('RGBA', (w, h), body[:w * h * 4])
    if info['storage_family'] != 13: raise ValueError('Only BC3 and RGBA8 supported')
    count = ((w + 3) // 4) * ((h + 3) // 4) * 16
    return info, Image.open(io.BytesIO(dds_bc(w, h, body[:count], b'DXT5'))).convert('RGBA')


def bc3_block(px):
    # px: 16 RGBA tuples
    alphas = [p[3] for p in px]
    a0, a1 = max(alphas), min(alphas)
    if a0 == a1:
        aidx = [0] * 16
    else:
        aidx = []
        for a in alphas:
            t = round((a0 - a) * 7 / (a0 - a1))  # 0 -> a0, 7 -> a1
            aidx.append(0 if t == 0 else 1 if t == 7 else t + 1)
    abits = 0
    for i, v in enumerate(aidx): abits |= v << (3 * i)
    vis = [p for p in px if p[3] > 0] or px
    lum = [p[0] * 3 + p[1] * 6 + p[2] for p in vis]
    hi, lo = vis[lum.index(max(lum))], vis[lum.index(min(lum))]
    def c565(c): return ((c[0] >> 3) << 11) | ((c[1] >> 2) << 5) | (c[2] >> 3)
    def e565(v): return ((v >> 11 & 31) * 255 // 31, (v >> 5 & 63) * 255 // 63, (v & 31) * 255 // 31)
    c0, c1 = c565(hi), c565(lo)
    if c0 < c1: c0, c1 = c1, c0
    if c0 == c1:
        cbits = 0
    else:
        e0, e1 = e565(c0), e565(c1)
        pal = [e0, e1, tuple((2 * a + b) // 3 for a, b in zip(e0, e1)), tuple((a + 2 * b) // 3 for a, b in zip(e0, e1))]
        cbits = 0
        for i, p in enumerate(px):
            d = [sum((p[k] - q[k]) ** 2 for k in range(3)) for q in pal]
            cbits |= d.index(min(d)) << (2 * i)
    return struct.pack('<BB', a0, a1) + abits.to_bytes(6, 'little') + struct.pack('<HHI', c0, c1, cbits)


def encode_bc3(im):
    w, h = im.size
    pix = im.load(); out = bytearray()
    for by in range(0, h, 4):
        for bx in range(0, w, 4):
            out += bc3_block([pix[min(bx + x, w - 1), min(by + y, h - 1)] for y in range(4) for x in range(4)])
    return bytes(out)


def encode_tex(original, im, info):
    body = b''; level = im
    for _ in range(info['levels']):
        body += level.convert('RGBA').tobytes() if info.get('rgba8') else encode_bc3(level)
        level = level.resize((max(1, level.width // 2), max(1, level.height // 2)), Image.LANCZOS)
    raw = bytes.fromhex(info['descriptor']) + body
    old = struct.unpack_from('>4sIII', original, 128)
    packed = zlib.compress(raw, 9)
    head = bytearray(original[:144])
    struct.pack_into('>4sIII', head, 128, old[0], len(raw), len(packed), old[3])
    result = bytes(head) + packed
    if decode(result)[0]['payload_bytes'] != info['payload_bytes']: raise ValueError('Size mismatch')
    return result


def fit_font(text, size, avail):
    while True:
        font = ImageFont.truetype(str(FONT), size)
        bb = font.getbbox(text)
        if bb[2] - bb[0] <= avail or size <= 8: return font, bb
        size -= 1


def button_inner_rows(im, margin):
    """Find the two-pixel frame, excluding transparent atlas padding."""
    x = margin
    rows = [y for y in range(im.height)
            if im.getpixel((x, y))[3] > 128 and min(im.getpixel((x, y))[:3]) > 210]
    if not rows or rows[-1]-rows[0] < im.height//2:
        raise ValueError('Cannot identify the option button frame')
    return rows[0]+2, rows[-1]-1  # exclusive bottom; two-pixel horizontal strokes


def centered_glyph(text, font, top, bottom):
    """Position the raster's visible pixels, rather than the font's metrics."""
    bb = font.getbbox(text)
    mask = Image.new('L', (bb[2]-bb[0]+4, bb[3]-bb[1]+4))
    ImageDraw.Draw(mask).text((2-bb[0], 2-bb[1]), text, font=font, fill=255)
    box = mask.point(lambda v: 255 if v > 128 else 0).getbbox()
    if box is None:raise ValueError('Empty option text')
    visible_top = box[1]+bb[1]-2
    visible_height = box[3]-box[1]
    if visible_height > bottom-top:raise ValueError('Option text exceeds frame height')
    return top+(bottom-top-visible_height)//2-visible_top


def label(im, text, size):
    w, h = im.size
    alpha = im.getchannel('A').load()
    xs = [x for y in range(h) for x in range(w) if alpha[x, y] > 128]
    left = min(xs) if xs else 2
    font = ImageFont.truetype(str(FONT), size)
    bb = font.getbbox(text)
    if bb[2] - bb[0] > w - left - 3:
        raise ValueError(f'Label does not fit at the shared font size: {text}')
    # Match the source's visible white glyph height without widening English.
    red, green, blue, alpha = im.split()
    white = ImageChops.darker(ImageChops.darker(red, green), blue).point(lambda v: 255 if v > 180 else 0)
    mask = ImageChops.multiply(white, alpha)
    source_box = mask.point(lambda v: 255 if v > 128 else 0).getbbox()
    glyph = Image.new('RGBA', (bb[2]-bb[0], bb[3]-bb[1]), (0,0,0,0))
    ImageDraw.Draw(glyph).text((-bb[0], -bb[1]), text, font=font, fill='white')
    target_height = min(h-8, source_box[3]-source_box[1] if source_box else glyph.height)
    glyph = glyph.resize((glyph.width, target_height), Image.Resampling.LANCZOS)
    pos = (left, (h-target_height)//2)
    shadow = Image.new('RGBA', (w, h), (0, 0, 0, 0))
    shadow.paste((0,0,0,235), (pos[0]+1,pos[1]+2,pos[0]+1+glyph.width,pos[1]+2+glyph.height), glyph.getchannel('A'))
    shadow = shadow.filter(ImageFilter.GaussianBlur(2))
    out = Image.new('RGBA', (w, h), (0, 0, 0, 0)); out.alpha_composite(shadow)
    out.alpha_composite(glyph, pos)
    return out


def notice(im, item):
    """Plain opening notices; rebuild only the source's uniform text backdrop."""
    w,h=im.size
    background=im.getpixel((0,0))
    # These two audited screens contain text on one uniform opaque backdrop.
    if any(im.getpixel(p)!=background for p in ((w-1,0),(0,h-1),(w-1,h-1))):
        raise ValueError('Notice backdrop is no longer uniform at its corners')
    out=Image.new('RGBA',(w,h),background);draw=ImageDraw.Draw(out)
    font=ImageFont.truetype(str(FONT),item['size'])
    width=item['width'];line_height=item['line_height'];paragraphs=[]
    for paragraph in item['paragraphs']:
        lines=[];current=''
        for word in paragraph.split():
            if font.getlength(word)>width:raise ValueError('Notice word exceeds its text area')
            candidate=(current+' '+word).strip()
            if current and font.getlength(candidate)>width:lines.append(current);current=word
            else:current=candidate
        if current:lines.append(current)
        paragraphs.append(lines)
    total=sum(len(p)*line_height for p in paragraphs)+item.get('paragraph_gap',36)*(len(paragraphs)-1)
    top=item.get('top',(h-total)/2)
    if top+total>h-60:raise ValueError('Notice exceeds its vertical text area')
    if item.get('heading'):
        heading=ImageFont.truetype(str(FONT),76)
        draw.text(((w-heading.getlength(item['heading']))/2,155),item['heading'],font=heading,fill=(214,59,28,255))
    for paragraph in paragraphs:
        for line in paragraph:
            left=(w-font.getlength(line))/2 if item.get('align')=='center' else (w-width)/2
            draw.text((left,top),line,font=font,fill=tuple(item.get('color',[102,102,102,255])))
            top+=line_height
        top+=item.get('paragraph_gap',36)
    return out


SERIF = Path('C:/Windows/Fonts/cambriab.ttf')


def glow_text(out, pos, text, font):
    shadow = Image.new('RGBA', out.size, (0, 0, 0, 0))
    d = ImageDraw.Draw(shadow)
    for dx, dy in ((0, 0), (2, 2), (-2, 2), (2, -2), (-2, -2)):
        d.text((pos[0] + dx, pos[1] + dy), text, font=font, fill=(0, 0, 0, 255))
    out.alpha_composite(shadow.filter(ImageFilter.GaussianBlur(7)))
    ImageDraw.Draw(out).text(pos, text, font=font, fill=(255, 255, 255, 255))


def title(im, stage, text, size=50, left_aligned=False):
    """Stage title card: small 'Stage NN' line above a large centred title, white with dark glow."""
    w, h = im.size
    fontfile = str(SERIF if SERIF.exists() else FONT)
    out = Image.new('RGBA', (w, h), (0, 0, 0, 0))
    lines = [text]
    while True:
        font = ImageFont.truetype(fontfile, size)
        widths = [font.getbbox(l)[2] - font.getbbox(l)[0] for l in lines]
        if max(widths) <= w - 70: break
        if len(lines) == 1 and size <= 40:
            words = text.split(); best = None
            for i in range(1, len(words)):
                a, b = ' '.join(words[:i]), ' '.join(words[i:])
                m = max(font.getlength(a), font.getlength(b))
                if best is None or m < best[0]: best = (m, [a, b])
            lines = best[1]; size = 44; continue
        size -= 1
    if left_aligned:
        # Stage-clear card: small label top-left, title lines left-aligned below.
        small = ImageFont.truetype(fontfile, 26)
        glow_text(out, (12, 4), stage, small)
        top = 44 if len(lines) == 1 else 38; lh = size * 1.1
        for i, l in enumerate(lines): glow_text(out, (12, top + i * lh), l, font)
        return out
    small = ImageFont.truetype(fontfile, 40)
    lh = size * 1.15
    top = 100 if len(lines) == 1 else 72
    block_w = max(widths); left = (w - block_w) / 2
    glow_text(out, (left, 44 if len(lines) == 1 else 22), stage, small)
    for i, l in enumerate(lines):
        lw = font.getlength(l)
        glow_text(out, ((w - lw) / 2, top + (i * lh) + (0 if len(lines) == 1 else 4)), l, font)
    return out


def relabel(im, text, align, margin, size=20):
    w, h = im.size
    rgba = im.copy(); px = rgba.load()
    x0, y0, x1, y1 = margin, min(margin, 4), w - margin, h - min(margin, 4)
    region = [px[x, y] for y in range(y0, y1) for x in range(x0, x1)]
    # background = most common opaque-ish colour in region (quantised)
    hist = {}
    for p in region:
        k = (p[0] // 8, p[1] // 8, p[2] // 8, p[3] // 16); hist[k] = hist.get(k, 0) + 1
    bk = max(hist, key=hist.get)
    bgs = [p for p in region if (p[0] // 8, p[1] // 8, p[2] // 8, p[3] // 16) == bk]
    bg = tuple(sum(c[i] for c in bgs) // len(bgs) for i in range(4))
    def far(p): return sum(abs(p[i] - bg[i]) for i in range(3)) > 60
    mask = Image.new('L', (w, h)); mp = mask.load(); xs = []; ys = []
    for y in range(y0, y1):
        for x in range(x0, x1):
            if far(px[x, y]): mp[x, y] = 255; xs.append(x); ys.append(y)
    if not xs: raise ValueError('No text found')
    tx0, ty0, tx1, ty1 = min(xs), min(ys), max(xs), max(ys)
    lum = lambda p: p[0] * 3 + p[1] * 6 + p[2]
    textc = max((px[x, y] for x, y in zip(xs, ys)), key=lambda p: abs(lum(p) - lum(bg)))
    # The clean inner left column preserves the original row-by-row gradient.
    # Keep the frame/corners untouched, instead of filling glyph masks with
    # a single colour that leaves rectangular marks on selected buttons.
    for y in range(y0, y1):
        row = px[x0, y]
        for x in range(x0, x1):
            px[x, y] = row
    # fit font: cap height of original text ~ glyph box height
    avail = (x1 - x0 - 6) if align == 'center' else (x1 - tx0 - 2)
    font = ImageFont.truetype(str(FONT), size)
    bb = font.getbbox(text)
    if bb[2] - bb[0] > avail:
        raise ValueError(f'Button does not fit at the shared font size: {text}')
    d = ImageDraw.Draw(rgba)
    inner_top, inner_bottom = button_inner_rows(im, margin)
    tw = bb[2] - bb[0]
    x = (w - tw) / 2 - bb[0] if align == 'center' else tx0 - bb[0]
    y = centered_glyph(text, font, inner_top, inner_bottom)
    d.text((x, y), text, font=font, fill=(textc[0], textc[1], textc[2], 255))
    return rgba


def inpaint(im, box, threshold=40, grow=2, axis='h', iters=400):
    """Rebuild only glyph pixels from their surroundings, keeping glows and gradients."""
    x0, y0, x1, y1 = box
    a = np.asarray(im, dtype=np.float32).copy()
    sub = a[y0:y1, x0:x1]
    left, right = a[y0:y1, x0 - 1:x0], a[y0:y1, x1:x1 + 1]
    t = (np.arange(x1 - x0, dtype=np.float32) + 1) / (x1 - x0 + 1)
    model = left + (right - left) * t[None, :, None]
    mask = np.abs(sub - model).max(axis=2) > threshold
    for _ in range(grow):
        m = mask.copy()
        m[1:] |= mask[:-1]; m[:-1] |= mask[1:]; m[:, 1:] |= mask[:, :-1]; m[:, :-1] |= mask[:, 1:]
        mask = m
    fill = sub.copy(); fill[mask] = model[mask]
    for _ in range(iters):
        p = np.pad(fill, ((1, 1), (1, 1), (0, 0)), mode='edge')
        avg = (p[1:-1, :-2] + p[1:-1, 2:]) / 2 if axis == 'h' else (p[1:-1, :-2] + p[1:-1, 2:] + p[:-2, 1:-1] + p[2:, 1:-1]) / 4
        fill[mask] = avg[mask]
    a[y0:y1, x0:x1] = fill
    return Image.fromarray(np.clip(a + 0.5, 0, 255).astype(np.uint8), 'RGBA').copy()


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


def regions(im, item):
    """Erase listed boxes and draw English text in them; for multi-label UI art."""
    out = im.copy(); px = out.load(); src = im.load(); body = None
    blank = None
    if not item.get('_blank') and any(r.get('text') and r.get('anchor', True) for r in item['regions']):
        # The source glyphs were placed by the original artists against icons and frames;
        # English is anchored to where they were (same edge, same optical centre).
        blank = np.asarray(regions(im, dict(item, _blank=True, regions=[dict(r, text=None) for r in item['regions']]))).astype(int)
        orig = np.asarray(im).astype(int)
    for r in item['regions']:
        x0, y0, x1, y1 = r.get('box', (0, 0, im.width, im.height))
        if 'kill' in r:
            # Remove leader-line pixels (green strokes and their soft edges) and rebuild what lies beneath.
            a = np.asarray(out).astype(int); holes = []
            for bx0, by0, bx1, by1 in r['kill']:
                c = a[by0:by1, bx0:bx1]; gg, mm = c[..., 1], np.maximum(c[..., 0], c[..., 2])
                m = (c[..., 3] > 15) & (gg > 1.12 * mm) & (gg > 25) if not r.get('kill_all') else c[..., 3] >= 0
                holes += [(bx0 + x, by0 + y) for y, x in zip(*np.where(m))]
            for x, y in holes: px[x, y] = (0, 0, 0, 0)
            if holes:
                xs = [h[0] for h in holes]; ys = [h[1] for h in holes]
                out = refill(out, holes, (min(xs), min(ys), max(xs) + 1, max(ys) + 1)); px = out.load()
            if not r.get('text') and 'graft' not in r and 'paste' not in r: continue
        if 'graft' in r:
            # Copy native leader lines / button symbols from another screen of the same artwork.
            gv = r['graft']; other = np.asarray(decode((item['_source'] / gv['res']).read_bytes())[1]).astype(int)
            a = np.asarray(out).astype(int)
            for bx0, by0, bx1, by1 in gv['boxes']:
                o, c = other[by0:by1, bx0:bx1], a[by0:by1, bx0:bx1]
                m = np.abs(o - c).max(axis=2) > 24
                if gv.get('green_only'):
                    gg, mm = o[..., 1], np.maximum(o[..., 0], o[..., 2]); m &= (o[..., 3] > 15) & (gg > 1.12 * mm)
                c[m] = o[m]; a[by0:by1, bx0:bx1] = c
            out = Image.fromarray(a.astype(np.uint8), 'RGBA'); px = out.load()
            if not r.get('text') and 'paste' not in r: continue
        if 'paste' in r:
            # Replace a block with untouched source art (e.g. swap a button icon), keeping alpha exactly.
            pv = r['paste']; base = decode((item['_source'] / pv['res']).read_bytes())[1] if pv.get('res') else im
            out.paste(base.crop(tuple(pv['from'])), tuple(pv['to'])); px = out.load()
            if not r.get('text') and r.get('erase', 'none') == 'none': continue
        erase = r.get('erase', 'auto'); holes = []
        if erase == 'plate':
            # Copy the untouched background from another screen of the same artwork that has no text here,
            # then put back this screen's own leader lines.
            plate = decode((item['_source'] / r['plate']).read_bytes())[1]
            a = np.asarray(im)[y0:y1, x0:x1].astype(int); pl = np.asarray(plate)[y0:y1, x0:x1].astype(int)
            def lines_of(c):
                gg, mm = c[..., 1], np.maximum(c[..., 0], c[..., 2])
                st = (c[..., 3] > 150) & (gg > 90) & (gg > 1.4 * mm); nr = st.copy()
                for _ in range(2):
                    n = nr.copy(); n[1:] |= nr[:-1]; n[:-1] |= nr[1:]; n[:, 1:] |= nr[:, :-1]; n[:, :-1] |= nr[:, 1:]; nr = n
                return nr & (c[..., 3] > 20) & (gg > 1.15 * mm)
            own, theirs = lines_of(a), lines_of(pl)
            new = pl.copy(); new[own] = a[own]
            patch = Image.fromarray(new.astype(np.uint8), 'RGBA'); out.paste(patch, (x0, y0)); px = out.load()
            extra = theirs & ~own
            holes = [(x0 + x, y0 + y) for y, x in zip(*np.where(extra))]
            if holes: out = refill(out, holes, (x0, y0, x1, y1)); px = out.load(); holes = []
        if erase == 'keep_green':
            # Keep leader lines, their soft edges, coloured icons and the controller's large black body;
            # everything else in the box (glyphs and their dark halos) is rebuilt from the surroundings.
            if body is None:
                full = np.asarray(im).astype(int)
                black = ((full[..., :3].max(axis=2) < 40) & (full[..., 3] > 235)).astype(np.int32)
                c = np.pad(black, ((8, 8), (8, 8))).cumsum(0).cumsum(1)
                c = np.pad(c, ((1, 0), (1, 0)))
                dens = c[17:, 17:] - c[:-17, 17:] - c[17:, :-17] + c[:-17, :-17]
                body = (black == 1) & (dens[:full.shape[0], :full.shape[1]] > 0.8 * 289)
            a = np.asarray(im)[y0:y1, x0:x1].astype(int); gg, mm = a[..., 1], np.maximum(a[..., 0], a[..., 2])
            strong = (a[..., 3] > 150) & (gg > 90) & (gg > 1.4 * mm)
            near = strong.copy()
            for _ in range(2):
                n = near.copy(); n[1:] |= near[:-1]; n[:-1] |= near[1:]; n[:, 1:] |= near[:, :-1]; n[:, :-1] |= near[:, 1:]; near = n
            bd = body[y0:y1, x0:x1]; edge = bd.copy(); glow = bd.copy()
            for i in range(24):
                n = glow.copy(); n[1:] |= glow[:-1]; n[:-1] |= glow[1:]; n[:, 1:] |= glow[:, :-1]; n[:, :-1] |= glow[:, 1:]; glow = n
                if i == 2: edge = glow.copy()
            greenish = (a[..., 3] > 20) & (gg > 1.15 * mm)
            # The controller's white outline hugs its black body; its soft green glow spreads outward from it.
            keep_mask = (near & greenish) | bd | (edge & (a[..., :3].min(axis=2) > 150)) | (glow & greenish)
        if erase == 'glyphs':
            # Only the light glyph strokes plus a halo margin are removed, then rebuilt from the surrounding glow.
            a = np.asarray(im)[y0:y1, x0:x1].astype(int)
            m = (a[..., 3] > r.get('alpha_min', 120)) & (a[..., :3].min(axis=2) > r.get('light_min', 120))
            if r.get('red_halo'): m |= (a[..., 3] > 60) & (a[..., 0] > 90) & (a[..., 0] > 1.6 * a[..., 1])
            for _ in range(r.get('grow', 5)):
                g = m.copy(); g[1:] |= m[:-1]; g[:-1] |= m[1:]; g[:, 1:] |= m[:, :-1]; g[:, :-1] |= m[:, 1:]; m = g
            holes = [(x0 + x, y0 + y) for y, x in zip(*np.where(m))]
            for x, y in holes: px[x, y] = (0, 0, 0, 0)
            out = refill(out, holes, (x0, y0, x1, y1)); px = out.load(); holes = []
        elif erase == 'inpaint':
            out.paste(inpaint(out, (x0, y0, x1, y1), r.get('threshold', 40), r.get('grow', 2), r.get('axis', 'h')).crop((x0, y0, x1, y1)), (x0, y0))
            px = out.load()
        opaque = [src[x, y] for y in range(y0, y1) for x in range(x0, x1) if src[x, y][3] > 200]
        if erase == 'auto':
            # Replace glyph pixels (far from the box's dominant colour) with that colour.
            box = [src[x, y] for y in range(y0, y1) for x in range(x0, x1)]
            q = lambda p: (p[0] // 16, p[1] // 16, p[2] // 16, p[3] // 32) if p[3] > 16 else (0, 0, 0, 0)
            hist = {}
            for p in box: hist[q(p)] = hist.get(q(p), 0) + 1
            k = max(hist, key=hist.get); same = [p for p in box if q(p) == k]
            bg = tuple(sum(c[i] for c in same) // len(same) for i in range(4))
            far = lambda p: abs(p[3] - bg[3]) > 60 or (min(p[3], bg[3]) > 60 and sum(abs(p[i] - bg[i]) for i in range(3)) > 60)
            hit = {(x, y) for y in range(y0, y1) for x in range(x0, x1) if far(src[x, y])}
            glyph = {}
            for x, y in hit:
                p = src[x, y]
                if p[3] > 200: glyph[q(p)] = glyph.get(q(p), []) + [p]
            if glyph: opaque = max(glyph.values(), key=len)
            if r.get('keep_edges', True):
                # Glyphs float inside the box; frame strokes cross its border.
                seen = set()
                for start in list(hit):
                    if start in seen: continue
                    comp, stack = [], [start]; seen.add(start)
                    while stack:
                        x, y = stack.pop(); comp.append((x, y))
                        for n in ((x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)):
                            if n in hit and n not in seen: seen.add(n); stack.append(n)
                    if any(x in (x0, x1 - 1) or y in (y0, y1 - 1) for x, y in comp): hit -= set(comp)
            grow = {(x + dx, y + dy) for x, y in hit for dx in (-1, 0, 1) for dy in (-1, 0, 1)}
            for x, y in grow:
                if x0 <= x < x1 and y0 <= y < y1: px[x, y] = bg
        for y in range(y0, y1) if erase != 'plate' else ():
            for x in range(x0, x1):
                if erase == 'clear': px[x, y] = (0, 0, 0, 0)
                elif erase == 'keep_green':
                    if not keep_mask[y - y0, x - x0]: px[x, y] = (0, 0, 0, 0); holes.append((x, y))
                elif erase == 'mirror':
                    # Glow symmetric about the icon's centre row: copy the clean rows above it.
                    my = 2 * r['center'][1] - y
                    px[x, y] = src[x, my] if 0 <= my < im.height else (0, 0, 0, 0)
                elif erase == 'rotate':
                    # Radial glow around an icon: rebuild pixels below it from the glow beside it (quarter turn).
                    cx, cy = r['center']; sx = cx + (y - cy) * (1 if x >= cx else -1); sy = cy - abs(x - cx)
                    rot = src[sx, sy] if 0 <= sx < im.width and 0 <= sy < im.height else (0, 0, 0, 0)
                    f = min(1.0, (y - y0) / max(1, r.get('feather', 1)))
                    px[x, y] = tuple(round(src[x, y][i] * (1 - f) + rot[i] * f) for i in range(4))
                elif erase == 'row': px[x, y] = src[x0 - 1, y]
                elif erase == 'col': px[x, y] = src[x, y0 - 1]
                elif erase == 'vlerp':
                    a, b, t = src[x, y0 - 1], src[x, y1], (y - y0 + 1) / (y1 - y0 + 1)
                    px[x, y] = tuple(round(a[i] + (b[i] - a[i]) * t) for i in range(4))
                elif erase == 'hlerp':
                    a, b, t = src[x0 - 1, y], src[x1, y], (x - x0 + 1) / (x1 - x0 + 1)
                    px[x, y] = tuple(round(a[i] + (b[i] - a[i]) * t) for i in range(4))
                elif erase == 'fill': px[x, y] = tuple(r['fill'])
        if holes:
            out = refill(out, holes, (x0, y0, x1, y1)); px = out.load()
        if 'move' in r:
            # Re-place an untouched piece of the source art (e.g. a button cap) horizontally.
            mx0, my0, mx1, my1 = r['move']['from']
            piece = im.crop((mx0, my0, mx1, my1)); out.paste(piece, (mx0 + r['move']['dx'], my0)); px = out.load()
        text = r.get('text')
        if not text or os.environ.get('KYOEI_ERASE_ONLY'): continue
        if 'color' in r: color = tuple(r['color'])
        elif opaque: color = tuple(sum(p[i] for p in opaque) // len(opaque) for i in range(3)) + (255,)
        else: color = (255, 255, 255, 255)
        if 'text_box' in r: x0, y0, x1, y1 = r['text_box']
        size = r.get('size', 20)
        fontfile = str(FONTS[r.get('font', 'sans')])
        def wrap(font):
            explicit = text.split(chr(10))
            if not r.get('wrap') or max(font.getlength(l) for l in explicit) <= x1 - x0: return explicit
            if size > r.get('min_size', 15): return explicit  # shrink a little before re-wrapping
            out_lines = []
            for para in text.split(chr(10)):
                cur = ''
                for word in para.split(' '):
                    cand = (cur + ' ' + word).strip()
                    if cur and font.getlength(cand) > x1 - x0: out_lines.append(cur); cur = word
                    else: cur = cand
                out_lines.append(cur)
            return out_lines
        while True:
            font = ImageFont.truetype(fontfile, size)
            if 'weight' in r:
                try: font.set_variation_by_axes([r['weight']])
                except (OSError, ValueError): pass  # static fallback font
            lines = wrap(font)
            if (max(font.getlength(l) for l in lines) <= x1 - x0 and len(lines) <= r.get('max_lines', 99)) or size <= 7: break
            size -= 1
        if size < r.get('size', 20): print('shrunk', item['resource'], text, size, file=sys.stderr)
        asc, desc = font.getmetrics(); lh = r.get('line_height', asc + desc)
        # Centre on the visible cap height (like the source's glyphs), not on font metrics.
        cap = font.getbbox('H')
        if r.get('valign') == 'top': top = y0 - cap[1] + r.get('dy', 0)
        else: top = (y0 + y1) / 2 - (len(lines) - 1) * lh / 2 - (cap[1] + cap[3]) / 2 + r.get('dy', 0)
        layer = Image.new('RGBA', out.size, (0, 0, 0, 0)); d = ImageDraw.Draw(layer)
        pos = []
        for i, l in enumerate(lines):
            lw = font.getlength(l); align = r.get('align', 'center')
            x = x0 if align == 'left' else x1 - lw if align == 'right' else (x0 + x1 - lw) / 2
            pos.append((x, top + i * lh, l))
        if blank is not None and r.get('anchor', True):
            ax0, ay0, ax1, ay1 = r.get('anchor_box', r.get('box', [x0, y0, x1, y1]))
            ax0, ay0 = max(0, ax0), max(0, ay0); ax1, ay1 = min(out.width, ax1), min(out.height, ay1)
            a, b = orig[ay0:ay1, ax0:ax1], blank[ay0:ay1, ax0:ax1]
            m = (np.abs(a - b).max(axis=2) > 60) & (a[..., 3] > 120) & (a[..., :3].max(axis=2) > 110)
            m &= ~(a[..., 1] > 1.15 * np.maximum(a[..., 0], a[..., 2]))  # leader lines are not glyphs
            if m.sum() >= 8:
                ys, xs = np.where(m); jx0, jx1 = ax0 + xs.min(), ax0 + xs.max() + 1; jy0, jy1 = ay0 + ys.min(), ay0 + ys.max() + 1
                align = r.get('align', 'center'); new = []
                for i, (x, y, l) in enumerate(pos):
                    bb = font.getbbox(l)
                    if len(lines) == 1: ny = (jy0 + jy1) / 2 - (cap[1] + cap[3]) / 2
                    else: ny = jy0 - cap[1] + i * lh
                    if align == 'left': nx = jx0 - bb[0]
                    elif align == 'right': nx = jx1 - bb[2]
                    else: nx = (jx0 + jx1) / 2 - (bb[0] + bb[2]) / 2
                    if r.get('anchor') == 'y': nx = x
                    new.append((round(nx), round(ny), l))
                pos = new
        if r.get('shadow'):
            sh = Image.new('RGBA', out.size, (0, 0, 0, 0)); sd = ImageDraw.Draw(sh)
            ox, oy = r.get('shadow_offset', (1, 2))
            for _ in range(r.get('shadow_strength', 1)):
                for x, y, l in pos: sd.text((x + ox, y + oy), l, font=font, fill=tuple(r.get('shadow_color', [0, 0, 0, 230])), stroke_width=r.get('shadow_stroke', 0))
            out.alpha_composite(sh.filter(ImageFilter.GaussianBlur(r.get('shadow_blur', 2))))
        for x, y, l in pos: d.text((x, y), l, font=font, fill=color)
        if r.get('blur'): layer = layer.filter(ImageFilter.GaussianBlur(r['blur']))  # focus-glow art
        out.alpha_composite(layer)
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('labels', type=Path); ap.add_argument('output', type=Path)
    ap.add_argument('--source', type=Path, required=True)
    ap.add_argument('--preview', type=Path)
    ap.add_argument('--language',choices=('en','pt-BR'),default='en')
    a = ap.parse_args()
    table = json.loads(a.labels.read_text(encoding='utf8'))
    from texture_language import localized_table
    table=localized_table(table,a.language)
    previews = []
    for item in table['textures']:
        data = (a.source / item['resource']).read_bytes()
        info, im = decode(data)
        if item.get('mode') == 'regions': item['_source'] = a.source; new = regions(im, item)
        elif item.get('mode') == 'title': new = title(im, item['stage'], item['en'], left_aligned=item.get('align') == 'left')
        elif item.get('mode') == 'notice': new = notice(im,item)
        elif item.get('mode') == 'label': new = label(im, item['en'], item.get('size', 22))
        else: new = relabel(im, item['en'], item.get('align', 'left'), item.get('margin', 2), item.get('size', 20))
        tex = encode_tex(data, new, info)
        target = a.output / item['resource']; target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(tex)
        previews.append((im, decode(tex)[1]))
    if a.preview:
        W = max(i.width for i, _ in previews) * 2 + 30; H = sum(i.height + 8 for i, _ in previews)
        sheet = Image.new('RGBA', (W, H), (70, 70, 100, 255)); y = 0
        for o, n in previews:
            sheet.alpha_composite(o, (0, y)); sheet.alpha_composite(n, (W // 2, y)); y += o.height + 8
        sheet.save(a.preview)
    print(json.dumps(dict(textures=len(previews), output=str(a.output))))


if __name__ == '__main__':
    main()
