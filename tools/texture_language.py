"""Translation-only overrides; preserve the existing texture layout definitions."""
import copy
import json
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]

def localized_table(table,language):
    if language not in ('en','pt-BR'):raise ValueError('Unsupported texture language')
    # A region marked with "language" only exists for that language
    # (e.g. original English art such as "or" that needs replacing in Portuguese).
    result=copy.deepcopy(table)
    for item in result['textures']:
        if 'regions' in item:
            item['regions']=[r for r in item['regions'] if r.get('language',language)==language]
    # Drop art that only changes in another language, so it is left untouched here.
    result['textures']=[i for i in result['textures'] if 'regions' not in i or i['regions']]
    if language=='en':return result
    labels=json.loads((ROOT/'translations/texture-labels-pt-BR.json').read_text(encoding='utf8'))
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
