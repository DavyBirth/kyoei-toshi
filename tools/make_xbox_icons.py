"""Xbox and Nintendo button prompts drawn over the game's own PlayStation icons.

Each icon keeps its native body (disc, shoulder tag, pin, banner): only the
PlayStation symbol is removed, the background under it rebuilt (refill) and the
Xbox symbol drawn in its place, at the same size. Mapping follows the American
pad (src: CMakeLists KYOEI_PAD, bottom button confirms): circle->A, cross->B,
triangle->Y, square->X, L1/R1->LB/RB, L2/R2->LT/RT, L3/R3->LS/RS,
OPTIONS->Menu, touch pad->View. D-pad and sticks are the same on both pads.
Button colors are those of the Kenney Input Prompts set (CC0).
Outputs are derived from the user's dump and stay private.
"""
import argparse, json, sys
from pathlib import Path
from PIL import Image, ImageDraw, ImageFilter, ImageFont
import numpy as np
sys.path.insert(0, str(Path(__file__).parent))
from tex_preview import preview
from make_keyboard_icons import BINDINGS, FONT, fit
# Xbox's own interface face when installed (Windows); the bundled Noto otherwise.
BOLD = Path('C:/Windows/Fonts/segoeuib.ttf')
LETTER_FONT = BOLD if BOLD.exists() else FONT
from make_american_icons import refill, encode_patch

COLORS = {'A': (125, 183, 0), 'B': (239, 78, 41), 'X': (0, 159, 235), 'Y': (254, 181, 4)}
# Nintendo pads print plain letters; the red dodge prompt stays red (the game's danger color).
NINTENDO_LETTER = (238, 238, 236)
# Nintendo: A on the right confirms, as the circle in the Japanese layout; X on top, Y on the left.
NINTENDO_FACE = {'A': 'A', 'B': 'B', 'Y': 'X', 'X': 'Y'}
NINTENDO_TEXT = {'LB': 'L', 'RB': 'R', 'LT': 'ZL', 'RT': 'ZR', 'LS': 'LS', 'RS': 'RS', '@menu': '@plus', '@view': '@minus'}
SCALE = 8

# archive path -> what replaces the PlayStation symbol
FACE = {'maru': 'A', 'batu': 'B', 'sankaku': 'Y', 'sikaku': 'X'}
TEXT = {
    'font/icon/IC_L1_10.tex': 'LB', 'font/icon/IC_L2_11.tex': 'LT',
    'font/icon/IC_R1_12.tex': 'RB', 'font/icon/IC_R2_13.tex': 'RT',
    'font/icon/IC_s_L3_16.tex': 'LS', 'font/icon/IC_s_R3_17.tex': 'RS',
    'texture/intermission/IM_l_cursor_button.tex': 'LB', 'texture/intermission/IM_r_cursor_button.tex': 'RB',
    'texture/intermission/L1_btn.tex': 'LB', 'texture/intermission/R1_btn.tex': 'RB',
    'font/icon/IC_options_18.tex': '@menu', 'font/icon/IC_touch_pad_20.tex': '@view',
}
KEEP = {'font/icon/IC_juzhi_5.tex', 'font/icon/IC_juzhi_migi_6.tex', 'font/icon/IC_juzhi_hidari_7.tex',
        'font/icon/IC_juzhi_ue_8.tex', 'font/icon/IC_juzhi_sita_9.tex', 'font/icon/IC_s_L_14.tex',
        'font/icon/IC_s_R_15.tex', 'texture/controller_operation/cmnd_marker_op.tex'}


def face_letter(name):
    if name.startswith('texture/text marker/talk_icon_') or name in (
            'texture/hud/cursol_nextchat.tex', 'texture/stage_clear/sc_next.tex', 'texture/hud/resalt_next.tex'):
        return 'A'
    if 'demo_skip' in name: return 'Y'
    if 'kaihi_batu' in name: return 'B'
    if '_o_btn' in name: return 'A'
    if '_x_btn' in name: return 'B'
    for key, letter in FACE.items():
        if key in name: return letter
    raise KeyError(name)


def dilate(mask, steps):
    for _ in range(steps):
        n = mask.copy(); n[1:] |= mask[:-1]; n[:-1] |= mask[1:]; n[:, 1:] |= mask[:, :-1]; n[:, :-1] |= mask[:, 1:]
        mask = n
    return mask


def erase(image, mask, grow=1):
    """Clear masked pixels (grown a little for antialiasing) and rebuild what lay under them."""
    ys, xs = np.nonzero(mask)
    if not len(xs): raise ValueError('symbol not found')
    symbol = (int(xs.min()), int(ys.min()), int(xs.max()) + 1, int(ys.max()) + 1)
    ys, xs = np.nonzero(dilate(mask, grow))
    holes = list(zip(xs.tolist(), ys.tolist()))
    box = (int(xs.min()), int(ys.min()), int(xs.max()) + 1, int(ys.max()) + 1)
    out = image.copy(); px = out.load()
    for x, y in holes: px[x, y] = (0, 0, 0, 0)
    return refill(out, holes, box), symbol


