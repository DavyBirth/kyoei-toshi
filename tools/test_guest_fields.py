"""Synthetic pointer/CFG tests, including same-offset objects and ABI clobbers."""
import unittest
from trace_guest_fields import analyze


def code(lines):
    rows = []
    for address, opcode, operands, rip in lines:
        direct = None
        if opcode in ('jmp', 'call') and operands.startswith('0x'):
            direct = int(operands, 0)
        rows.append((address, 1, opcode, operands, rip, direct))
    return rows


class GuestFieldTests(unittest.TestCase):
    def test_global_slot_dereference_and_three_operand_read(self):
        rows = code([(1, 'lea', 'rax, [rip + 0x100]', 0x2000),
                     (2, 'mov', 'rcx, qword ptr [rax]', None),
                     (3, 'vmulss', 'xmm0, xmm1, dword ptr [rcx + 0xf0]', None)])
        result = analyze(rows, {0x2000: 'app'})
        self.assertEqual(result['fields'], [dict(site='0x3', object='app', offset='0xf0', access='read', opcode='vmulss')])

    def test_different_objects_with_equal_offsets_do_not_merge(self):
        rows = code([(1, 'mov', 'rax, qword ptr [rip + 0x100]', 0x2000),
                     (2, 'mov', 'dword ptr [rax + 0xe8], 0x0', None),
                     (3, 'mov', 'rax, qword ptr [rsi]', None),
                     (4, 'mov', 'dword ptr [rax + 0xe8], 0x1', None)])
        result = analyze(rows, {0x2000: 'app'})
        self.assertEqual(len(result['fields']), 1)
        self.assertEqual(result['fields'][0]['immediate'], '0x0')

    def test_sysv_call_kills_volatile_pointers_but_keeps_rbx(self):
        rows = code([(1, 'mov', 'rax, qword ptr [rip]', 0x2000),
                     (2, 'mov', 'rbx, rax', None),
                     (3, 'call', '0x1000', None),
                     (4, 'mov', 'dword ptr [rax + 0xe8], 0x1', None),
                     (5, 'mov', 'dword ptr [rbx + 0xe8], 0x0', None)])
        result = analyze(rows, {0x2000: 'app'})
        self.assertEqual([r['site'] for r in result['fields']], ['0x5'])

    def test_cfg_join_removes_conflicting_provenance(self):
        rows = code([(1, 'mov', 'rax, qword ptr [rip]', 0x2000),
                     (2, 'jne', '0x5', None),
                     (3, 'mov', 'rax, qword ptr [rsi]', None),
                     (4, 'jmp', '0x6', None),
                     (5, 'nop', '', None),
                     (6, 'mov', 'dword ptr [rax + 0xe8], 0x0', None)])
        self.assertEqual(analyze(rows, {0x2000: 'app'})['fields'], [])

    def test_cfg_join_preserves_equal_provenance(self):
        rows = code([(1, 'mov', 'rbx, qword ptr [rip]', 0x2000),
                     (2, 'jne', '0x5', None),
                     (3, 'nop', '', None),
                     (4, 'jmp', '0x6', None),
                     (5, 'nop', '', None),
                     (6, 'mov', 'dword ptr [rbx + 0xe8], 0x0', None)])
        self.assertEqual(len(analyze(rows, {0x2000: 'app'})['fields']), 1)

    def test_loop_fixpoint_removes_alias_invalidated_on_backedge(self):
        rows = code([(1, 'mov', 'rax, qword ptr [rip]', 0x2000),
                     (2, 'mov', 'ecx, dword ptr [rax + 0xe8]', None),
                     (3, 'xor', 'eax, eax', None),
                     (4, 'jne', '0x2', None), (5, 'ret', '', None)])
        self.assertEqual(analyze(rows, {0x2000: 'app'})['fields'], [])

    def test_unreachable_code_after_trap_is_not_attributed(self):
        rows = code([(1, 'ud2', '', None),
                     (2, 'mov', 'dword ptr [rdi + 0xe8], 0x0', None)])
        self.assertEqual(analyze(rows, {}, {'rdi': ('object', 'app', 0)})['fields'], [])

    def test_unconfigured_switch_is_not_followed(self):
        rows = code([(1, 'mov', 'rbx, rdi', None), (2, 'jmp', 'rax', None),
                     (3, 'mov', 'dword ptr [rbx + 0xe8], 0x0', None)])
        initial = {'rdi': ('object', 'app', 0)}
        self.assertEqual(analyze(rows, {}, initial)['fields'], [])
        result = analyze(rows, {}, initial, {2: [3]})
        self.assertEqual(len(result['fields']), 1)

    def test_partial_register_write_invalidates_pointer(self):
        rows = code([(1, 'mov', 'rax, rdi', None), (2, 'xor', 'eax, eax', None),
                     (3, 'mov', 'dword ptr [rax + 0xe8], 0x0', None)])
        self.assertEqual(analyze(rows, {}, {'rdi': ('object', 'app', 0)})['fields'], [])
        for opcode, operand in [('mov', 'al, 0x0'), ('xor', 'ah, ah'), ('div', 'rsi'), ('xchg', 'rax, rbx')]:
            changed = code([(1, 'mov', 'rax, rdi', None), (2, opcode, operand, None),
                            (3, 'mov', 'dword ptr [rax + 0xe8], 0x0', None)])
            self.assertEqual(analyze(changed, {}, {'rdi': ('object', 'app', 0)})['fields'], [])

    def test_atomic_and_string_prefixes_do_not_preserve_clobbered_aliases(self):
        for opcode, operands in [('lock', 'cmpxchg qword ptr [rsi], rdx'), ('rep', 'stosq')]:
            rows = code([(1, 'mov', 'rax, rdi', None), (2, opcode, operands, None),
                         (3, 'mov', 'dword ptr [rax + 0xe8], 0x0', None)])
            self.assertEqual(analyze(rows, {}, {'rdi': ('object', 'app', 0)})['fields'], [])

    def test_virtual_slot_is_identified_without_claiming_target(self):
        rows = code([(1, 'mov', 'rax, qword ptr [rdi]', None),
                     (2, 'call', 'qword ptr [rax + 0x60]', None)])
        result = analyze(rows, {}, {'rdi': ('object', 'app', 0)})
        self.assertEqual(result['virtual_calls'], [dict(site='0x2', object='app', slot='0x60', receiver_runtime_verified=False)])

    def test_hidden_return_receiver_can_be_rsi(self):
        rows = code([(1, 'mov', 'rbx, rsi', None),
                     (2, 'mov', 'dword ptr [rbx + 0xe8], 0x0', None)])
        self.assertEqual(len(analyze(rows, {}, {'rsi': ('object', 'app', 0)})['fields']), 1)

    def test_interior_pointer_and_unmodeled_indexed_address(self):
        rows = code([(1, 'lea', 'rbx, [rdi + 0xe0]', None),
                     (2, 'mov', 'dword ptr [rbx + 0x8], 0x0', None),
                     (3, 'mov', 'dword ptr [rbx + 4*rcx + 0x8], 0x1', None)])
        result = analyze(rows, {}, {'rdi': ('object', 'app', 0)})
        self.assertEqual(len(result['fields']), 2)  # Address escape plus direct write.
        self.assertEqual(result['fields'][1]['offset'], '0xe8')


if __name__ == '__main__':
    unittest.main()
