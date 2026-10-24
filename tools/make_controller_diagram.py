"""Controls diagram (texture/controller_operation/operation_normal*.tex) in English.

The game's drawing stays: controller, glow and green call-out lines. Only the
labels change: their pixels are cleared (green lines and the colored button
symbols in the headings are kept) and the English text is drawn in the native
style (light grey, dark soft outline). The PlayStation layout follows the
American pad (src: CMakeLists KYOEI_PAD): the bottom button (cross) checks and
talks, circle cancels, so the two call-outs keep pointing at the physical
buttons and only their descriptions trade places.
Outputs are derived from the user's dump and stay private.
"""
import argparse, json, sys
from pathlib import Path
import numpy as np
from PIL import Image, ImageDraw, ImageFont
sys.path.insert(0, str(Path(__file__).parent))
from localize_textures import decode, encode_tex

BOLD = Path('C:/Windows/Fonts/segoeuib.ttf')
FONT = BOLD if BOLD.exists() else Path(__file__).resolve().parents[1] / \
    'third_party/godzilla-core/third_party/shadPS4/src/imgui/renderer/fonts/NotoSans-Regular.ttf'
TEXT = (206, 203, 198, 245)
OUTLINE = (8, 16, 8, 170)

# One call-out: heading line box (y0, y1, x0, x1), body line boxes, right limit, texts.
# Coordinates measured on the 1920x1080 operation_normal.tex.
NORMAL = [
    ((282, 294, 121, 260), [(304, 322, 121, 322)], 330, 'Touch Pad Button', ['Map: Show / Hide']),
    ((339, 351, 121, 287), [(361, 379, 121, 230)], 380, 'Touch Pad Slide', ['Scroll the Map']),
    ((396, 408, 120, 202), [(419, 436, 121, 242), (441, 458, 120, 286)], 298, 'L2 Button', ['Switch Camera:', 'Close → Mid → Far']),
    ((475, 487, 120, 202), [(498, 515, 121, 241), (520, 537, 121, 285)], 282, 'L1 Button', ['Switch Camera:', 'Far → Mid → Close']),
    ((555, 568, 70, 190), [(577, 594, 72, 204), (599, 617, 71, 204), (621, 638, 71, 199)], 250, 'D-Pad (Left / Right)',
     ['Change Target', '(Check / Talk /', 'Pick Up)']),
    ((814, 826, 72, 171), [(836, 853, 73, 315), (858, 876, 73, 278)], 325, 'Left Stick', ['Move: Walk / Run', 'Crawl (While Down)']),
    ((892, 905, 73, 154), [(915, 932, 72, 312)], 325, 'L3 Button', ['HUD: Large → Small → Off']),
    ((815, 826, 367, 466), [(836, 853, 368, 477)], 530, 'Right Stick', ['Move Camera']),
    ((867, 879, 367, 451), [(889, 906, 368, 551), (911, 928, 367, 422)], 540, 'R3 Button', ['Reset Camera']),
    ((945, 957, 405, 538), [(968, 984, 406, 566)], 560, 'OPTIONS Button', ['Menu / Pause']),
    # The call-out to the right button (circle): with the American pad it cancels.
    ((227, 240, 416, 490), [(250, 267, 416, 656), (272, 289, 416, 572)], 650, '@ Button', ['Cancel /', 'Stop Crawling']),
    ((307, 319, 435, 494), [(330, 347, 418, 527), (351, 370, 417, 548)], 618, '@ Button', ['Read Mail', '(When One Arrives)']),
    ((388, 400, 417, 501), [(410, 427, 417, 525), (432, 450, 417, 585)], 600, 'R2 Button', ['Giant Shadow Camera', '(When Available)']),
    ((469, 481, 417, 597), [(491, 508, 418, 571), (513, 529, 418, 505)], 590, 'Left Stick + R1 Button', ['Move: Dash']),
    # The call-out to the bottom button (cross): checks, talks and advances text.
    ((840, 852, 596, 667), [(862, 878, 595, 717), (884, 901, 596, 742)], 745, '@ Button', ['Check / Talk / Pick Up', '/ Advance Text']),
    ((918, 931, 594, 670), [(943, 960, 595, 742)], 745, '@ Button', ['Evade / Get Down']),
]


def font_for(cap):
    """Bold face whose capitals are `cap` pixels tall."""
    size = cap
    while True:
        font = ImageFont.truetype(str(FONT), size)
        if -font.getbbox('H', anchor='ls')[1] >= cap: return font
        size += 1


def label_mask(shape, box, left=3):
    """Pixels of one label line (with `left` px of margin before it)."""
    y0, y1, x0, x1 = box
    mask = np.zeros(shape, bool)
    mask[max(0, y0 - 5):y1 + 6, max(0, x0 - left):x1 + 6] = True
    return mask


