import tempfile
import unittest
import types
from pathlib import Path
from prepare_portable_image import validate_output,stage_paths,validate_prepared_stages,STAGE_NAMES,stage_linker

class PortableImage(unittest.TestCase):
    def test_never_overwrites_inputs_or_an_existing_output(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);game=root/'game';firmware=root/'firmware'
            game.mkdir();firmware.mkdir();existing=root/'existing';existing.mkdir()
            for out in (game,game/'runtime',firmware/'out',existing):
                with self.assertRaises(ValueError):validate_output(game,firmware,out)
            self.assertEqual(validate_output(game,firmware,root/'new')[2],root/'new')

    def test_missing_later_stage_is_rejected_before_preparation(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory)
            for name in STAGE_NAMES:
                path=root/'module/MasterORBIS'/name/'StageModule.prx'
                path.parent.mkdir(parents=True);path.touch()
            self.assertEqual(len(stage_paths(root)),19)
            (root/'module/MasterORBIS/stage218/StageModule.prx').unlink()
            with self.assertRaisesRegex(ValueError,'stage218'):stage_paths(root)

    def test_prepared_image_requires_every_stage_once(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);table=root/'deferred.tsv'
            rows=[f'M\t/app0/module/MasterORBIS/{name}/StageModule.prx\t1000\t1000' for name in STAGE_NAMES]
            table.write_text('\n'.join(rows),encoding='utf8')
            self.assertEqual(validate_prepared_stages(root),19)
            for changed in (rows[:1],rows[:-1],rows+[rows[0]]):
                table.write_text('\n'.join(changed),encoding='utf8')
                with self.assertRaisesRegex(ValueError,'complete stage set'):validate_prepared_stages(root)

    def test_only_stage_to_stage_duplicate_exports_are_allowed(self):
        with tempfile.TemporaryDirectory() as directory:
            source=Path(directory)/'linker.py'
            source.write_text('''from pathlib import Path
def check(files):
    exports={}
    for filename in files:
        for symbols in [[dict(identity='shared')]]:
            for s in symbols:
                if s['identity'] in exports:
                    raise ValueError(f'{filename}: ambiguous export')
                exports[s['identity']] = (0,1,filename)
    return exports
''',encoding='utf8')
            adapted=stage_linker(types.SimpleNamespace(__file__=str(source)))
            first='module/MasterORBIS/stage201/StageModule.prx'
            second='module/MasterORBIS/stage218/StageModule.prx'
            self.assertEqual(adapted.check([first,second])['shared'][2],first)
            for files in ([first,'libc.prx'],['libc.prx',second]):
                with self.assertRaisesRegex(ValueError,'ambiguous export'):adapted.check(files)

if __name__=='__main__':unittest.main()
