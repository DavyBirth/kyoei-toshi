import struct
import unittest
import zlib
from english_typography import native_english
from build_ui_corrections import hud_labels

class Corrections(unittest.TestCase):
    def test_contractions_and_quotes_use_latin_glyphs(self):
        self.assertEqual(native_english('I\u2019m okay. \u201cLet\u2019s go!\u201d'), 'I\'m okay. "Let\'s go!"')
    def test_label_slots_keep_size_and_surrounding_bytes(self):
        label='\u5dee\u51fa\u4eba\uff1a'.encode()
        suffix=bytes.fromhex('6722e131')+b'suffix'
        raw=b'prefix'+struct.pack('<I',len(label))+label+suffix
        data=b'x'*128+struct.pack('>4sIII',b'BILZ',len(raw),len(zlib.compress(raw)),123)+zlib.compress(raw)
        out,count=hud_labels(data);result=zlib.decompress(out[144:])
        self.assertEqual(count,1);self.assertEqual(len(result),len(raw))
        self.assertEqual(result,b'prefix'+struct.pack('<I',len(label))+b'From:'.ljust(len(label),b' ')+suffix)
        self.assertEqual(out[:128],data[:128]);self.assertEqual(struct.unpack_from('>I',out,140)[0],123)
    def test_embedded_names_are_not_replaced(self):
        value='\u8fd4\u4fe1_NPC'.encode()
        raw=struct.pack('<I',len(value))+value
        packed=zlib.compress(raw)
        data=b'x'*128+struct.pack('>4sIII',b'BILZ',len(raw),len(packed),0)+packed
        self.assertEqual(hud_labels(data),(data,0))
if __name__=='__main__':unittest.main()
