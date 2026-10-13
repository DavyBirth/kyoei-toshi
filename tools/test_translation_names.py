import unittest
from translation_names import validate_names

class Names(unittest.TestCase):
    glossary={'entries':[{'id':'hero','source_forms':['ウルトラマン'],
                          'en':'Ultraman','pt-BR':'Ultraman'}]}
    def test_approved_spelling(self):
        validate_names('ウルトラマン！','Ultraman!','en',self.glossary)
    def test_literal_translation_rejected(self):
        with self.assertRaises(ValueError):
            validate_names('ウルトラマン！','Ultra Homem!','pt-BR',self.glossary)
    def test_occurrences_and_boundaries(self):
        for value in ('SuperUltraman','Ultraman and Ultraman'):
            with self.assertRaises(ValueError):
                validate_names('ウルトラマン',value,'en',self.glossary)
    def test_custom_name(self):
        validate_names('Olá, Davy!','Hello, Davy!','en',self.glossary,['Davy'])
        with self.assertRaises(ValueError):
            validate_names('Olá, Davy!','Hello, David!','en',self.glossary,['Davy'])
    def test_engine_name_reference(self):
        with self.assertRaises(ValueError):
            validate_names('Olá [PLAYER_NAME]','Hello John','en',self.glossary)

if __name__=='__main__': unittest.main()
