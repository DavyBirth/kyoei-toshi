"""Protect engine formatting without depending on original game text."""
import unittest
from prepare_translation_catalog import TOKENS

class Controls(unittest.TestCase):
    def test_name_color_and_button_references(self):
        original='<P_LNAME><P_FNAME> [#ff44ff44]label[/#] [i10]/[i11] %02d {item}'
        translated='<P_LNAME> <P_FNAME> [#ff44ff44]translated[/#] [i10]/[i11] %02d {item}'
        self.assertEqual(TOKENS.findall(original),TOKENS.findall(translated))
        expected=['<P_LNAME>','<P_FNAME>','[#ff44ff44]','[/#]','[i10]','[i11]','%02d','{item}']
        self.assertEqual(TOKENS.findall(original),expected)
    def test_deleted_or_changed_control_differs(self):
        source='[#ff44ff44]text[/#] [i10]'
        for value in ('text [i10]','[#ff44ff44]text [i10]','[#ff44ff44]text[/#] [i11]'):
            self.assertNotEqual(TOKENS.findall(source),TOKENS.findall(value))

if __name__=='__main__':unittest.main()
