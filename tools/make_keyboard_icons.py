"""Draw PC keyboard/mouse prompts over the controller button icons.

Keeps each original texture's size, mip count and container header, so the
game's icon font tables stay valid. Bindings follow src/kyoei_pc_controls.h.
Outputs are derived from the user's dump (headers) and stay private.
"""
import argparse, io, json, struct, sys, zlib
from pathlib import Path
from PIL import Image, ImageDraw, ImageFont
sys.path.insert(0, str(Path(__file__).parent))
from tex_preview import unpack_tex
from localize_textures import decode as decode_bc3, encode_tex as encode_bc3_tex

REPO = Path(__file__).resolve().parents[1]
FONT = REPO / 'third_party/godzilla-core/third_party/shadPS4/src/imgui/renderer/fonts/NotoSans-Regular.ttf'

# archive path -> keycap label ('@mouse' draws a mouse, '@arrows' an arrow cluster)
BINDINGS = {
    'font/icon/IC_maru_1.tex': 'E',
    'font/icon/IC_batu_2.tex': 'Space',
    'font/icon/IC_sankaku_3.tex': 'F',
    'font/icon/IC_sikaku_4.tex': 'Ctrl',
    'font/icon/IC_juzhi_5.tex': '@arrows',
    'font/icon/IC_juzhi_migi_6.tex': '@right',
    'font/icon/IC_juzhi_hidari_7.tex': '@left',
    'font/icon/IC_juzhi_ue_8.tex': '@up',
    'font/icon/IC_juzhi_sita_9.tex': '@down',
    'font/icon/IC_L1_10.tex': 'Q',
    'font/icon/IC_L2_11.tex': 'V',
    'font/icon/IC_R1_12.tex': 'Shift',
    'font/icon/IC_R2_13.tex': 'G',
    'font/icon/IC_s_L_14.tex': 'WASD',
    'font/icon/IC_s_R_15.tex': '@mouse',
    'font/icon/IC_s_L3_16.tex': 'H',
    'font/icon/IC_s_R3_17.tex': 'R',
    'font/icon/IC_options_18.tex': 'Esc',
    'font/icon/IC_touch_pad_20.tex': 'M',
    'texture/hud/maru_button.tex': 'E',
    'texture/treasure_change/TC_maru_01.tex': 'E',
    'texture/treasure_change/TC_batu_01.tex': 'Space',
    'texture/hud/sikaku_button.tex': 'Ctrl',
    # BC3 HUD/intermission prompts.
    'texture/intermission/IM_next_maru_button.tex': 'E',
    'texture/intermission/IM_next_batu_button.tex': 'Space',
    'texture/intermission/IM_dialog_o_btn.tex': 'E',
    'texture/intermission/IM_dialog_x_btn.tex': 'Space',
    'texture/intermission/IM_l_cursor_button.tex': 'Q',
    'texture/intermission/IM_r_cursor_button.tex': 'Shift',
    'texture/controller_operation/cmnd_marker_op.tex': '@marker',
    'texture/hud/action_kaihi_batu_1.tex': '@dodge1',
    'texture/hud/action_kaihi_batu_2.tex': '@dodge2',
    'texture/intermission/L1_btn.tex': 'Q',
    'texture/intermission/R1_btn.tex': 'Shift',
    'texture/pause/demo_skip_icon.tex': '@skip',
    'texture/hud/demo_skip_icon.tex': '@skip',
    'texture/hud/cursor_lr.tex': '@cursor_lr',
    **{f'texture/text marker/talk_icon_0{i}.tex': '@talk' for i in range(1, 7)},
    'texture/hud/cursol_nextchat.tex': '@nextchat',
    'texture/stage_clear/sc_next.tex': '@scnext',
    'texture/hud/resalt_next.tex': '@resalt',
}
SCALE = 8


