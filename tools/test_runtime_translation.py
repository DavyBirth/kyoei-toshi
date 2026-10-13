"""Exercise selected-language validation using synthetic resources only."""
import contextlib
import hashlib
import io
import json
from pathlib import Path
import tempfile
import unittest
from unittest.mock import patch
import build_runtime_translation as builder
from test_text import lua_function, resource_fixture
from text_formats import Lua52


class RuntimeTranslation(unittest.TestCase):
    def run_validation(self, language, corrupt=False):
        with tempfile.TemporaryDirectory() as directory:
            root=Path(directory)
            for folder in ('docs/m0','docs/m2','translations','source/common/lua'):
                (root/folder).mkdir(parents=True)
            data=resource_fixture(Lua52.HEADER+lua_function([b'Listen [i10]',b'untouched']),lua=True)
            digest=hashlib.sha256(data).hexdigest()
            (root/'source/common/lua/test.luc').write_bytes(data)
            audit=dict(source=str(root/'source'),records=[dict(path='common/lua/test.luc',sha256=digest)])
            (root/'docs/m2/audit.json').write_text(json.dumps(audit))
            (root/'docs/m0/inventory.json').write_text(json.dumps(dict(source='unused',files=[])))
            (root/'translations/glossary.json').write_text(json.dumps(dict(entries=[])))
            entry=dict(id='p0/k3',en='Listen to them [i10]',**{'pt-BR':None})
            if language=='pt-BR':
                entry.update(en=None,**{'pt-BR':'Ouvir [i10]'})
            if corrupt: entry[language]='Lost the button'
            draft=dict(format='kyoei-translation-draft-v1',title='CUSA03562',version='01.03',
                resources=[dict(path='common/lua/test.luc',source_sha256=digest,entries=[entry])])
            path=root/'draft.json'; path.write_text(json.dumps(draft))
            output=root/'not-created'
            argv=['builder',str(output),str(path),'--preview','--language',language,'--validate-only']
            stream=io.StringIO()
            with patch.object(builder,'REPO',root),patch.object(builder,'output_path',return_value=output), \
                 patch('sys.argv',argv),patch.object(builder,'patch') as archive_patch,contextlib.redirect_stdout(stream):
                builder.main()
                archive_patch.assert_not_called()
            self.assertFalse(output.exists())
            self.assertFalse((root/'docs/m4/runtime-preview.json').exists())
            return json.loads(stream.getvalue())

    def test_english_does_not_require_portuguese_or_create_archives(self):
        report=self.run_validation('en')
        self.assertEqual(report['languages'],['en'])
        self.assertTrue(report['resource_roundtrip_verified'])

    def test_portuguese_can_be_validated_separately(self):
        self.assertEqual(self.run_validation('pt-BR')['languages'],['pt-BR'])

    def test_selected_language_still_requires_control_tokens(self):
        with self.assertRaisesRegex(ValueError,'Control tokens'):
            self.run_validation('en',corrupt=True)


if __name__=='__main__': unittest.main()
