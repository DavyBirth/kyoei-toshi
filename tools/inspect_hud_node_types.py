"""Report candidate HUD node headers for explicitly requested ASCII labels.

Reads an extracted BILZ HUD; never writes game resources or emits dialogue.
This is a targeted structural probe, not a complete HUD deserializer.
"""
import argparse
import hashlib
import json
from pathlib import Path
import struct
import zlib


def type_id(label):
    if not label or not label.isascii() or '\0' in label:
        raise ValueError('Expected a nonempty ASCII technical label')
    value = 37
    for byte in label.encode('ascii'):
        value = (value * 37 + byte) & 0xffffffff
    return value


def inspect(data, labels, types):
    if len(data) < 144 or data[128:132] != b'BILZ':
        raise ValueError('Expected a HUD with BILZ at offset 128')
    payload = zlib.decompress(data[144:])
    known = {type_id(name): name for name in types}
    rows = []
    for label in labels:
        type_id(label)  # Validate requested technical labels only.
        needle = label.encode('ascii')
        start = 0
        while True:
            offset = payload.find(needle, start)
            if offset < 0:
                break
            start = offset + 1
            if offset < 8:
                continue
            kind, length = struct.unpack_from('<II', payload, offset - 8)
            if length != len(needle):
                continue
            rows.append(dict(label=label, payload_offset=offset,
                             candidate_type_id=f'0x{kind:08x}',
                             known_type=known.get(kind)))
    return dict(source_sha256=hashlib.sha256(data).hexdigest(),
                payload_bytes=len(payload), candidate_nodes=rows,
                runtime_verified=False)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('hud', type=Path)
    parser.add_argument('--label', action='append', required=True)
    parser.add_argument('--type', action='append', default=[])
    args = parser.parse_args()
    print(json.dumps(inspect(args.hud.read_bytes(), args.label, args.type), indent=2))


if __name__ == '__main__':
    main()
