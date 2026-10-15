"""Private paired controller/keyboard texture payloads for live GPU selection."""
import argparse
import struct
from pathlib import Path
from psarc import Psarc
from tex_preview import unpack_tex
from make_keyboard_icons import build as keyboard_icons
from make_keyboard_icons import cursor_lr
from localize_textures import decode, encode_tex


def build(base, output):
    keyboard = keyboard_icons(base)
    packed = bytearray(b'KYPROMP1' + struct.pack('<I', len(keyboard)+2))
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
        for name, replacement,kind in entries:
            original = b''.join(archive.chunks(names[name]))
            info, controller = unpack_tex(original)
            other, keys = unpack_tex(replacement)
            if info != other or len(controller) != len(keys):
                raise ValueError('Prompt layouts differ: ' + name)
            packed += struct.pack('<7I', info['width'], info['height'], info['levels'],
                                  info['storage_family'], info['channel_layout'], kind, len(keys))
            packed += controller + keys
    output.parent.mkdir(parents=True, exist_ok=True)
    output.write_bytes(packed)
    print(f'Paired prompt pack: {len(keyboard)+2} textures; {len(packed)} bytes')


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('base', type=Path)
    p.add_argument('output', type=Path)
    a = p.parse_args()
    build(a.base, a.output)
