"""Translate a language workspace in batches: show, fill and publish.

The workspace (private, it holds the Japanese text) is a copy of an external-translation
export (translate-<lang>-NNNN.json: resources, entries with id, original, category).
  show FILE            one line per entry: id, category, Japanese, approved English
  apply FILE LINES     LINES has "id<TAB>translation" (empty = leave for review, needs a
                       note "id<TAB><TAB>note"); control tokens and protected names checked
  publish OUT          repository drafts (no Japanese): id, source hash, translation, notes
  status               filled / pending per file
"""
import argparse
import glob
import hashlib
import json
import sys
from pathlib import Path
sys.path.insert(0, str(Path(__file__).parent))
from prepare_translation_catalog import TOKENS
from translation_names import validate_names

REPO = Path(__file__).resolve().parents[1]
LANGUAGE = 'pt-BR'


def load_english():
    english = {}
    for path in sorted((REPO / 'translations').glob('localization-en*.json')):
        for resource in json.loads(path.read_text(encoding='utf-8'))['resources']:
            for entry in resource['entries']:
                english[(resource['path'], entry['id'])] = entry.get('en', '')
    return english


def files(workspace):
    return sorted(Path(p) for p in glob.glob(str(workspace / f'translate-{LANGUAGE}-*.json')))


def show(path):
    english = load_english()
    data = json.loads(path.read_text(encoding='utf-8'))
    for resource in data['resources']:
        print(f'## {resource["path"]}')
        for e in resource['entries']:
            flag = '*' if e.get(LANGUAGE) else ' '
            ja = e['original'].replace('\n', '\\n')
            en = english.get((resource['path'], e['id']), '').replace('\n', '\\n')
            print(f'{flag}{e["id"]}\t{e.get("category", "")}\t{ja}\t{en}')


def apply(path, lines):
    glossary = json.loads((REPO / 'translations/glossary.json').read_text(encoding='utf-8'))
    data = json.loads(path.read_text(encoding='utf-8'))
    entries = {}
    for resource in data['resources']:
        for e in resource['entries']:
            entries[e['id']] = e
    done = 0
    for line in lines.read_text(encoding='utf-8').splitlines():
        if not line.strip() or line.startswith('#'):
            continue
        parts = line.split('\t')
        key, text = parts[0].lstrip('*'), parts[1] if len(parts) > 1 else ''
        note = parts[2] if len(parts) > 2 else ''
        if key not in entries:
            raise ValueError(f'{path.name}: unknown id {key}')
        e = entries[key]
        text = text.replace('\\n', '\n')
        if text:
            if sorted(TOKENS.findall(text)) != sorted(TOKENS.findall(e['original'])):
                raise ValueError(f'{path.name} {key}: control tokens differ: {text!r}')
            validate_names(e['original'], text, LANGUAGE, glossary)
        elif not note:
            raise ValueError(f'{path.name} {key}: an empty translation needs a note')
        e[LANGUAGE] = text
        if note:
            e['notes'] = note
        done += 1
    path.write_text(json.dumps(data, ensure_ascii=False, indent=1) + '\n', encoding='utf-8')
    print(f'{path.name}: {done} entries written')


def publish(workspace, out):
    resources = {}
    for path in files(workspace):
        data = json.loads(path.read_text(encoding='utf-8'))
        for resource in data['resources']:
            target = resources.setdefault(resource['path'], {'path': resource['path'],
                                                             'source_sha256': resource['source_sha256'], 'entries': []})
            for e in resource['entries']:
                if not e.get(LANGUAGE):
                    continue
                target['entries'].append({'id': e['id'], 'source_text_sha256': e['source_text_sha256'],
                                          LANGUAGE: e[LANGUAGE], 'notes': e.get('notes', ''),
                                          'editorial_status': 'pending'})
    draft = {'format': 'kyoei-translation-draft-v1', 'title': 'CUSA03562', 'version': '01.03',
             'language': LANGUAGE, 'resources': [r for r in resources.values() if r['entries']]}
    out.write_text(json.dumps(draft, ensure_ascii=False, indent=1) + '\n', encoding='utf-8')
    print(f'{out}: {sum(len(r["entries"]) for r in draft["resources"])} entries')


def status(workspace):
    total = filled = 0
    for path in files(workspace):
        data = json.loads(path.read_text(encoding='utf-8'))
        n = sum(len(r['entries']) for r in data['resources'])
        f = sum(bool(e.get(LANGUAGE)) or bool(e.get('notes')) for r in data['resources'] for e in r['entries'])
        total += n; filled += f
        if f < n:
            print(f'{path.name}\t{f}/{n}\t{data["resources"][0]["path"]}')
    print(f'TOTAL {filled}/{total}')


def main():
    ap = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument('workspace', type=Path)
    ap.add_argument('command', choices=('show', 'apply', 'publish', 'status'))
    ap.add_argument('args', nargs='*')
    a = ap.parse_args()
    if a.command == 'show':
        show(a.workspace / a.args[0])
    elif a.command == 'apply':
        apply(a.workspace / a.args[0], Path(a.args[1]))
    elif a.command == 'publish':
        publish(a.workspace, Path(a.args[0]))
    else:
        status(a.workspace)


if __name__ == '__main__':
    main()
