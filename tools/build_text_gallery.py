"""Text gallery for testing: every dialogue line of the loaded stage, in the real window.

Patches Plan.MessageAndWaitButton (plan.luc proto 46). The first message shown in
a session first walks the gallery list: every message table whose key exists in
the stage that is loaded (Text_Stage_*, Heroine, Kasiwagi, Common...), skipping
choice menus (MES_TYPE_3/10). One key press per line; afterwards the game goes
on normally. Test-only: the result goes into a separate gallery delta, never into
the normal translation.
"""
import argparse
import re
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


def patch(plan, keys, mails=(), items=(), intermission=False, warp_npc=None, warp_all=()):
    """Prologue run once per session: send mails, give items, then show each message."""
    root = lua52.load(plan); f = root.protos[46]
    if f.params != 2 or f.k[:1] != [b'print'] or b'SetText' not in f.k:
        raise ValueError('Plan.MessageAndWaitButton differs from CUSA03562 01.03')
    def k(v):
        for i, c in enumerate(f.k):
            if type(c) is type(v) and c == v: return i
        f.k.append(v); return len(f.k) - 1
    names = (b'KyoeiGalleryDone', b'Plan', b'window', b'SetText', b'WaitForInput', b'PauseFrame',
             b'Mail', b'send', b'Player', b'addItem', b'InterMissionDemoStartFlag', b'StartInterMissionWithoutDemo',
             b'Npc', b'getPos', b'NPCID', b'setPos', b'print', b'TalkEnd', b'TalkStart') + ((warp_npc,) if warp_npc else ())
    rk = {name: k(name) for name in names}
    one, ten, true = k(1.0), k(10.0), k(True)
    if max(rk.values()) >= 256 or true >= 256: raise ValueError('RK constant out of range')
    K = lambda name: 256 + rk[name]
    body = [E('GETTABUP', 2, 0, K(b'KyoeiGalleryDone')), E('TEST', 2, 0, 1), None,
            E('SETTABUP', 0, K(b'KyoeiGalleryDone'), 256 + true)]

    def loop(values, inner):
        """for each value v in list: inner(v in R7). Table in R2, counter R3..R6."""
        nonlocal body
        if not values: return
        body.append(E('NEWTABLE', 2, 0, 0))
        for start in range(0, len(values), 50):
            batch = values[start:start + 50]
            body += [E('LOADK', 3 + j, bx=k(v)) for j, v in enumerate(batch)]
            body.append(E('SETLIST', 2, len(batch), start // 50 + 1))
        body += [E('LOADK', 3, bx=one), E('LEN', 4, 2), E('LOADK', 5, bx=one)]
        prep = len(body); body.append(None); top = len(body)
        body.append(E('GETTABLE', 7, 2, 6)); inner(); forloop = len(body)
        for i in range(top, forloop):
            if body[i] == 'SKIP': body[i] = E('JMP', 0, sbx=forloop - (i + 1))
        body.append(E('FORLOOP', 3, sbx=top - (forloop + 1)))
        body[prep] = E('FORPREP', 3, sbx=forloop - (prep + 1))

    def call2(table, field, arg2=None):
        def inner():
            nonlocal body
            body += [E('GETTABUP', 8, 0, K(table)), E('GETTABLE', 8, 8, K(field)), E('MOVE', 9, 7)]
            if arg2 is None: body.append(E('CALL', 8, 2, 1))
            else: body += [E('LOADK', 10, bx=arg2), E('CALL', 8, 3, 1)]
        return inner

    def show():
        nonlocal body
        body += [E('GETTABUP', 7, 0, 7), E('TEST', 7, 0, 0), 'SKIP',
                 E('GETTABUP', 8, 0, K(b'Plan')), E('GETTABLE', 8, 8, K(b'window')),
                 E('SELF', 8, 8, K(b'SetText')), E('MOVE', 10, 7), E('CALL', 8, 3, 1),
                 E('GETTABUP', 8, 0, K(b'PauseFrame')), E('LOADK', 9, bx=one), E('CALL', 8, 2, 1),
                 E('GETTABUP', 8, 0, K(b'Plan')), E('GETTABLE', 8, 8, K(b'window')),
                 E('SELF', 8, 8, K(b'WaitForInput')), E('CALL', 8, 2, 1),
                 E('GETTABUP', 8, 0, K(b'PauseFrame')), E('LOADK', 9, bx=ten), E('CALL', 8, 2, 1)]

    if warp_npc:   # test: stand the player 1.5 m beside an NPC (NPCID.<name>), e.g. Chr201_0460;
        # Npc.getPos(id) returns ok, x, y, z, rotation
        dx, zero = k(1.5), k(0.0)
        if max(dx, zero) >= 256: raise ValueError('RK constant out of range')
        body += [E('GETTABUP', 8, 0, K(b'Npc')), E('GETTABLE', 8, 8, K(b'getPos')),
                 E('GETTABUP', 9, 0, K(b'NPCID')), E('GETTABLE', 9, 9, K(warp_npc)), E('CALL', 8, 2, 6),
                 E('GETTABUP', 13, 0, K(b'Player')), E('GETTABLE', 13, 13, K(b'setPos')),
                 E('ADD', 14, 9, 256 + dx), E('MOVE', 15, 10), E('MOVE', 16, 11), E('LOADK', 17, bx=zero),
                 E('CALL', 13, 5, 1)]
    def visit():   # NPC name in R7: print it, stand beside it, wait ~3 s for the balloon
        nonlocal body
        dx, zero, wait = k(1.5), k(0.0), k(180.0)
        body += [E('GETTABUP', 8, 0, K(b'print')), E('MOVE', 9, 7), E('CALL', 8, 2, 1),
                 E('GETTABUP', 8, 0, K(b'Npc')), E('GETTABLE', 8, 8, K(b'getPos')),
                 E('GETTABUP', 9, 0, K(b'NPCID')), E('GETTABLE', 9, 9, 7), E('CALL', 8, 2, 6),
                 E('TEST', 8, 0, 0), 'SKIP',
                 E('GETTABUP', 13, 0, K(b'Player')), E('GETTABLE', 13, 13, K(b'setPos')),
                 E('ADD', 14, 9, 256 + dx), E('MOVE', 15, 10), E('MOVE', 16, 11), E('LOADK', 17, bx=zero),
                 E('CALL', 13, 5, 1),
                 E('GETTABUP', 8, 0, K(b'PauseFrame')), E('LOADK', 9, bx=wait), E('CALL', 8, 2, 1)]
    if warp_all:   # leave the talk state so the field HUD and NPC balloons show during the tour
        body += [E('GETTABUP', 8, 0, K(b'Plan')), E('GETTABLE', 8, 8, K(b'TalkEnd')), E('CALL', 8, 1, 1)]
    loop(list(warp_all), visit)
    if warp_all:
        body += [E('GETTABUP', 8, 0, K(b'Plan')), E('GETTABLE', 8, 8, K(b'TalkStart')), E('CALL', 8, 1, 1)]
    loop([float(m) for m in mails], call2(b'Mail', b'send'))
    loop([float(i) for i in items], call2(b'Player', b'addItem', one))
    if intermission:   # news/shop/result of the loaded stage; returns when it closes
        body += [E('SETTABUP', 0, K(b'InterMissionDemoStartFlag'), 256 + true),
                 E('GETTABUP', 8, 0, K(b'StartInterMissionWithoutDemo')), E('CALL', 8, 1, 1)]
    loop(list(keys), show)
    body[2] = E('JMP', 0, sbx=len(body) - 3)               # past the gallery
    if len(f.k) >= 1 << 18: raise ValueError('too many constants')
    f.code = body + f.code
    f.lineinfo = [f.line] * len(body) + f.lineinfo
    f.locvars = []                                         # debug ranges shift; not needed
    f.stack = max(f.stack, 60)
    return lua52.dump(root)


def mail_ids(chunk):
    f = lua52.load(chunk)
    return [int(f.k[x['c'] - 256]) for x in map(lua52.fields, f.code)
            if lua52.OPS[x['op']] == 'SETTABLE' and x['b'] >= 256 and f.k[x['b'] - 256] == b'id' and x['c'] >= 256]


def item_ids(chunk, prefixes=()):
    """Inventory item ids from item_data (hair/face styles and empty slots are not items)."""
    f = lua52.load(chunk); reg = {}; out = []
    for x in map(lua52.fields, f.code):
        n = lua52.OPS[x['op']]
        if n == 'LOADK': reg[x['a']] = f.k[x['bx']]
        elif n == 'SETTABUP':
            name = f.k[x['b'] - 256] if x['b'] >= 256 else reg.get(x['b'])
            v = f.k[x['c'] - 256] if x['c'] >= 256 else reg.get(x['c'])
            if (isinstance(name, bytes) and name.startswith(b'ITEM_') and isinstance(v, float)
                    and not re.search(rb'^ITEM_(HAIR|FACE|ALL)_|EMPTY', name)
                    and (not prefixes or name.decode().startswith(tuple(prefixes)))):
                out.append(int(v))
    return out


def build(archive_path, only=(), mails=False, items=False, dialogs=True, intermission=False, item_prefix=(), warp_npc=None, warp_stage=None):
    with Psarc(archive_path) as archive:
        entries = {e.name: e for e in archive.entries[1:]}
        read = lambda name: b''.join(archive.chunks(entries[name]))
        names = sorted(n for n in entries if n.startswith('lua/Text_') and n.endswith('.luc'))
        names.sort(key=lambda n: next((i for i, p in enumerate(FIRST) if n[4:].startswith(p)), 99))
        keys = []
        for name in names:
            for key in message_keys(Resource(read(name)).payload):
                if key not in keys and (not only or key.decode().startswith(tuple(only))): keys.append(key)
        if not dialogs: keys = []
        m = mail_ids(Resource(read('lua/mail_data.luc')).payload) if mails else []
        it = item_ids(Resource(read('lua/item_data.luc')).payload, item_prefix) if items or item_prefix else []
        warp_all = []
        if warp_stage:   # every NPCID of stage_<n>_npc.luc, in file order
            f = lua52.load(Resource(read(f'lua/stage_{warp_stage}_npc.luc')).payload)
            warp_all = [c for c in f.k if isinstance(c, bytes) and re.fullmatch(rb'Chr\d+_\d+', c)]
        plan = Resource(read('lua/plan.luc'))
        return plan.rebuild(patch(plan.payload, keys, m, it, intermission, warp_npc, warp_all)), (len(keys), len(m), len(it) + len(warp_all))


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('archive', type=Path); p.add_argument('output', type=Path)
    p.add_argument('--only', action='append', default=[], metavar='PREFIX',
                   help='only keys starting with PREFIX (e.g. NpcMess460 for the florist)')
    p.add_argument('--mails', action='store_true', help='first send every mail in mail_data')
    p.add_argument('--items', action='store_true', help='first give one of every inventory item')
    p.add_argument('--intermission', action='store_true', help='open the stage intermission (news, shop, result)')
    p.add_argument('--item-prefix', action='append', default=[], help='only items whose name starts with this (e.g. ITEM_READING_)')
    p.add_argument('--no-dialogs', action='store_true', help='skip the message walk')
    p.add_argument('--warp-npc', help='first move the player beside NPCID.<name> (e.g. Chr201_0460, the florist)')
    p.add_argument('--warp-stage', help='visit every NPC of stage_<n>_npc.luc for ~3 s each, printing its NPCID (e.g. 201)')
    a = p.parse_args()
    warp = a.warp_npc.encode() if a.warp_npc else None
    data, n = build(a.archive, a.only, a.mails, a.items, not a.no_dialogs, a.intermission, a.item_prefix, warp, a.warp_stage)
    if not any(n) and not a.intermission and not warp: raise ValueError('nothing to show')
    a.output.parent.mkdir(parents=True, exist_ok=True); a.output.write_bytes(data)
    print(f'plan.luc with gallery: {n[0]} messages, {n[1]} mails, {n[2]} items -> {a.output}')


if __name__ == '__main__':
    main()
