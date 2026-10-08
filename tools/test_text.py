"""Testes com textos e contêineres sintéticos, sem dados do jogo."""
import copy
import json
import struct
import unittest
import zlib
from game_resource import Resource
from text_formats import Csh, Lua52
from text_tool import export, insert


def lua_string(raw):
    return struct.pack('<Q', len(raw) + 1) + raw + b'\0'


def lua_function(strings, children=()):
    # Fixtures estruturais; as instruções são marcadores, nunca executados.
    data = struct.pack('<iiBBB', 1, 2, 0, 1, 4)
    data += struct.pack('<i', 1) + bytes.fromhex('01020304')
    constants = [b'\0', b'\1\1', b'\3' + struct.pack('<f', 1.25)]
    constants += [b'\4' + lua_string(s) for s in strings]
    data += struct.pack('<i', len(constants)) + b''.join(constants)
    data += struct.pack('<i', len(children)) + b''.join(children)
    data += struct.pack('<i', 1) + b'\1\0'
    data += lua_string(b'@synthetic.lua')
    data += struct.pack('<ii', 1, 12)  # uma linha de debug
    data += struct.pack('<i', 1) + lua_string(b'local_name') + struct.pack('<ii', 0, 1)
    data += struct.pack('<i', 1) + lua_string(b'upvalue_name')
    return data


def lua_fixture():
    child = lua_function(['テスト'.encode(), b'child_constant'])
    return Lua52.HEADER + lua_function([b'root_constant', b'\xffbinary'], [child])


def csh_fixture():
    columns = 8
    pool = 12 + columns * 8
    raw = 'テスト'.encode() + b'\0'
    raw += bytes(-len(raw) % 4)
    integer = pool + len(raw)
    floating = integer + 4
    opaque = floating + 4
    cells = [(pool, len(raw)//4), (pool, len(raw)//4),
             (integer, 0x40000001), (floating, 0x80000001), (0, 0x60000000),
             (0, 0xa0000000), (0, 0xc0000000), (opaque, 0x20000002)]
    return (struct.pack('>3I', 1, columns, 1) + b''.join(struct.pack('>2I', *c) for c in cells)
            + raw + struct.pack('>if', 7, 1.25) + bytes.fromhex('4e16030000000002'))


def resource_fixture(payload, lua=False):
    header = bytearray(128)
    header[:4] = b'\0cul' if lua else b'\0hsc'
    struct.pack_into('>I', header, 4, 2)
    header[20:24] = b'KEEP'  # metadado desconhecido deve ser preservado
    compressed = zlib.compress(payload, 1)
    return bytes(header) + struct.pack('>4sIII', b'BILZ', len(payload), len(compressed), 7) + compressed


class TextTest(unittest.TestCase):
    def test_lua_nested_strings_growth_shrink_and_debug_preservation(self):
        data = lua_fixture()
        parsed = Lua52(data)
        self.assertEqual(parsed.prototypes, 2)
        self.assertEqual(parsed.rebuild({s.id: s.raw for s in parsed.strings}), data)
        changes = {'p0/k3': b'', 'p0/p0/k3': 'Ação, coração, café e tradução maior'.encode()}
        rebuilt = parsed.rebuild(changes)
        strings = {s.id: s.raw for s in Lua52(rebuilt).strings}
        self.assertEqual(strings['p0/k3'], b'')
        self.assertEqual(strings['p0/p0/k3'], changes['p0/p0/k3'])
        self.assertEqual(strings['p0/k4'], b'\xffbinary')
        self.assertIn(b'local_name', rebuilt)
        self.assertEqual(rebuilt.count(bytes.fromhex('01020304')), 2)

    def test_csh_aliases_numbers_nulls_and_opaque_value_preserved(self):
        data = csh_fixture()
        table = Csh(data)
        self.assertEqual(table.rebuild({s.id: s.raw for s in table.strings}), data)
        changed = table.rebuild({'r0/c0': 'Texto maior com ç, ã e ó'.encode()})
        parsed = Csh(changed)
        self.assertEqual(parsed.strings[0].text(), 'Texto maior com ç, ã e ó')
        self.assertEqual(parsed.strings[1].text(), 'テスト')
        self.assertEqual(changed[28:table.pool], data[28:table.pool])
        self.assertEqual(changed[table.pool:len(data)], data[table.pool:])
        self.assertEqual(Csh(table.rebuild({'r0/c0': b''})).strings[0].raw, b'')

    def test_json_roundtrip_and_changed_resource_lengths(self):
        for lua, payload in [(True, lua_fixture()), (False, csh_fixture())]:
            with self.subTest(lua=lua):
                data = resource_fixture(payload, lua)
                document = json.loads(json.dumps(export(data), ensure_ascii=False))
                self.assertEqual(insert(data, document), data)
                candidate = next(r for r in document['strings'] if r['japanese_candidate'])
                candidate['translation'] = 'Teste de tradução: ação e coração.'
                changed = insert(data, document)
                self.assertEqual(changed[:128], data[:128])
                check = export(changed)
                self.assertEqual(next(r['original'] for r in check['strings'] if r['id'] == candidate['id']), candidate['translation'])
                self.assertNotEqual(changed, data)

    def test_wrong_document_hash_original_ids_and_missing_records(self):
        data = resource_fixture(csh_fixture())
        original = export(data)
        for mutation in ('hash', 'original', 'duplicate', 'missing', 'unknown'):
            doc = copy.deepcopy(original)
            if mutation == 'hash': doc['source_sha256'] = 'wrong'
            if mutation == 'original': doc['strings'][0]['original'] = 'wrong'
            if mutation == 'duplicate': doc['strings'].append(copy.deepcopy(doc['strings'][0]))
            if mutation == 'missing': doc['strings'].pop()
            if mutation == 'unknown': doc['strings'][0]['id'] = 'r999/c999'
            with self.subTest(mutation=mutation), self.assertRaises(ValueError):
                insert(data, doc)

    def test_bad_bilz_checksum_sizes_and_truncated_lua(self):
        data = resource_fixture(lua_fixture(), True)
        corrupt = bytearray(data)
        corrupt[-1] ^= 1
        with self.assertRaises((ValueError, zlib.error)):
            Resource(bytes(corrupt))
        with self.assertRaises(ValueError):
            Resource(data[:-1])
        with self.assertRaises(ValueError):
            Lua52(lua_fixture()[:-1])

    def test_invalid_csh_pointer_and_unknown_type(self):
        for bad_tag in (None, 0xe0000001):
            data = bytearray(csh_fixture())
            if bad_tag is None: struct.pack_into('>I', data, 12, 4)
            else: struct.pack_into('>I', data, 16, bad_tag)
            with self.assertRaises(ValueError):
                Csh(bytes(data))


if __name__ == '__main__':
    unittest.main()
