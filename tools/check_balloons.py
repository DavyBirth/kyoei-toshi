"""Check every NPC balloon text (Fk* message tables) against the balloon width.

The balloon (UnitHudTextMarker) is a fixed single line of ~12.6 full-width cells:
the native code keeps 12 characters + "..." (cut between words in en/pt-BR by
kyoei_im_wrap.c). Rule agreed with Davy: a line that fits whole in Japanese must
fit whole in the translation; a line cut in Japanese may be cut (between words).
Usage: python tools/check_balloons.py <common.psarc> [--language pt-BR|en]
Exit 1 lists the lines to shorten. Reads only; prints no Japanese text.
"""
import argparse, glob, json, struct, sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))
from psarc import Psarc
from game_resource import Resource
import lua52

ROOT = Path(__file__).resolve().parents[1]


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('archive'); p.add_argument('--language', default='pt-BR', choices=('pt-BR', 'en'))
    p.add_argument('--metrics', default=str(ROOT / 'build/City-Shrouded-in-Shadow-Portable/assets/hud-metrics.bin'))
    a = p.parse_args()
    adv = struct.unpack('<95f', open(a.metrics, 'rb').read()[8:388])
    cells = lambda s: sum(adv[ord(c) - 32] if 32 <= ord(c) < 127 else (.6 if ord(c) < 0x800 else 1) for c in s)
    files = [ROOT / 'translations/localization-pt-BR.json'] if a.language == 'pt-BR' else \
        sorted(Path(x) for x in glob.glob(str(ROOT / 'translations/localization-en*.json')))
    text = {}
    for f in files:
        for r in json.loads(f.read_text(encoding='utf8'))['resources']:
            for e in r['entries']:
                if e.get(a.language): text[(r['path'], e['id'])] = e[a.language]
    bad = total = 0
    with Psarc(a.archive) as archive:
        entries = {e.name: e for e in archive.entries[1:]}
        for name in sorted(entries):
            if not (name.startswith('lua/Text_') and name.endswith('.luc')): continue
            f = lua52.load(Resource(b''.join(archive.chunks(entries[name]))).payload); reg = {}; seen = set()
            for ins in f.code:
                x = lua52.fields(ins); op = lua52.OPS[x['op']]
                if op == 'LOADK': reg[x['a']] = x['bx']
                elif op == 'SETTABUP':
                    ki = x['b'] - 256 if x['b'] >= 256 else reg.get(x['b'])
                    key = f.k[ki] if ki is not None else None
                    # Message table: R2 window type, R3 speaker, R4 text.
                    if isinstance(key, bytes) and key.startswith(b'Fk') and 4 in reg and reg[4] not in seen:
                        seen.add(reg[4]); jp = f.k[reg[4]]
                        if isinstance(jp, bytes) and jp.strip():
                            jp = jp.decode('utf8', 'replace'); total += 1
                            t = text.get(('common/' + name, f'p0/k{reg[4]}'))
                            if t and len(jp) <= 12 and '\n' not in jp and cells(t) > 12.99:
                                bad += 1; print(f'{name[4:]}\t{key.decode()}\t{cells(t):.1f} cells\t{t}')
                    reg = {}
    print(f'{total} balloon texts, {bad} wider than the balloon where the Japanese fits')
    sys.exit(1 if bad else 0)


if __name__ == '__main__': main()
