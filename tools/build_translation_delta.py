"""Whole English translation as one small read-time delta over the disc archive.

Input: the user's original DAT/common.psarc, a translated archive built by
psarc_patch (original body + appended blocks) and optionally the HUD delta
built on top of it. Output: one KYDELTA1 file (a few MB) that the runtime
applies to the untouched disc archive, so no 6 GB translated copy is needed.
"""
import argparse, json, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))
from psarc import Psarc
from psarc_delta import Virtual, build, verify


def translated_resources(original, translated, overlay=None):
    """Entries whose data lives outside the original body, i.e. replaced ones."""
    with Psarc(original) as base:
        body = base.archive_size - base.header['toc_bytes']
        old = {e.name: (e.block, e.size, e.offset - base.header['toc_bytes']) for e in base.entries[1:]}
    view = Virtual(translated, Path(overlay).read_bytes()) if overlay else None
    archive = Psarc(translated, view, view.size) if view else Psarc(translated)
    try:
        toc = archive.header['toc_bytes']
        if {e.name for e in archive.entries[1:]} != old.keys():
            raise ValueError('Translated archive has different resource names')
        changed = {}
        for entry in archive.entries[1:]:
            if entry.offset - toc >= body:
                changed[entry.name] = b''.join(archive.chunks(entry))
            elif (entry.block, entry.size, entry.offset - toc) != old[entry.name]:
                raise ValueError(f'Translated archive is not original body + appended blocks: {entry.name}')
        return changed
    finally:
        archive.file.close()


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('original', type=Path, help='disc DAT/common.psarc')
    parser.add_argument('translated', type=Path, help='translated common.psarc')
    parser.add_argument('output', type=Path)
    parser.add_argument('--overlay', type=Path, help='HUD .kydelta built on the translated archive')
    args = parser.parse_args()
    if args.output.exists():
        raise SystemExit('Output already exists')
    changed = translated_resources(args.original, args.translated, args.overlay)
    delta, count = build(args.original, changed)
    verify(args.original, delta, changed)
    args.output.parent.mkdir(parents=True, exist_ok=True)
    args.output.write_bytes(delta)
    print(json.dumps(dict(output=str(args.output), bytes=len(delta), changed=count, verified=True)))


if __name__ == '__main__':
    main()
