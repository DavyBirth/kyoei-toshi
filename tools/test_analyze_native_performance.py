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
print('PERFORMANCE ANALYSIS PASS: diagnostics isolated, legacy unknown, slow frames retained')
