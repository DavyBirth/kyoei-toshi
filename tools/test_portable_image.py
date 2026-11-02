import tempfile
import unittest
from pathlib import Path
from prepare_portable_image import validate_output

class PortableImage(unittest.TestCase):
    def test_never_overwrites_inputs_or_an_existing_output(self):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory);game=root/'game';firmware=root/'firmware'
            game.mkdir();firmware.mkdir();existing=root/'existing';existing.mkdir()
            for out in (game,game/'runtime',firmware/'out',existing):
                with self.assertRaises(ValueError):validate_output(game,firmware,out)
            self.assertEqual(validate_output(game,firmware,root/'new')[2],root/'new')

if __name__=='__main__':unittest.main()
