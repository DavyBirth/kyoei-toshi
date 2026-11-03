"""All localized fixed labels must fit and preserve engine controls."""
import json
import unittest
from pathlib import Path
from binary_language import localized_binary_table
from texture_language import localized_table
from apply_binary_text import TOKENS

ROOT=Path(__file__).resolve().parents[1]

class LanguageLabels(unittest.TestCase):
    def test_every_fixed_slot_preserves_functional_tokens(self):
        en=localized_binary_table('en');pt=localized_binary_table('pt-BR')
        self.assertEqual(len(en['files']),len(pt['files']))
        total=0
        for before,after in zip(en['files'],pt['files']):
            self.assertEqual(before['source_sha256'],after['source_sha256'])
            for original,translated in zip(before['changes'],after['changes']):
                self.assertLessEqual(len(translated['en'].encode()),original['bytes'])
                self.assertEqual(sorted(TOKENS.findall(original['en'])),sorted(TOKENS.findall(translated['en'])))
                total+=1
        self.assertEqual(total,413)

    def test_all_texture_labels_translate_without_mutating_layout(self):
        en=json.loads((ROOT/'translations/textures-en.json').read_text(encoding='utf8'))
        pt=localized_table(en,'pt-BR');self.assertEqual(len(pt["textures"]),214)
        # Art whose only regions are Portuguese (title/pause menus) is left untouched in English.
        self.assertEqual(len(localized_table(en,'en')['textures']),168)
        self.assertEqual(pt['language'],'pt-BR')
        for before,after in zip(en['textures'],pt['textures']):
            self.assertEqual(before['resource'],after['resource'])
            for a,b in zip(before.get('regions',[]),after.get('regions',[])):
                self.assertEqual(a.get('box'),b.get('box'))
        self.assertEqual(en['textures'][0]['en'],'On')
        self.assertEqual(pt['textures'][0]['en'],'Ligado')

if __name__=='__main__':unittest.main()
