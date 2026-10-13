import unittest
from measure_translation_layout import measure


class Layout(unittest.TestCase):
    glyphs={char:dict(advance=advance) for char,advance in [('W',38),('i',8),(' ',13),('à',21)]}
    def test_variable_width_and_lines(self):
        report=measure('Wi\niii',self.glyphs)
        self.assertEqual(report['known_line_advances'],[46,24])
        self.assertEqual(report['max_static_advance'],46)
        self.assertTrue(report['complete_static_measurement'])

    def test_styles_and_dynamic_content(self):
        self.assertTrue(measure('[#ff44ff44]Wi[/#]',self.glyphs)['complete_static_measurement'])
        report=measure('<P_FNAME> Wi [i10]',self.glyphs)
        self.assertFalse(report['complete_static_measurement'])
        self.assertEqual(report['unresolved_controls'],['<P_FNAME>','[i10]'])

    def test_missing_glyph_not_hidden(self):
        report=measure('àW',{'W':dict(advance=38)})
        self.assertFalse(report['complete_static_measurement'])
        self.assertEqual(report['missing_glyphs'],['à'])
        self.assertEqual(measure('Wi\r\ni',self.glyphs)['lines'],2)


if __name__=='__main__': unittest.main()
