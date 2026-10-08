"""Contêiner de recursos do jogo: 128 bytes seguidos de BILZ/zlib."""
import struct
import zlib


class Resource:
    def __init__(self, data):
        if len(data) < 144 or data[:4] not in (b'\0cul', b'\0hsc'):
            raise ValueError('Tipo de recurso não suportado')
        if struct.unpack_from('>I', data, 4)[0] != 2:
            raise ValueError('Versão de recurso não suportada')
        magic, raw_size, stored_size, self.flags = struct.unpack_from('>4sIII', data, 128)
        if magic != b'BILZ' or stored_size != len(data) - 144 or raw_size > 64 * 1024 * 1024:
            raise ValueError('Cabeçalho BILZ inválido')
        decoder = zlib.decompressobj()
        payload = decoder.decompress(data[144:], raw_size + 1)
        if (not decoder.eof or decoder.unused_data or decoder.unconsumed_tail
                or len(payload) != raw_size):
            raise ValueError('Dados BILZ inválidos')
        self.original = data
        self.payload = payload

    def rebuild(self, payload):
        if not isinstance(payload, bytes) or len(payload) > 64 * 1024 * 1024:
            raise ValueError('Payload de recurso inválido ou grande demais')
        # Streams equivalentes não são necessariamente byte a byte iguais.
        if payload == self.payload:
            return self.original
        packed = zlib.compress(payload, 9)
        return self.original[:128] + struct.pack('>4sIII', b'BILZ', len(payload), len(packed), self.flags) + packed
