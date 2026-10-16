import hashlib
import json
from pathlib import Path
import tempfile
import unittest
import zipfile

from build_runtime_update import accepted_update_hash


class AcceptedUpdates(unittest.TestCase):
    def check(self, version='01.03', corrupt=False):
        with tempfile.TemporaryDirectory() as temporary:
            archive = Path(temporary)/'update.zip'
            payload = b'previous runtime'
            expected = hashlib.sha256(payload).hexdigest()
            with zipfile.ZipFile(archive,'w') as output:
                output.writestr('Atualizacao-Kyoei/kyoei.exe', b'damaged' if corrupt else payload)
                output.writestr('Atualizacao-Kyoei/update.json', json.dumps(dict(
                    schema=1,platform='windows-x64',title='CUSA03562',version=version,sha256=expected)))
            return accepted_update_hash(archive,'CUSA03562','01.03'),expected

    def test_valid_previous_update(self):
        actual,expected=self.check()
        self.assertEqual(actual,expected)

    def test_wrong_revision(self):
        with self.assertRaises(ValueError): self.check(version='01.00')

    def test_corrupt_previous_payload(self):
        with self.assertRaises(ValueError): self.check(corrupt=True)


if __name__=='__main__':
    unittest.main()