def fit(draw, text, font_path, box_w, box_h):
    size = box_h
    while size > 4:
        font = ImageFont.truetype(str(font_path), size)
        l, t, r, b = draw.textbbox((0, 0), text, font=font, stroke_width=max(1, size // 18))
        if r - l <= box_w and b - t <= box_h: return font, (l, t, r, b)
        size -= 2
    font = ImageFont.truetype(str(font_path), 4)
    return font, draw.textbbox((0, 0), text, font=font)


def keycap(w, h, label):
    W, H = w * SCALE, h * SCALE
    im = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    pad = max(2, min(W, H) // 14)
    radius = min(W, H) // 5
    # Drop shadow, glossy grey body and a lighter rim, like the original icons.
    d.rounded_rectangle((pad, pad + SCALE // 2, W - pad, H - pad), radius, fill=(0, 0, 0, 80))
    # Body: smooth top-to-bottom gradient (no separate highlight shape, which
    # read as a white box at the game's icon size), then the light rim.
    body = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    shade = ImageDraw.Draw(body)
    top, bottom = pad, H - pad - SCALE
    for y in range(top, bottom):
        t = (y - top) / max(1, bottom - top)
        v = int(78 - 30 * t)
        shade.line((0, y, W, y), fill=(v, v + 2, v + 6, 255))
    mask = Image.new('L', (W, H), 0)
    ImageDraw.Draw(mask).rounded_rectangle((pad, pad, W - pad, H - pad - SCALE), radius, fill=255)
    im.paste(body, (0, 0), mask)
    d.rounded_rectangle((pad, pad, W - pad, H - pad - SCALE), radius, outline=(160, 164, 170, 255), width=SCALE)
    box = (pad + 4 * SCALE, pad + 3 * SCALE, W - pad - 4 * SCALE, H - pad - 4 * SCALE)
    bw, bh = box[2] - box[0], box[3] - box[1]
    if label == '@mouse':
        mw, mh = int(bh * .62), bh
        x0, y0 = (W - mw) // 2, box[1]
        d.rounded_rectangle((x0, y0, x0 + mw, y0 + mh), mw // 2, outline=(255, 255, 255, 255), width=SCALE + 4)
        d.line((x0 + mw // 2, y0, x0 + mw // 2, y0 + mh * 0.42), fill=(255, 255, 255, 255), width=SCALE + 2)
        d.line((x0, y0 + mh * 0.42, x0 + mw, y0 + mh * 0.42), fill=(255, 255, 255, 255), width=SCALE + 2)
    elif label in ('@arrows', '@up', '@down', '@left', '@right'):
        # Arrow keys as drawn triangles; the cluster shows all four.
        def arrow(cx, cy, r, direction):
            pts = {'@up': [(0, -1), (1, .8), (-1, .8)], '@down': [(0, 1), (1, -.8), (-1, -.8)],
                   '@left': [(-1, 0), (.8, -1), (.8, 1)], '@right': [(1, 0), (-.8, -1), (-.8, 1)]}[direction]
            d.polygon([(cx + x * r, cy + y * r) for x, y in pts], fill=(255, 255, 255, 255))
        cx, cy = (box[0] + box[2]) / 2, (box[1] + box[3]) / 2
        if label == '@arrows':
            r = min(bw, bh) / 7
            arrow(cx, cy - 2.1 * r, r, '@up'); arrow(cx, cy + 2.1 * r, r, '@down')
            arrow(cx - 2.1 * r, cy, r, '@left'); arrow(cx + 2.1 * r, cy, r, '@right')
        else:
            arrow(cx, cy, min(bw, bh) / 3, label)
    else:
        font, (l, t, r, b) = fit(d, label, FONT, bw, int(bh * (.78 if len(label) == 1 else .62)))
        stroke = max(1, font.size // 18)
        d.text(((box[0] + box[2]) / 2, (box[1] + box[3]) / 2), label, font=font, fill=(255, 255, 255, 255),
               anchor='mm', stroke_width=stroke, stroke_fill=(255, 255, 255, 255))
    return im.resize((w, h), Image.LANCZOS)


def marker(w, h, english):
    """Arrow-key cap followed by the 'switch target' label (cmnd_marker_op)."""
    im = Image.new('RGBA', (w * SCALE, h * SCALE), (0, 0, 0, 0))
    cap = keycap(h, h, '@arrows').resize((h * SCALE, h * SCALE), Image.LANCZOS)
    im.alpha_composite(cap, (0, 0))
    d = ImageDraw.Draw(im)
    text = 'Trocar alvo' if english=='pt-BR' else 'Switch target' if english else '対象切替'
    font_path = FONT if english else REPO / 'third_party/godzilla-core/third_party/shadPS4/src/imgui/renderer/fonts/NotoSansCJK-Regular.ttc'
    box_w, box_h = (w - h - 4) * SCALE, int(h * SCALE * .5)
    font, _ = fit(d, text, font_path, box_w, box_h)
    d.text(((h + 4) * SCALE, h * SCALE / 2), text, font=font, fill=(235, 235, 235, 255), anchor='lm',
           stroke_width=max(1, font.size // 16), stroke_fill=(30, 30, 30, 200))
    return im.resize((w, h), Image.LANCZOS)


def dodge(w, h, glow):
    """Red quick-time prompt: Space instead of the cross."""
    W, H = w * SCALE, h * SCALE
    im = Image.new('RGBA', (W, H), (0, 0, 0, 0))
    d = ImageDraw.Draw(im)
    font, _ = fit(d, 'SPACE', FONT, int(W * .8), int(H * .3))
    color = (235, 30, 40, 255) if glow else (150, 25, 30, 230)
    if glow:
        from PIL import ImageFilter
        halo = Image.new('RGBA', (W, H), (0, 0, 0, 0))
        ImageDraw.Draw(halo).text((W / 2, H / 2), 'SPACE', font=font, fill=(255, 40, 40, 200), anchor='mm',
                                  stroke_width=font.size // 6, stroke_fill=(255, 40, 40, 160))
        im.alpha_composite(halo.filter(ImageFilter.GaussianBlur(SCALE * 3)))
    d.text((W / 2, H / 2), 'SPACE', font=font, fill=color, anchor='mm', stroke_width=max(1, font.size // 14), stroke_fill=color)
    return im.resize((w, h), Image.LANCZOS)


def talk(original_image):
    """Talk pin: keep the stem, replace the circle head with an E key."""
    im = original_image.copy()
    alpha = im.getchannel('A')
    w, h = im.size
    # Head = rows wider than the stem.
    rows = [y for y in range(h) if sum(1 for x in range(w) if alpha.getpixel((x, y)) > 40) > max(3, w // 6)]
    if not rows: return im
    top, bottom = rows[0], rows[-1]
    cols = [x for x in range(w) if any(alpha.getpixel((x, y)) > 8 for y in range(top, bottom + 1))]
    im.paste(Image.new('RGBA', (cols[-1] - cols[0] + 1, bottom - top + 1), (0, 0, 0, 0)), (cols[0], top))
    size = bottom - top + 1
    left = (cols[0] + cols[-1] + 1 - size) // 2
    im.alpha_composite(keycap(size, size, 'E'), (left, top))
    return im


def skip(original_image, english):
    """'Scene Skip' banner: the triangle becomes an F key, the text stays."""
    im = original_image.copy()
    h = im.height
    head = int(h * 1.0)
    im.paste(Image.new('RGBA', (head, h), (0, 0, 0, 0)), (0, 0))
    im.alpha_composite(keycap(head, h, 'F'), (0, 0))
    return im


def cursor_lr(original_image, page_keys=False):
    """Pause-menu page arrows: L1/R1 tags above the arrows become Q/Shift."""
    im = original_image.copy()
    # The tags are the only dark, opaque pixels (arrows are green glow).
    dark = Image.eval(im.convert('L'), lambda v: 255 if v < 70 else 0)
    mask = Image.composite(dark, Image.new('L', im.size, 0), im.getchannel('A').point(lambda a: 255 if a > 90 else 0))
    for left_half, label in ((True, 'A' if page_keys else '@left'), (False, 'D' if page_keys else '@right')):
        x0, x1 = (0, im.width // 2) if left_half else (im.width // 2, im.width)
        box = mask.crop((x0, 0, x1, im.height)).getbbox()
        bx0, by0 = x0 + box[0], box[1]
        bw, bh = box[2] - box[0], box[3] - box[1]
        im.paste(Image.new('RGBA', (bw + 12, bh + 12), (0, 0, 0, 0)), (bx0 - 6, by0 - 6))
        cw = max(bw, int(bh * 1.2))
        im.alpha_composite(keycap(cw, bh, label), (bx0 + (bw - cw) // 2, by0))
    return im


def face_key(original_image, box):
    """Replace one face-button glyph (inside box) by an E keycap of the same height."""
    im = original_image.copy(); x0, y0, x1, y1 = box
    im.paste(Image.new('RGBA', (x1 - x0, y1 - y0), (0, 0, 0, 0)), (x0, y0))
    size = y1 - y0
    im.alpha_composite(keycap(size, size, 'E'), ((x0 + x1 - size) // 2, y0))
    return im


def face_box(original_image):
    """Bounding box of the colored button disc (opaque pixels with chroma)."""
    px = original_image.load(); w, h = original_image.size
    pts = [(x, y) for y in range(h) for x in range(w) if px[x, y][3] > 150 and max(px[x, y][:3]) - min(px[x, y][:3]) > 18]
    xs = [p[0] for p in pts]; ys = [p[1] for p in pts]
    cx, cy = (min(xs) + max(xs)) // 2, (min(ys) + max(ys)) // 2; r = max(max(xs) - min(xs), max(ys) - min(ys)) // 2 + 6
    return (max(0, cx - r), max(0, cy - r), min(w, cx + r + 1), min(h, cy + r + 1))


def resalt(original_image):
    """'Press ◎ Button': the ◎ becomes an E keycap; the background behind it is rebuilt."""
    from make_american_icons import refill
    cx, cy, r = 251.5, 62.5, 16
    holes = [(x, y) for y in range(int(cy - r), int(cy + r) + 2) for x in range(int(cx - r), int(cx + r) + 2) if (x - cx) ** 2 + (y - cy) ** 2 <= r * r]
    im = original_image.copy(); px = im.load()
    for x, y in holes: px[x, y] = (0, 0, 0, 0)
    im = refill(im, holes, (int(cx - r) - 1, int(cy - r) - 1, int(cx + r) + 3, int(cy + r) + 3))
    size = 30
    im.alpha_composite(keycap(size, size, 'E'), (int(cx - size / 2 + .5), int(cy - size / 2 + .5)))
    return im


def draw(label, w, h, english, original_image=None):
    if label == '@nextchat': return face_key(original_image, face_box(original_image))
    if label == '@scnext': return face_key(original_image, face_box(original_image))
    if label == '@resalt': return resalt(original_image)
    if label == '@cursor_lr': return cursor_lr(original_image)
    if label == '@talk': return talk(original_image)
    if label == '@skip': return skip(original_image, english)
    if label == '@marker': return marker(w, h, english)
    if label in ('@dodge1', '@dodge2'): return dodge(w, h, label == '@dodge2')
    return keycap(w, h, label)


def encode_rgba(original, image):
    info, body = unpack_tex(original)
    if info['storage_family'] != 1 or info['channel_layout'] not in (0, 0x10):
        raise ValueError('Only linear RGBA8 icons are supported')
    raw, level = bytearray(bytes.fromhex(info['descriptor'])), image
    for _ in range(info['levels']):
        raw += level.tobytes()
        level = level.resize((max(1, level.width // 2), max(1, level.height // 2)), Image.LANCZOS)
    if len(raw) - 12 != info['payload_bytes']: raise ValueError('Mip chain size mismatch')
    packed = zlib.compress(bytes(raw), 9)
    head = bytearray(original[:144])
    old = struct.unpack_from('>4sIII', original, 128)
    struct.pack_into('>4sIII', head, 128, old[0], len(raw), len(packed), old[3])
    result = bytes(head) + packed
    check, _ = unpack_tex(result)
    if (check['width'], check['height'], check['levels']) != (info['width'], info['height'], info['levels']):
        raise ValueError('Re-encoded header differs')
    return result


def build(archive_path, english=True):
    from psarc import Psarc
    out = {}
    with Psarc(archive_path) as archive:
        names = {e.name: e for e in archive.entries[1:]}
        for name, label in BINDINGS.items():
            original = b''.join(archive.chunks(names[name]))
            info, _ = unpack_tex(original)
            from tex_preview import preview
            image = draw(label, info['width'], info['height'], english, preview(original)[0].convert('RGBA'))
            if info['storage_family'] == 13:
                info, _ = decode_bc3(original)
                out[name] = encode_bc3_tex(original, image, info)
            else:
                out[name] = encode_rgba(original, image)
    return out


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('archive', type=Path, help='common.psarc from the user dump (or a translation of it)')
    parser.add_argument('--sheet', type=Path, help='write a preview PNG of the generated icons')
    args = parser.parse_args()
    icons = build(args.archive)
    if args.sheet:
        from tex_preview import preview
        sheet = Image.new('RGBA', (260 * 6, 270 * ((len(icons) + 5) // 6)), (40, 40, 40, 255))
        for i, (name, data) in enumerate(icons.items()):
            image, _ = preview(data)
            sheet.alpha_composite(image, ((i % 6) * 260 + 5, (i // 6) * 270 + 5))
        sheet.save(args.sheet)
    print(json.dumps(dict(icons=len(icons), bytes=sum(map(len, icons.values())))))


if __name__ == '__main__':
    main()