def erase(a, labels):
    """Remove every label line at once and rebuild the background under them from the clean
    pixels around (never from another label or from a call-out line and its halo)."""
    rgb = a[..., :3].astype(int)
    line = (rgb[..., 1] > 100) & (rgb[..., 1] > rgb[..., 0] + 40) & (a[..., 3] > 60)
    near = line.copy()
    for _ in range(4):
        n = near.copy(); n[1:] |= near[:-1]; n[:-1] |= near[1:]; n[:, 1:] |= near[:, :-1]; n[:, :-1] |= near[:, 1:]
        near = n
    # The controller itself (fully opaque black body and the white outline along it) is kept:
    # labels sit beside it and the old text's outline is never fully opaque.
    body = (a[..., 3] >= 252) & (rgb.max(axis=2) < 40)
    edge = body.copy()
    for _ in range(3):
        n = edge.copy(); n[1:] |= edge[:-1]; n[:-1] |= edge[1:]; n[:, 1:] |= edge[:, :-1]; n[:, :-1] |= edge[:, 1:]
        edge = n
    drawing = body | (edge & (a[..., 3] >= 252) & (rgb.min(axis=2) > 180))
    line |= drawing
    holes = labels & ~line
    ys, xs = np.nonzero(labels)
    y0, y1, x0, x1 = max(0, ys.min() - 48), ys.max() + 49, max(0, xs.min() - 48), xs.max() + 49
    # The drawing stays as it is but never stands for the glow around it.
    # Nor does the green halo of the call-out lines (wider than `near` at their bends).
    halo = (rgb[..., 1] > rgb[..., 0] + 30) & (a[..., 3] > 40)
    halo |= (rgb.max(axis=2) - rgb.min(axis=2) > 40) & (a[..., 3] > 40)  # heading symbols
    unknown = holes | near | edge | halo
    a[y0:y1, x0:x1] = diffuse(a[y0:y1, x0:x1], holes[y0:y1, x0:x1], unknown[y0:y1, x0:x1])


def diffuse(sub, holes, unknown, radius=9):
    """Fill `holes` with the per-channel median of the known pixels around each one: robust
    against stray letter edges and line halos, and it follows the glow's gradient."""
    out = sub.copy()
    known = ~unknown
    h, w = holes.shape
    for y, x in zip(*np.nonzero(holes)):
        r = radius
        while True:
            y0, y1, x0, x1 = max(0, y - r), min(h, y + r + 1), max(0, x - r), min(w, x + r + 1)
            pick = known[y0:y1, x0:x1]
            if pick.sum() >= 12 or r > 40: break
            r *= 2
        values = sub[y0:y1, x0:x1][pick]
        out[y, x] = np.median(values, axis=0).astype(np.uint8) if len(values) else 0
    return out


def symbol_end(a, box):
    """Right edge of a colored button symbol at the start of a heading, if any."""
    y0, y1, x0, x1 = box
    sub = a[y0:y1 + 1, x0 - 18:x0 + 24].astype(int)
    rgb = sub[..., :3]
    line = (rgb[..., 1] > rgb[..., 0] + 25) & (rgb[..., 1] > rgb[..., 2] + 5) & (rgb[..., 1] > 120)
    colored = (sub[..., 3] > 120) & (rgb.max(axis=2) - rgb.min(axis=2) > 25) & ~line
    # A symbol is a compact blob before the text; a call-out line is a long run.
    cols = np.nonzero(colored.any(axis=0))[0]
    if not len(cols): return None
    end = cols[0]
    while end + 1 in cols or end + 2 in cols: end += 1  # the symbol's own columns only
    return x0 - 18 + int(end) + 1


def text_line(draw, x, base, text, font, limit, fill=TEXT):
    # Narrow the font a little rather than cross a call-out line.
    while draw.textlength(text, font=font) > limit - x and font.size > 8:
        font = ImageFont.truetype(str(FONT), font.size - 1)
    draw.text((x, base), text, font=font, fill=fill, anchor='ls', stroke_width=2, stroke_fill=OUTLINE)


def relabel(image, callouts):
    a = np.array(image.convert('RGBA'))
    starts = []
    labels = np.zeros(a.shape[:2], bool)
    for heading, bodies, _, title, *_ in callouts:
        start = symbol_end(a, heading) if title.startswith('@') else None
        starts.append(start)
        labels |= label_mask(a.shape[:2], (heading[0], heading[1], start + 2, heading[3]), 0) if start             else label_mask(a.shape[:2], heading)
        for body in bodies: labels |= label_mask(a.shape[:2], body)
    erase(a, labels)
    out = Image.fromarray(a)
    draw = ImageDraw.Draw(out)
    head_font, body_font = font_for(10), font_for(13)
    for (heading, bodies, limit, title, lines, *color), start in zip(callouts, starts):
        y0, y1, x0, _ = heading
        text_line(draw, (start + 3) if start else x0, y1, title.replace('@ ', ''), head_font, limit)
        step = (bodies[-1][1] - bodies[0][1]) / max(1, len(bodies) - 1) if len(bodies) > 1 else 22
        for i, line in enumerate(lines):
            text_line(draw, min(b[2] for b in bodies), bodies[0][1] - 1 + round(i * step), line, body_font, limit,
                      color[0] if color else TEXT)
    return out


