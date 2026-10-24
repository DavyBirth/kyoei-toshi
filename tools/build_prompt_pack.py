"""Private original/keyboard/American/Xbox/Nintendo texture payloads for live GPU selection."""
import argparse
import struct
from pathlib import Path
from psarc import Psarc
from tex_preview import unpack_tex
from make_keyboard_icons import build as keyboard_icons
from make_keyboard_icons import cursor_lr
from localize_textures import decode, encode_tex
from make_american_icons import build as american_icons
from make_xbox_icons import build as xbox_icons


def build(base, output):
    keyboard = keyboard_icons(base)
    packed = bytearray(b'KYPROMP4' + struct.pack('<I', len(keyboard)+2))
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
        for name, replacement,kind in entries:
            original = originals[name]
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
    output.write_bytes(packed)
    print(f'Paired prompt pack: {len(keyboard)+2} textures; {len(packed)} bytes')


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('base', type=Path)
    p.add_argument('output', type=Path)
    a = p.parse_args()
    build(a.base, a.output)
