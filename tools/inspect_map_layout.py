"""Technical names/position records in the private minimap HUD; no prose."""
from pathlib import Path
import struct
import zlib
for name in ('cockpit_map.hud','cockpit_map_wide.hud'):
    raw=zlib.decompress((Path('G:/kyoei-toshi-work/m1/common/hud')/name).read_bytes()[144:])
    for at in range(len(raw)-4):
        n=struct.unpack_from('<I',raw,at)[0]
        if not 4<=n<=80: continue
        value=raw[at+4:at+4+n]
        if len(value)!=n:continue
        try:text=value.decode('utf8')
        except UnicodeDecodeError:continue
        if not text.isprintable():continue
        if at<4 or raw[at-4:at] not in (bytes.fromhex('f5c3aa86'),bytes.fromhex('4def3579')):continue
        end=at+4+n
        print(name,hex(at+4),text.encode('unicode_escape').decode(),raw[end:end+72].hex())
