"""Synthetic R8 atlas/table regression; contains no original game assets."""
import struct
import unittest
import zlib
from localize_font import extend_intermission
from check_game_font_tables import glyph_rows
from tex_preview import unpack_tex


def resource(payload,magic=b'\0hsc'):
    header=magic+struct.pack('>I',2)+bytes(120)
    packed=zlib.compress(payload)
    return header+struct.pack('>4sIII',b'BILZ',len(payload),len(packed),0)+packed


def table(rows):
    cells=bytearray();pool=bytearray();start=12+len(rows)*72
    for char,page,x,y in rows:
        values=[char.encode().ljust(4,b'\0'),struct.pack('>I',page),struct.pack('>I',x),struct.pack('>I',y)]
        values += [struct.pack('>f',v) for v in (2,2,0,0,2)]
        for i,raw in enumerate(values):
            cells+=struct.pack('>II',start+len(pool),0x40000001 if i<4 else 0x80000001);pool+=raw
    return resource(struct.pack('>III',1,9,len(rows))+cells+pool)


def atlas(occupied=False):
    pixels=bytearray(32*32);pixels[0:2]=b'\xff\x80';pixels[32:34]=b'\x40\x20'
    if occupied:pixels[6*32+4]=255
    descriptor=struct.pack('<HH',32,32)+bytes.fromhex('1241301100000000')
    return resource(descriptor+pixels,b'\0xet')


class FontLocalization(unittest.TestCase):
    def inputs(self,occupied=False):
        return {'font/utf8all_table.csh':table([('\u00e9',0,0,0)]),
                'font/utf8all_IM_table.csh':table([('A',2,0,0)]),
                'font/utf8all_0.tex':atlas(), 'font/utf8all_IM_2.tex':atlas(occupied)}

    def test_preserves_existing_rows_and_pixels_and_copies_missing_glyph(self):
        data=self.inputs();result=extend_intermission(data.__getitem__,{'A','\u00e9'})
        before=glyph_rows(data['font/utf8all_IM_table.csh'])[0]
        after=glyph_rows(result['font/utf8all_IM_table.csh'])[0]
        self.assertEqual(before['A'],after['A'])
        self.assertEqual(set(after),{'A','\u00e9'})
        glyph=after['\u00e9'];self.assertEqual((glyph['page'],glyph['x'],glyph['y']),(2,4,6))
        old=unpack_tex(data['font/utf8all_IM_2.tex'])[1]
        new=unpack_tex(result['font/utf8all_IM_2.tex'])[1]
        allowed={6*32+4,6*32+5,7*32+4,7*32+5}
        self.assertEqual(new[6*32+4:6*32+6],b'\xff\x80')
        self.assertEqual(new[7*32+4:7*32+6],b'\x40\x20')
        self.assertTrue(all(a==b for i,(a,b) in enumerate(zip(old,new)) if i not in allowed))
        self.assertEqual(extend_intermission({**data,**result}.__getitem__,{'A','\u00e9'}),{})

    def test_refuses_overwriting_pixels_or_missing_donor(self):
        data=self.inputs(True)
        with self.assertRaisesRegex(ValueError,'not empty'):extend_intermission(data.__getitem__,{'\u00e9'})
        with self.assertRaisesRegex(ValueError,'absent'):extend_intermission(data.__getitem__,{'\u00e7'})


if __name__=='__main__':unittest.main()
