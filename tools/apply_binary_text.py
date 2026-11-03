"""Apply translations/binary-<lang>.json to the owner's own HUD resources and eboot.bin.

Each change replaces one fixed-size display string in place. English never exceeds the
original slot (HUD pads with spaces, eboot with NUL), so every other byte, record and
length field is preserved. The original text is identified by its SHA-256, so the
patch file carries no Japanese text. Outputs are private derivatives; never distribute them.
"""
import argparse, hashlib, json, re, struct, zlib
from pathlib import Path

HUD_MARK = bytes.fromhex('f6287f44')
TOKENS = re.compile(r'\[(?:i|s|#)[^\]]*\]|\[/[s#]\]|%[-0-9]*l*[dsx%]|<[A-Z_]+>')


def unwrap(data):
    if data[128:132] != b'BILZ': raise ValueError('Unsupported HUD wrapper')
    raw_size, stored, _ = struct.unpack_from('>III', data, 132)
    if stored != len(data) - 144: raise ValueError('Unexpected BILZ length')
    raw = zlib.decompress(data[144:])
    if len(raw) != raw_size: raise ValueError('BILZ size mismatch')
    return raw


def rewrap(original, raw):
    packed = zlib.compress(raw, 9)
    head = bytearray(original[:144])
    struct.pack_into('>II', head, 132, len(raw), len(packed))
    return bytes(head) + packed


def patch(record, data):
    hud = record['kind'] == 'hud_text_property'
    if hashlib.sha256(data).hexdigest() != record['source_sha256']: raise ValueError(f"{record['path']}: not the expected original")
    raw = bytearray(unwrap(data) if hud else data)
    for c in record['changes']:
        o, n = c['offset'], c['bytes']; old = bytes(raw[o:o + n]); en = c['en'].encode('utf8')
        if hashlib.sha256(old).hexdigest() != c['source_sha256']: raise ValueError(f"{record['path']}@{o}: source text differs")
        if len(en) > n or b'\0' in en: raise ValueError(f"{record['path']}@{o}: English exceeds slot")
        if sorted(TOKENS.findall(old.decode('utf8'))) != sorted(TOKENS.findall(c['en'])): raise ValueError(f"{record['path']}@{o}: tokens differ")
        if hud:
            if raw[o - 16:o - 12] != HUD_MARK or struct.unpack_from('<I', raw, o - 4)[0] != n: raise ValueError(f"{record['path']}@{o}: not a HUD text property")
        elif raw[o - 1] != 0 or raw[o + n] != 0: raise ValueError(f"eboot@{o}: not an isolated C string")
        raw[o:o + n] = en + (b' ' if hud else b'\0') * (n - len(en))
    return rewrap(data, bytes(raw)) if hud else bytes(raw)


def main():
    ap = argparse.ArgumentParser(description=__doc__)
    ap.add_argument('table', type=Path); ap.add_argument('output', type=Path)
    ap.add_argument('--resources', type=Path, required=True)
    ap.add_argument('--eboot', type=Path, required=True)
    a = ap.parse_args()
    table = json.loads(a.table.read_text(encoding='utf8')); n = 0
    for record in table['files']:
        src = a.eboot if record['path'] == 'eboot.bin' else a.resources / record['path']
        out = patch(record, src.read_bytes())
        dst = a.output / record['path']; dst.parent.mkdir(parents=True, exist_ok=True); dst.write_bytes(out)
        n += len(record['changes'])
    print(json.dumps(dict(files=len(table['files']), changes=n, output=str(a.output))))


if __name__ == '__main__':
    main()
