"""Synthetic instruction coverage checks; no game files."""
import unittest
from index_guest_code import complete, instruction


class InstructionIndexTests(unittest.TestCase):
    def test_lengths_and_targets(self):
        row = instruction(' 1000: e8 1b 00 00 00    call 0x1020 <synthetic>')
        self.assertEqual(row, (0x1000, 5, 'call', '0x1020 <synthetic>', None, 0x1020))
        row = instruction(' 1005: 48 8d 05 f4 0f 00 00   lea rax, [rip + 0xff4] # 0x2000')
        self.assertEqual(row[1], 7)
        self.assertEqual(row[4], 0x2000)
        self.assertIsNone(row[5])

    def test_rejects_gaps_crossed_boundaries_and_unknown_instructions(self):
        rows = [instruction('1000: 90 nop'), instruction('1001: c3 ret')]
        self.assertTrue(complete(0x1000, 0x1002, rows))
        self.assertFalse(complete(0x1000, 0x1003, rows))
        self.assertFalse(complete(0x1000, 0x1001, rows))
        self.assertFalse(complete(0x1000, 0x1002, rows[1:]))
        self.assertFalse(complete(0x1000, 0x1001, [instruction('1000: ff <unknown>')]))

    def test_rejects_headers_and_unresolved_virtual_calls_stay_unresolved(self):
        self.assertIsNone(instruction('0000000000001000 <synthetic>:'))
        row = instruction('1000: ff 50 60 call qword ptr [rax + 0x60]')
        self.assertIsNone(row[5])
        self.assertTrue(complete(0x1000, 0x1003, [row]))


if __name__ == '__main__':
    unittest.main()
