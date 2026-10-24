"""Xbox and Nintendo button icons drawn from scratch, in each pad's own shapes.

The game's font icons (font/icon/IC_*.tex) are PlayStation drawings: a pill for
OPTIONS, a touch pad, domed L2/R2, a split D-pad. Swapping only the symbol on
top of them still looks like a PlayStation pad, so these are redrawn whole:
round face buttons (Xbox colors, Nintendo plain), rounded bumpers, triggers,
round Menu/View and +/- buttons, a one-piece cross D-pad and round sticks.
Every icon keeps the native canvas, the native occupied box (so the game's text
layout is unchanged) and a soft drop shadow like the originals.
"""
from PIL import Image, ImageDraw, ImageFilter, ImageFont
from pathlib import Path
from make_keyboard_icons import FONT

BOLD = Path('C:/Windows/Fonts/segoeuib.ttf')
LETTER_FONT = BOLD if BOLD.exists() else FONT
SCALE = 8
XBOX_COLORS = {'A': (125, 183, 0), 'B': (239, 78, 41), 'X': (0, 159, 235), 'Y': (254, 181, 4)}
BODY, BODY_LIGHT, RIM, WHITE = (36, 38, 42), (70, 73, 79), (120, 124, 130), (244, 244, 242)
# Logical button (American layout: A confirms) -> letter printed on each pad.
NINTENDO_FACE = {'A': 'A', 'B': 'B', 'Y': 'X', 'X': 'Y'}
FACE = {'IC_maru_1': 'A', 'IC_batu_2': 'B', 'IC_sankaku_3': 'Y', 'IC_sikaku_4': 'X'}
SHOULDER = {'IC_L1_10': ('bumper', 'LB', 'L'), 'IC_R1_12': ('bumper', 'RB', 'R'),
            'IC_L2_11': ('trigger', 'LT', 'ZL'), 'IC_R2_13': ('trigger', 'RT', 'ZR'),
            'IC_s_L3_16': ('press', 'LS', 'LS'), 'IC_s_R3_17': ('press', 'RS', 'RS'),
            'IC_s_L_14': ('stick', 'L', 'L'), 'IC_s_R_15': ('stick', 'R', 'R')}
DPAD = {'IC_juzhi_5': None, 'IC_juzhi_migi_6': 'right', 'IC_juzhi_hidari_7': 'left',
        'IC_juzhi_ue_8': 'up', 'IC_juzhi_sita_9': 'down'}


def _font(text, height, width):
    size = int(height * 1.6) + 2
    while size > 4:
        font = ImageFont.truetype(str(LETTER_FONT), size)
        l, t, r, b = ImageDraw.Draw(Image.new('L', (1, 1))).textbbox((0, 0), text, font=font, anchor='ls')
        if -t <= height and r - l <= width: return font, t
        size -= 1
    return ImageFont.truetype(str(LETTER_FONT), 5), 0


def _label(d, cx, cy, text, height, width, color):
    font, t = _font(text, height, width)
    d.text((cx, cy - t / 2), text, font=font, fill=color + (255,), anchor='ms')


def _body(d, shape, box, radius=0):
    """Dark body with a lighter top half and a thin rim, the way the game shades its icons."""
    x0, y0, x1, y1 = box
    draw = d.ellipse if shape == 'circle' else lambda b, **k: d.rounded_rectangle(b, radius, **k)
    draw(box, fill=RIM + (255,))
    inset = max(2, (y1 - y0) * .06)
    inner = (x0 + inset, y0 + inset, x1 - inset, y1 - inset)
    draw(inner, fill=BODY + (255,))
    top = (inner[0], inner[1], inner[2], inner[1] + (inner[3] - inner[1]) * .55)
    layer = Image.new('L', d.im.size, 0)
    ImageDraw.Draw(layer).rectangle(top, fill=255)
    return inner