NAME = 'texture/controller_operation/operation_normal.tex'
NAME_207 = 'texture/controller_operation/operation_normal_207.tex'
PINK = (181, 121, 123, 245)
# Forklift chapter (207): the up button resets the container and forklift (pink, like the game).
NORMAL_207 = [c for c in NORMAL if c[3] != 'D-Pad (Left / Right)'] + [
    ((560, 573, 78, 164), [(583, 598, 80, 153), (605, 620, 81, 223), (627, 642, 82, 239)], 250,
     'D-Pad (Up)', ['Reset Container /', 'Forklift Position'], PINK),
    ((659, 673, 77, 180), [(680, 699, 77, 199), (702, 721, 77, 199), (724, 744, 77, 199)], 250,
     'D-Pad (Left / Right)', ['Change Target', '(Check / Talk /', 'Pick Up)']),
]
NAME_SMALL = 'texture/controller_operation/operation_normal_s.tex'
# The small list (616x732): text row (center y, x0, x1) -> English. Measured on operation_normal_s.
SMALL = [
    ((220, 97, 211), 'Check / Talk'), ((257, 97, 192), 'Cancel'), ((294, 98, 192), 'Read Mail'),
    ((331, 97, 230), 'Evade / Get Down'), ((369, 97, 133), 'Move'), ((406, 154, 225), 'Dash'),
    ((443, 98, 191), 'Move Camera'), ((480, 97, 211), 'Controls Display'), ((517, 98, 250), 'Reset Camera'),
    ((555, 98, 231), 'Change Target'), ((589, 153, 247), 'Switch Camera'), ((628, 98, 191), 'Giant Shadow Camera'),
    ((665, 98, 308), 'Map / Scroll Map'), ((702, 98, 172), 'Menu'),
]
# American pad: the first row (check/talk) shows the cross, the second (cancel) the circle.
SMALL_SWAP = ((220, 257), (26, 76), 18)
NAME_207_SMALL = 'texture/controller_operation/operation_normal_207_s.tex'
# Forklift chapter list: one more row (pink), every row 37 px higher.
SMALL_207 = [((cy - 37, x0, x1), text) for (cy, x0, x1), text in SMALL[:9]] + [
    ((516, 97, 415), 'Reset Container / Forklift Position', PINK)] + [
    ((cy - 37 + 37, x0, x1), text) for (cy, x0, x1), text in SMALL[9:]]
SMALL_207_SWAP = ((183, 220), (26, 76), 18)


def small_list(image, rows=SMALL, swap=SMALL_SWAP):
    a = np.array(image.convert('RGBA'))
    (first, second), (x0, x1), half = swap
    top = a[first - half:first + half, x0:x1].copy()
    a[first - half:first + half, x0:x1] = a[second - half:second + half, x0:x1]
    a[second - half:second + half, x0:x1] = top
    labels = np.zeros(a.shape[:2], bool)
    for (cy, lx0, lx1), *_ in rows:
        labels |= label_mask(a.shape[:2], (cy - 10, cy + 9, lx0, lx1))
    erase(a, labels)
    out = Image.fromarray(a)
    draw = ImageDraw.Draw(out)
    font = font_for(13)
    for (cy, lx0, _), text, *color in rows:
        text_line(draw, lx0, cy + 7, text, font, 600, color[0] if color else TEXT)
    return out


def english_small(data, forklift=False):
    info, image = decode(data)
    return encode_tex(data, small_list(image, *((SMALL_207, SMALL_207_SWAP) if forklift else (SMALL, SMALL_SWAP))), info)


# Archive path -> English bytes, for the read-time delta (tools/build_ui_corrections.py).
VARIANTS = {
    NAME: lambda data: english(data, NORMAL),
    NAME_207: lambda data: english(data, NORMAL_207),
    NAME_SMALL: lambda data: english_small(data),
    NAME_207_SMALL: lambda data: english_small(data, True),
}


def english(data, callouts=NORMAL):
    """operation_normal(_207).tex bytes -> the English PlayStation (American) diagram."""
    info, image = decode(data)
    return encode_tex(data, relabel(image, callouts), info)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('archive', type=Path, help="common.psarc from the user's dump")
    parser.add_argument('--preview', type=Path, help='write the relabeled diagram as PNG')
    parser.add_argument('--small', action='store_true', help='the small list (operation_normal_s)')
    parser.add_argument('--forklift', action='store_true', help='the forklift chapter diagram (_207)')
    args = parser.parse_args()
    from psarc import Psarc
    name = (NAME_207_SMALL if args.forklift else NAME_SMALL) if args.small else NAME_207 if args.forklift else NAME
    with Psarc(args.archive) as archive:
        entry = {e.name: e for e in archive.entries[1:]}[name]
        data = b''.join(archive.chunks(entry))
    tex = VARIANTS[name](data)
    new = decode(tex)[1]
    if args.preview:
        sheet = Image.new('RGBA', new.size, (24, 30, 40, 255)); sheet.alpha_composite(decode(tex)[1]); sheet.save(args.preview)
    print(json.dumps(dict(texture=name, bytes=len(tex))))


if __name__ == '__main__':
    main()
