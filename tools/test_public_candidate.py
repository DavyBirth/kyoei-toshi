import tempfile
from pathlib import Path
import unittest
from build_public_candidate import ROOT,audit,tool_closure

class PublicCandidate(unittest.TestCase):
    def test_rejects_game_derivatives_profiles_and_nonportable_python(self):
        for name in ('game/eboot.bin','languages/en/translation.kydelta','data/user/save.dat',
                     'runtime/out/boot-linked.bin','original.psarc','ui.kyart','support/python/pyvenv.cfg'):
            with self.subTest(name=name),tempfile.TemporaryDirectory() as directory:
                root=Path(directory);path=root/name;path.parent.mkdir(parents=True,exist_ok=True);path.write_bytes(b'fixture')
                with self.assertRaises(ValueError):audit(root)
    def test_hashes_public_inputs(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);(root/'LICENSE').write_bytes(b'abc')
            row=audit(root)[0]
            self.assertEqual(row['sha256'],'ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad')
    def test_production_closure_omits_private_preparer_and_uses_explicit_inputs(self):
        tools=tool_closure(ROOT)
        self.assertNotIn('prepare_native',tools)
        self.assertIn('native_module_metadata',tools)
        for name in tools:
            text=(ROOT/'tools'/(name+'.py')).read_text(encoding='utf8')
            for private in ('G:/kyoei-toshi-work','G:/GodzillaShadPS4PC'):
                self.assertNotIn(private,text,name)
    def test_follows_transitive_and_function_local_imports(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);(root/'tools').mkdir()
            for name,code in {'a':'def run():\n import b\n','b':'from c import value\n','c':'value=1\n'}.items():
                (root/'tools'/(name+'.py')).write_text(code)
            self.assertEqual(tool_closure(root,['a']),['a','b','c'])

if __name__=='__main__':unittest.main()
