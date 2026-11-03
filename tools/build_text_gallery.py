"""Text gallery for testing: every dialogue line of the loaded stage, in the real window.

Patches Plan.MessageAndWaitButton (plan.luc proto 46). The first message shown in
a session first walks the gallery list: every message table whose key exists in
the stage that is loaded (Text_Stage_*, Heroine, Kasiwagi, Common...), skipping
choice menus (MES_TYPE_3/10). One key press per line; afterwards the game goes
on normally. Test-only: the result goes into a separate gallery delta, never into
the normal translation.
"""
import argparse
from pathlib import Path
from game_resource import Resource
from psarc import Psarc
import lua52
from lua52 import encode as E

SKIP_TYPES = {b'MES_TYPE_3', b'MES_TYPE_10'}
FIRST = ('Text_Stage_', 'Text_Heroine', 'Text_Kasiwagi', 'Text_Common', 'Text_Plan00', 'Text_EpilogueText')


def message_keys(chunk):
    """Keys of the message tables, in file order, with their window type."""
    f = lua52.load(chunk); reg = {}; kind = None; out = []
    for i in f.code:
        x = lua52.fields(i); n = lua52.OPS[x['op']]
        if n == 'LOADK': reg[x['a']] = f.k[x['bx']]
        elif n == 'GETTABLE' and x['c'] >= 256:
            k = f.k[x['c'] - 256]
            if isinstance(k, bytes) and k.startswith(b'MES_TYPE'): kind = k
        elif n == 'SETTABUP':
            key = f.k[x['b'] - 256] if x['b'] >= 256 else reg.get(x['b'])
            if isinstance(key, bytes) and kind not in SKIP_TYPES: out.append(key)
    return out


def patch(plan, keys):
    root = lua52.load(plan); f = root.protos[46]
    if f.params != 2 or f.k[:1] != [b'print'] or b'SetText' not in f.k:
        raise ValueError('Plan.MessageAndWaitButton differs from CUSA03562 01.03')
    def k(v):
        if v not in f.k: f.k.append(v)
        return f.k.index(v)
    rk = {name: k(name) for name in (b'KyoeiGalleryDone', b'Plan', b'window', b'SetText', b'WaitForInput', b'PauseFrame')}
    one, ten, true = k(1.0), k(10.0), k(True)
    if max(rk.values()) >= 256 or true >= 256: raise ValueError('RK constant out of range')
    K = lambda name: 256 + rk[name]
    body = []
    # if KyoeiGalleryDone then goto original end
    body += [E('GETTABUP', 2, 0, K(b'KyoeiGalleryDone')), E('TEST', 2, 0, 1), None]
    body += [E('SETTABUP', 0, K(b'KyoeiGalleryDone'), 256 + true), E('NEWTABLE', 2, 0, 0)]
    for start in range(0, len(keys), 50):
        batch = keys[start:start + 50]
        body += [E('LOADK', 3 + j, bx=k(key)) for j, key in enumerate(batch)]
        body.append(E('SETLIST', 2, len(batch), start // 50 + 1))
    body += [E('LOADK', 3, bx=one), E('LEN', 4, 2), E('LOADK', 5, bx=one)]
    prep = len(body); body.append(None)                  # FORPREP R3
    loop = len(body)
    body += [E('GETTABLE', 7, 2, 6), E('GETTABUP', 7, 0, 7), E('TEST', 7, 0, 0), None,
             E('GETTABUP', 8, 0, K(b'Plan')), E('GETTABLE', 8, 8, K(b'window')),
             E('SELF', 8, 8, K(b'SetText')), E('MOVE', 10, 7), E('CALL', 8, 3, 1),
             E('GETTABUP', 8, 0, K(b'PauseFrame')), E('LOADK', 9, bx=one), E('CALL', 8, 2, 1),
             E('GETTABUP', 8, 0, K(b'Plan')), E('GETTABLE', 8, 8, K(b'window')),
             E('SELF', 8, 8, K(b'WaitForInput')), E('CALL', 8, 2, 1),
             E('GETTABUP', 8, 0, K(b'PauseFrame')), E('LOADK', 9, bx=ten), E('CALL', 8, 2, 1)]
    skip = loop + 3; forloop = len(body)
    body[skip] = E('JMP', 0, sbx=forloop - (skip + 1))
    body.append(E('FORLOOP', 3, sbx=loop - (forloop + 1)))
    body[prep] = E('FORPREP', 3, sbx=forloop - (prep + 1))
    body[2] = E('JMP', 0, sbx=len(body) - 3)               # past the gallery
    if max(k(key) for key in keys) >= 1 << 18: raise ValueError('too many constants')
    f.code = body + f.code
    f.lineinfo = [f.line] * len(body) + f.lineinfo
    f.locvars = []                                         # debug ranges shift; not needed
    f.stack = max(f.stack, 60)
    return lua52.dump(root)


def build(archive_path, only=()):
    with Psarc(archive_path) as archive:
        entries = {e.name: e for e in archive.entries[1:]}
        read = lambda name: b''.join(archive.chunks(entries[name]))
        names = sorted(n for n in entries if n.startswith('lua/Text_') and n.endswith('.luc'))
        names.sort(key=lambda n: next((i for i, p in enumerate(FIRST) if n[4:].startswith(p)), 99))
        keys = []
        for name in names:
            for key in message_keys(Resource(read(name)).payload):
                if key not in keys and (not only or key.decode().startswith(tuple(only))): keys.append(key)
        plan = Resource(read('lua/plan.luc'))
        return plan.rebuild(patch(plan.payload, keys)), len(keys)


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('archive', type=Path); p.add_argument('output', type=Path)
    p.add_argument('--only', action='append', default=[], metavar='PREFIX',
                   help='only keys starting with PREFIX (e.g. NpcMess460 for the florist)')
    a = p.parse_args()
    data, n = build(a.archive, a.only)
    if not n: raise ValueError('no message keys matched')
    a.output.parent.mkdir(parents=True, exist_ok=True); a.output.write_bytes(data)
    print(f'plan.luc with gallery: {n} message keys -> {a.output}')


if __name__ == '__main__':
    main()
