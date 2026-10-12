"""Independently validate PNG chunks and reconstruct pixels with Python zlib."""
from pathlib import Path
import struct
import subprocess
import zlib

ROOT = Path(__file__).resolve().parents[1]
OUT = ROOT / 'build/test-png'


def decode(path, mask):
    blob = path.read_bytes()
    assert blob[:8] == b'\x89PNG\r\n\x1a\n'
    offset, compressed, header, ended = 8, bytearray(), None, False
    while offset < len(blob):
        size = struct.unpack_from('>I', blob, offset)[0]
        kind = blob[offset + 4:offset + 8]
        data = blob[offset + 8:offset + 8 + size]
        crc = struct.unpack_from('>I', blob, offset + 8 + size)[0]
        assert zlib.crc32(kind + data) == crc
        if kind == b'IHDR':
            header = struct.unpack('>IIBBBBB', data)
        elif kind == b'IDAT':
            compressed.extend(data)
        elif kind == b'IEND':
            assert size == 0
            ended = True
        offset += size + 12
    assert ended and offset == len(blob)
    width, height, depth, color, compression, filtering, interlace = header
    assert (width, height, depth, compression, filtering, interlace) == (3, 2, 8, 0, 0, 0)
    channels = 4 if color == 6 else 3
    assert color in (2, 6)
    stream = zlib.decompress(compressed)
    row_bytes = width * channels
    assert len(stream) == (row_bytes + 1) * height
    rows, previous = [], bytearray(row_bytes)
    allowed = {0} if mask == 0 else {f for f in range(1, 5) if mask & (1 << (f - 1))}
    if mask == 15:
        allowed.add(0)
    for y in range(height):
        start = y * (row_bytes + 1)
        tag = stream[start]
        assert tag in allowed
        row = bytearray(stream[start + 1:start + row_bytes + 1])
        for x in range(row_bytes):
            left = row[x - channels] if x >= channels else 0
            above = previous[x]
            corner = previous[x - channels] if x >= channels else 0
            if tag == 0:
                predict = 0
            elif tag == 1:
                predict = left
            elif tag == 2:
                predict = above
            elif tag == 3:
                predict = (left + above) // 2
            else:
                estimate = left + above - corner
                predict = min((left, above, corner), key=lambda value: abs(estimate - value))
            row[x] = (row[x] + predict) & 255
        rows.append(bytes(row))
        previous = row
    return channels, b''.join(rows)


if __name__ == '__main__':
    OUT.mkdir(parents=True, exist_ok=True)
    subprocess.run([str(ROOT / 'build/test_kyoei_png.exe'), str(OUT)], check=True)
    pixels = [(255, 0, 0, 10), (0, 255, 0, 20), (0, 0, 255, 30),
              (10, 20, 30, 40), (50, 60, 70, 80), (90, 100, 110, 120)]
    for format in range(2):
        for alpha in range(2):
            expected = bytearray()
            for r, g, b, a in pixels:
                expected.extend((b, g, r) if format else (r, g, b))
                if alpha:
                    expected.append(a)
            for mask in range(16):
                channels, decoded = decode(OUT / f'png-{format}-{alpha}-{mask}.png', mask)
                assert channels == 3 + alpha and decoded == expected
    print('PNG PIXELS PASS: 64 files decoded independently; RGBA/BGRA, RGB/alpha, every filter mask and CRC verified')
