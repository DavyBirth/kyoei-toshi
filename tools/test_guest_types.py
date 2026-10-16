import unittest
from unittest.mock import patch

from inspect_guest_types import registrations


def row(address, op, operands='', rip=None, direct=None):
    return (address, 1, op, operands, rip, direct)


class RegistrationTests(unittest.TestCase):
    def fixture(self):
        return [row(1, 'lea', 'rdi, [rip]', 100),
                row(2, 'call', '0x302ff0', direct=0x302ff0),
                row(3, 'lea', 'rax, [rip]', 200),
                row(4, 'mov', 'qword ptr [rsp], rax'),
                row(5, 'lea', 'rdi, [rip]', 300),
                row(6, 'mov', 'edx, 0x100'),
                row(7, 'lea', 'r9, [rip]', 400),
                row(9, 'call', '0x311570', direct=0x311570)]

    def recover(self, rows):
        with patch('inspect_guest_types.technical_name', return_value='UnitSynthetic'):
            return registrations(rows, b'', [], {200})

    def test_registration(self):
        item, = self.recover(self.fixture())
        self.assertEqual((item['constructor'], item['descriptor'], item['size'], item['parent']),
                         ('0xc8', '0x12c', 256, '0x190'))

    def test_caller_saved_receiver(self):
        rows = self.fixture()
        rows.insert(-1, row(8, 'call', '0x999', direct=0x999))
        self.assertEqual(self.recover(rows), [])

    def test_branch_invalidates_window(self):
        rows = self.fixture()
        rows.insert(-1, row(8, 'jne', '0x9'))
        self.assertEqual(self.recover(rows), [])

    def test_partial_receiver_write(self):
        rows = self.fixture()
        rows.insert(-1, row(8, 'mov', 'dil, 0x1'))
        self.assertEqual(self.recover(rows), [])

    def test_overlapping_stack_store(self):
        rows = self.fixture()
        rows.insert(-1, row(8, 'mov', 'byte ptr [rsp], 0x0'))
        self.assertEqual(self.recover(rows), [])

    def test_implicit_clobber(self):
        rows = self.fixture()
        rows.insert(-1, row(8, 'mul', 'rcx'))
        self.assertEqual(self.recover(rows), [])


if __name__ == '__main__':
    unittest.main()
