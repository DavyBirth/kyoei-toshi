import hashlib
from pathlib import Path
import tempfile
import unittest
from bootstrap_portable import commit,copy_owned_game,texture_sources

class Bootstrap(unittest.TestCase):
    def test_copies_only_verified_owned_inputs_not_profiles(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);source=root/'dump';source.mkdir()
            (source/'eboot.bin').write_bytes(b'owned synthetic fixture')
            (source/'save.bin').write_bytes(b'private progress')
            record=dict(path='eboot.bin',bytes=23,sha256=hashlib.sha256(b'owned synthetic fixture').hexdigest())
            target=root/'game';copy_owned_game(source,target,dict(files=[record]))
            self.assertEqual((target/'eboot.bin').read_bytes(),(source/'eboot.bin').read_bytes())
            self.assertFalse((target/'save.bin').exists())
            with self.assertRaises(ValueError):copy_owned_game(source,target,dict(files=[record]))
    def test_bad_input_hash_does_not_create_destination(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);source=root/'dump';source.mkdir();(source/'file').write_bytes(b'bad')
            with self.assertRaises(ValueError):
                copy_owned_game(source,root/'new',dict(files=[dict(path='file',bytes=3,sha256='wrong')]))
            self.assertFalse((root/'new').exists())
    def test_extracts_cross_referenced_texture_sources(self):
        table=dict(textures=[dict(resource='texture/main.tex',regions=[dict(plate='texture/plate.tex',
                    graft=dict(res='texture/graft.tex'),paste=dict(res='texture/paste.tex'))])])
        self.assertEqual(texture_sources(table),{'texture/main.tex','texture/plate.tex','texture/graft.tex','texture/paste.tex'})
    def test_failed_commit_undoes_partial_install_and_keeps_profile(self):
        with tempfile.TemporaryDirectory() as directory:
            base=Path(directory);stage=base/'stage';root=base/'app'
            (stage/'game').mkdir(parents=True);(stage/'game/eboot.bin').write_bytes(b'x')
            for language in ('en','pt-BR','ja'):
                (stage/'languages'/language).mkdir(parents=True);(stage/'languages'/language/'prompts.kypack').write_bytes(b'p')
            (stage/'runtime/out').mkdir(parents=True);(stage/'runtime/out/boot-linked.bin').write_bytes(b'b')
            assets=stage/'assets';assets.mkdir();(assets/'hud-metrics.bin').write_bytes(b'm')
            (root/'data/user').mkdir(parents=True);(root/'data/user/save.dat').write_bytes(b'progress')
            (root/'assets').mkdir();(root/'assets/hud-metrics.bin').write_bytes(b'old')
            with self.assertRaises(FileNotFoundError):commit(stage,root,assets,{},dict(files=[]))
            for path in ('game','languages/en','languages/ja','runtime/out','setup-manifest.json','manifest.json'):
                self.assertFalse((root/path).exists(),path)
            self.assertEqual((root/'data/user/save.dat').read_bytes(),b'progress')
            self.assertEqual((root/'assets/hud-metrics.bin').read_bytes(),b'old')

if __name__=='__main__':unittest.main()
