"""Private original/keyboard/American/Xbox/Nintendo texture payloads for live GPU selection."""
import argparse
import struct
import zlib
from pathlib import Path
from psarc import Psarc
from tex_preview import unpack_tex
from make_keyboard_icons import build as keyboard_icons
from make_keyboard_icons import cursor_lr
from localize_textures import decode, encode_tex
from make_american_icons import build as american_icons
from make_xbox_icons import build as xbox_icons
from PIL import Image
import numpy as np


def bleed(image):
    """Fill fully transparent texels with the colour of their visible neighbours.

    The native talk marker stores them as black: filtering blended that black into the
    white ring, a dark outline where the ring overlaps the white text plate."""
    rgba = np.asarray(image.convert('RGBA'), dtype=np.float32)
    rgb, known = rgba[..., :3].copy(), rgba[..., 3] > 0
    for _ in range(max(image.size)):
        if known.all():
            break
        total = np.zeros_like(rgb); count = np.zeros(known.shape, np.float32)
        for dy, dx in ((-1, 0), (1, 0), (0, -1), (0, 1), (-1, -1), (-1, 1), (1, -1), (1, 1)):
            k = np.roll(known, (dy, dx), (0, 1)); c = np.roll(rgb, (dy, dx), (0, 1))
            total += c * k[..., None]; count += k
        grow = ~known & (count > 0)
        if not grow.any():
            break
        rgb[grow] = total[grow] / count[grow][:, None]; known = known | grow
    rgba[..., :3] = rgb
    return Image.fromarray(rgba.round().astype(np.uint8), 'RGBA')


def bled(data):
    info, image = decode(data)
    return encode_tex(data, bleed(image), info)


ICON_PAGE = 'font/utf8all_icon_0.tex'


def icon_cells(page, icons):
    """Where each font/icon/IC_* image sits in the font's icon page ([iN] in texts are drawn
    from the page, not from the IC_* files): exact matches, found once per base archive."""
    from numpy.lib.stride_tricks import sliding_window_view as windows
    sheet = np.asarray(page, np.int16); cells = {}
    for name, icon in icons.items():
        art = np.asarray(icon, np.int16); h, w = art.shape[:2]
        distance = sum(np.abs(windows(sheet[..., c], (h, w)) - art[..., c]).sum(axis=(2, 3)) for c in range(4))
        y, x = np.unravel_index(distance.argmin(), distance.shape)
        if distance[y, x]:
            raise ValueError(f'{name} is not in the icon page')
        cells[name] = (int(x), int(y))
    return cells


def icon_page(original, cells, variant):
    """The icon page with each icon replaced by its variant (same size, same place)."""
    info, page = decode(original)
    page = page.copy()
    for name, at in cells.items():
        if name in variant:
            page.paste(decode(variant[name])[1], at)
    return encode_tex(original, page, info)


