"""Import audited private English workspace as a translation-only runtime draft."""
import argparse
import json
from pathlib import Path

from audit_localization_return import audit, load
from external_translation import audit_rows, source_resource
from text_tool import REPO


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('workspace', type=Path)
    parser.add_argument('output', type=Path)
    parser.add_argument('--only-new', action='store_true',
                        help='Export only IDs absent from current English drafts; refuse conflicts')
    args = parser.parse_args()
    root = args.workspace.resolve()
    target = args.output.resolve()
    if target.parent != (REPO / 'translations').resolve() or target.suffix != '.json' or target.exists():
        raise ValueError('Choose a new JSON directly under translations/')
    originals, working = load(root / 'original'), load(root / 'working')
    report = audit(originals, working)
    if len(working) != 173 or not report['structural_pass']:
        raise ValueError('Private workspace audit failed')
    glossary = json.loads((REPO / 'translations/glossary.json').read_text(encoding='utf-8'))
    existing = {}
    if args.only_new:
        for path in sorted((REPO / 'translations').glob('*.json')):
            draft = json.loads(path.read_text(encoding='utf-8'))
            for resource in draft.get('resources', []):
                for entry in resource['entries']:
                    if 'en' not in entry:
                        continue
                    key = (resource['path'], entry['id'])
                    value = (entry['en'], entry.get('source_text_sha256'))
                    if key in existing and existing[key] != value:
                        raise ValueError('Existing English drafts conflict')
                    existing[key] = value
    grouped = {}
    for filename, document in sorted(working.items()):
        if (document.get('format'), document.get('title'), document.get('version'), document.get('language')) != (
                'kyoei-external-translation-v1', 'CUSA03562', '01.03', 'en'):
            raise ValueError('Unexpected private document identity')
        for resource in document['resources']:
            name = resource['path']
            group = grouped.setdefault(name, dict(path=name, source_sha256=resource['source_sha256'], entries=[]))
            if group['source_sha256'] != resource['source_sha256']:
                raise ValueError('Mixed resource revisions')
            for entry in resource['entries']:
                if not entry['en']:
                    continue
                # Japanese prose and private editorial history never enter the patch.
                # Source text exists solely in this in-memory validation input.
                incoming = dict(id=entry['id'], original=entry['original'],
                                source_text_sha256=entry['source_text_sha256'], en=entry['en'],
                                notes=f'English draft imported from audited private workspace, {filename}:{entry["id"]}; human editorial and runtime review pending.')
                group['entries'].append(incoming)
    resources = []
    retained = 0
    for name, group in sorted(grouped.items()):
        if not group['entries']:
            continue
        _, canonical = source_resource(name)
        validated = audit_rows(group, canonical, glossary)
        # An explicit output allowlist protects even against future validator changes.
        rows = [dict(id=row['id'], source_text_sha256=row['source_text_sha256'],
                     en=row['en'], notes=row['notes'], editorial_status='pending')
                for row in validated]
        if args.only_new:
            new_rows = []
            for row in rows:
                previous = existing.get((name, row['id']))
                if previous is None:
                    new_rows.append(row)
                    continue
                if previous[0] != row['en'] or (previous[1] and previous[1] != row['source_text_sha256']):
                    raise ValueError('Private English differs from an existing draft; document a revision first')
                retained += 1
            rows = new_rows
        if rows:
            resources.append(dict(path=name, source_sha256=group['source_sha256'], entries=rows))
    total = sum(len(r['entries']) for r in resources)
    if total + retained != report['totals']['filled']:
        raise ValueError('Imported count differs from audited coverage')
    result = dict(format='kyoei-translation-draft-v1', title='CUSA03562', version='01.03',
                  status='draft_requires_human_review', runtime_verified=False, resources=resources)
    if any(set(entry) != {'id','source_text_sha256','en','notes','editorial_status'}
           for resource in resources for entry in resource['entries']):
        raise ValueError('Output contains a field outside the translation allowlist')
    with target.open('x', encoding='utf-8') as stream:
        stream.write(json.dumps(result, ensure_ascii=False, indent=2) + '\n')
    print(json.dumps(dict(output=str(target), resources=len(resources), entries=total,
                         existing_entries_retained=retained,
                         structural_pass=True, names_tokens_sources_checked=True,
                         original_prose_included=False, runtime_verified=False)))


if __name__ == '__main__':
    main()
