"""Analyze opt-in frame intervals and overlapping CPU scopes, never summing nested work."""
import argparse
import csv
import json
import math
from pathlib import Path

NAMES = ('prepare', 'present', 'linear-downloads', 'readback-finish', 'readback-publish',
         'readback-map', 'shader-compile', 'graphics-pipeline', 'compute-pipeline',
         'scheduler-submit', 'scheduler-wait', 'queue-lock', 'command-batch',
         'present-pacing', 'command-callback', 'video-label-wait', 'raster-draw',
         'raster-dispatch', 'cp-sync', 'graphics-construct', 'graphics-register', 'graphics-driver')
DETAIL = {200: 'gfx-packet', 201: 'draw', 202: 'find-image', 203: 'refresh-image',
          204: 'texture-gc', 205: 'image-buffer', 206: 'command-callbacks',
          210: 'vulkan-record-backpressure', 211: 'vulkan-record-chunk', 299: 'streaming-other'}


def summarize(events, start=None, end=None):
    # Stage112 is metadata for the exact stage12 scope, never another timed scope.
    cycle_samples = {(e['thread'], e['end_ms'], e['duration_ms']): e['address']
                     for e in events if e['stage'] == 112}
    frames = [e for e in events if e['stage'] == 100 and
              (start is None or e['end_ms'] - e['duration_ms'] >= start) and
              (end is None or e['end_ms'] <= end)]
    work = [e for e in events if e['stage'] not in (100, 112)]
    result = {}
    for label, diagnostic in [('unflagged', False), ('diagnostic', True)]:
        selected = [f for f in frames if f['diagnostic'] == diagnostic]
        values = sorted(f['duration_ms'] for f in selected)
        total = sum(values)
        def percentile(p):
            return values[max(0, math.ceil(len(values) * p) - 1)] if values else None
        result[label] = dict(frames=len(values), seconds=total / 1000,
                             fps=len(values) * 1000 / total if total else None,
                             p50_ms=percentile(.5), p95_ms=percentile(.95),
                             p99_ms=percentile(.99), p999_ms=percentile(.999),
                             max_ms=max(values, default=None),
                             over_50ms=sum(v > 50 for v in values),
                             over_100ms=sum(v > 100 for v in values),
                             over_30hz=sum(v > 1000/30 for v in values))
    worst = []
    for frame in sorted((f for f in frames if not f['diagnostic']),
                        key=lambda f: f['duration_ms'], reverse=True)[:20]:
        overlaps = []
        for scope in work:
            duration = min(scope['end_ms'], frame['end_ms']) - max(
                scope['end_ms'] - scope['duration_ms'], frame['end_ms'] - frame['duration_ms'])
            if duration > 0:
                entry = dict(stage=NAMES[scope['stage']] if scope['stage'] < len(NAMES)
                                     else DETAIL.get(scope['stage'], str(scope['stage'])), overlap_ms=duration, **{
                                         k: scope[k] for k in ('duration_ms', 'thread', 'address', 'bytes', 'diagnostic')})
                if scope['stage'] == 12:
                    entry.update(batch_draw_calls=scope['address'], batch_dispatch_calls=scope['bytes'])
                    cycles = cycle_samples.get((scope['thread'], scope['end_ms'], scope['duration_ms']))
                    if cycles is not None:
                        entry['batch_thread_cycles'] = cycles
                overlaps.append(entry)
        worst.append(dict(end_ms=frame['end_ms'], duration_ms=frame['duration_ms'],
                          overlaps=sorted(overlaps, key=lambda e: e['overlap_ms'], reverse=True)[:25]))
    result['worst'] = worst
    # Keep exact batch boundaries for a future timestamped host profile.
    # Rank whole batches, never apportion their cycles to overlapping frames.
    batches = []
    for scope in work:
        if scope['stage'] != 12 or scope['diagnostic']:
            continue
        begin = scope['end_ms'] - scope['duration_ms']
        if (start is not None and begin < start) or (end is not None and scope['end_ms'] > end):
            continue
        entry = dict(start_ms=begin, **{k: scope[k] for k in ('end_ms', 'duration_ms', 'thread')},
                     batch_draw_calls=scope['address'], batch_dispatch_calls=scope['bytes'])
        cycles = cycle_samples.get((scope['thread'], scope['end_ms'], scope['duration_ms']))
        if cycles is not None:
            entry['batch_thread_cycles'] = cycles
            entry['cycles_per_draw'] = cycles / scope['address'] if scope['address'] else None
        batches.append(entry)
    measured = [b for b in batches if 'batch_thread_cycles' in b]
    result['batches'] = dict(count=len(batches), with_cycles=len(measured),
                            longest=sorted(batches, key=lambda b: b['duration_ms'], reverse=True)[:20],
                            highest_cycles=sorted(measured, key=lambda b: b['batch_thread_cycles'], reverse=True)[:20])
    result['note'] = ('Host scopes are wall times, may overlap/nest, and may belong to different frames. '
                      'Temporal overlap does not prove causation. Basic draw/dispatch scopes retain stride128; '
                      'optional packet/draw detail is unsampled and adds timing overhead. '
                      'Scopes >=8ms are traced, except detailed Vulkan backpressure >=1ms. '
                      'Batch thread cycles, when available, cover the entire batch, not just its overlap; '
                      'they are not milliseconds or a CPU utilization percentage. No GPU timestamp measurement.')
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('trace', type=Path)
    parser.add_argument('--range', type=Path, help='Private script JSON with start_clock_ms/end_clock_ms')
    parser.add_argument('--out', type=Path)
    args = parser.parse_args()
    with args.trace.open(encoding='utf-8') as source:
        header = source.readline().strip()
        dropped = int(header.removeprefix('# dropped='))
        if dropped:
            raise ValueError('Trace capacity exceeded; incomplete tail metrics are not a valid comparison')
        events = []
        for row in csv.DictReader(source):
            event = {k: int(row[k], 0) for k in ('stage', 'thread', 'address', 'bytes')}
            event.update({k: float(row[k]) for k in ('end_ms', 'duration_ms')})
            event['diagnostic'] = bool(int(row['diagnostic']))
            if event['duration_ms'] <= 0 or not all(math.isfinite(event[k]) for k in ('end_ms', 'duration_ms')):
                raise ValueError('Invalid event timing')
            events.append(event)
    selection = json.loads(args.range.read_text(encoding='utf-8-sig')) if args.range else {}
    if args.range and not (selection.get('end_clock_ms', 0) > selection.get('start_clock_ms', 0) > 0):
        raise ValueError('Selected range must contain valid monotonic start_clock_ms/end_clock_ms')
    report = summarize(events, selection.get('start_clock_ms'), selection.get('end_clock_ms'))
    report.update(trace=str(args.trace.resolve()), dropped=dropped, complete=dropped == 0)
    if args.out:
        args.out.write_text(json.dumps(report, indent=2) + '\n', encoding='utf-8')
    print(json.dumps({k: report[k] for k in ('unflagged', 'diagnostic', 'dropped', 'complete')}, indent=2))


if __name__ == '__main__':
    main()
