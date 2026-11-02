"""Runtime pack for the eboot display texts of translations/binary-<lang>.json.

The table addresses the owner's eboot.bin (SELF) by file offset. Each original
string is found, unique, in the prepared eboot.elf and mapped to its virtual
address through the ELF segments. The pack (KYTEXT1) holds per string: virtual
address, slot length, FNV-1a of the original bytes and the English text; the
runtime (src/kyoei_ui_text.c) checks the hash before writing, so the pack carries
no Japanese text and is ignored by any other executable. Output stays private.
"""
import argparse, hashlib, json, struct
from pathlib import Path


def fnv(data):
    h = 14695981039346656037
    for b in data: h = ((h ^ b) * 1099511628211) & 0xFFFFFFFFFFFFFFFF
    return h


def segments(elf):
    phoff = struct.unpack_from('<Q', elf, 0x20)[0]; count = struct.unpack_from('<H', elf, 0x38)[0]
    out = []
    for i in range(count):
        kind, _, offset, vaddr, _, filesz, _, _ = struct.unpack_from('<IIQQQQQQ', elf, phoff + i * 56)
        if kind == 1: out.append((offset, filesz, vaddr))
    return out


def build(table, eboot, elf):
    record = next(r for r in table['files'] if r['path'] == 'eboot.bin')
    if hashlib.sha256(eboot).hexdigest() != record['source_sha256']: raise ValueError('not the expected eboot.bin')
    loads = segments(elf)
    entries = []
    for change in record['changes']:
        o, n = change['offset'], change['bytes']
        original = eboot[o:o + n]
        if hashlib.sha256(original).hexdigest() != change['source_sha256']: raise ValueError(f'eboot@{o}: text differs')
        # The isolated C string, NUL on both sides, must occur once in the ELF.
        needle = b'\0' + original + b'\0'
        at = elf.find(needle)
        if at < 0 or elf.find(needle, at + 1) >= 0: raise ValueError(f'eboot@{o}: not unique in the ELF')
        at += 1
        vaddr = next((v + at - off for off, size, v in loads if off <= at < off + size), None)
        if vaddr is None: raise ValueError(f'eboot@{o}: outside the loaded segments')
        english = change['en'].encode('utf8')
        if len(english) > n or b'\0' in english: raise ValueError(f'eboot@{o}: English exceeds the slot')
        entries.append((vaddr, n, fnv(original), english))
    return entries


def write(entries, path):
    out = bytearray(b'KYTEXT1\0' + struct.pack('<I', len(entries)))
    for vaddr, n, h, english in entries:
        out += struct.pack('<QIQH', vaddr, n, h, len(english)) + english
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_bytes(out)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('table', type=Path); ap.add_argument('output', type=Path)
    ap.add_argument('--eboot', type=Path, required=True, help="the owner's eboot.bin")
    ap.add_argument('--elf', type=Path, required=True, help='the prepared eboot.elf (native-*/out)')
    ap.add_argument('--language',choices=('en','pt-BR'),default='en')
    a = ap.parse_args()
    from binary_language import localized_binary_table
    table=localized_binary_table(a.language) if a.language=='pt-BR' else json.loads(a.table.read_text(encoding='utf8'))
    entries = build(table, a.eboot.read_bytes(), a.elf.read_bytes())
    write(entries, a.output)
    print(json.dumps(dict(strings=len(entries), output=str(a.output))))


if __name__ == '__main__':
    main()
