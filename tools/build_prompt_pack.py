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
from PIL import Image,ImageChops
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


def icon_cells(page, icons, table_data):
    """Use the native icon table, validating every source cell pixel for pixel.

    The same glyph can exist at several sizes. Its numeric ID and page are
    authoritative; searching all sliding windows was slow and ambiguous.
    """
    from text_tool import decode as decode_resource
    import re
    resource,table=decode_resource(table_data)
    if table.columns!=9 or table.rows>128:raise ValueError('Unexpected icon table')
    cells={}
    for row in range(table.rows):
        values=[]
        for col in range(6):
            at,tag=struct.unpack_from('>II',resource.payload,12+(row*9+col)*8)
            values.append(struct.unpack_from('>I' if col<4 else '>f',resource.payload,at)[0])
        key,page_id,x,y,w,h=values
        if page_id!=0:continue
        cells[key]=(int(x),int(y),int(w),int(h))
    result={}
    for name,icon in icons.items():
        key=int(re.search(r'_(\d+)\.tex$',name).group(1))
        if key not in cells:raise ValueError('Icon ID missing: '+name)
        x,y,w,h=cells[key]
        if icon.size!=(w,h) or x<0 or y<0 or x+w>page.width or y+h>page.height or not np.array_equal(np.asarray(page.crop((x,y,x+w,y+h))),np.asarray(icon)):
            raise ValueError('Native icon cell differs: '+name)
        result[name]=(x,y)
    return result


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
        cells = icon_cells(decode(page_data)[1], {n: decode(originals[n])[1] for n in icon_names},
                           b''.join(archive.chunks(names['font/utf8all_icon_table.csh'])))
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
        packed = bytearray(b'KYPROMP4' + struct.pack('<I', len(entries)+1))
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
    # Font drawing also uploads the icon atlas as an R8 alpha mask. Its
    # keyboard/controller variants must follow the same live input selection.
    masks=[decode(data)[1].getchannel('A').tobytes() for data in
           (page_data,icon_page(page_data,cells,keyboard),american[ICON_PAGE],xbox[ICON_PAGE],nintendo[ICON_PAGE])]
    key_page=decode(icon_page(page_data,cells,keyboard))[1]
    masks[1]=ImageChops.multiply(key_page.convert('L'),key_page.getchannel('A')).tobytes()
    page=decode(page_data)[1]
    packed += struct.pack('<7I',page.width,page.height,1,2,0,0,len(masks[0]))
    for mask in masks:packed += mask
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
