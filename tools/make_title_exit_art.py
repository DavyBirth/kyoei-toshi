"""Draw the 'Return to Desktop' title-menu item in the native item style.

The native items (texture/game_title/title_menu_*.tex) are white text with a
soft halo, plus a blurred '_f' layer for the focused item. This tool fits the
font size and both blurs against the native 'New Game' pair from the user's
dump, then renders the new label with the same parameters. Output PNGs are
private derivatives and go into the portable's assets folder.
"""
import argparse, json, sys
from pathlib import Path
from PIL import Image, ImageDraw, ImageFilter, ImageFont
import numpy as np
sys.path.insert(0, str(Path(__file__).parent))
from psarc import Psarc
from tex_preview import preview

FONT = Path('C:/Windows/Fonts/seguisb.ttf')  # closest shape match (fit against New Game)
LABEL = 'Return to Desktop'
STROKE = 2  # in 4x pixels: half a pixel heavier than Segoe UI Semibold


def alpha(archive, name):
    names = {e.name: e for e in archive.entries[1:]}
    image, _ = preview(b''.join(archive.chunks(names[name])))
    return np.array(image.convert('RGBA').getchannel('A'), dtype=float)


def glyphs(text, size, canvas, origin):
    # 4x supersampling allows a half-pixel stroke: the native weight sits
    # between Segoe UI Semibold and Semibold + 1 px.
    scale = 4
    font = ImageFont.truetype(str(FONT), size * scale)
    layer = Image.new('L', (canvas[0] * scale, canvas[1] * scale), 0)
    ImageDraw.Draw(layer).text((origin[0] * scale, origin[1] * scale), text, font=font, fill=255, anchor='ls',
                               stroke_width=STROKE, stroke_fill=255)
    return layer.resize(canvas, Image.LANCZOS)


def fit(native, focus):
    core = native > 200
    ys, xs = np.where(core)
    height = ys.max() - ys.min()
    best = None
    for size in range(24, 44):
        probe = np.array(glyphs('New Game', size, (400, 120), (50, 90)), dtype=float)
        yy, xx = np.where(probe > 200)
        if abs((yy.max() - yy.min()) - height) > 2: continue
        shifted = Image.new('L', native.shape[::-1], 0)
        shifted.paste(Image.fromarray(probe.astype(np.uint8)), (int(xs.min() - xx.min()), int(ys.min() - yy.min())))
        error = np.logical_xor(np.array(shifted) > 128, core).mean()
        if best is None or error < best[0]: best = (error, size, shifted, (int(xs.min() - xx.min() + 50), int(ys.min() - yy.min() + 90)))
    _, size, text, origin = best
    sharp = np.array(text, dtype=float)
    halo = min(((np.abs(np.maximum(sharp, np.minimum(255, np.array(text.filter(ImageFilter.GaussianBlur(r)), dtype=float) * k)) - native).mean(), r, k)
                for r in range(2, 15) for k in (0.3, 0.5, 0.7, 1.0, 1.4, 2.0)))
    glow = min(((np.abs(np.minimum(255, np.array(text.filter(ImageFilter.GaussianBlur(r)), dtype=float) * k) - focus).mean(), r, k)
                for r in range(3, 18) for k in (1.0, 1.5, 2.0, 2.5, 3.0, 4.0)))
    return dict(size=size, origin=origin, halo_radius=halo[1], halo_gain=halo[2], glow_radius=glow[1], glow_gain=glow[2],
                halo_error=round(float(halo[0]), 3), glow_error=round(float(glow[0]), 3))


def render(params, text=LABEL):
    font = ImageFont.truetype(str(FONT), params['size'])
    width = int(ImageDraw.Draw(Image.new('L', (1, 1))).textlength(text, font=font)) + 2 + 2 * params['origin'][0]
    canvas = (max(256, (width + 15) // 16 * 16), 72)
    text_layer = glyphs(text, params['size'], canvas, params['origin'])
    halo = text_layer.filter(ImageFilter.GaussianBlur(params['halo_radius'])).point(lambda v: min(255, int(v * params['halo_gain'])))
    normal_alpha = Image.fromarray(np.maximum(np.array(text_layer), np.array(halo)).astype(np.uint8))
    focus_alpha = text_layer.filter(ImageFilter.GaussianBlur(params['glow_radius'])).point(lambda v: min(255, int(v * params['glow_gain'])))
    def white(a):
        image = Image.new('RGBA', canvas, (255, 255, 255, 0)); image.putalpha(a); return image
    return white(normal_alpha), white(focus_alpha)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('archive', type=Path)
    parser.add_argument('output', type=Path, help='folder for title-exit.png and title-exit-f.png')
    parser.add_argument('--language',choices=('en','pt-BR'),default='en')
    args = parser.parse_args()
    with Psarc(args.archive) as archive:
        params = fit(alpha(archive, 'texture/game_title/title_menu_newgame.tex'),
                     alpha(archive, 'texture/game_title/title_menu_newgame_f.tex'))
    normal, focus = render(params,'Sair do jogo' if args.language=='pt-BR' else LABEL)
    args.output.mkdir(parents=True, exist_ok=True)
    normal.save(args.output / 'title-exit.png'); focus.save(args.output / 'title-exit-f.png')
    # Runtime form for the overlay: b'KYART1' and two zero bytes, width, height (u32 LE), then
    # the normal and focused RGBA layers.
    import struct
    (args.output / 'title-exit.kyart').write_bytes(b'KYART1' + bytes(2) + struct.pack('<II', normal.width, normal.height)
                                                  + normal.tobytes() + focus.tobytes())
    print(json.dumps(dict(params, width=normal.width, height=normal.height)))


if __name__ == '__main__':
    main()
