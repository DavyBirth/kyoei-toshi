import struct
import unittest
from inspect_elf_references import references


def fixture():
    data=bytearray(320)
    struct.pack_into('<16sHHIQQQIHHHHHH',data,0,b'\x7fELF\x02\x01\x01'+bytes(9),
        2,62,1,0x5000,64,0,0,64,56,2,0,0,0)
    struct.pack_into('<IIQQQQQQ',data,64,1,5,192,0x5000,0,32,32,1)
    struct.pack_into('<IIQQQQQQ',data,120,1,6,256,0x9000,0,64,64,1)
    data[192:199]=b'\x48\x8d\x35'+struct.pack('<i',0x9000-0x5007)
    data[199:204]=b'\xe8'+struct.pack('<i',0x6000-0x500c)
    data[256:267]=b'font/test\0\0'
    return bytes(data)


class ElfReferences(unittest.TestCase):
    def test_address_mapping_and_requested_calls(self):
        report=references(fixture(),['font/test'],[0x6000])
        self.assertEqual(report['labels'][0]['address'],'0x9000')
        self.assertEqual([(r['address'],r['target']) for r in report['references']],
                         [('0x5000','0x9000'),('0x5007','0x6000')])
        self.assertFalse(report['instruction_boundaries_verified'])

    def test_invalid_segments_and_architecture(self):
        for offset,value in ((18,183),(64+32,500),(64+40,1)):
            data=bytearray(fixture())
            struct.pack_into('<H' if offset==18 else '<Q',data,offset,value)
            with self.assertRaises(ValueError): references(data,['font/test'])

    def test_missing_label_and_truncated_input(self):
        self.assertEqual(references(fixture(),['absent'])['references'],[])
        with self.assertRaises(ValueError): references(b'ELF',['font/test'])


if __name__=='__main__': unittest.main()
