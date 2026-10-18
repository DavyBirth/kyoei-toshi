"""Check frame-tail metrics, window boundaries and overlapping nested scopes."""
from analyze_stutter_trace import summarize


def event(stage, end, duration, diagnostic=False, address=0, size=0):
    return dict(stage=stage, end_ms=end, duration_ms=duration, diagnostic=diagnostic,
                address=address, bytes=size, thread=1)


events = [event(100, 110, 10), event(100, 150, 40), event(100, 270, 120),
          event(100, 370, 100, True), event(3, 250, 110, address=0x1234, size=256),
          event(12, 260, 120, address=7, size=2), event(9, 100, 10)]
report = summarize(events, 110, 270)
assert report['unflagged']['frames'] == 2
assert report['unflagged']['fps'] == 12.5
assert report['unflagged']['p50_ms'] == 40
assert report['unflagged']['p95_ms'] == report['unflagged']['p99_ms'] == 120
assert report['unflagged']['over_50ms'] == report['unflagged']['over_100ms'] == 1
assert report['diagnostic']['frames'] == 0
assert report['worst'][0]['duration_ms'] == 120
assert len(report['worst'][0]['overlaps']) == 2
assert report['worst'][0]['overlaps'][0]['overlap_ms'] == 110
assert report['worst'][0]['overlaps'][0]['batch_draw_calls'] == 7
assert report['worst'][0]['overlaps'][0]['batch_dispatch_calls'] == 2
assert report['worst'][0]['overlaps'][1]['overlap_ms'] == 100
assert report['worst'][0]['overlaps'][1]['bytes'] == 256
all_frames = summarize(events)
assert all_frames['unflagged']['frames'] == 3
assert all_frames['diagnostic']['frames'] == 1
assert all_frames['diagnostic']['over_100ms'] == 0
empty = summarize(events, 500, 600)
assert empty['unflagged']['p99_ms'] is None and empty['worst'] == []
print('stutter analysis: individual percentiles, strict thresholds, diagnostic/window boundaries and nested overlaps PASS')
