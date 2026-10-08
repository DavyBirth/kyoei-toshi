"""Fixtures sintéticas: nenhum dado do jogo é usado nestes testes."""
import hashlib
import contextlib
import io
import json
import struct
import tempfile
import unittest
import zlib
from pathlib import Path
from psarc import Psarc, safe_name
import verify_m1


def fixture(files):
    names = b'\0'.join(name.encode() for name, data in files)
    entries = [(bytes(16), names)] + [(hashlib.md5(n.upper().encode()).digest(), d) for n, d in files]
    # TOC ordenada por hash, não pela ordem do manifesto.
    entries[1:] = sorted(entries[1:])
    payloads, rows, sizes = [], [], []
    offset = 0
    for digest, data in entries:
        start = len(sizes)
        chunks = []
        for i in range(0, len(data), 65536):
            raw = data[i:i+65536]
            packed = zlib.compress(raw)
            stored = packed if len(packed) < len(raw) else raw
            sizes.append(len(stored) % 65536)
            chunks.append(stored)
        payload = b''.join(chunks)
        rows.append((digest, start, len(data), offset))
        payloads.append(payload)
        offset += len(payload)
    toc_size = 32 + 30 * len(rows) + 2 * len(sizes)
    toc = b''.join(d + b.to_bytes(4, 'big') + s.to_bytes(5, 'big') + (o+toc_size).to_bytes(5, 'big') for d, b, s, o in rows)
    header = struct.pack('>4sI4s5I', b'PSAR', 0x10004, b'zlib', toc_size, 30, len(rows), 65536, 13)
    return header + toc + b''.join(s.to_bytes(2, 'big') for s in sizes) + b''.join(payloads)


class PsarcTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.path = Path(self.temp.name) / 'test.psarc'

    def test_reordered_names_compressed_raw_empty_and_multiple_blocks(self):
        # Random-looking bytes deterministically exercise a raw 64-KiB block.
        raw = b''.join(hashlib.sha256(str(i).encode()).digest() for i in range(2048))
        files = [('mixed/Title.bin', raw + b'A' * 70000), ('small.bin', b'123'), ('empty.bin', b'')]
        self.path.write_bytes(fixture(files))
        with Psarc(self.path) as archive:
            self.assertEqual({e.name: b''.join(archive.chunks(e)) for e in archive.entries[1:]}, dict(files))

    def test_path_escape_and_windows_aliases(self):
        for name in ('../x', '/x', 'a/../../x', 'C:/x', 'a\\x', 'a//x', 'a/./x', 'NUL.bin', 'x:ads', 'a. /x', 'a./x'):
            with self.subTest(name=name), self.assertRaises(ValueError):
                safe_name(name)

    def test_case_collision(self):
        self.path.write_bytes(fixture([('Foo.bin', b'a'), ('foo.bin', b'b')]))
        with self.assertRaises(ValueError):
            Psarc(self.path)

    def test_truncated_archive(self):
        self.path.write_bytes(fixture([('x.bin', b'hello')])[:-1])
        with self.assertRaises(ValueError):
            Psarc(self.path)

    def test_wrong_path_hash(self):
        data = bytearray(fixture([('x.bin', b'hello')]))
        data[62] ^= 1
        self.path.write_bytes(data)
        with self.assertRaises(ValueError):
            Psarc(self.path)

    def test_offset_40_bits_is_not_truncated(self):
        data = bytearray(fixture([('x.bin', b'hello')]))
        data[87] = 1  # byte alto do offset uint40 da segunda entrada
        self.path.write_bytes(data)
        with self.assertRaises(ValueError):
            Psarc(self.path)

    def test_block_index_out_of_bounds(self):
        data = bytearray(fixture([('x.bin', b'hello')]))
        data[78:82] = (999).to_bytes(4, 'big')
        self.path.write_bytes(data)
        with self.assertRaises(ValueError):
            Psarc(self.path)

    def test_corrupt_zlib_is_not_silently_accepted(self):
        data = bytearray(fixture([('x.bin', b'A' * 1000)]))
        data[-1] ^= 1
        self.path.write_bytes(data)
        with Psarc(self.path) as archive:
            with self.assertRaises((ValueError, zlib.error)):
                b''.join(archive.chunks(archive.entries[1]))

    def test_saved_extraction_proof_rejects_changes_and_extra_files(self):
        repo = Path(self.temp.name)
        (repo / 'docs/m1').mkdir(parents=True)
        (repo / 'work/m1').mkdir(parents=True)
        extracted = repo / 'extracted/example'
        extracted.mkdir(parents=True)
        payload = b'fixture data'
        (extracted / 'file.bin').write_bytes(payload)
        catalog = dict(extracted_and_verified=True, extraction=str(extracted.parent),
                       archives=[dict(archive='example.psarc', files=[dict(path='file.bin',
                           bytes=len(payload), sha256=hashlib.sha256(payload).hexdigest())])])
        raw = json.dumps(catalog).encode()
        ledger = repo / 'work/m1/catalog.json'
        ledger.write_bytes(raw)
        proof = dict(catalog_sha256=hashlib.sha256(raw).hexdigest(), verified_files=1, verified_bytes=len(payload))
        (repo / 'docs/m1/formats.json').write_text(json.dumps(proof), encoding='utf8')
        original = verify_m1.__file__
        verify_m1.__file__ = str(repo / 'tools/verify_m1.py')
        self.addCleanup(setattr, verify_m1, '__file__', original)
        with contextlib.redirect_stdout(io.StringIO()):
            verify_m1.main()
        (extracted / 'file.bin').write_bytes(b'changed data')
        with self.assertRaises(ValueError):
            verify_m1.main()
        (extracted / 'file.bin').write_bytes(payload)
        (extracted / 'extra.bin').write_bytes(b'extra')
        with self.assertRaises(ValueError):
            verify_m1.main()
        ledger.write_bytes(raw + b' ')
        with self.assertRaises(ValueError):
            verify_m1.main()


if __name__ == '__main__':
    unittest.main()
