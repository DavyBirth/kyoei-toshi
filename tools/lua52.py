"""Lua 5.2 bytecode reader/writer for the game's scripts (float32 numbers).

Header: 1B 'LuaR' 52 00 01 04 08 04 04 00 + LUAC_TAIL. Written from the Lua 5.2
format description; only what the game's chunks use. Round-trips exactly.
"""
import struct

HEADER = b'\x1bLuaR\x00\x01\x04\x08\x04\x04\x00\x19\x93\r\n\x1a\n'
OPS = ('MOVE LOADK LOADKX LOADBOOL LOADNIL GETUPVAL GETTABUP GETTABLE SETTABUP '
       'SETUPVAL SETTABLE NEWTABLE SELF ADD SUB MUL DIV MOD POW UNM NOT LEN CONCAT '
       'JMP EQ LT LE TEST TESTSET CALL TAILCALL RETURN FORLOOP FORPREP TFORCALL '
       'TFORLOOP SETLIST CLOSURE VARARG EXTRAARG').split()
OP = {n: i for i, n in enumerate(OPS)}


class Proto:
    def __init__(self):
        self.line = self.lastline = 0; self.params = self.vararg = 0; self.stack = 2
        self.code = []; self.k = []; self.protos = []; self.upvals = []
        self.source = None; self.lineinfo = []; self.locvars = []; self.upnames = []


class Reader:
    def __init__(self, data): self.d = data; self.p = 0
    def take(self, n): b = self.d[self.p:self.p + n]; self.p += n; return b
    def u8(self): return self.take(1)[0]
    def i32(self): return struct.unpack('<i', self.take(4))[0]
    def string(self):
        n = struct.unpack('<Q', self.take(8))[0]
        return None if n == 0 else self.take(n)[:-1]

    def proto(self):
        f = Proto()
        f.line, f.lastline = self.i32(), self.i32()
        f.params, f.vararg, f.stack = self.u8(), self.u8(), self.u8()
        f.code = [struct.unpack('<I', self.take(4))[0] for _ in range(self.i32())]
        for _ in range(self.i32()):
            t = self.u8()
            if t == 0: f.k.append(None)
            elif t == 1: f.k.append(bool(self.u8()))
            elif t == 3: f.k.append(struct.unpack('<f', self.take(4))[0])
            elif t == 4: f.k.append(self.string())
            else: raise ValueError('constant type %d' % t)
        f.protos = [self.proto() for _ in range(self.i32())]
        f.upvals = [(self.u8(), self.u8()) for _ in range(self.i32())]
        f.source = self.string()
        f.lineinfo = [self.i32() for _ in range(self.i32())]
        f.locvars = [(self.string(), self.i32(), self.i32()) for _ in range(self.i32())]
        f.upnames = [self.string() for _ in range(self.i32())]
        return f


def load(data):
    if data[:len(HEADER)] != HEADER: raise ValueError('not a float32 Lua 5.2 chunk')
    r = Reader(data); r.p = len(HEADER); f = r.proto()
    if r.p != len(data): raise ValueError('trailing bytes')
    return f


def _string(s):
    if s is None: return struct.pack('<Q', 0)
    return struct.pack('<Q', len(s) + 1) + s + b'\0'


def _proto(f):
    out = [struct.pack('<iiBBB', f.line, f.lastline, f.params, f.vararg, f.stack),
           struct.pack('<i', len(f.code)), b''.join(struct.pack('<I', i) for i in f.code),
           struct.pack('<i', len(f.k))]
    for k in f.k:
        if k is None: out.append(b'\0')
        elif isinstance(k, bool): out.append(bytes([1, int(k)]))
        elif isinstance(k, (int, float)): out.append(b'\x03' + struct.pack('<f', k))
        else: out.append(b'\x04' + _string(k))
    out.append(struct.pack('<i', len(f.protos))); out += [_proto(p) for p in f.protos]
    out.append(struct.pack('<i', len(f.upvals))); out += [bytes(u) for u in f.upvals]
    out.append(_string(f.source))
    out.append(struct.pack('<i', len(f.lineinfo))); out += [struct.pack('<i', x) for x in f.lineinfo]
    out.append(struct.pack('<i', len(f.locvars)))
    for n, a, b in f.locvars: out.append(_string(n) + struct.pack('<ii', a, b))
    out.append(struct.pack('<i', len(f.upnames))); out += [_string(n) for n in f.upnames]
    return b''.join(out)


def dump(f): return HEADER + _proto(f)


def fields(i):
    return dict(op=i & 63, a=(i >> 6) & 255, c=(i >> 14) & 511, b=(i >> 23) & 511,
                bx=i >> 14, sbx=(i >> 14) - 131071, ax=i >> 6)


def disasm(f, depth=0, out=None):
    out = [] if out is None else out
    pad = '  ' * depth
    out.append('%sfunction line %d params=%d stack=%d upvals=%s' % (pad, f.line, f.params, f.stack, f.upvals))
    def rk(x): return ('K[%r]' % (f.k[x - 256],)) if x >= 256 else 'R%d' % x
    for pc, i in enumerate(f.code):
        x = fields(i); n = OPS[x['op']]
        if n in ('LOADK',): arg = 'R%d = %r' % (x['a'], f.k[x['bx']])
        elif n == 'GETTABUP': arg = 'R%d = U%d[%s]' % (x['a'], x['b'], rk(x['c']))
        elif n == 'SETTABUP': arg = 'U%d[%s] = %s' % (x['a'], rk(x['b']), rk(x['c']))
        elif n == 'GETTABLE': arg = 'R%d = R%d[%s]' % (x['a'], x['b'], rk(x['c']))
        elif n == 'SETTABLE': arg = 'R%d[%s] = %s' % (x['a'], rk(x['b']), rk(x['c']))
        elif n == 'SELF': arg = 'R%d = R%d; R%d = R%d[%s]' % (x['a'] + 1, x['b'], x['a'], x['b'], rk(x['c']))
        elif n in ('JMP', 'FORLOOP', 'FORPREP', 'TFORLOOP'): arg = 'A=%d -> %d' % (x['a'], pc + 1 + x['sbx'])
        elif n == 'CLOSURE': arg = 'R%d = proto%d' % (x['a'], x['bx'])
        else: arg = 'A=%d B=%d C=%d' % (x['a'], x['b'], x['c'])
        out.append('%s  %4d %-9s %s' % (pad, pc, n, arg))
    for j, p in enumerate(f.protos):
        out.append('%s proto%d:' % (pad, j)); disasm(p, depth + 1, out)
    return out


def encode(op, a=0, b=0, c=0, bx=None, sbx=None):
    o = OP[op]
    if sbx is not None: bx = sbx + 131071
    if bx is not None: return o | (a << 6) | (bx << 14)
    return o | (a << 6) | (c << 14) | (b << 23)
