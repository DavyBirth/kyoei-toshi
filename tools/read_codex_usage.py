"""Read only recent rate-limit telemetry from an explicitly selected local session."""
import argparse
from datetime import datetime, timezone
import json
from pathlib import Path


def read_usage(path, reserve=13):
    with path.open('rb') as source:
        size=source.seek(0,2)
        offset=max(0,size-(8<<20))
        source.seek(offset)
        lines=source.read(size-offset).splitlines()
    if offset: lines=lines[1:]  # first line can be truncated
    for line in reversed(lines):
        if b'"token_count"' not in line: continue
        try:
            entry=json.loads(line)
            payload=entry.get('payload',{})
            limits=payload.get('rate_limits')
            if entry.get('type')!='event_msg' or payload.get('type')!='token_count' or not limits:continue
            primary,weekly=limits.get('primary'),limits.get('secondary')
            if not primary or not weekly:continue
            remaining=[100-float(window['used_percent']) for window in (primary,weekly)]
            if not all(0<=value<=100 for value in remaining):continue
            observed=datetime.fromisoformat(entry['timestamp'].replace('Z','+00:00'))
            now=datetime.now(timezone.utc)
            age=(now-observed).total_seconds()
            return dict(checked_at=now.isoformat(),observed_at=entry['timestamp'],
                        primary_remaining=remaining[0],weekly_remaining=remaining[1],
                        primary_resets_at=primary['resets_at'],weekly_resets_at=weekly['resets_at'],
                        stale=not 0<=age<=600,finish_requested=min(remaining)<=reserve)
        except (ValueError,TypeError,KeyError):continue
    raise ValueError('No recent rate-limit record found in the selected session')


if __name__=='__main__':
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('session',type=Path)
    parser.add_argument('--reserve',type=float,default=13)
    args=parser.parse_args()
    print(json.dumps(read_usage(args.session,args.reserve)))
