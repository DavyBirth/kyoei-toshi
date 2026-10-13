"""Synthetic texture fixtures: no game assets."""
import struct
import unittest
import zlib
from tex_preview import preview,unpack_tex

def fixture(width,height,channel,format_,body):
    header=bytearray(128);header[:4]=b'\0xet';struct.pack_into('>I',header,4,2)
    payload=struct.pack('<HH4B4x',width,height,0x12,0x41,channel,format_)+body
    packed=zlib.compress(payload)
    return bytes(header)+struct.pack('>4sIII',b'BILZ',len(payload),len(packed),7)+packed

class Preview(unittest.TestCase):
    def test_bc3_red(self):
        block=bytes([255,255])+bytes(6)+struct.pack('<HHI',0xf800,0,0)
        image,info=preview(fixture(4,4,0x20,0x1d,block))
        self.assertEqual(image.getpixel((0,0)),(255,0,0,255));self.assertEqual(info['decoded_format'],'bc3')
    def test_rgba_order_and_alpha(self):
        image,info=preview(fixture(2,1,0,0x11,bytes([255,0,0,128,0,255,0,255])))
        self.assertEqual(image.getpixel((0,0)),(255,0,0,128));self.assertEqual(image.getpixel((1,0)),(0,255,0,255))
        self.assertEqual(info['decoded_format'],'rgba8')
    def test_r8_mask(self):
        image,info=preview(fixture(2,1,0x30,0x11,bytes([0,255])))
        self.assertEqual(image.getpixel((0,0)),(255,255,255,0));self.assertEqual(image.getpixel((1,0)),(255,255,255,255))
        self.assertEqual(info['decoded_format'],'r8_mask')
    def test_truncated_and_unknown_format(self):
        for data in (fixture(8,8,0x20,0x1d,b''),fixture(2,1,0,0x11,b'123'),fixture(2,1,0x30,0x11,b'1'),fixture(4,4,0x20,0x1b,bytes(16))):
            with self.assertRaises(ValueError): preview(data)
    def test_trailing_stream_rejected(self):
        data=bytearray(fixture(2,1,0x30,0x11,b'12'));data.extend(b'junk');struct.pack_into('>I',data,136,len(data)-144)
        with self.assertRaises(ValueError):unpack_tex(bytes(data))

if __name__=='__main__': unittest.main()
