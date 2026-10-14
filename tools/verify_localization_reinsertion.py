"""Prove private localization reinsertion against audited source resources, in memory."""
import argparse
from collections import defaultdict
from datetime import datetime
import hashlib
import json
from pathlib import Path
from audit_localization_return import audit, load
from external_translation import source_resource
from text_tool import export, insert


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('workspace', type=Path)
    args = parser.parse_args()
    root = args.workspace.resolve()
    working = load(root / 'working')
    originals = load(root / 'original')
    structure = audit(originals, working)
    if not structure['structural_pass']:
        raise ValueError('Protected metadata audit failed')
    translated = defaultdict(dict)
    for document in working.values():
        for resource in document['resources']:
            for entry in resource['entries']:
                if entry['en']:
                    if entry['id'] in translated[resource['path']]:
                        raise ValueError('Duplicate source entry')
                    translated[resource['path']][entry['id']] = entry
    records = []
    for name, entries in sorted(translated.items()):
        data, canonical = source_resource(name)
        source_strings = {row['id']: row['original'] for row in canonical['strings']}
        if not entries.keys() <= source_strings.keys():
            raise ValueError('Translation has unknown source IDs')
        expected = dict(source_strings)
        for row in canonical['strings']:
            if row['id'] in entries:
                entry = entries[row['id']]
                if entry['original'] != row['original']:
                    raise ValueError('Translation source differs from actual resource')
                row['translation'] = entry['en']
                expected[row['id']] = entry['en']
        rebuilt = insert(data, canonical)
        decoded = {row['id']: row['original'] for row in export(rebuilt)['strings']}
        if decoded != expected:
            raise ValueError('Reinserted translations or untouched source strings differ')
        records.append(dict(resource=name, translated_entries=len(entries),
            source_strings=len(source_strings), untouched_preserved=True,
            source_sha256=hashlib.sha256(data).hexdigest(),
            rebuilt_sha256=hashlib.sha256(rebuilt).hexdigest(),
            rebuilt_bytes=len(rebuilt)))
    report = dict(timestamp=datetime.now().astimezone().isoformat(),
        structural_pass=True, reinsertion_pass=True, runtime_verified=False,
        scope='All nonempty English entries in current working JSONs; in-memory rebuild and decode against own audited dump.',
        resources=len(records), translated_entries=sum(r['translated_entries'] for r in records),
        records=records)
    (root / 'reports' / 'current-reinsertion.json').write_text(
        json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({k:v for k,v in report.items() if k!='records'}))


if __name__ == '__main__':
    main()
