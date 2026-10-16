"""Exercise updater preflight, replacement, rollback backup and save preservation."""
import hashlib
import json
import os
from pathlib import Path
import shutil
import subprocess
import tempfile
import unittest

ROOT = Path(__file__).resolve().parents[1]


@unittest.skipUnless(os.name == 'nt', 'Windows updater')
class UpdateTests(unittest.TestCase):
    def setUp(self):
        self.temp = tempfile.TemporaryDirectory(dir=ROOT/'build')
        self.addCleanup(self.temp.cleanup)
        self.root = Path(self.temp.name)
        (self.root/'runtime').mkdir()
        (self.root/'data').mkdir()
        (self.root/'data/save.bin').write_bytes(b'unique save')
        self.target = self.root/'runtime/kyoei.exe'
        self.target.write_bytes(b'old runtime')
        self.package = self.root/'Atualizacao-Kyoei'
        self.package.mkdir()
        shutil.copyfile(ROOT/'scripts/apply_runtime_update.ps1',self.package/'apply_runtime_update.ps1')
        (self.package/'kyoei.exe').write_bytes(b'new runtime')
        self.old = hashlib.sha256(b'old runtime').hexdigest()
        (self.root/'manifest.json').write_text(json.dumps(dict(title='CUSA03562',version='01.03')))
        (self.package/'update.json').write_text(json.dumps(dict(schema=1,platform='windows-x64',
            title='CUSA03562',version='01.03',sha256=hashlib.sha256(b'new runtime').hexdigest(),
            accepted_sha256=[self.old])))

    def run_update(self,*args):
        return subprocess.run(['powershell','-NoProfile','-ExecutionPolicy','Bypass','-File',
            str(self.package/'apply_runtime_update.ps1'),'-GamePath',str(self.root),*args],capture_output=True)

    def test_apply_and_idempotence(self):
        result=self.run_update('-CheckOnly')
        self.assertEqual(result.returncode,0,result.stderr)
        self.assertEqual(self.target.read_bytes(),b'old runtime')
        result=self.run_update()
        self.assertEqual(result.returncode,0,result.stderr)
        self.assertEqual(self.target.read_bytes(),b'new runtime')
        self.assertEqual((self.root/'data/update-backups'/f'{self.old}.exe').read_bytes(),b'old runtime')
        self.assertEqual((self.root/'data/save.bin').read_bytes(),b'unique save')
        self.assertEqual(self.run_update().returncode,0)

    def test_wrong_installed_runtime(self):
        self.target.write_bytes(b'other runtime')
        self.assertNotEqual(self.run_update().returncode,0)
        self.assertEqual(self.target.read_bytes(),b'other runtime')
        self.assertFalse((self.root/'data/update-backups').exists())

    def test_damaged_payload(self):
        (self.package/'kyoei.exe').write_bytes(b'damaged')
        self.assertNotEqual(self.run_update().returncode,0)
        self.assertEqual(self.target.read_bytes(),b'old runtime')


if __name__ == '__main__':
    unittest.main()
