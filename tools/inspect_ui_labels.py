"""Locate short, explicitly listed UI labels inside compressed HUD layouts."""
from pathlib import Path
import struct
import zlib
import re

LABELS = ['\u5dee\u51fa\u4eba', '\u8fd4\u4fe1', '\u5929\u6c17', '\u73fe\u5728\u5730', '\u6240\u6301']
if __name__ == '__main__':
    from build_ui_corrections import HUD_LABELS
    from inspect_elf_references import load_segments
    image = Path('G:/kyoei-toshi-work/native-05/out/eboot.elf').read_bytes()
    for original, english in HUD_LABELS.items():
        for segment in load_segments(image):
            body = image[segment['offset']:segment['offset']+segment['size']]
            at = body.find(original.encode()+b'\0')
            if at >= 0: print('ELF',english,hex(segment['address']+at),len(original.encode()))
    for path in Path('G:/kyoei-toshi-work/m1/common/hud').rglob('*.hud'):
        data = path.read_bytes()
        if data[128:132] != b'BILZ': continue
        raw = zlib.decompress(data[144:])
        if path.name in ('cockpit_timedate.hud','menu_mail_detail.hud','menu_mail_detail_reply.hud','menu_mail_detail_select.hud'):
            for offset in range(len(raw)-4):
                size = struct.unpack_from('<I',raw,offset)[0]
                if not 3 <= size <= 90: continue
                try: text = raw[offset+4:offset+4+size].decode('utf8')
                except UnicodeDecodeError: continue
                if len(text.encode()) == size and all(c.isprintable() for c in text) and re.search('[\u3040-\u30ff\u3400-\u9fff]',text):
                    print(path.name,hex(offset+4),text.encode('unicode_escape').decode())
        for label in LABELS:
            at = raw.find(label.encode())
            if at >= 0:
                print(path.name, label.encode('unicode_escape').decode(), hex(at),
                      raw[max(0,at-12):at].hex(), raw[at:at+60].hex())
