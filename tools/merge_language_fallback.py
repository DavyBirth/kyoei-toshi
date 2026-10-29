"""Write one draft for a partial language, filling untranslated entries from English.

Lets a partial pt-BR translation be played: translated lines in Portuguese, the rest
in the approved English instead of Japanese. Output stays private under build/.
"""
import argparse
import json
from pathlib import Path
from english_typography import native_english

REPO = Path(__file__).resolve().parents[1]


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('draft', type=Path, help='Partial language draft (e.g. localization-pt-BR.json)')
    parser.add_argument('out', type=Path)
    parser.add_argument('--language', default='pt-BR')
    args = parser.parse_args()
    partial = {}
    for resource in json.loads(args.draft.read_text(encoding='utf-8'))['resources']:
        for entry in resource['entries']:
            partial[(resource['path'], entry['id'])] = entry
    resources = {}
    for path in sorted((REPO / 'translations').glob('localization-en*.json')):
        for resource in json.loads(path.read_text(encoding='utf-8'))['resources']:
            target = resources.setdefault(resource['path'], dict(path=resource['path'],
                                          source_sha256=resource['source_sha256'], entries=[]))
            for entry in resource['entries']:
                own = partial.pop((resource['path'], entry['id']), None)
                value = own[args.language] if own else entry['en']
                row = dict(entry, **{args.language: native_english(value)})
                if own and own.get('source_text_sha256') != entry.get('source_text_sha256'):
                    raise ValueError('Source text differs: ' + resource['path'] + ' ' + entry['id'])
                target['entries'].append(row)
    if partial:
        raise ValueError(f'{len(partial)} entries have no English counterpart')
    draft = dict(format='kyoei-translation-draft-v1', title='CUSA03562', version='01.03',
                 language=args.language, resources=list(resources.values()))
    args.out.write_text(json.dumps(draft, ensure_ascii=False) + '\n', encoding='utf-8')
    print(f'{args.out}: {sum(len(r["entries"]) for r in draft["resources"])} entries')


if __name__ == '__main__':
    main()
