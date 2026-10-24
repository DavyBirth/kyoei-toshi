"""UI art atlas for the PC screens (photo mode, saves, pause entry) in the game's own style.

Pieces come from the owner's dump: the Option menu's title bar (its label
removed), selection marker, row line, accent bar and description frame, the
pause screen's line, and the button icons for each pad. PlayStation uses the
American layout (cross confirms, circle goes back); Xbox and keyboard use the
same drawings as the live prompts (make_xbox_icons, make_keyboard_icons); Nintendo too.
Output KYART2: magic, count, then per image a 24-byte name, width, height and
RGBA pixels. It stays private, next to the game data.
"""
import argparse, json, struct, sys
from pathlib import Path
import numpy as np
from PIL import Image
sys.path.insert(0, str(Path(__file__).parent))
from psarc import Psarc
from tex_preview import preview
import make_keyboard_icons, make_xbox_icons

PIECES = {
    'option_point': 'texture/menu_option/M_option_point.tex',
    'option_line': 'texture/menu_option/M_option_line.tex',
    'option_bar': 'texture/menu_option/M_option_bar.tex',
    'option_frame': 'texture/menu_option/M_option_frame.tex',
    'pause_line': 'texture/pause/pause_line.tex',
}
TITLE = 'texture/menu_option/mm05_title.tex'
# Logical button -> the game's icon texture showing that physical button.
FACE = {'A': 'font/icon/IC_batu_2.tex', 'B': 'font/icon/IC_maru_1.tex',
        'X': 'font/icon/IC_sikaku_4.tex', 'Y': 'font/icon/IC_sankaku_3.tex'}
OTHER = {'LB': 'font/icon/IC_L1_10.tex', 'RB': 'font/icon/IC_R1_12.tex', 'LT': 'font/icon/IC_L2_11.tex',
         'RT': 'font/icon/IC_R2_13.tex', 'LS': 'font/icon/IC_s_L3_16.tex', 'RS': 'font/icon/IC_s_R3_17.tex',
         'LSTICK': 'font/icon/IC_s_L_14.tex', 'RSTICK': 'font/icon/IC_s_R_15.tex', 'DPAD': 'font/icon/IC_juzhi_5.tex',
         'DPAD_UD': 'font/icon/IC_juzhi_ue_8.tex', 'MENU': 'font/icon/IC_options_18.tex',
         'VIEW': 'font/icon/IC_touch_pad_20.tex'}
# Xbox: the circle texture becomes A, the cross B (American layout, as the live prompts).
XBOX_SOURCE = {'A': 'font/icon/IC_maru_1.tex', 'B': 'font/icon/IC_batu_2.tex',
               'X': 'font/icon/IC_sikaku_4.tex', 'Y': 'font/icon/IC_sankaku_3.tex'}
KEYS = ['E', 'Esc', 'Tab', 'F9', 'F10', 'F11', 'F12', 'R', 'WASD', 'Space', 'Ctrl', 'Shift', 'Enter',
        'Q', '@mouse', '@arrows', '@up', '@down', '@left', '@right', 'Backspace']


def title_bar(image):
    """The Option title bar without its label: the stripes are rebuilt across the label by
    blending the columns on both sides of it."""
    a = np.asarray(image.convert('RGBA')).astype(np.float32)
    left, right = 196, 430
    for x in range(left, right):
        t = (x - left) / (right - left)
        a[:, x] = a[:, left - 1] * (1 - t) + a[:, right] * t
    return Image.fromarray(np.clip(a + .5, 0, 255).astype(np.uint8))


def build(archive_path):
    images = {}
    with Psarc(archive_path) as archive:
        entries = {e.name: e for e in archive.entries[1:]}
        read = lambda name: b''.join(archive.chunks(entries[name]))
        look = lambda name: preview(read(name))[0].convert('RGBA')
        images['option_title'] = title_bar(look(TITLE))
        for key, name in PIECES.items(): images[key] = look(name)
        for key, name in {**FACE, **OTHER}.items(): images['ps_' + key] = look(name)
        # Named by role: A confirms, B goes back, X is the left button, Y the top one. Nintendo
        # draws them where its pad has them (A right, B bottom, Y left, X top).
        for family in ('xbox', 'nintendo'):
            for key, name in {**XBOX_SOURCE, **OTHER}.items():
                drawn = make_xbox_icons.draw(name, look(name), family)
                images[family + '_' + key] = drawn if drawn is not None else look(name)
    # Long lines are stretched on screen anyway: keep the font atlas small.
    for key, image in images.items():
        if image.width > 1024: images[key] = image.resize((1024, image.height), Image.LANCZOS)
    for label in KEYS:
        name = label.strip('@')
        images['key_' + name] = make_keyboard_icons.keycap(56 if len(label) <= 3 or label.startswith('@') else 96, 50, label)
    return images


def write(images, path):
    out = bytearray(b'KYART2\0\0' + struct.pack('<I', len(images)))
    for name, image in images.items():
        if len(name) >= 24: raise ValueError('name too long: ' + name)
        image = image.convert('RGBA')
        out += name.encode().ljust(24, b'\0') + struct.pack('<II', *image.size) + image.tobytes()
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(out)
    return len(out)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('archive', type=Path, help="common.psarc from the user's dump")
    parser.add_argument('output', type=Path)
    parser.add_argument('--sheet', type=Path, help='write every piece on one PNG for review')
    args = parser.parse_args()
    images = build(args.archive)
    size = write(images, args.output)
    if args.sheet:
        y, rows = 0, []
        for name, image in images.items():
            im = image if image.width <= 1400 else image.resize((1400, max(1, image.height * 1400 // image.width)))
            rows.append((y, im)); y += im.height + 8
        sheet = Image.new('RGBA', (1420, y), (40, 50, 60, 255))
        for top, im in rows: sheet.alpha_composite(im, (10, top))
        sheet.save(args.sheet)
    print(json.dumps(dict(images=len(images), bytes=size)))


if __name__ == '__main__':
    main()