def _render(size, box, paint):
    """Paint at SCALE x, reduce, then add the native-like soft shadow under it."""
    big = Image.new('RGBA', (size[0] * SCALE, size[1] * SCALE))
    paint(ImageDraw.Draw(big), [v * SCALE for v in box])
    icon = big.resize(size, Image.Resampling.LANCZOS)
    shadow = Image.new('RGBA', size)
    shadow.putalpha(icon.getchannel('A').point(lambda a: a * 150 // 255))
    shadow = shadow.transform(size, Image.Transform.AFFINE, (1, 0, -1, 0, 1, -2)).filter(ImageFilter.GaussianBlur(1.5))
    out = Image.new('RGBA', size)
    out.alpha_composite(shadow)
    out.alpha_composite(icon)
    return out


def _shade(d, inner, shape, radius=0):
    """Soft highlight on the upper half of the body."""
    x0, y0, x1, y1 = inner
    hl = (x0 + (x1 - x0) * .12, y0 + (y1 - y0) * .06, x1 - (x1 - x0) * .12, y0 + (y1 - y0) * .5)
    color = BODY_LIGHT + (110,)
    if shape == 'circle': d.ellipse(hl, fill=color)
    else: d.rounded_rectangle(hl, max(1, radius * .7), fill=color)


def face(size, box, logical, family):
    x0, y0, x1, y1 = box
    side = min(x1 - x0, y1 - y0)
    cx, cy = (x0 + x1) / 2, (y0 + y1) / 2
    disc = (cx - side / 2, cy - side / 2, cx + side / 2, cy + side / 2)
    letter = NINTENDO_FACE[logical] if family == 'nintendo' else logical
    color = XBOX_COLORS[logical] if family == 'xbox' else WHITE

    def paint(d, b):
        inner = _body(d, 'circle', b)
        _shade(d, inner, 'circle')
        bx0, by0, bx1, by1 = b
        _label(d, (bx0 + bx1) / 2, (by0 + by1) / 2, letter, (by1 - by0) * .46, (bx1 - bx0) * .6, color)
    return _render(size, disc, paint)


def shoulder(size, box, kind, text):
    x0, y0, x1, y1 = box
    h, w = y1 - y0, x1 - x0

    def paint(d, b):
        bx0, by0, bx1, by1 = b; bh = by1 - by0; bw = bx1 - bx0
        if kind == 'bumper':
            body = (bx0, by0 + bh * .04, bx1, by1 - bh * .04)
            inner = _body(d, 'rect', body, (body[3] - body[1]) * .38)
            _shade(d, inner, 'rect', (body[3] - body[1]) * .3)
            _label(d, (bx0 + bx1) / 2, (body[1] + body[3]) / 2, text, (body[3] - body[1]) * .52, bw * .7, WHITE)
        elif kind == 'trigger':
            # Rounded on top like a trigger seen from the front; flatter base.
            body = (bx0 + bw * .06, by0, bx1 - bw * .06, by1)
            inner = _body(d, 'rect', body, bh * .32)
            _shade(d, inner, 'rect', bh * .25)
            _label(d, (bx0 + bx1) / 2, (by0 + by1) / 2 + bh * .05, text, bh * .42, bw * .7, WHITE)
        else:
            side = min(bw, bh)
            cx, cy = (bx0 + bx1) / 2, (by0 + by1) / 2
            disc = (cx - side / 2, cy - side / 2, cx + side / 2, cy + side / 2)
            inner = _body(d, 'circle', disc)
            _shade(d, inner, 'circle')
            ring = side * .07
            d.ellipse((inner[0] + ring, inner[1] + ring, inner[2] - ring, inner[3] - ring), outline=RIM + (255,), width=max(2, int(side * .035)))
            _label(d, cx, cy, text, side * (.3 if len(text) > 1 else .4), side * .55, WHITE)
    return _render(size, box, paint)


def system(size, box, symbol):
    """Xbox Menu/View or Nintendo +/- : a small round button with its glyph."""
    x0, y0, x1, y1 = box
    side = min(x1 - x0, y1 - y0)
    cx, cy = (x0 + x1) / 2, (y0 + y1) / 2
    disc = (cx - side / 2, cy - side / 2, cx + side / 2, cy + side / 2)

    def paint(d, b):
        inner = _body(d, 'circle', b)
        _shade(d, inner, 'circle')
        bx0, by0, bx1, by1 = b; c = ((bx0 + bx1) / 2, (by0 + by1) / 2); s = (by1 - by0) * .2
        white = WHITE + (255,)
        if symbol == 'menu':
            for k in (-1, 0, 1):
                d.rounded_rectangle((c[0] - s, c[1] + k * s * .62 - s * .13, c[0] + s, c[1] + k * s * .62 + s * .13), s * .13, fill=white)
        elif symbol == 'view':
            w = max(2, round(s * .24))
            d.rounded_rectangle((c[0] - s * 1.05, c[1] - s * 1.05, c[0] + s * .45, c[1] + s * .45), s * .2, outline=white, width=w)
            d.rounded_rectangle((c[0] - s * .45, c[1] - s * .45, c[0] + s * 1.05, c[1] + s * 1.05), s * .2, fill=BODY + (255,), outline=white, width=w)
        else:
            w = s * .32
            d.rounded_rectangle((c[0] - s, c[1] - w / 2, c[0] + s, c[1] + w / 2), w / 2, fill=white)
            if symbol == 'plus': d.rounded_rectangle((c[0] - w / 2, c[1] - s, c[0] + w / 2, c[1] + s), w / 2, fill=white)
    return _render(size, disc, paint)


def dpad(size, box, lit):
    """One-piece cross D-pad; the pressed direction is lit."""
    x0, y0, x1, y1 = box
    side = min(x1 - x0, y1 - y0)
    cx, cy = (x0 + x1) / 2, (y0 + y1) / 2
    square = (cx - side / 2, cy - side / 2, cx + side / 2, cy + side / 2)

    def paint(d, b):
        bx0, by0, bx1, by1 = b; s = by1 - by0; t = s * .34; c = ((bx0 + bx1) / 2, (by0 + by1) / 2)
        arms = {'up': (c[0] - t / 2, by0, c[0] + t / 2, c[1]), 'down': (c[0] - t / 2, c[1], c[0] + t / 2, by1),
                'left': (bx0, c[1] - t / 2, c[0], c[1] + t / 2), 'right': (c[0], c[1] - t / 2, bx1, c[1] + t / 2)}
        r = t * .22
        d.rounded_rectangle((c[0] - t / 2, by0, c[0] + t / 2, by1), r, fill=RIM + (255,))
        d.rounded_rectangle((bx0, c[1] - t / 2, bx1, c[1] + t / 2), r, fill=RIM + (255,))
        i = s * .045
        d.rounded_rectangle((c[0] - t / 2 + i, by0 + i, c[0] + t / 2 - i, by1 - i), r, fill=BODY + (255,))
        d.rounded_rectangle((bx0 + i, c[1] - t / 2 + i, bx1 - i, c[1] + t / 2 - i), r, fill=BODY + (255,))
        for name, arm in arms.items():
            ax0, ay0, ax1, ay1 = arm
            m = t * .28
            tip = (ax0 + m, ay0 + m, ax1 - m, ay1 - m)
            if name == lit: d.rounded_rectangle((ax0 + i, ay0 + i, ax1 - i, ay1 - i), r, fill=WHITE + (255,))
            # Small arrow on each arm, like the pads print them.
            ax, ay = (tip[0] + tip[2]) / 2, (tip[1] + tip[3]) / 2; a = t * .16
            color = (BODY if name == lit else (150, 154, 160)) + (255,)
            pts = {'up': [(ax, ay - a), (ax - a, ay + a * .6), (ax + a, ay + a * .6)],
                   'down': [(ax, ay + a), (ax - a, ay - a * .6), (ax + a, ay - a * .6)],
                   'left': [(ax - a, ay), (ax + a * .6, ay - a), (ax + a * .6, ay + a)],
                   'right': [(ax + a, ay), (ax - a * .6, ay - a), (ax - a * .6, ay + a)]}[name]
            d.polygon(pts, fill=color)
    return _render(size, square, paint)


def font_icon(name, image, family):
    """Whole replacement for a font/icon/IC_*.tex image (None: not a font icon)."""
    stem = Path(name).stem
    a = image.getchannel('A')
    box = a.point(lambda v: 255 if v > 40 else 0).getbbox()
    # The shadow sits low; the body is about the top 88% of the occupied box.
    box = (box[0], box[1], box[2], box[3] - max(2, (box[3] - box[1]) // 10))
    size = image.size
    if stem in FACE: return face(size, box, FACE[stem], family)
    if stem in SHOULDER:
        kind, xbox, nintendo = SHOULDER[stem]
        return shoulder(size, box, kind, xbox if family == 'xbox' else nintendo)
    if stem == 'IC_options_18': return system(size, box, 'menu' if family == 'xbox' else 'plus')
    if stem == 'IC_touch_pad_20': return system(size, box, 'view' if family == 'xbox' else 'minus')
    if stem in DPAD: return dpad(size, box, DPAD[stem])
    return None
