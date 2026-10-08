"""Leitores estruturais de constantes Lua 5.2 e células CSH.

Não executa scripts. Preserva instruções, números e metadados desconhecidos.
"""
import re
import struct
from dataclasses import dataclass

JAPANESE = re.compile(r'[\u3040-\u30ff\u3400-\u9fff\uff66-\uff9f]')


@dataclass
class String:
    id: str
    raw: bytes
    start: int
    end: int
    pointer: int = 0

    def text(self):
        return self.raw.decode('utf8')


class Lua52:
    HEADER = bytes.fromhex('1b4c7561520001040804040019930d0a1a0a')

    def __init__(self, data):
        if not data.startswith(self.HEADER):
            raise ValueError('Cabeçalho Lua 5.2 diferente da variante PS4 medida')
        self.data = data
        self.pos = len(self.HEADER)
        self.strings = []
        self.prototypes = 0
        self.function('p0', 0)
        if self.pos != len(data):
            raise ValueError('Bytes restantes após o chunk Lua')

    def take(self, size):
        if size < 0 or self.pos + size > len(self.data):
            raise ValueError('Chunk Lua truncado')
        result = self.data[self.pos:self.pos + size]
        self.pos += size
        return result

    def count(self):
        n = struct.unpack('<i', self.take(4))[0]
        if not 0 <= n <= len(self.data):
            raise ValueError('Contagem Lua inválida')
        return n

    def string(self, id=None):
        start = self.pos
        length = struct.unpack('<Q', self.take(8))[0]
        raw = self.take(length)
        if length and raw[-1:] != b'\0':
            raise ValueError('String Lua sem terminador')
        if id is not None:
            if not length:
                raise ValueError('Constante string Lua nula')
            self.strings.append(String(id, raw[:-1], start, self.pos))

    def function(self, id, depth):
        if depth > 100:
            raise ValueError('Profundidade Lua excessiva')
        self.prototypes += 1
        self.take(11)  # linhas inicial/final, parâmetros, vararg e maxstack
        self.take(self.count() * 4)  # instruções
        for i in range(self.count()):
            tag = self.take(1)[0]
            if tag == 0:
                pass
            elif tag == 1:
                self.take(1)
            elif tag == 3:
                self.take(4)  # lua_Number é float32 neste jogo
            elif tag == 4:
                self.string(f'{id}/k{i}')
            else:
                raise ValueError(f'Tipo de constante Lua inválido: {tag}')
        for i in range(self.count()):
            self.function(f'{id}/p{i}', depth + 1)
        self.take(self.count() * 2)  # descritores de upvalues
        self.string()  # caminho fonte de debug, não traduzir
        self.take(self.count() * 4)  # números de linha
        for _ in range(self.count()):
            self.string()  # nome de variável local
            self.take(8)  # startpc/endpc
        for _ in range(self.count()):
            self.string()  # nomes dos upvalues

    def rebuild(self, changes):
        known = {s.id for s in self.strings}
        if changes.keys() - known:
            raise ValueError('ID de constante Lua desconhecido')
        output = bytearray()
        pos = 0
        for string in self.strings:
            raw = changes.get(string.id, string.raw)
            if not isinstance(raw, bytes):
                raise ValueError('Constante Lua deve ser bytes')
            output.extend(self.data[pos:string.start])
            output.extend(struct.pack('<Q', len(raw) + 1) + raw + b'\0')
            pos = string.end
        output.extend(self.data[pos:])
        result = bytes(output)
        # Verificar que uma mudança de tamanho não destruiu a estrutura.
        check = Lua52(result)
        if {s.id: s.raw for s in check.strings} != {s.id: changes.get(s.id, s.raw) for s in self.strings}:
            raise ValueError('Constantes Lua reconstruídas não conferem')
        return result


class Csh:
    def __init__(self, data):
        if len(data) < 12:
            raise ValueError('Tabela CSH truncada')
        self.flags, self.columns, self.rows = struct.unpack_from('>3I', data)
        self.pool = 12 + self.columns * self.rows * 8
        if not self.columns or self.pool > len(data):
            raise ValueError('Dimensões CSH inválidas')
        self.data = data
        self.strings = []
        self.types = [0] * 8
        for i in range(self.columns * self.rows):
            cell = 12 + i * 8
            pointer, tag = struct.unpack_from('>2I', data, cell)
            kind = tag >> 29
            words = tag & 0x1fffffff
            self.types[kind] += 1
            # Sentinelas medidas, sem referência ao pool.
            if kind in (3, 5, 6):
                if pointer or words:
                    raise ValueError('Célula CSH nula com dados inesperados')
                continue
            size = words * 4
            if not self.pool <= pointer <= len(data) or pointer + size > len(data) or pointer % 4:
                raise ValueError('Referência CSH fora do pool')
            if kind in (2, 4):
                if words != 1:
                    raise ValueError('Número CSH com tamanho inesperado')
                continue
            if kind == 1:
                if words != 2:
                    raise ValueError('Célula CSH opaca de 8 bytes com tamanho inesperado')
                continue
            if kind != 0:
                raise ValueError('Tipo CSH não suportado')
            raw = data[pointer:pointer + size]
            null = raw.find(b'\0')
            if null < 0 or any(raw[null:]):
                raise ValueError('String CSH sem terminador/padding zero')
            self.strings.append(String(f'r{i//self.columns}/c{i%self.columns}', raw[:null],
                                       pointer, pointer + size, cell))

    def rebuild(self, changes):
        known = {s.id for s in self.strings}
        if changes.keys() - known:
            raise ValueError('ID de célula CSH desconhecido')
        result = bytearray(self.data)
        for string in self.strings:
            raw = changes.get(string.id, string.raw)
            if not isinstance(raw, bytes) or b'\0' in raw:
                raise ValueError('Texto CSH inválido')
            if raw == string.raw:
                # Regrava o valor extraído, conservando o padding original.
                result[string.start:string.end] = raw + bytes(string.end-string.start-len(raw))
                continue
            padding = (-len(raw)-1) % 4
            encoded = raw + bytes(padding + 1)
            if len(result) % 4:
                result.extend(bytes(-len(result) % 4))
            pointer = len(result)
            result.extend(encoded)
            struct.pack_into('>2I', result, string.pointer, pointer, len(encoded)//4)
        rebuilt = bytes(result)
        check = Csh(rebuilt)
        if {s.id: s.raw for s in check.strings} != {s.id: changes.get(s.id, s.raw) for s in self.strings}:
            raise ValueError('Células CSH reconstruídas não conferem')
        return rebuilt
