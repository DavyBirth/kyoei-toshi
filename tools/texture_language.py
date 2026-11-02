"""Translation-only overrides; preserve the existing texture layout definitions."""
import copy
import json
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]

def localized_table(table,language):
    if language=='en':return table
    if language!='pt-BR':raise ValueError('Unsupported texture language')
    labels=json.loads((ROOT/'translations/texture-labels-pt-BR.json').read_text(encoding='utf8'))
    result=copy.deepcopy(table)
    def text(value):
        if value not in labels:raise ValueError('Untranslated texture label: '+value)
        return labels[value]
    for item in result['textures']:
        for key in ('en','heading'):
            if key in item:item[key]=text(item[key])
        if 'paragraphs' in item:item['paragraphs']=[text(v) for v in item['paragraphs']]
        for region in item.get('regions',[]):
            if region.get('text'):region['text']=text(region['text'])
    result['language']=language
    return result
