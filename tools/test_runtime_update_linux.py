import hashlib
import json
import mmap
from pathlib import Path
import tempfile
import unittest
import sys

if sys.platform == 'linux':
    from apply_runtime_update_linux import apply


@unittest.skipUnless(sys.platform == 'linux', 'Linux updater')
class UpdateTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory()
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        (self.root/'runtime').mkdir()
        (self.root/'data').mkdir()
        self.save = self.root/'data/save.bin'
        self.save.write_bytes(b'unique save')
        self.target = self.root/'runtime/kyoei.exe'
        self.target.write_bytes(b'old runtime')
        self.target.chmod(0o755)
        self.package = self.root/'Atualizacao-Kyoei'
        self.package.mkdir()
        self.payload = self.package/'kyoei.exe'
        self.payload.write_bytes(b'new runtime')
        self.old = hashlib.sha256(b'old runtime').hexdigest()
        (self.root/'manifest.json').write_text(json.dumps(dict(title='CUSA03562',version='01.03')))
        (self.package/'update.json').write_text(json.dumps(dict(schema=1,platform='windows-x64',
            title='CUSA03562',version='01.03',sha256=hashlib.sha256(b'new runtime').hexdigest(),
            accepted_sha256=[self.old])))

    def test_apply_backup_and_idempotence(self):
        apply(self.package,self.root,True)
        self.assertEqual(self.target.read_bytes(),b'old runtime')
        apply(self.package,self.root)
        self.assertEqual(self.target.read_bytes(),b'new runtime')
        self.assertEqual((self.root/'data/update-backups'/f'{self.old}.exe').read_bytes(),b'old runtime')
        self.assertEqual(self.save.read_bytes(),b'unique save')
        self.assertEqual(self.target.stat().st_mode & 0o777,0o755)
        apply(self.package,self.root)

    def test_bad_payload(self):
        self.payload.write_bytes(b'corrupt')
        with self.assertRaises(ValueError): apply(self.package,self.root)
        self.assertEqual(self.target.read_bytes(),b'old runtime')

    def test_wrong_base(self):
        self.target.write_bytes(b'wrong base')
        with self.assertRaises(ValueError): apply(self.package,self.root)
        self.assertEqual(self.target.read_bytes(),b'wrong base')

    def test_mapped_runtime_is_refused(self):
        with self.target.open('rb') as source, mmap.mmap(source.fileno(),0,access=mmap.ACCESS_READ):
            with self.assertRaises(RuntimeError): apply(self.package,self.root)
        self.assertEqual(self.target.read_bytes(),b'old runtime')


if __name__ == '__main__':
    unittest.main()
