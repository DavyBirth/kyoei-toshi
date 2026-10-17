import hashlib
import json
import mmap
from pathlib import Path
import tempfile
import unittest
import sys
from unittest.mock import patch

if sys.platform == 'linux':
    from apply_runtime_update_linux import apply, require_closed


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

    def fake_process(self, command, comm):
        proc = self.root/'proc'
        entry = proc/'123'
        entry.mkdir(parents=True)
        (entry/'cmdline').write_bytes(command)
        (entry/'comm').write_text(comm)
        return proc, entry

    def check_unreadable_maps(self, command, comm, blocked):
        proc, entry = self.fake_process(command, comm)
        original = Path.read_text
        def read(path, *args, **kwargs):
            if path == entry/'maps':
                raise PermissionError('ptrace denied')
            return original(path, *args, **kwargs)
        with patch.object(Path, 'read_text', read):
            if blocked:
                with self.assertRaises(RuntimeError): require_closed(self.root, proc)
            else:
                require_closed(self.root, proc)

    def test_unrelated_protected_process_is_ignored(self):
        self.check_unreadable_maps(b'/usr/bin/kitten\0run-shell\0', 'kitten', False)

    def test_protected_wine_is_refused(self):
        self.check_unreadable_maps(b'/usr/bin/wine64-preloader\0', 'wine64-preload', True)

    def test_protected_windows_process_is_refused(self):
        self.check_unreadable_maps(b'Z:\\games\\kyoei.exe\0', 'kyoei.exe', True)

    def test_unknown_protected_process_is_refused(self):
        self.check_unreadable_maps(b'', 'unknown', True)

    def test_unreadable_identity_is_refused(self):
        proc, entry = self.fake_process(b'/usr/bin/kitten\0', 'kitten')
        original = Path.read_text
        def read(path, *args, **kwargs):
            if path in (entry/'maps', entry/'comm'):
                raise PermissionError('denied')
            return original(path, *args, **kwargs)
        with patch.object(Path, 'read_text', read):
            with self.assertRaises(RuntimeError): require_closed(self.root, proc)

    def test_escaped_mapping_and_exact_path(self):
        proc, entry = self.fake_process(b'wine\0', 'wine')
        target = str((self.root/'City Shrouded in Shadow.exe').resolve()).replace(' ', r'\040')
        (entry/'maps').write_text('1000-2000 r--p 00000000 00:00 1 '+target+' (deleted)\n')
        with self.assertRaises(RuntimeError): require_closed(self.root, proc)
        (entry/'maps').write_text('1000-2000 r--p 00000000 00:00 1 '+target+'.other\n')
        require_closed(self.root, proc)

    def test_process_exiting_during_check_is_ignored(self):
        proc, entry = self.fake_process(b'wine\0', 'wine')
        require_closed(self.root, proc)  # maps vanished before the read


if __name__ == '__main__':
    unittest.main()
