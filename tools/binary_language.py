"""Portuguese fixed-slot UI text over the hash-verified English location table."""
import copy
import json
import re
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]

def localized_binary_table(language):
    table=json.loads((ROOT/'translations/binary-en.json').read_text(encoding='utf8'))
    if language=='en':return table
    if language!='pt-BR':raise ValueError('Unsupported binary language')
    labels=json.loads((ROOT/'translations/texture-labels-pt-BR.json').read_text(encoding='utf8'))
    labels.update(json.loads((ROOT/'translations/binary-labels-pt-BR.json').read_text(encoding='utf8')))
    table=copy.deepcopy(table);table['language']=language
    for record in table['files']:
        for change in record['changes']:
            key=change['en'];match=re.fullmatch(r'Start from Stage ([0-9AB-]+)',key)
            value='Começar na fase '+match[1] if match else labels.get(key)
            if value is None:raise ValueError('Untranslated fixed text: '+key)
            if len(value.encode('utf8'))>change['bytes']:
                raise ValueError(f'Portuguese exceeds {change["bytes"]}-byte slot: {key} -> {value}')
            change['en']=value
    return table
