"""Leitor PSARC do Kyoei: TOC em claro, zlib e caminhos identificados por MD5.

Implementação independente; não altera nem reempacota arquivos do jogo.
"""
import hashlib
import re
import struct
import zlib
from dataclasses import dataclass
from pathlib import Path


@dataclass
class Entry:
    digest: bytes
    block: int
    size: int
    offset: int
    name: str = ''

@dataclass(frozen=True)
class DeltaSource:
    """Explicit read-only base + verified delta, reopened for each consumer."""
    base: Path
    delta: bytes


def safe_name(name):
    """Caminho relativo utilizável no Windows, sem escape ou alias."""
    if not name or '\\' in name or name.startswith('/'):
        raise ValueError(f'Caminho inválido: {name!r}')
    reserved = {'CON', 'PRN', 'AUX', 'NUL'} | {f'{p}{i}' for p in ('COM', 'LPT') for i in range(1, 10)}
    for part in name.split('/'):
        if (not part or part in ('.', '..') or part.endswith((' ', '.'))
                or re.search(r'[<>:"|?*\x00-\x1f]', part)
                or part.split('.')[0].upper() in reserved):
            raise ValueError(f'Caminho inválido: {name!r}')
    return name


class Psarc:
    def __init__(self, path, file=None, size=None):
        # file/size read a virtual archive (e.g. base + runtime delta).
        if isinstance(path,DeltaSource):
            if file is not None:raise ValueError('Choose one virtual archive source')
            from psarc_delta import Virtual
            file=Virtual(path.base,path.delta);size=file.size;path=path.base
        self.path = Path(path)
        self.file = file or self.path.open('rb')
        try:
            self.archive_size = size if file else self.path.stat().st_size
            header = self.read_exact(32)
            magic, version, codec, toc_size, stride, count, block_size, flags = struct.unpack('>4sI4s5I', header)
            if (magic, version, codec, stride, block_size, flags) != (b'PSAR', 0x10004, b'zlib', 30, 65536, 13):
                raise ValueError('Variante PSARC não suportada')
            if not count or not 32 + count * stride <= toc_size <= min(self.archive_size, 128 * 1024 * 1024):
                raise ValueError('TOC inválida')
            self.header = dict(version=version, compression='zlib', toc_bytes=toc_size,
                               entry_bytes=stride, entries=count, block_bytes=block_size, flags=flags)
            self.block_size = block_size
            toc = self.read_exact(toc_size - 32)
            table = toc[count * stride:]
            if len(table) % 2:
                raise ValueError('Tabela de blocos truncada')
            self.blocks = [int.from_bytes(table[i:i+2], 'big') for i in range(0, len(table), 2)]
            self.entries = []
            for i in range(count):
                row = toc[i * stride:(i + 1) * stride]
                entry = Entry(row[:16], int.from_bytes(row[16:20], 'big'),
                              int.from_bytes(row[20:25], 'big'), int.from_bytes(row[25:30], 'big'))
                nblocks = (entry.size + block_size - 1) // block_size
                if entry.block + nblocks > len(self.blocks):
                    raise ValueError('Índice de bloco fora da tabela')
                stored = sum(b or block_size for b in self.blocks[entry.block:entry.block+nblocks])
                if not toc_size <= entry.offset <= self.archive_size or entry.offset + stored > self.archive_size:
                    raise ValueError('Dados fora do arquivo')
                self.entries.append(entry)
            manifest = self.entries[0]
            if manifest.digest != bytes(16) or manifest.size > 32 * 1024 * 1024:
                raise ValueError('Manifesto inválido')
            raw_names = b''.join(self.chunks(manifest))
            names = raw_names.rstrip(b'\0').split(b'\0') if b'\0' in raw_names else raw_names.splitlines()
            if len(names) != count - 1:
                raise ValueError('Contagem de nomes diferente da TOC')
            by_hash = {}
            seen = set()
            for raw in names:
                name = safe_name(raw.decode('utf8'))
                if name.casefold() in seen:
                    raise ValueError('Nomes duplicados no Windows')
                seen.add(name.casefold())
                # Convenção medida no dump: MD5 do caminho ASCII em maiúsculas.
                digest = hashlib.md5(raw.upper()).digest()
                if digest in by_hash:
                    raise ValueError('Hash de nome duplicado')
                by_hash[digest] = name
            for entry in self.entries[1:]:
                if entry.digest not in by_hash:
                    raise ValueError(f'Nome não encontrado para MD5 {entry.digest.hex()}')
                entry.name = by_hash.pop(entry.digest)
        except BaseException:
            self.file.close()
            raise

    def read_exact(self, length):
        data = self.file.read(length)
        if len(data) != length:
            raise ValueError('Arquivo truncado')
        return data

    def chunks(self, entry):
        self.file.seek(entry.offset)
        remaining = entry.size
        index = entry.block
        while remaining:
            expected = min(remaining, self.block_size)
            size = self.blocks[index] or self.block_size
            data = self.read_exact(size)
            if size != expected:
                decoder = zlib.decompressobj()
                data = decoder.decompress(data, expected + 1)
                if not decoder.eof or decoder.unused_data or decoder.unconsumed_tail:
                    raise ValueError(f'Bloco zlib inválido: {entry.name}')
            if len(data) != expected:
                raise ValueError(f'Tamanho descompactado incorreto: {entry.name}')
            yield data
            remaining -= expected
            index += 1

    def __enter__(self):
        return self

    def __exit__(self, *args):
        self.file.close()
