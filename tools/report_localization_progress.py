"""Summarize a private localization workspace without modifying its translations."""
import argparse
from collections import Counter, defaultdict
from datetime import datetime
import json
from pathlib import Path
from audit_localization_return import audit, load


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('workspace',type=Path)
    args=parser.parse_args();root=args.workspace.resolve()
    working=load(root/'working');baseline=load(root/'baseline-stages-1-7')
    check=audit(load(root/'original'),working)
    if not check['structural_pass']:raise ValueError('Structural audit failed')
    resources=defaultdict(Counter);categories=defaultdict(Counter)
    changes=[];pending=[];files=[]
    for name,doc in sorted(working.items()):
        prior={(r['path'],e['id']):e for r in baseline[name]['resources'] for e in r['entries']}
        count=Counter()
        for resource in doc['resources']:
            for entry in resource['entries']:
                filled=bool(entry['en']);state='filled' if filled else 'empty'
                count[state]+=1;resources[resource['path']][state]+=1
                categories[entry.get('category','unknown')][state]+=1
                before=prior[(resource['path'],entry['id'])]
                if before['en'] and entry['en']!=before['en']:
                    changes.append(dict(file=name,resource=resource['path'],id=entry['id'],
                        before=before['en'],after=entry['en'],cleared=not filled))
                if not filled:
                    pending.append(dict(file=name,resource=resource['path'],id=entry['id'],
                        category=entry.get('category','unknown'),has_notes=bool(entry['notes']),
                        source_text_sha256=entry['source_text_sha256']))
        files.append(dict(file=name,**count))
    report=dict(timestamp=datetime.now().astimezone().isoformat(),files=len(working),
        totals=check['totals'],new_english_since_received=check['totals']['filled']-4124,
        new_english_since_overnight_start=check['totals']['filled']-4795,
        baseline_nonempty_modified=len(changes),baseline_nonempty_cleared=sum(x['cleared'] for x in changes),
        structural_pass=True,editorial_approved=False,runtime_verified=False,
        categories={k:dict(v) for k,v in sorted(categories.items())},
        resources=[dict(resource=k,**v) for k,v in sorted(resources.items())],file_coverage=files,
        limitations=['Blank entries include internal/test material and unresolved names, not just spoken lines.',
            'Counts apply to this handoff only; full visible-text coverage is not certified.',
            'A filled English field does not imply editorial approval or runtime layout verification.'])
    for name,data in [('progress-current.json',report),('pending-entry-index.json',pending),('received-english-changes.json',changes)]:
        (root/'reports'/name).write_text(json.dumps(data,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({k:v for k,v in report.items() if k not in ('resources','file_coverage','categories','limitations')}))


if __name__=='__main__':main()
