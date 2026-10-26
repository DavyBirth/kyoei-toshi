"""Print only technical metadata of a private paired prompt pack."""
import struct
import sys
import zlib
from pathlib import Path
data=Path(sys.argv[1]).read_bytes();at=12
if data[:8]==b'KYPROMZ1':data=zlib.decompress(data[16:])  # compressed pack
assert data[:8] in (b'KYPROMP1',b'KYPROMP2',b'KYPROMP3',b'KYPROMP4')
variants={b'KYPROMP4':5,b'KYPROMP3':4,b'KYPROMP2':3}.get(data[:8],2)
for i in range(struct.unpack_from('<I',data,8)[0]):
    w,h,levels,storage,channels,kind,size=struct.unpack_from('<7I',data,at)
    print(i,w,h,levels,storage,channels,kind,size)
    at+=28+size*variants
assert at==len(data)