def build(base, output, diagrams=False,language='en',legend_diagrams=False):
    keyboard = keyboard_icons(base,english='pt-BR' if language=='pt-BR' else language!='ja')
    with Psarc(base) as archive:
        names = {e.name: e for e in archive.entries[1:]}
        status='texture/hud/status_base.tex'
        status_data=b''.join(archive.chunks(names[status]))
        cursor='texture/hud/cursor_lr.tex'
        cursor_data=b''.join(archive.chunks(names[cursor]))
        cursor_info,cursor_image=decode(cursor_data)
        alternative=encode_tex(cursor_data,cursor_lr(cursor_image,page_keys=True),cursor_info)
        entries=[(name,replacement,int(name==cursor)) for name,replacement in keyboard.items()]
        entries += [(status,status_data,2),(cursor,alternative,3)]
        originals={name:b''.join(archive.chunks(names[name])) for name,_,_ in entries}
        american=american_icons(originals)
        xbox=xbox_icons(originals)
        nintendo=xbox_icons(originals,'nintendo')
        # The font's icon page, per device, from the icon variants above.
        page_data = b''.join(archive.chunks(names[ICON_PAGE]))
        icon_names = [n for n in originals if n.startswith('font/icon/IC_')]
        cells = icon_cells(decode(page_data)[1], {n: decode(originals[n])[1] for n in icon_names})
        originals[ICON_PAGE] = page_data
        entries.append((ICON_PAGE, icon_page(page_data, cells, keyboard), 0))
        american[ICON_PAGE] = icon_page(page_data, cells, american)
        xbox[ICON_PAGE] = icon_page(page_data, cells, xbox)
        nintendo[ICON_PAGE] = icon_page(page_data, cells, nintendo)
        if diagrams:
            # Controls diagrams for each device (make_pad_diagrams), from the English base: the
            # PlayStation art stays the American English one already in the archive.
            from make_pad_diagrams import build_all
            drawn = build_all(base, ('keyboard', 'xbox', 'nintendo'),language,leaders=not legend_diagrams)
            for name in drawn['xbox']:
                original = b''.join(archive.chunks(names[name]))
                info = decode(original)[0]
                originals[name] = original
                entries.append((name, encode_tex(original, drawn['keyboard'][name], info), 0))
                xbox[name] = encode_tex(original, drawn['xbox'][name], info)
                nintendo[name] = encode_tex(original, drawn['nintendo'][name], info)
        packed = bytearray(b'KYPROMP4' + struct.pack('<I', len(entries)))
        for name, replacement,kind in entries:
            original = originals[name]
            if name.startswith('texture/text marker/talk_icon_'):
                american[name] = bled(american.get(name, original))
                xbox[name] = bled(xbox.get(name, original))
                nintendo[name] = bled(nintendo.get(name, original))
                replacement = bled(replacement)  # the original stays: it recognises uploads
            info, controller = unpack_tex(original)
            other, keys = unpack_tex(replacement)
            if info != other or len(controller) != len(keys):
                raise ValueError('Prompt layouts differ: ' + name)
            us_info,us=unpack_tex(american.get(name,original))
            if any(us_info[k]!=info[k] for k in ('width','height','levels','storage_family','channel_layout')) or len(us)!=len(keys):
                raise ValueError('American prompt layouts differ: '+name)
            packed += struct.pack('<7I', info['width'], info['height'], info['levels'],
                                  info['storage_family'], info['channel_layout'], kind, len(keys))
            xb_info,xb=unpack_tex(xbox.get(name,original))
            if any(xb_info[k]!=info[k] for k in ('width','height','levels','storage_family','channel_layout')) or len(xb)!=len(keys):
                raise ValueError('Xbox prompt layouts differ: '+name)
            nt_info,nt=unpack_tex(nintendo.get(name,original))
            if any(nt_info[k]!=info[k] for k in ('width','height','levels','storage_family','channel_layout')) or len(nt)!=len(keys):
                raise ValueError('Nintendo prompt layouts differ: '+name)
            packed += controller + keys + us + xb + nt
    output.parent.mkdir(parents=True, exist_ok=True)
    # KYPROMZ1: the pack compressed (the runtime inflates it once at start; 177 MB -> ~9 MB).
    stored = b'KYPROMZ1' + struct.pack('<Q', len(packed)) + zlib.compress(bytes(packed), 6)
    output.write_bytes(stored)
    print(f'Paired prompt pack: {len(entries)} textures; {len(packed)} bytes, {len(stored)} compressed')


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('base', type=Path)
    p.add_argument('output', type=Path)
    p.add_argument('--diagrams', action='store_true', help='controls diagrams per device (English base)')
    p.add_argument('--language',choices=('en','pt-BR','ja'),default='en')
    p.add_argument('--delta',type=Path,help='Read a verified translation delta over the base archive')
    p.add_argument('--legend-diagrams',action='store_true',help='Use named control legends without pointer lines')
    a = p.parse_args()
    from psarc import DeltaSource
    base=DeltaSource(a.base,a.delta.read_bytes()) if a.delta else a.base
    build(base, a.output, a.diagrams,a.language,a.legend_diagrams)
