"""Private English handoff and audited import; source prose never enters the patch."""
import argparse
import hashlib
import json
import zipfile
from pathlib import Path
from prepare_translation_catalog import TOKENS, classify
from text_tool import REPO, export, insert, decode, output_path
from translation_names import validate_names


def digest(text):
    return hashlib.sha256(text.encode('utf8')).hexdigest()


def source_resource(name):
    audit = json.loads((REPO/'docs/m2/audit.json').read_text(encoding='utf8'))
    row = next((row for row in audit['records'] if row['path'] == name), None)
    if row is None:
        raise ValueError('Unknown audited resource')
    data = (Path(audit['source'])/name).read_bytes()
    if hashlib.sha256(data).hexdigest() != row['sha256']:
        raise ValueError('Source changed since audit')
    return data, export(data)


def known_ids(name, language='en'):
    ids = set()
    for path in (REPO/'translations').glob('*.json'):
        draft = json.loads(path.read_text(encoding='utf8'))
        for resource in draft.get('resources', []):
            if resource['path'] == name:
                ids.update(row['id'] for row in resource['entries'] if language in row)
    return ids


def audit_rows(resource, canonical, glossary, existing=(), language='en'):
    if resource.get('source_sha256') != canonical['source_sha256']:
        raise ValueError('Resource hash differs')
    originals = {row['id']: row['original'] for row in canonical['strings']}
    result = []
    seen = set()
    for row in resource['entries']:
        key = row.get('id')
        if key not in originals or key in seen or key in existing:
            raise ValueError('Unknown, duplicate or already translated ID')
        seen.add(key)
        original = originals[key]
        if row.get('source_text_sha256') != digest(original) or row.get('original') != original:
            raise ValueError('Original or text hash differs')
        translated = row.get(language)
        if not isinstance(translated, str) or '\0' in translated:
            raise ValueError('Invalid translated value')
        if not translated.strip():
            continue  # Uncertain rows may be returned untranslated.
        if TOKENS.findall(translated) != TOKENS.findall(original):
            raise ValueError('Formatting tokens differ')
        validate_names(original, translated, language, glossary)
        notes = row.get('notes', '')
        if not isinstance(notes, str):
            raise ValueError('Invalid notes')
        # Construct an allowlist rather than copying GPT's fields/originals.
        entry=dict(id=key, source_text_sha256=digest(original),
                   notes=notes, editorial_status='pending')
        entry[language]=translated
        result.append(entry)
    return result


PROMPT = '''Translate the attached JSON from Japanese to natural English for City Shrouded in Shadow.
Return the complete JSON as a downloadable UTF-8 file. Keep format, title, version,
language, resource paths, IDs, original text and SHA-256 fields exactly unchanged.
Fill only en and notes. Preserve every control token, icon, color tag and player-name
placeholder, in the same order. Never translate internal identifiers or developer/debug
notes: leave en empty and explain in notes. Distinguish NPC labels, dialogue, choices
and tutorials. Neighboring constants provide context but are not guaranteed chronological.
Use the attached protected glossary. Do not invent readings of human names or translate
their meanings. Leave uncertain names/lines empty with a concise question in notes.
Do not shorten natural dialogue to hide HUD bugs. Do not add ellipses for UI clipping;
preserve ellipses that belong to the source. English only; Portuguese is a later pass.
This is a draft for human review. Do not claim it is tested or approved.
'''


def prepare(args):
    folder = output_path(args.output)
    data, canonical = source_resource(args.resource)
    existing = known_ids(args.resource)
    candidates = [row for row in canonical['strings']
                  if row['japanese_candidate'] and row['id'] not in existing
                  and int(row['id'].split('/k')[-1]) >= args.start]
    rows = [dict(id=row['id'], source_text_sha256=digest(row['original']),
                 original=row['original'], en='', notes='') for row in candidates[:args.limit]]
    if not rows:
        raise ValueError('No untranslated Japanese candidates')
    document = dict(format='kyoei-external-translation-v1', title='CUSA03562', version='01.03',
                    language='en', resources=[dict(path=args.resource,
                    source_sha256=canonical['source_sha256'], entries=rows)])
    folder.mkdir(parents=True)
    (folder/'translate-en.json').write_text(json.dumps(document, ensure_ascii=False, indent=2)+'\n', encoding='utf8')
    (folder/'INSTRUCTIONS.txt').write_text(PROMPT, encoding='utf8')
    (folder/'glossary.json').write_bytes((REPO/'translations/glossary.json').read_bytes())
    print(f'Private handoff: {len(rows)} candidates; {folder}')


