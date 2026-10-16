"""Verify diagnostic attribution without dropping slow presentation windows."""
import json
from pathlib import Path
import subprocess
import sys
import tempfile


with tempfile.TemporaryDirectory() as temporary:
    root=Path(temporary)
    log=root/'sample.log'
    output=root/'report.json'
    log.write_text('''Kyoei frame stats: intervals=120 fps=60.00 mean_ms=16.67 p50_ms=16.00 p95_ms=18.00 max_ms=21.00 diagnostic=0
Kyoei frame stats: intervals=120 fps=10.00 mean_ms=100.00 p50_ms=90.00 p95_ms=140.00 max_ms=200.00 diagnostic=1
Kyoei frame stats: intervals=120 fps=30.00 mean_ms=33.33 p50_ms=30.00 p95_ms=40.00 max_ms=50.00
Kyoei work stats: stage=linear-downloads calls=120 mean_ms=11.00 p95_ms=15.00 max_ms=20.00 diagnostic=0
Kyoei work stats: stage=linear-downloads calls=120 mean_ms=90.00 p95_ms=120.00 max_ms=190.00 diagnostic=1
Kyoei compile stall: stage=shader-compile milliseconds=44.00 diagnostic=0
Kyoei work stall: stage=video-label-wait milliseconds=77.00 end_ms=1000.00 diagnostic=0
Kyoei work stall: stage=command-batch milliseconds=90.00 end_ms=1001.00 diagnostic=1
Kyoei command batch: milliseconds=90.00 resume_ms=15.00 max_resume_ms=12.00 resumes=2 completed=1 yielded=1 empty=1 end_ms=1001.00 diagnostic=1
Kyoei work stats: stage=raster-draw calls=120 mean_ms=0.02 p95_ms=0.03 max_ms=0.10 diagnostic=0 sample_stride=128
Kyoei readback stall: bytes=256 width=8 height=8 milliseconds=240.00 end_ms=1002.00
Kyoei GPU wait: requested=20 next=21 known_before=18 known_after=20 forced_flush=0 milliseconds=240.00 end_ms=1002.00 diagnostic=0
Kyoei small image: address=0x1234 width=1 height=1 pitch=64 format=37 role=3
Kyoei readback stall: bytes=256 width=1 height=1 milliseconds=90.00 end_ms=1100.00 address=0x1234
Kyoei frame deadlines: intervals=120 over_60hz=20 over_30hz=10 over_50ms=5 over_100ms=2 diagnostic=0
Kyoei frame deadlines: intervals=120 over_60hz=120 over_30hz=100 over_50ms=80 over_100ms=40 diagnostic=1
''',encoding='utf-8')
    subprocess.run([sys.executable,str(Path(__file__).with_name('analyze_native_performance.py')),
                    str(log),'--out',str(output)],check=True,capture_output=True)
    report=json.loads(output.read_text(encoding='utf-8'))
    assert report['summarized_intervals']==360
    assert report['largest_interval_ms']==200
    assert [row['diagnostic'] for row in report['frames']]==[False,True,None]
    groups=report['diagnostic_partitions']
    assert all(row['windows']==1 for row in groups.values())
    assert groups['diagnostic']['mean_ms']==100
    assert groups['legacy_unknown']['mean_ms']==33.33
    assert report['work_diagnostic_partitions']['linear-downloads']['unflagged']['mean_ms']==11
    assert report['target_60fps']['windows_p95_over_budget']==3
    assert report['compile_stalls']==[dict(stage='shader-compile',milliseconds=44.0,diagnostic=False)]
    assert report['work_sampling_strides']=={'raster-draw':[128]}
    assert report['readback_stalls']==[
        dict(bytes=256,width=8,height=8,milliseconds=240.0,end_ms=1002.0),
        dict(bytes=256,width=1,height=1,milliseconds=90.0,end_ms=1100.0,address='0x1234')]
    assert report['gpu_waits']==[dict(requested=20,next=21,known_before=18,known_after=20,
        forced_flush=False,milliseconds=240.0,end_ms=1002.0,diagnostic=False)]
    assert report['small_images']==[dict(address='0x1234',width=1,height=1,pitch=64,format=37,role=3)]
    assert report['command_batches']==[dict(milliseconds=90.,resume_ms=15.,max_resume_ms=12.,resumes=2,completed=1,yielded=1,empty=1,end_ms=1001.,diagnostic=True)]
    assert report['work_stalls']==[
        dict(stage='video-label-wait',milliseconds=77.0,end_ms=1000.0,diagnostic=False),
        dict(stage='command-batch',milliseconds=90.0,end_ms=1001.0,diagnostic=True)]
    assert report['measured_deadlines']['unflagged']==dict(intervals=120,over_60hz=20,over_30hz=10,over_50ms=5,over_100ms=2)
    assert report['measured_deadlines']['diagnostic']['over_100ms']==40
    # The legacy window has no deadline samples; never invent counts for it.
    assert sum(row['intervals'] for row in report['measured_deadlines'].values())==240
print('PERFORMANCE ANALYSIS PASS: diagnostics isolated, legacy unknown, slow frames retained')
