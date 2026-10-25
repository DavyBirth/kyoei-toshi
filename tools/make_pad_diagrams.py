"""Controls diagrams (texture/controller_operation/*.tex) for Xbox, Nintendo and keyboard.

The PlayStation diagrams keep the native drawing. For the other devices the whole picture is
drawn again in the native style (black body, white outlines, green glow, green call-out lines)
from the label layout in translations/textures-en.json: each label keeps its place and text
style, gets the device's button name, and its line runs to that button on the new drawing.
Outputs are private derivatives of the owner's dump; only this tool is part of the patch.
"""
import argparse, itertools, json, math, sys
from pathlib import Path
import numpy as np
from PIL import Image, ImageDraw, ImageFilter, ImageFont
sys.path.insert(0, str(Path(__file__).parent))
from localize_textures import FONTS

REPO = Path(__file__).resolve().parents[1]
LINE = (33, 142, 90, 255)
GLOW = (49, 154, 107)
S = 4  # supersampling of the drawing

# Physical American layout of the PlayStation labels -> device button.
XBOX = {'cross': 'A', 'circle': 'B', 'square': 'X', 'triangle': 'Y', 'L1': 'LB', 'L2': 'LT',
        'R1': 'RB', 'R2': 'RT', 'L3': 'LS', 'R3': 'RS', 'touchpad': 'View', 'options': 'Menu',
        'lstick': 'LS', 'rstick': 'RS', 'dpad': 'dpad'}
NINTENDO = {'cross': 'A', 'circle': 'B', 'square': 'Y', 'triangle': 'X', 'L1': 'L', 'L2': 'ZL',
            'R1': 'R', 'R2': 'ZR', 'L3': 'LS', 'R3': 'RS', 'touchpad': 'Minus', 'options': 'Plus',
            'lstick': 'LS', 'rstick': 'RS', 'dpad': 'dpad'}
NAMES = {'A': 'A Button', 'B': 'B Button', 'X': 'X Button', 'Y': 'Y Button', 'LB': 'LB', 'LT': 'LT',
         'RB': 'RB', 'RT': 'RT', 'L': 'L Button', 'ZL': 'ZL Button', 'R': 'R Button', 'ZR': 'ZR Button',
         'View': 'View Button', 'Menu': 'Menu Button', 'Minus': '− Button', 'Plus': '+ Button'}
# Labels that the new drawing would cover: (left, top) instead of the native place.
LABEL_MOVES = {}
FACE_COLORS = {'xbox': {'A': (96, 176, 60), 'B': (210, 60, 52), 'X': (52, 120, 210), 'Y': (232, 186, 40)},
               'nintendo': {}}


def classify(label, icon):
    """PlayStation label text (or its leading icon) -> control id, plus the rest of the label."""
    if icon:
        return [icon], ''
    t = label.replace(' Button', '').strip()
    words = {'Touchpad': 'touchpad', 'OPTIONS': 'options', 'L1': 'L1', 'L2': 'L2', 'R1': 'R1',
             'R2': 'R2', 'L3': 'L3', 'R3': 'R3', 'Left Stick': 'lstick', 'Right Stick': 'rstick'}
    if t.startswith('D-Pad'):
        return ['dpad'], t[len('D-Pad'):]
    if t == 'Slide Touchpad':
        return ['slide'], ''
    parts = [p.strip() for p in t.split('+')]
    if all(p in words for p in parts):
        return [words[p] for p in parts], ''
    raise ValueError('Unknown diagram label: ' + label)


def detect_icon(image, box):
    """Leading PlayStation symbol of an icon label, from the American English diagram.

    Call-out lines run through the crop, so their green is left out; then the colour tells the
    triangle (green) and the cross (grey) apart from the pink circle and square, and the
    corners of the symbol's box tell the square from the circle."""
    x0, y0, _, y1 = box
    crop = np.asarray(image.crop((x0 - 22, y0 - 2, x0 + 4, y1 + 2)).convert('RGBA')).astype(int)
    rgb, alpha = crop[..., :3], crop[..., 3]
    line = np.abs(rgb - np.array(LINE[:3])).sum(-1) < 60
    keep = (alpha > 150) & ~line
    if keep.sum() < 12:
        return None
    r, g, b = rgb[keep].mean(0)
    if g > r + 20:
        return 'triangle'
    if abs(r - g) < 25 and abs(g - b) < 25:
        return 'cross'
    ys, xs = np.nonzero(keep)
    corners = keep[ys.min():ys.min() + 3, xs.min():xs.min() + 3].sum() + keep[ys.max() - 2:ys.max() + 1, xs.max() - 2:xs.max() + 1].sum()
    return 'square' if corners >= 6 else 'circle'


