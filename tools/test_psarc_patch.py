"""Provas do reinseridor PSARC com arquivos inteiramente sintéticos."""
import hashlib
import tempfile
import unittest
from pathlib import Path
from psarc import Psarc
from psarc_patch import patch
from test_psarc import fixture


class PatchTest(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        self.source = self.root / 'source.psarc'
        self.output = self.root / 'output.psarc'
        self.files = [('csv/Text.csh', b'original'), ('lua/Scene.luc', b'A' * 90000), ('empty.bin', b'')]
        self.source.write_bytes(fixture(self.files))

    def test_unchanged_reinsertion_is_byte_identical(self):
        report = patch(self.source, self.output, dict(self.files))
        self.assertTrue(report['identical_archive'])
        self.assertEqual(self.output.read_bytes(), self.source.read_bytes())

    def test_growth_multiblock_shrink_empty_and_unrelated_preservation(self):
        random = b''.join(hashlib.sha256(str(i).encode()).digest() for i in range(2048))
        changed = {'csv/Text.csh': random + 'Ação, café, ç e ã'.encode()*8000, 'lua/Scene.luc': b''}
        report = patch(self.source, self.output, changed)
        self.assertEqual(report['changed_resources'], 2)
        with Psarc(self.output) as archive:
            actual = {e.name: b''.join(archive.chunks(e)) for e in archive.entries[1:]}
        self.assertEqual(actual, dict(self.files) | changed)
        self.assertFalse(report['identical_archive'])

    def test_source_hash_mismatch_rejected(self):
        with self.assertRaises(ValueError):
            patch(self.source, self.output, {}, 'wrong')

    def test_unknown_resource_and_overwrite_rejected(self):
        with self.assertRaises(ValueError):
            patch(self.source, self.output, {'missing.csh': b'bad'})
        with self.assertRaises(ValueError):
            patch(self.source, self.source, {})
        self.output.write_bytes(b'existing')
        with self.assertRaises(ValueError):
            patch(self.source, self.output, {})
        self.assertEqual(self.output.read_bytes(), b'existing')


if __name__ == '__main__':
    unittest.main()
