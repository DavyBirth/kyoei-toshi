"""Print only technical metadata of a private paired prompt pack."""
import struct
import sys
from pathlib import Path
data=Path(sys.argv[1]).read_bytes();at=12
assert data[:8] in (b'KYPROMP1',b'KYPROMP2')
variants=3 if data[:8]==b'KYPROMP2' else 2
for i in range(struct.unpack_from('<I',data,8)[0]):
    w,h,levels,storage,channels,kind,size=struct.unpack_from('<7I',data,at)
    print(i,w,h,levels,storage,channels,kind,size)
    at+=28+size*variants
assert at==len(data)