def bezier(p0, p1, p2, p3, n=24):
    return [tuple((1 - t) ** 3 * np.array(p0) + 3 * (1 - t) ** 2 * t * np.array(p1) +
                  3 * (1 - t) * t * t * np.array(p2) + t ** 3 * np.array(p3)) for t in np.linspace(0, 1, n)]


def shape(right_half):
    """Closed outline from the right half (cubic segments from top centre to bottom centre)."""
    half = []
    for segment in right_half:
        half += bezier(*segment)
    mirror = [(916 - x, y) for x, y in reversed(half)]
    return half + mirror


# Right half of the Xbox body: top edge, shoulder, grip, inner curve back to the centre.
XBOX_OUTLINE = [((458, 566), (520, 560), (590, 556), (640, 566)),
                ((640, 566), (690, 576), (712, 610), (722, 660)),
                ((722, 660), (738, 740), (735, 805), (690, 812)),
                ((690, 812), (650, 818), (622, 786), (596, 760)),
                ((596, 760), (565, 732), (520, 728), (458, 728))]


class Xbox:
    """Xbox controller in the native diagram's footprint (body about 200..715 x 535..810)."""
    def __init__(self):
        c = lambda x, y: (x, y)
        self.points = {'LS': c(300, 640), 'dpad': c(378, 722), 'RS': c(540, 722), 'Y': c(617, 610),
                       'X': c(585, 642), 'B': c(649, 642), 'A': c(617, 674), 'View': c(415, 640),
                       'Menu': c(503, 640), 'LB': c(300, 542), 'RB': c(616, 542), 'LT': c(296, 508),
                       'RT': c(620, 508)}

    def anchor(self, button, kind=''):
        """Where a call-out line ends. A stick has two lines (move, press): they reach it side by
        side instead of sharing one path."""
        x, y = self.points[button]
        if button == 'dpad':  # each direction its own arm, so two D-pad lines never share a path
            return {'up': (x + 2, y - 20), 'down': (x + 2, y + 20), 'side': (x - 20, y)}.get(kind, (x, y))
        if button == 'View':
            return x - 11, y  # left edge: the right column's lines start just past it
        return (x - 12, y) if kind == 'move' else (x + 6, y) if kind == 'press' else (x, y)

    def body(self, d):
        k = lambda pts: [(x * S, y * S) for x, y in pts]
        def blob(cx, cy, rx, ry, a0=0, a1=360, n=64):
            return [(cx + rx * math.cos(math.radians(a)), cy + ry * math.sin(math.radians(a)))
                    for a in np.linspace(a0, a1, n)]
        # Triggers, then bumpers, behind the body (left side mirrored to the right).
        white = (255, 255, 255, 255)
        for mirror in (False, True):
            m = (lambda pts: [(916 - x, y) for x, y in pts]) if mirror else (lambda pts: pts)
            trigger = bezier((236, 560), (240, 516), (270, 500), (300, 502)) + bezier((300, 502), (332, 504), (350, 522), (352, 552))
            bumper = bezier((212, 590), (222, 552), (262, 534), (318, 534)) + bezier((318, 534), (370, 534), (398, 548), (404, 566)) + [(404, 590), (212, 612)]
            d.polygon(k(m(trigger + [(352, 580), (236, 580)])), fill=(14, 14, 14, 255), outline=white, width=2 * S)
            d.polygon(k(m(bumper)), fill=(22, 22, 22, 255), outline=white, width=2 * S)
        outline = shape(XBOX_OUTLINE)
        d.polygon(k(outline), fill=(8, 8, 8, 255), outline=(255, 255, 255, 255), width=3 * S)
        return outline

    def details(self, d, face, names):
        k = lambda x, y: (x * S, y * S)
        ring = lambda p, r, w=2, fill=(8, 8, 8, 255): d.ellipse(
            [k(p[0] - r, p[1] - r), k(p[0] + r, p[1] + r)], fill=fill, outline=(255, 255, 255, 255), width=w * S)
        for stick in ('LS', 'RS'):
            p = self.points[stick]; ring(p, 34, 2, (20, 20, 20, 255)); ring(p, 22, 2, (30, 30, 30, 255))
        x, y = self.points['dpad']; w, r = 9, 25  # one cross, as moulded
        cross = [(x - w, y - r), (x + w, y - r), (x + w, y - w), (x + r, y - w), (x + r, y + w), (x + w, y + w),
                 (x + w, y + r), (x - w, y + r), (x - w, y + w), (x - r, y + w), (x - r, y - w), (x - w, y - w)]
        d.polygon([k(*p) for p in cross], fill=(30, 30, 30, 255), outline=(255, 255, 255, 255), width=2 * S)
        font = ImageFont.truetype(str(FONTS['bold']), 17 * S)
        for b in ('A', 'B', 'X', 'Y'):
            p = self.points[b]; color = face.get(b, (240, 240, 240))
            ring(p, 15, 2, (12, 12, 12, 255))
            d.text(k(*p), b, font=font, fill=color + (255,), anchor='mm')
        for b in ('View', 'Menu'):
            p = self.points[b]; ring(p, 11, 2, (20, 20, 20, 255))
        x, y = self.points['View']  # two overlapping windows
        for dx, dy in ((-2, -2), (2, 2)):
            d.rectangle([k(x + dx - 4, y + dy - 3), k(x + dx + 4, y + dy + 3)], fill=(20, 20, 20, 255),
                        outline=(255, 255, 255, 255), width=S + S // 2)
        x, y = self.points['Menu']  # three lines
        for dy in (-4, 0, 4):
            d.line([k(x - 5, y + dy), k(x + 5, y + dy)], fill=(255, 255, 255, 255), width=S + S // 2)
        ring((458, 588), 17, 2, (40, 40, 40, 255))  # guide button
        small = ImageFont.truetype(str(FONTS['bold']), 10 * S)
        for b, (x, y) in (('LB', (300, 549)), ('RB', (616, 549)), ('LT', (296, 518)), ('RT', (620, 518))):
            d.text(k(x, y), b, font=small, fill=(255, 255, 255, 255), anchor='mm')


def wrap_lines(text):
    return text.split('\n')


def pair_regions(regions):
    """(label, action) pairs by place: each action under the label right above it."""
    boxed = [r for r in regions if 'box' in r]
    labels = [r for r in boxed if r.get('size', 0) <= 13]
    pairs = []
    for action in (r for r in boxed if r.get('size', 0) > 13):
        ax, ay = action['box'][:2]
        near = [l for l in labels if 0 < ay - l['box'][1] < 40 and abs(ax - l['box'][0]) < 45]
        if not near:
            raise ValueError(f"No label above {action.get('text')!r} at {action['box']}")
        pairs.append((min(near, key=lambda l: ay - l['box'][1]), action))
    return pairs


def audit(entries, drawing, buttons=None):
    """Layout problems: lines crossing, lines through text or over another button, texts touching,
    text over the drawing."""
    issues = []
    for e in entries:
        for name, (bx, by) in (buttons or {}).items():
            if abs(bx - e['target'][0]) + abs(by - e['target'][1]) < 30:
                continue
            r = 36 if name in ('LS', 'RS') else 16
            for seg in e['segments'][:1]:  # the run across; the last leg ends on its own button
                (x0, y0), (x1, y1) = seg
                if bx - r < max(x0, x1) and min(x0, x1) < bx + r and by - r < y0 < by + r:
                    issues.append(f"line of {e['name']} over {name}")
    def hits(seg, box, pad=3):
        (x0, y0), (x1, y1) = seg; bx0, by0, bx1, by1 = box[0] - pad, box[1] - pad, box[2] + pad, box[3] + pad
        if y0 == y1:
            return by0 <= y0 <= by1 and min(x0, x1) < bx1 and max(x0, x1) > bx0
        return bx0 <= x0 <= bx1 and min(y0, y1) < by1 and max(y0, y1) > by0
    def cross(a, b):
        h, v = (a, b) if a[0][1] == a[1][1] else (b, a)
        if h[0][1] != h[1][1] or v[0][0] != v[1][0]:
            return False
        x, y = v[0][0], h[0][1]
        return (min(h[0][0], h[1][0]) + 1 < x < max(h[0][0], h[1][0]) - 1 and
                min(v[0][1], v[1][1]) + 1 < y < max(v[0][1], v[1][1]) - 1)
    alpha = None if drawing is None else np.asarray(drawing.getchannel('A'))
    for i, e in enumerate(entries):
        for box in e['boxes']:
            x0, y0, x1, y1 = map(int, box)
            if alpha is not None and (alpha[y0:y1, x0:x1] > 40).any():
                issues.append(f"text over drawing: {e['name']}")
        for j, f in enumerate(entries):
            if i == j:
                continue
            for seg in e['segments']:
                for box in f['boxes']:
                    if hits(seg, box):
                        issues.append(f"line of {e['name']} through text of {f['name']}")
                if j > i:
                    for other in f['segments']:
                        if cross(seg, other):
                            issues.append(f"lines cross: {e['name']} / {f['name']}")
            if j > i:
                for a in e['boxes']:
                    for b in f['boxes']:
                        if a[0] < b[2] + 4 and b[0] < a[2] + 4 and a[1] < b[3] + 4 and b[1] < a[3] + 4:
                            issues.append(f"texts touch: {e['name']} / {f['name']}")
    return sorted(set(issues))


def render(resource, regions, base, device):
    mapping = XBOX if device == 'xbox' else NINTENDO
    pad = Xbox()
    size = base.size
    big = Image.new('RGBA', (size[0] * S, size[1] * S), (0, 0, 0, 0))
    d = ImageDraw.Draw(big)
    outline = pad.body(d)
    # Glow: blurred silhouette under the drawing, in the native green.
    mask = Image.new('L', size, 0)
    ImageDraw.Draw(mask).polygon(outline, fill=255)
    glow = mask.filter(ImageFilter.MaxFilter(15)).filter(ImageFilter.GaussianBlur(14))
    glow_layer = Image.new('RGBA', size, GLOW + (0,))
    glow_layer.putalpha(glow.point(lambda v: v * 70 // 255))
    pad.details(d, FACE_COLORS.get(device, {}), mapping)
    drawing = big.resize(size, Image.LANCZOS)
    out = Image.new('RGBA', size, (0, 0, 0, 0))
    out.alpha_composite(glow_layer); out.alpha_composite(drawing)
    lines = ImageDraw.Draw(out)
    label_font = ImageFont.truetype(str(FONTS['bold']), 13)
    action_font = ImageFont.truetype(str(FONTS['bold']), 16)
    entries = []
    for label, action in pair_regions(regions):
        icon = detect_icon(base, label['box']) if (label.get('text') in (None, '', 'Button')) else None
        if label.get('text') in (None, '', 'Button') and not icon:
            if not (action.get('text') or '').strip():
                continue  # an emptied slot of the native drawing
            raise ValueError(f'{resource}: no symbol found at {label["box"]}')
        controls, rest = classify(label.get('text') or '', icon)
        if controls == ['slide']:
            continue  # no touch surface on this device
        buttons = [mapping[c] for c in controls]
        sticks = {'LS': 'Left Stick', 'RS': 'Right Stick', 'dpad': 'D-Pad'}
        name = ' + '.join(NAMES[b] if b in NAMES else sticks[b] for b in buttons)
        if controls == ['L3']: name = 'Left Stick Button'
        if controls == ['R3']: name = 'Right Stick Button'
        rows = (action['text'] or '').split('\n')
        kind = 'press' if controls in (['L3'], ['R3']) else 'move' if controls[-1] in ('lstick', 'rstick') else ''
        if controls == ['dpad']:
            kind = 'up' if 'Up' in rest else 'down' if 'Down' in rest else 'side' if 'Left' in rest else ''
        entries.append({'label': label, 'action': action, 'name': name + rest, 'rows': rows,
                        'target': pad.anchor(buttons[-1], kind)})
    # Columns: labels sharing a left edge (within 50 px), above or below the body. Inside a
    # column the order of the entries is free: the one whose lines cross nothing and run through
    # no text is kept, as the native diagrams nest theirs.
    columns = []
    for e in sorted(entries, key=lambda e: e['label']['box'][0]):
        x0, y0 = e['label']['box'][:2]
        below = y0 > 760
        if e['name'].startswith('D-Pad'):
            x0 += 45  # its native place sits beside the PlayStation body: join the column above
        for column in columns:
            if column['below'] == below and abs(column['x0'] - x0) < 50:
                column['entries'].append(e); break
        else:
            columns.append({'x0': x0, 'below': below, 'entries': [e]})
    for column in columns:
        column['x0'] = min(e['label']['box'][0] for e in column['entries'])
        column['top'] = min(e['label']['box'][1] for e in column['entries'])
        for e in column['entries']:
            if e['name'].startswith('D-Pad') and device in LABEL_MOVES:
                e['place'] = LABEL_MOVES[device]
    def place(order, column):
        y = column['top']; gap = column.get('gap', 14)
        for e in order:
            x0 = column['x0']
            if 'place' in e:
                x0, e['y'] = e['place']
            else:
                e['y'] = y
                y += 24 + 22 * len(e['rows']) + gap
            ax = x0 - 4
            e['boxes'] = [(x0 + 2, e['y'] + 1, x0 + 2 + label_font.getlength(e['name']), e['y'] + 17)]
            e['boxes'] += [(ax + 10, e['y'] + 27 + 22 * n, ax + 10 + action_font.getlength(row), e['y'] + 45 + 22 * n)
                           for n, row in enumerate(e['rows'])]
            under = e['y'] + 22
            tx, ty = e['target']
            left, right = min(tx, x0 - 6), max(tx, x0 + label_font.getlength(e['name']) + 12)
            e['segments'] = [((left, under), (right, under)), ((tx, under), (tx, ty))]
            e['x0'] = x0
    # Each column takes the lowest place (nearest the native one) and widest spacing where some
    # order of its entries has no layout problem; a column above the body may climb to the top.
    for column in columns:
        native_top, best = column['top'], None
        tops = [native_top] if column['below'] else range(native_top, 23, -4)
        for gap in (14, 10, 6):
            column['gap'] = gap
            for top in tops:
                column['top'] = top
                for order in itertools.permutations(column['entries']):
                    place(order, column)
                    score = len(audit(list(order), drawing, pad.points))
                    if best is None or score < best[0]:
                        best = (score, order, top, gap)
                    if not score:
                        break
                if not best[0]:
                    break
            if not best[0]:
                break
        _, order, column['top'], column['gap'] = best
        place(order, column)
    e_issues = audit(entries, drawing, pad.points)
    for e in entries:
        x0 = e['x0']; ax = x0 - 4; y = e['y']
        lines.text((x0 + 2, y + 1), e['name'], font=label_font, fill=(205, 205, 205, 255))
        for n, row in enumerate(e['rows']):
            lines.text((ax + 10, y + 27 + 22 * n), row, font=action_font, fill=(242, 242, 242, 255))
        for segment in e['segments']:
            lines.line(segment, fill=LINE, width=2)
    out.issues = e_issues
    return out


# Small lists (*_s.tex): a column of PlayStation symbols, then the action. The symbols are
# swapped for the device's font icons (make_xbox_icons, by game function as on screen) and the
# text stays. operation_normal_s, labelled once by hand, gives the symbol templates.
# Lists labelled by hand: their symbols are the templates (D-pad art differs per highlighted arm).
TEMPLATE_LISTS = {
    'texture/controller_operation/operation_normal_s.tex': [
        ['cross'], ['circle'], ['triangle'], ['square'], ['lstick'], ['lstick', 'plus', 'R1'], ['rstick'],
        ['L3'], ['R3'], ['dpad'], ['L1', 'or', 'L2'], ['R2'], ['touchpad'], ['options']],
    'texture/controller_operation/operation_normal_207_s.tex': [
        ['cross'], ['circle'], ['triangle'], ['square'], ['lstick'], ['lstick', 'plus', 'R1'], ['rstick'],
        ['L3'], ['R3'], ['dpad'], ['dpad'], ['L1', 'or', 'L2'], ['R2'], ['touchpad'], ['options']],
    'texture/controller_operation/operation_forklift_s.tex': [
        ['cross'], ['circle'], ['triangle'], ['lstick'], ['rstick'], ['L3'], ['R3'], ['dpad'], ['dpad'],
        ['L1', 'or', 'L2'], ['R1'], ['R2'], ['touchpad'], ['options']]}
# Kept as drawn on every pad (as the font icons): only the others are replaced.
LIST_KEEP = {'dpad', 'lstick', 'rstick', 'plus', 'or'}
LIST_ICONS = {'cross': 'font/icon/IC_maru_1.tex', 'circle': 'font/icon/IC_batu_2.tex',
              'triangle': 'font/icon/IC_sankaku_3.tex', 'square': 'font/icon/IC_sikaku_4.tex',
              'lstick': 'font/icon/IC_s_L_14.tex', 'rstick': 'font/icon/IC_s_R_15.tex',
              'L3': 'font/icon/IC_s_L3_16.tex', 'R3': 'font/icon/IC_s_R3_17.tex', 'L1': 'font/icon/IC_L1_10.tex',
              'L2': 'font/icon/IC_L2_11.tex', 'R1': 'font/icon/IC_R1_12.tex', 'R2': 'font/icon/IC_R2_13.tex',
              'dpad': 'font/icon/IC_juzhi_5.tex', 'touchpad': 'font/icon/IC_touch_pad_20.tex',
              'options': 'font/icon/IC_options_18.tex'}


def list_rows(regions):
    return [r for r in regions if 'box' in r and r.get('text')]


def list_glyphs(image, row):
    """Symbols left of a row's text, as boxes split at empty columns."""
    x0, y0, _, y1 = row['box']
    alpha = np.asarray(image.getchannel('A'))[y0:y1, 8:x0 - 3] > 60
    used = alpha.any(0)
    boxes, start = [], None
    for x, on in enumerate(list(used) + [False]):
        if on and start is None:
            start = x
        elif not on and start is not None:
            if x - start > 2:
                rows = np.nonzero(alpha[:, start:x].any(1))[0]
                boxes.append((8 + start, y0 + rows[0], 8 + x, y0 + rows[-1] + 1))
            start = None
    # A symbol drawn in two parts (the stick's L, the D-pad arms) has gaps: join close boxes.
    joined = []
    for box in boxes:
        if joined and box[0] - joined[-1][2] < 4:
            last = joined.pop(); box = (last[0], min(last[1], box[1]), box[2], max(last[3], box[3]))
        joined.append(box)
    return joined


def glyph_key(image, box):
    crop = image.crop(box).convert('RGBA').resize((40, 40), Image.LANCZOS)
    rgba = np.asarray(crop, np.float32) / 255
    # Colour for the face buttons, plus the light strokes (letters L/R, 1/2/3) at full weight.
    strokes = (rgba[..., :3].mean(-1) > .6) & (rgba[..., 3] > .5)
    return np.concatenate([rgba.reshape(-1), np.repeat(strokes.reshape(-1).astype(np.float32), 3)])


def list_templates(template_image, regions, rows_names):
    templates = []
    rows = list_rows(regions)
    if len(rows) != len(rows_names):
        raise ValueError(f'Template list has {len(rows)} rows, expected {len(rows_names)}')
    for row, names in zip(rows, rows_names):
        glyphs = list_glyphs(template_image, row)
        if len(glyphs) != len(names):
            raise ValueError(f'Template row {row["text"]!r}: {len(glyphs)} symbols, expected {names}')
        templates += [(name, glyph_key(template_image, box), box[3] - box[1]) for name, box in zip(names, glyphs)]
    return templates


def classify_glyph(templates, image, box, text=''):
    key = glyph_key(image, box)
    ranked = sorted(((float(np.abs(key - t).mean()), n) for n, t, _ in templates))
    distance, name = ranked[0]
    runner = next(d for d, n in ranked if n != name)
    if {name, next(n for d, n in ranked if n != name)} == {'L3', 'R3'}:
        # Same stick art, one letter apart: the row says which (as on every native list).
        return 'L3' if 'Controls Display' in text else 'R3' if 'Reset Camera' in text else name
    # Rows are drawn per list (glow, tint): accept the nearest symbol when it clearly stands out.
    if distance > .15 or runner < distance * 1.12:
        raise ValueError(f'Unclear list symbol at {box}: {ranked[:3]}')
    return name


def render_list(resource, regions, base, templates, icons):
    out = base.copy()
    for row in list_rows(regions):
        for box in list_glyphs(base, row):
            name = classify_glyph(templates, base, box, row['text'])
            if name in LIST_KEEP:
                continue
            icon = icons[LIST_ICONS[name]]
            icon = icon.crop(icon.getchannel('A').point(lambda v: 255 if v > 30 else 0).getbbox())
            h = box[3] - box[1] + 2
            icon = icon.resize((max(1, round(icon.width * h / icon.height)), h), Image.LANCZOS)
            cx, cy = (box[0] + box[2]) // 2, (box[1] + box[3]) // 2
            out.paste(Image.new('RGBA', (box[2] - box[0] + 4, box[3] - box[1] + 4), (0, 0, 0, 0)), (box[0] - 2, box[1] - 2))
            out.alpha_composite(icon, (cx - icon.width // 2, cy - icon.height // 2))
    return out


def build_all(archive_path, devices=('xbox', 'nintendo')):
    """Every controls diagram of an English archive, per device: {device: {resource: image}}."""
    from psarc import Psarc
    from tex_preview import preview
    data = json.loads((REPO / 'translations/textures-en.json').read_text(encoding='utf-8'))
    from localize_textures import decode
    from make_xbox_icons import build as pad_icons
    out = {d: {} for d in devices}
    with Psarc(archive_path) as archive:
        names = {e.name: e for e in archive.entries[1:]}
        originals = {n: b''.join(archive.chunks(names[n])) for n in set(LIST_ICONS.values())}
        icons = {d: {n: decode(v)[1] for n, v in pad_icons(originals, d).items()} for d in devices}
        for d in devices:  # sticks and D-pad keep the native art on every pad
            for n in LIST_ICONS.values():
                icons[d].setdefault(n, decode(originals[n])[1])
        templates = []
        for resource, rows_names in TEMPLATE_LISTS.items():
            regions = next(t for t in data['textures'] if t['resource'] == resource)['regions']
            image = preview(b''.join(archive.chunks(names[resource])))[0].convert('RGBA')
            templates += list_templates(image, regions, rows_names)
        for t in data['textures']:
            name = t['resource']
            if not name.startswith('texture/controller_operation/operation_'):
                continue
            base = preview(b''.join(archive.chunks(names[name])))[0].convert('RGBA')
            if name.endswith('_s.tex'):
                for device in devices:
                    out[device][name] = render_list(name, t['regions'], base, templates, icons[device])
                continue
            for device in devices:
                image = render(name, t['regions'], base, device)
                for issue in image.issues:
                    print(f'LAYOUT {device} {name}: {issue}')
                out[device][name] = image
    return out


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('base_png', type=Path, help='English PlayStation diagram as PNG (icon detection)')
    ap.add_argument('resource'); ap.add_argument('device', choices=('xbox', 'nintendo'))
    ap.add_argument('output', type=Path)
    a = ap.parse_args()
    data = json.loads((REPO / 'translations/textures-en.json').read_text(encoding='utf-8'))
    regions = next(t['regions'] for t in data['textures'] if t['resource'] == a.resource)
    image = render(a.resource, regions, Image.open(a.base_png).convert('RGBA'), a.device)
    for issue in image.issues:
        print('LAYOUT:', issue)
    image.save(a.output)


if __name__ == '__main__':
    main()
