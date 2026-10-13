import unittest
from analyze_raster_trace import analyze

CAPTURE='''Kyoei raster capture begin: id=1 frames=2 draws=12 profiles=1 dropped=0
Kyoei raster draw: id=1 p=0 repeats=12 target=abc vs=10 ps=20 primitive=4 blend=00000001 depth=0
Kyoei raster viewport: p=0 index=0 xywh=0,0,1920,1080 scissor_xywh=153,0,1767,1080 offset=153,0 disable_window=0 disable_generic=1 viewport_scissor=0
Kyoei raster bounds: p=0 screen=0,0,1920,1080 window=0,0,1920,1080 generic=0,0,1920,1080 viewport=0,0,1920,1080
Kyoei raster capture end: id=1
'''


class CaptureReports(unittest.TestCase):
    def test_left_cut_and_state(self):
        row=analyze(CAPTURE)['captures'][0]
        self.assertTrue(row['complete'])
        self.assertAlmostEqual(row['left_cut_candidates'][0]['left_cut_fraction'],153/1920)
        self.assertEqual(row['profiles'][0]['disable_generic'],'1')

    def test_truncated_and_dropped_remain_visible(self):
        row=analyze(CAPTURE.rsplit('Kyoei raster capture end',1)[0].replace('dropped=0','dropped=40'))['captures'][0]
        self.assertFalse(row['complete']);self.assertEqual(row['dropped'],40)

    def test_full_coverage_not_flagged(self):
        row=analyze(CAPTURE.replace('153,0,1767,1080','0,0,1920,1080'))['captures'][0]
        self.assertEqual(row['left_cut_candidates'],[])

    def test_unrelated_log_not_copied(self):
        self.assertEqual(analyze('Guest dialogue: irrelevant\n')['captures'],[])

    def test_multiple_captures_separate(self):
        rows=analyze(CAPTURE+CAPTURE.replace('id=1','id=2'))['captures']
        self.assertEqual([r['id'] for r in rows],[1,2]);self.assertTrue(all(r['complete'] for r in rows))

    def test_missing_bounds_not_complete(self):
        text='\n'.join(line for line in CAPTURE.splitlines() if 'raster bounds:' not in line)
        self.assertFalse(analyze(text)['captures'][0]['complete'])


if __name__=='__main__':unittest.main()
