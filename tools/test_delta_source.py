import tempfile
import unittest
from pathlib import Path
from psarc import Psarc,DeltaSource
from psarc_delta import build,verify
from test_psarc import fixture

class DeltaArchive(unittest.TestCase):
    def test_nested_readers_have_independent_cursors_and_leave_base_intact(self):
        raw=fixture([('font/test.csh',b'original'),('texture/test.tex',b'unchanged')])
        with tempfile.TemporaryDirectory() as directory:
            base=Path(directory)/'common.psarc';base.write_bytes(raw)
            changed={'font/test.csh':b'translated'}
            delta,_=build(base,changed);verify(base,delta,changed)
            source=DeltaSource(base,delta)
            with Psarc(source) as first,Psarc(source) as second:
                for archive in (first,second):
                    entries={e.name:e for e in archive.entries[1:]}
                    self.assertEqual(b''.join(archive.chunks(entries['font/test.csh'])),b'translated')
                    self.assertEqual(b''.join(archive.chunks(entries['texture/test.tex'])),b'unchanged')
            self.assertEqual(base.read_bytes(),raw)

if __name__=='__main__':unittest.main()
