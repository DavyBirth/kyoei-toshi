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
cycles = event(112, 260, 120, address=123456)
other_thread = event(112, 250, 110, address=999)
other_thread['thread'] = 2
with_cycles = summarize(events + [cycles, other_thread], 110, 270)
assert with_cycles['unflagged'] == report['unflagged']
assert len(with_cycles['worst'][0]['overlaps']) == 2
assert with_cycles['worst'][0]['overlaps'][0]['batch_thread_cycles'] == 123456
assert 'batch_thread_cycles' not in with_cycles['worst'][0]['overlaps'][1]
assert 'batch_thread_cycles' not in report['worst'][0]['overlaps'][0]
print('stutter analysis: individual percentiles, strict thresholds, diagnostic/window boundaries and nested overlaps PASS')

# Whole-batch selection excludes partial and diagnostic batches. Metadata from
# another thread or a nearly identical boundary must never be associated.
batch = event(12, 180, 60, address=3, size=2)
zero = event(12, 200, 10)
wrong = event(112, 180, 60, address=999)
wrong['thread'] = 2
ranked = summarize([batch, zero, wrong, event(112, 180.001, 60, address=888),
                    event(112, 180, 60, address=120), event(112, 200, 10, address=0),
                    event(12, 150, 50), event(12, 220, 10),
                    event(12, 190, 70, True)], 110, 210)['batches']
assert ranked['count'] == ranked['with_cycles'] == 2
assert ranked['longest'][0]['start_ms'] == 120
assert ranked['highest_cycles'][0]['batch_thread_cycles'] == 120
assert ranked['highest_cycles'][0]['cycles_per_draw'] == 40
assert ranked['highest_cycles'][1]['cycles_per_draw'] is None
assert summarize([batch])['batches']['with_cycles'] == 0
assert summarize([], 110, 210)['batches']['longest'] == []
assert summarize([event(12, 210, 100)], 110, 210)['batches']['count'] == 1
print('stutter analysis: whole-batch rankings, exact cycle association and zero draws PASS')
