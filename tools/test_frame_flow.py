"""Synthetic ELF/unwind/import tests; no game data or guest execution."""
import struct
import hashlib
import unittest

from inspect_frame_flow import (direct_candidates, import_thunks, mapped_offset,
                                parse_instructions, prepared_imports, program_headers,
                                unwind_functions, verify_facts, verify_virtual_slot)


def elf_fixture():
    data = bytearray(1024)
    struct.pack_into('<16sHHIQQQIHHHHHH', data, 0, b'\x7fELF\x02\x01\x01'+bytes(9),
                     2, 62, 1, 0x1000, 64, 0, 0, 64, 56, 2, 0, 0, 0)
    struct.pack_into('<IIQQQQQQ', data, 64, 1, 5, 256, 0x1000, 0, 768, 768, 1)
    struct.pack_into('<IIQQQQQQ', data, 120, 0x6474e550, 4, 512, 0x1100, 0, 28, 28, 4)
    data[512:516] = bytes.fromhex('011b033b')
    struct.pack_into('<iI', data, 516, 0x1180-0x1104, 2)
    for i, start in enumerate((0x1000, 0x1020)):
        fde = 0x11a0+i*32
        struct.pack_into('<ii', data, 524+i*8, start-0x1100, fde-0x1100)
        pos = 256+fde-0x1000
        struct.pack_into('<IIiI', data, pos, 20, fde+4-0x1180, start-(fde+8), 16)
    struct.pack_into('<II', data, 640, 20, 0)
    data[648:657] = bytes.fromhex('017a5200017810011b')
    data[256:261] = b'\xe8'+struct.pack('<i', 0x1020-0x1005)
    data[261:267] = b'\xff\x25'+struct.pack('<i', 0x1250-0x100b)
    return data


class FrameFlowTests(unittest.TestCase):
    def test_unwind_uses_fde_ranges(self):
        rows = unwind_functions(elf_fixture())
        self.assertEqual([(r['start'], r['end']) for r in rows], [(0x1000, 0x1010), (0x1020, 0x1030)])

    def test_refuses_unknown_cie_encoding(self):
        data = elf_fixture()
        data[656] = 0x03
        with self.assertRaisesRegex(ValueError, 'Unsupported CIE'):
            unwind_functions(data)

    def test_refuses_truncated_index(self):
        data = elf_fixture()
        struct.pack_into('<I', data, 520, 1000)
        with self.assertRaisesRegex(ValueError, 'Truncated unwind'):
            unwind_functions(data)

    def test_refuses_table_fde_disagreement(self):
        data = elf_fixture()
        struct.pack_into('<i', data, 524, -0xff)
        with self.assertRaisesRegex(ValueError, 'disagreement'):
            unwind_functions(data)

    def test_refuses_function_outside_executable_segment(self):
        data = elf_fixture()
        struct.pack_into('<I', data, 684, 0x1000)
        with self.assertRaisesRegex(ValueError, 'outside executable'):
            unwind_functions(data)

    def test_unmapped_pointer_and_truncated_elf(self):
        with self.assertRaises(ValueError):
            mapped_offset(program_headers(elf_fixture()), 0x2000)
        with self.assertRaises(ValueError):
            unwind_functions(b'\x7fELF')

    def test_candidates_exclude_bytes_outside_function_ranges(self):
        data = elf_fixture()
        data[272:277] = b'\xe8'+struct.pack('<i', 0x1020-0x1015)
        calls = direct_candidates(data, unwind_functions(data))
        self.assertEqual(calls[0x1020], [(0x1000, 0x1000)])

    def test_thunk_must_reference_known_import_slot(self):
        data = elf_fixture()
        self.assertEqual(import_thunks(data, {0x1250: 'synthetic'}),
                         {0x1005: dict(slot=0x1250, symbol='synthetic')})
        self.assertEqual(import_thunks(data, {0x1258: 'different'}), {})

    def test_prepared_relocations_and_reject_linked_image(self):
        data = bytearray(struct.pack('<8s6Q', b'BBPROBE2', 64, 0, 0, 1, 1, 0))
        data += b'synthetic'.ljust(128, b'\0')
        data += struct.pack('<QQqq', 16, 1, 0, 0)+bytes(64)
        self.assertEqual(prepared_imports(data), {16: 'synthetic'})
        data[:8] = b'BBPROBE5'
        with self.assertRaises(ValueError):
            prepared_imports(data)
        with self.assertRaises(ValueError):
            prepared_imports(bytes(7))

    def test_disassembler_distinguishes_indirect_calls_and_immediates(self):
        text = '''
1000:   mov eax, 0xe8
1005:   call 0x1020 <test>
100a:   call qword ptr [rax + 0x60]
100d:   jne 0x1000 <test>
'''
        rows = parse_instructions(text)
        self.assertEqual([r['target'] for r in rows], [None, 0x1020, None, None])
        self.assertFalse(any(r['address'] == 0x1001 for r in rows))

    def test_facts_reject_wrong_hash_value_and_unmapped_address(self):
        data = elf_fixture()
        facts = dict(source_sha256=hashlib.sha256(data).hexdigest(),
                     reads=[dict(label='sample', address='0x1020', type='u64', expected=['0x0'])])
        self.assertEqual(len(verify_facts(data, facts)), 1)
        facts['reads'][0]['expected'] = ['0x1']
        with self.assertRaisesRegex(ValueError, 'mismatch'):
            verify_facts(data, facts)
        facts['reads'][0]['address'] = '0x3000'
        with self.assertRaisesRegex(ValueError, 'Unmapped'):
            verify_facts(data, facts)
        facts['source_sha256'] = 'different'
        with self.assertRaisesRegex(ValueError, 'different ELF'):
            verify_facts(data, facts)

    def test_virtual_edge_is_conditional_and_checks_both_callsite_and_table(self):
        data = elf_fixture()
        struct.pack_into('<Q', data, 256+0x1200-0x1000+0x60, 0x1020)
        row = dict(label='synthetic', caller='0x1000', site='0x100a',
                   table='0x1200', slot='0x60', target='0x1020')
        instructions = parse_instructions('100a: call qword ptr [rax + 0x60]')
        self.assertFalse(verify_virtual_slot(data, row, instructions)['receiver_runtime_verified'])
        with self.assertRaisesRegex(ValueError, 'slot disagreement'):
            verify_virtual_slot(data, dict(row, slot='0x40'), instructions)
        with self.assertRaisesRegex(ValueError, 'target disagreement'):
            verify_virtual_slot(data, dict(row, target='0x1030'), instructions)
        with self.assertRaisesRegex(ValueError, 'Expected virtual'):
            verify_virtual_slot(data, dict(row, site='0x100b'), instructions)


if __name__ == '__main__':
    unittest.main()
