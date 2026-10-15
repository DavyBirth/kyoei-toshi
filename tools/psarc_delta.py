"""Small PSARC patch applied by the runtime at read time instead of a full copy.

Layout (little endian): b'KYDELTA1', base_size u64, base_toc u64, new_toc u64,
tail u64, fnv1a64(base[0:base_toc]) u64, then new_toc bytes of TOC and tail
bytes of appended blocks. The virtual archive is
new TOC + base[base_toc:] + tail, which is exactly what psarc_patch writes.
"""
import argparse, json, struct, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))
from psarc import Psarc
from psarc_patch import rebuild_toc

MAGIC = b'KYDELTA1'


def fnv1a64(data):
    value = 14695981039346656037
    for byte in data:
        value = ((value ^ byte) * 1099511628211) & 0xffffffffffffffff
    return value


def build(base, replacements):
    with Psarc(base) as archive:
        names = {e.name: e for e in archive.entries[1:]}
        missing = replacements.keys() - names.keys()
        if missing: raise ValueError(f'Not in archive: {sorted(missing)[:3]}')
        changed = {n: d for n, d in replacements.items() if d != b''.join(archive.chunks(names[n]))}
        old_toc = archive.header['toc_bytes']
        archive.file.seek(0)
        original_toc = archive.read_exact(old_toc)
        table, appended = rebuild_toc(archive, original_toc, changed)
        tail = b''.join(appended)
        head = MAGIC + struct.pack('<5Q', archive.archive_size, old_toc, len(table), len(tail), fnv1a64(original_toc))
        return head + table + tail, len(changed)


class Virtual:
    """File-like view of base + delta, used to verify the delta before shipping."""
    def __init__(self, base, delta):
        self.base = open(base, 'rb')
        size, self.old_toc, new_toc, tail, _ = struct.unpack_from('<5Q', delta, 8)
        self.table = delta[48:48 + new_toc]; self.tail = delta[48 + new_toc:48 + new_toc + tail]
        self.body = size - self.old_toc; self.size = new_toc + self.body + tail; self.pos = 0
    def seek(self, pos, whence=0):
        self.pos = pos if whence == 0 else self.pos + pos if whence == 1 else self.size + pos
        return self.pos
    def tell(self): return self.pos
    def read(self, n=-1):
        n = self.size - self.pos if n < 0 else min(n, self.size - self.pos)
        out = bytearray()
        while n > 0:
            p, toc = self.pos, len(self.table)
            if p < toc: chunk = self.table[p:p + n]
            elif p < toc + self.body:
                self.base.seek(self.old_toc + p - toc); chunk = self.base.read(min(n, toc + self.body - p))
            else: chunk = self.tail[p - toc - self.body:p - toc - self.body + n]
            if not chunk: break
            out += chunk; self.pos += len(chunk); n -= len(chunk)
        return bytes(out)
    def close(self): self.base.close()


def verify(base, delta, replacements):
    view = Virtual(base, delta)
    try:
        archive = Psarc(base, view, view.size)
        names = {e.name: e for e in archive.entries[1:]}
        for name, data in replacements.items():
            if b''.join(archive.chunks(names[name])) != data:
                raise ValueError(f'Delta readback differs: {name}')
    finally:
        view.close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('base', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--keyboard-icons', action='store_true')
    parser.add_argument('--language', choices=['en', 'ja'], default='en', help='language of text drawn into prompts')
    args = parser.parse_args()
    replacements = {}
    if args.keyboard_icons:
        from make_keyboard_icons import build as icons
        replacements.update(icons(args.base, english=args.language == 'en'))
    delta, changed = build(args.base, replacements)
    verify(args.base, delta, replacements)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(delta)
    print(json.dumps(dict(output=str(args.output), bytes=len(delta), changed=changed, verified=True)))


if __name__ == '__main__':
    main()