def prepare_all(args):
    folder = output_path(args.output)
    language = args.language
    context_key = 'existing_english_drafts' if language=='en' else 'existing_portuguese_drafts'
    audit = json.loads((REPO/'docs/m2/audit.json').read_text(encoding='utf8'))
    root = Path(audit['source'])
    known = {}
    for path in (REPO/'translations').glob('*.json'):
        for resource in json.loads(path.read_text(encoding='utf8')).get('resources', []):
            known.setdefault(resource['path'], {}).update({row['id']: row[language]
                for row in resource['entries'] if language in row})
    folder.mkdir(parents=True)
    manifest = dict(title='CUSA03562', version='01.03', language=language,
        complete_visible_text_coverage_verified=False,
        scope='All remaining Japanese candidates in audited Lua/CSH, excluding font data/name filters. Known target-language drafts included separately for context.',
        unresolved=['Text baked into textures', 'Videos/subtitles not yet classified',
                    'HUD binary labels and runtime-generated strings', 'Non-UTF8 encodings',
                    'Technical/debug constants mixed with dialogue; skip and annotate'],
        files=[], resources=[], excluded=[])
    manifest[context_key]=[]
    number = 0
    for record in audit['records']:
        name = record['path']
        if not record['japanese_strings']:
            continue
        if '/font/' in name.lower() or 'bannedwords' in name.lower():
            manifest['excluded'].append(dict(path=name, reason='Font or name-filter data, not dialogue'))
            continue
        data = (root/name).read_bytes()
        if hashlib.sha256(data).hexdigest() != record['sha256']:
            raise ValueError('Audited source changed')
        canonical = export(data)
        rows = []
        previous = []
        for row in canonical['strings']:
            if not row['japanese_candidate']:
                continue
            category, status = classify(name, row['original'])
            if status == 'excluded':
                continue
            entry = dict(id=row['id'], original=row['original'],
                         source_text_sha256=digest(row['original']), notes='',
                         category=category)
            entry[language]=''
            if row['id'] in known.get(name, {}):
                entry[language] = known[name][row['id']]
                previous.append(entry)
            else:
                rows.append(entry)
        if previous:
            manifest[context_key].append(dict(path=name,
                source_sha256=record['sha256'], entries=previous))
        if not rows:
            continue
        manifest['resources'].append(dict(path=name, source_sha256=record['sha256'],
            candidates=len(rows), non_utf8_pending=record['non_utf8_strings']))
        for start in range(0, len(rows), args.limit):
            chunk = rows[start:start+args.limit]
            number += 1
            filename = f'translate-{language}-{number:04d}.json'
            document = dict(format='kyoei-external-translation-v1', title='CUSA03562',
                version='01.03', language=language, resources=[dict(path=name,
                source_sha256=record['sha256'], entries=chunk)])
            (folder/filename).write_text(json.dumps(document, ensure_ascii=False, indent=2)+'\n', encoding='utf8')
            manifest['files'].append(dict(file=filename, resource=name, entries=len(chunk)))
    manifest['new_candidates'] = sum(row['entries'] for row in manifest['files'])
    manifest['existing_target_entries'] = sum(len(row['entries']) for row in manifest[context_key])
    (folder/'manifest.json').write_text(json.dumps(manifest, ensure_ascii=False, indent=2)+'\n', encoding='utf8')
    prompt = PROMPT if language=='en' else PROMPT.replace('natural English','natural Brazilian Portuguese').replace(
        'Fill only en and notes','Fill only pt-BR and notes').replace('leave en empty','leave pt-BR empty').replace(
        'English only; Portuguese is a later pass.',
        'Brazilian Portuguese only, with correct accents. This pass follows the English pass. Translate from Japanese; completed English may serve as context, not as a substitute for the source.')
    instruction = prompt + '''\nStart with dialogue/objectives/tutorials/shared_ui/shop/items files.
The ZIP contains many files: process them in batches and return each completed JSON
with its original filename. Do not pretend all files fit in one answer. Track completed
and pending filenames in a progress note. manifest.json identifies resources and gaps.
Existing target-language drafts in the manifest are context only: do not return them as new work.
Unclassified Japanese may be internal data; leave the translation empty whenever uncertain.
Translate the whole supplied collection over successive batches, not only the opening.
'''
    (folder/'INSTRUCTIONS.txt').write_text(instruction, encoding='utf8')
    (folder/'glossary.json').write_bytes((REPO/'translations/glossary.json').read_bytes())
    archive = folder/('Kyoei-English-handoff.zip' if language=='en' else 'Kyoei-Portugues-handoff.zip')
    with zipfile.ZipFile(archive, 'x', compression=zipfile.ZIP_DEFLATED) as bundle:
        for path in sorted(folder.iterdir()):
            if path.is_file() and path != archive:
                bundle.write(path, path.name)
    print(f'Full extracted-text handoff: {manifest["new_candidates"]} new candidates, '
          f'{manifest["existing_target_entries"]} prior drafts, {number} files; {archive}')


