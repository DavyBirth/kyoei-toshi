"""Private, verified read-time corrections for the owner's four UI reports.

Only changed resources enter the delta. HUD string slots retain their byte
length, all record IDs and every non-text byte. Original archives stay intact.
"""
import argparse
import json
import struct
import zlib
from pathlib import Path
from english_typography import native_english
from psarc import Psarc
from psarc_delta import build, verify
from text_tool import export, insert

HUD_LABELS = {
    '\u88c5\u5099': 'Gear',
    '\u30b3\u30b9\u30c1\u30e5\u30fc\u30e0': 'Costume',
    '\u5e3d\u5b50': 'Hat',
    '\u624b\u6301\u3061': 'Held',
    '\u30b0\u30ed\u30fc\u30d6': 'Gloves',
    '\u30e1\u30ac\u30cd': 'Glasses',
    '\u30de\u30b9\u30af': 'Mask',
    '\u8a73\u7d30': 'Info',
    '\u30ad\u30e3\u30e9\u30af\u30bf\u30fc\u64cd\u4f5c': 'Rotate',
    '\u3088\u304f\u4f7f\u3046\u9806': 'Most used',
    '\u30ab\u30c6\u30b4\u30ea\u9806': 'Category',
    '\u5165\u624b\u9806': 'Newest',
    '\u6240\u6301\u6570': 'Qty',
    '\u5dee\u51fa\u4eba\uff1a': 'From:',
    '\u8fd4\u4fe1\u9078\u629e': 'Reply',
    '\u8fd4\u4fe1\u6c7a\u5b9a': 'Confirm',
    '\u8fd4\u4fe1\u306f\u2026': 'Reply',
    '\u6c7a\u5b9a': 'OK',
    '\u9078\u629e': 'Select',
    '\u8fd4\u4fe1': 'Reply',
    '\u9589\u3058\u308b': 'Close',
    '\u623b\u308b': 'Back',
    '\u5348\u5f8c': 'PM',
    '\u5348\u524d': 'AM',
    '\u5929\u6c17': 'Sky',
    '\u73fe\u5728\u5730': 'Location',
    '\u6240\u6301\u91d1:': 'Money:',
    '\u6240\u6301\u91d1\uff1a': 'Money:',
    '\u6240\u6301\u5de8\u5f71\u30b3\u30a4\u30f3\uff1a': 'Shadow Coins:',
}


def hud_labels(data):
    raw = zlib.decompress(data[144:]); output = bytearray(raw); changes = 0
    # Only exact complete length-prefixed string values. Names/embedded text
    # that merely contain one of these words must never be changed.
    for original, english in HUD_LABELS.items():
        before, after = original.encode(), english.encode()
        if len(after) > len(before): continue
        needle = struct.pack('<I', len(before)) + before
        start = 0
        while (at := raw.find(needle, start)) >= 0:
            start = at + len(needle)
            # Bitmap text is immediately followed by its font property.
            # Node names instead precede geometry and are lookup identifiers.
            if raw[start:start+4] != bytes.fromhex('6722e131'): continue
            output[at+4:start] = after.ljust(len(before), b' ')
            changes += 1
    if not changes: return data, 0
    assert len(output) == len(raw)
    flags = struct.unpack_from('>I', data, 140)[0]
    packed = zlib.compress(bytes(output), 9)
    result = data[:128] + struct.pack('>4sIII', b'BILZ', len(output), len(packed), flags) + packed
    assert zlib.decompress(result[144:]) == output
    return result, changes


def corrections(base):
    manifest = json.loads(Path('docs/m4/runtime-preview.json').read_text(encoding='utf8'))
    resources = {row['resource'].split('/',1)[1] for row in manifest['languages']['en']['resources']}
    changed = {}; counts = {'punctuation_strings':0, 'hud_labels':0}
    with Psarc(base) as archive:
        for entry in archive.entries[1:]:
            name = entry.name
            if name in resources:
                data = b''.join(archive.chunks(entry)); doc = export(data); modified = False
                for s in doc['strings']:
                    value = s['original']
                    if not s['japanese_candidate']:
                        normalized = native_english(value)
                        if name == 'lua/Text_Mail.luc' and value == 'You seriously expect me to come to the park in a situation like this?!':
                            normalized = 'You seriously expect me to come to the park\nin a situation like this?!'
                        if name == 'lua/map_goal_text.luc' and value.startswith('Meet ') and value.endswith('First, reach the main street.'):
                            normalized = 'Reach the main street first.'
                        if normalized != value:
                            s['translation'] = normalized; modified = True
                            counts['punctuation_strings'] += int(native_english(value) != value)
                if modified: changed[name] = insert(data,doc)
            elif name.startswith('hud/') and name.endswith('.hud'):
                data = b''.join(archive.chunks(entry)); result, count = hud_labels(data)
                if count: changed[name] = result; counts['hud_labels'] += count
    return changed, counts


if __name__ == '__main__':
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('base', type=Path); p.add_argument('output', type=Path)
    a = p.parse_args()
    changed, counts = corrections(a.base)
    delta, number = build(a.base, changed); verify(a.base, delta, changed)
    a.output.parent.mkdir(parents=True,exist_ok=True); a.output.write_bytes(delta)
    print(json.dumps(dict(resources=number, bytes=len(delta), verified=True, **counts)))
