"""Build English versions of text-bearing TEX images (BC3 + mip chain) from the user's own dump.

Replaces the Japanese label with an English one drawn over the original background.
Output files mirror archive paths (e.g. OUT/texture/menu_option/M_option_01.tex).
Generated textures are private derivatives of the dump and must never be distributed;
only this tool and the label table are part of the patch.
"""
import argparse, io, json, struct, sys, zlib
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont, ImageFilter
sys.path.insert(0, str(Path(__file__).parent))
from tex_preview import unpack_tex, dds_bc

REPO = Path(__file__).resolve().parents[1]
FONT = REPO / 'third_party/godzilla-core/third_party/shadPS4/src/imgui/renderer/fonts/NotoSans-Regular.ttf'


def decode(data):
    info, body = unpack_tex(data)
    if info['storage_family'] != 13: raise ValueError('Only BC3 supported')
    w, h = info['width'], info['height']
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
    lum = [p[0] * 3 + p[1] * 6 + p[2] for p in px]
    hi, lo = px[lum.index(max(lum))], px[lum.index(min(lum))]
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
        body += encode_bc3(level)
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


def label(im, text, size):
    w, h = im.size
    alpha = im.getchannel('A').load()
    xs = [x for y in range(h) for x in range(w) if alpha[x, y] > 128]
    left = min(xs) if xs else 2
    font = ImageFont.truetype(str(FONT), size)
    bb = font.getbbox(text)
    if bb[2] - bb[0] > w - left - 3:
        raise ValueError(f'Label does not fit at the shared font size: {text}')
    y = h / 2 - (bb[1] + bb[3]) / 2
    shadow = Image.new('RGBA', (w, h), (0, 0, 0, 0))
    ImageDraw.Draw(shadow).text((left - bb[0] + 1, y + 1), text, font=font, fill=(0, 0, 0, 220))
    shadow = shadow.filter(ImageFilter.GaussianBlur(1))
    out = Image.new('RGBA', (w, h), (0, 0, 0, 0)); out.alpha_composite(shadow)
    ImageDraw.Draw(out).text((left - bb[0], y), text, font=font, fill=(255, 255, 255, 255))
    return out


SERIF = Path('C:/Windows/Fonts/cambriab.ttf')


def glow_text(out, pos, text, font):
    shadow = Image.new('RGBA', out.size, (0, 0, 0, 0))
    d = ImageDraw.Draw(shadow)
    for dx, dy in ((0, 0), (2, 2), (-2, 2), (2, -2), (-2, -2)):
        d.text((pos[0] + dx, pos[1] + dy), text, font=font, fill=(0, 0, 0, 255))
    out.alpha_composite(shadow.filter(ImageFilter.GaussianBlur(4)))
    ImageDraw.Draw(out).text(pos, text, font=font, fill=(255, 255, 255, 255))


def title(im, stage, text, size=50):
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
    small = ImageFont.truetype(fontfile, 32)
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
    cy = (ty0 + ty1) / 2
    tw = bb[2] - bb[0]
    x = (w - tw) / 2 - bb[0] if align == 'center' else tx0 - bb[0]
    y = cy - (bb[1] + bb[3]) / 2
    d.text((x, y), text, font=font, fill=(textc[0], textc[1], textc[2], 255))
    return rgba


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('labels', type=Path); ap.add_argument('output', type=Path)
    ap.add_argument('--source', type=Path, default=Path('G:/kyoei-toshi-work/m1/common'))
    ap.add_argument('--preview', type=Path)
    a = ap.parse_args()
    table = json.loads(a.labels.read_text(encoding='utf8'))
    previews = []
    for item in table['textures']:
        data = (a.source / item['resource']).read_bytes()
        info, im = decode(data)
        if item.get('mode') == 'title': new = title(im, item['stage'], item['en'])
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
