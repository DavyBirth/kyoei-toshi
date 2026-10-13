"""Validate portable review decisions against measured image metadata."""
import argparse
import collections
import json
from pathlib import Path

def validate(document,assets):
    if document.get('format')!='kyoei-image-review-v1' or document.get('title')!='CUSA03562' or document.get('version')!='01.03':
        raise ValueError('Wrong review identity')
    known={a['id']:a for a in assets}; seen=set(); decisions=collections.Counter()
    for row in document['images']:
        if row['id'] in seen or row['id'] not in known: raise ValueError('Duplicate/unknown image')
        seen.add(row['id']); source=known[row['id']]
        if any(row.get(k)!=source.get(k) for k in ('archive','resource','source_sha256','width','height')):
            raise ValueError('Image metadata differs')
        if row['decision'] not in ('pending','translate','preserve') or not isinstance(row.get('notes'),str):
            raise ValueError('Invalid review decision')
        decisions[row['decision']]+=1
    return dict(reviewed_records=len(seen),inventory_images=len(known),decision_counts=dict(decisions),
                unlisted_images=len(known)-len(seen),ai_edits_applied=False,reinsertion_verified=False)

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('review',type=Path); parser.add_argument('inventory',type=Path)
    args=parser.parse_args()
    print(json.dumps(validate(json.loads(args.review.read_text(encoding='utf8')),
                             json.loads(args.inventory.read_text(encoding='utf8'))),indent=2))

if __name__=='__main__': main()