def import_result(args):
    incoming = json.loads(args.input.read_text(encoding='utf-8-sig'))
    language=incoming.get('language')
    if language not in ('en','pt-BR'):
        raise ValueError('Unsupported handoff language')
    if (incoming.get('format'), incoming.get('title'), incoming.get('version'), incoming.get('language')) != (
            'kyoei-external-translation-v1', 'CUSA03562', '01.03', language):
        raise ValueError('Wrong handoff identity or language')
    target = args.output.resolve()
    if target.parent != (REPO/'translations').resolve() or target.suffix != '.json' or target.exists():
        raise ValueError('Choose a new JSON directly under translations/')
    glossary = json.loads((REPO/'translations/glossary.json').read_text(encoding='utf8'))
    resources = []
    seen = set()
    for resource in incoming['resources']:
        name = resource['path']
        if name in seen:
            raise ValueError('Duplicate resource')
        seen.add(name)
        data, canonical = source_resource(name)
        rows = audit_rows(resource, canonical, glossary, known_ids(name,language),language)
        if not rows:
            continue
        changes = {row['id']: row[language] for row in rows}
        for row in canonical['strings']:
            if row['id'] in changes:
                row['translation'] = changes[row['id']]
        rebuilt = insert(data, canonical)
        _, parsed = decode(rebuilt)
        actual = {row.id: row.text() for row in parsed.strings}
        if any(actual[row['id']] != changes.get(row['id'], row['original']) for row in canonical['strings']):
            raise ValueError('Translated or untouched string failed roundtrip')
        resources.append(dict(path=name, source_sha256=canonical['source_sha256'], entries=rows))
    if not resources:
        raise ValueError('No nonempty new translations')
    draft = dict(format='kyoei-translation-draft-v1', title='CUSA03562', version='01.03',
                 status='draft_requires_human_review', runtime_verified=False, resources=resources)
    # Exclusive creation occurs only after all resources pass validation.
    with target.open('x', encoding='utf8') as stream:
        stream.write(json.dumps(draft, ensure_ascii=False, indent=2)+'\n')
    print(f'Audited draft: {sum(len(r["entries"]) for r in resources)} entries; {target}')


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    sub = parser.add_subparsers(dest='action', required=True)
    prep = sub.add_parser('prepare')
    prep.add_argument('output', type=Path)
    prep.add_argument('--resource', default='common/lua/Text_Stage_201.luc')
    prep.add_argument('--start', type=int, default=265)
    prep.add_argument('--limit', type=int, choices=range(1, 1001), default=300, metavar='1..1000')
    full = sub.add_parser('prepare-all')
    full.add_argument('output', type=Path)
    full.add_argument('--limit', type=int, choices=range(1, 1001), default=300, metavar='1..1000')
    full.add_argument('--language', choices=('en','pt-BR'),default='en')
    imp = sub.add_parser('import')
    imp.add_argument('input', type=Path)
    imp.add_argument('output', type=Path)
    args = parser.parse_args()
    {'prepare': prepare, 'prepare-all': prepare_all, 'import': import_result}[args.action](args)


if __name__ == '__main__':
    main()