def symbol_layer(size, box, draw_symbol):
    """Draw at SCALE x into a transparent layer and reduce: smooth edges at native size."""
    layer = Image.new('RGBA', (size[0] * SCALE, size[1] * SCALE))
    draw_symbol(ImageDraw.Draw(layer), [v * SCALE for v in box])
    return layer.resize(size, Image.Resampling.LANCZOS)


def letter(d, box, text, color, width=1.3):
    """Text whose capitals fill the box height (centered on the box)."""
    x0, y0, x1, y1 = box
    size = int(2 * (y1 - y0)) + 2  # em size; capitals are ~70% of it
    while size > 4:
        font = ImageFont.truetype(str(LETTER_FONT), size)
        l, t, r, b = d.textbbox((0, 0), text, font=font, anchor='ls')
        if -t <= (y1 - y0) and r - l <= (x1 - x0) * width: break
        size -= 1
    # Capitals: center the cap height, not the line box.
    d.text(((x0 + x1) / 2, (y0 + y1) / 2 - t / 2), text, font=font, fill=color, anchor='ms')


def face(image, name, family='xbox'):
    """Colored PlayStation glyph -> Xbox (colored) or Nintendo (plain) letter, same box, same glow."""
    a = np.asarray(image, dtype=np.int16)
    chroma = a[..., :3].max(axis=2) - a[..., :3].min(axis=2)
    if name == 'texture/hud/resalt_next.tex':
        # The 'Press ◎ Button' ring sits on a green banner: only its round area.
        h, w = a.shape[:2]; yy, xx = np.mgrid[0:h, 0:w]
        mask = (xx - 251.5) ** 2 + (yy - 62.5) ** 2 <= 16 ** 2
    else:
        mask = (a[..., 3] > 8) & (chroma > 18)
        if 'nextchat' in name:
            mask &= a[..., 0] > a[..., 1] + 20  # the pink ring only; the green glow is the frame
    if 'demo_skip' in name:
        mask[:, a.shape[1] // 4:] = False  # the text 'Scene Skip' stays
    if not mask.any():
        # Dialog buttons: a white glyph on a black disc.
        mask = (a[..., 3] > 60) & (a[..., :3].min(axis=2) > 150)
    glow = 'kaihi_batu_2' in name or 'nextchat' in name
    out, box = erase(image, mask, 2 if 'kaihi' in name else 1)
    text = face_letter(name)
    if family == 'nintendo': text = NINTENDO_FACE[text]
    color = (NINTENDO_LETTER if family == 'nintendo' and 'kaihi' not in name else COLORS[text if 'kaihi' not in name else 'B']) + (255,)
    if 'kaihi' in name and not glow: color = tuple(int(c * .7) for c in color[:3]) + (230,)
    if name == 'texture/hud/resalt_next.tex':
        # A full Xbox disc with a white letter replaces the ring, like the Kenney glyph.
        def disc(d, b):
            d.ellipse(b, fill=(COLORS['A'] if family == 'xbox' else (58, 60, 62)) + (255,)); x0, y0, x1, y1 = b
            letter(d, (x0, y0 + (y1 - y0) * .25, x1, y1 - (y1 - y0) * .25), 'A', (255, 255, 255, 255))
        layer = symbol_layer(out.size, (236, 47, 268, 79), disc)
    else:
        # Letter cap height: about 70% of the round glyph (the X is as tall as the cross).
        h = (box[3] - box[1]) * (.95 if text == 'B' and 'kaihi' in name else .68)
        cy = (box[1] + box[3]) / 2
        layer = symbol_layer(out.size, (box[0], cy - h / 2, box[2], cy + h / 2), lambda d, b: letter(d, b, text, color))
    if glow:
        out.alpha_composite(layer.filter(ImageFilter.GaussianBlur(3)))
    out.alpha_composite(layer)
    return out


def text_tag(image, label, white_level=185, family='xbox'):
    """White label on a grey tag (L1, OPTIONS, ...) -> Xbox label in the same white.
    white_level: lower bound of the label's channels (the triggers' lit dome is light too)."""
    a = np.asarray(image, dtype=np.int16)
    # Only well inside the tag: its lit rim is as light as the label.
    inside = ~dilate(a[..., 3] <= 120, 1 if label == '@view' else max(2, min(a.shape[:2]) // 16))
    bright = inside & (a[..., :3].min(axis=2) > white_level) & (a[..., :3].max(axis=2) - a[..., :3].min(axis=2) < 40)
    out, box = erase(image, bright, 1)
    white = (245, 245, 245, 255)
    x0, y0, x1, y1 = box
    if family == 'nintendo': label = NINTENDO_TEXT[label]
    if label in ('@menu', '@view', '@plus', '@minus'):
        # Symbols take the size of the button, not of the text they replace.
        ys, xs = np.nonzero(a[..., 3] > 120)
        cx, cy = (xs.min() + xs.max() + 1) / 2, (ys.min() + ys.max() + 1) / 2
        r = (ys.max() + 1 - ys.min()) * .36
        box = (cx - r, cy - r, cx + r, cy + r)
    if label == '@menu':
        def menu(d, b):
            bx0, by0, bx1, by1 = b; cx, cy = (bx0 + bx1) / 2, (by0 + by1) / 2; s = (by1 - by0) * .42
            for k in (-1, 0, 1):
                d.rounded_rectangle((cx - s, cy + k * s * .62 - s * .12, cx + s, cy + k * s * .62 + s * .12), s * .12, fill=white)
        return out_with(out, symbol_layer(out.size, box, menu))
    if label == '@view':
        def view(d, b):
            bx0, by0, bx1, by1 = b; cx, cy = (bx0 + bx1) / 2, (by0 + by1) / 2; s = (by1 - by0) * .32; w = max(2, s * .22)
            d.rounded_rectangle((cx - s * 1.1, cy - s * 1.1, cx + s * .5, cy + s * .5), s * .15, outline=white, width=round(w))
            d.rounded_rectangle((cx - s * .5, cy - s * .5, cx + s * 1.1, cy + s * 1.1), s * .15, fill=(70, 72, 76, 255), outline=white, width=round(w))
        return out_with(out, symbol_layer(out.size, box, view))
    if label in ('@plus', '@minus'):
        def sign(d, b):
            bx0, by0, bx1, by1 = b; cx, cy = (bx0 + bx1) / 2, (by0 + by1) / 2; s = (by1 - by0) * .34; w = s * .3
            d.rounded_rectangle((cx - s, cy - w / 2, cx + s, cy + w / 2), w / 2, fill=white)
            if label == '@plus': d.rounded_rectangle((cx - w / 2, cy - s, cx + w / 2, cy + s), w / 2, fill=white)
        return out_with(out, symbol_layer(out.size, box, sign))
    # Same cap height as the native label; two letters may be a little wider.
    cy, h = (y0 + y1) / 2, y1 - y0
    return out_with(out, symbol_layer(out.size, (x0, cy - h / 2, x1, cy + h / 2), lambda d, b: letter(d, b, label, white, 1.6)))


def out_with(base, layer):
    base.alpha_composite(layer)
    return base


def cursor_tags(image, family='xbox'):
    """Pause page arrows: the dark L1/R1 tags above the arrows become LB/RB (L/R on Nintendo)."""
    out = image.copy()
    w, h = out.size
    for half, label in ((0, 'LB'), (1, 'RB')):
        part = out.crop((half * w // 2, 0, (half + 1) * w // 2, h))
        a = np.asarray(part, dtype=np.int16)
        tag = (a[..., 3] > 90) & (a[..., :3].max(axis=2) < 70)
        ys, xs = np.nonzero(tag)
        if not len(xs): raise ValueError('cursor tag missing')
        bx0, by0, bx1, by1 = int(xs.min()), int(ys.min()), int(xs.max()) + 1, int(ys.max()) + 1
        crop = part.crop((bx0, by0, bx1, by1))
        part.paste(text_tag(crop, label, family=family), (bx0, by0))
        out.paste(part, (half * w // 2, 0))
    return out


def draw(name, image, family='xbox'):
    if name in KEEP: return None
    if name == 'texture/hud/cursor_lr.tex': return cursor_tags(image, family)
    if name in TEXT: return text_tag(image, TEXT[name], 140 if TEXT[name] == '@view' else 185, family)
    return face(image, name, family)


def build(originals, family='xbox'):
    """originals: archive path -> .tex bytes. Returns the changed icons only."""
    out = {}
    for name, original in originals.items():
        if name not in BINDINGS and name != 'texture/hud/status_base.tex': continue
        if name == 'texture/hud/status_base.tex': continue
        image = draw(name, preview(original)[0].convert('RGBA'), family)
        if image is not None:
            out[name] = encode_patch(original, image)
    return out


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('archive', type=Path, help="common.psarc from the user's dump")
    parser.add_argument('--sheet', type=Path, help='write original/new preview pairs')
    parser.add_argument('--family', choices=('xbox', 'nintendo'), default='xbox')
    args = parser.parse_args()
    from psarc import Psarc
    with Psarc(args.archive) as archive:
        names = {e.name: e for e in archive.entries[1:]}
        originals = {n: b''.join(archive.chunks(names[n])) for n in BINDINGS}
    icons = build(originals, args.family)
    if args.sheet:
        cell, cols = 150, 8
        items = list(originals)
        sheet = Image.new('RGBA', (cell * cols, cell * 2 * ((len(items) + cols - 1) // cols)), (70, 70, 70, 255))
        for i, name in enumerate(items):
            x, y = (i % cols) * cell, (i // cols) * cell * 2
            for row, data in enumerate((originals[name], icons.get(name, originals[name]))):
                im = preview(data)[0].convert('RGBA'); im.thumbnail((cell - 8, cell - 8))
                sheet.alpha_composite(im, (x + 4, y + row * cell + 4))
        sheet.save(args.sheet)
    print(json.dumps(dict(icons=len(icons), bytes=sum(map(len, icons.values())))))


if __name__ == '__main__':
    main()
