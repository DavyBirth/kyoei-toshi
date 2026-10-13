"""Summarize measured presentation windows in a selected log byte range."""
import argparse
import json
from pathlib import Path
import re

parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('log',type=Path)
parser.add_argument('--start-byte',type=int,default=0)
parser.add_argument('--end-byte',type=int)
parser.add_argument('--out',type=Path)
args=parser.parse_args()
with args.log.open('rb') as file:
    encoding='utf-16-le' if file.read(2)==b'\xff\xfe' else 'utf-8-sig'
    file.seek(args.start_byte)
    data=file.read() if args.end_byte is None else file.read(max(0,args.end_byte-args.start_byte))
text=data.decode(encoding,errors='replace')
frames=[dict(zip(('fps','mean_ms','p50_ms','p95_ms','max_ms'),map(float,row))) for row in re.findall(
    r'Kyoei frame stats: intervals=120 fps=([\d.]+) mean_ms=([\d.]+) p50_ms=([\d.]+) p95_ms=([\d.]+) max_ms=([\d.]+)',text)]
work={}
for name,mean,p95,maximum in re.findall(r'Kyoei work stats: stage=([^ ]+) calls=120 mean_ms=([\d.]+) p95_ms=([\d.]+) max_ms=([\d.]+)',text):
    work.setdefault(name,[]).append(dict(mean_ms=float(mean),p95_ms=float(p95),max_ms=float(maximum)))
tls=[int(value) for value in re.findall(r'Kyoei TLS stats: handled_total=(\d+)',text)]
signals=[line for line in text.splitlines() if re.search(r'STOP:|Kyoei map failure:|Kyoei flip failure:|Assertion Failed|\bVK_ERROR_\w+|GPU .*\[(?:Critical|Error)',line,re.I)]
report=dict(log=str(args.log.resolve()),start_byte=args.start_byte,end_byte=args.start_byte+len(data),
            presentation_windows=len(frames),summarized_intervals=120*len(frames),
            estimated_presentation_seconds=round(sum(row['mean_ms']*120/1000 for row in frames),3),
            note='Aggregate summaries of 120 intervals; no raw frame trace. Menus/loading may be mixed unless scene is observed.',
            fps_over_summarized_intervals=round(1000*len(frames)/sum(row['mean_ms'] for row in frames),2) if frames else None,
            slowest_window_fps=min((row['fps'] for row in frames),default=None),
            fastest_window_fps=max((row['fps'] for row in frames),default=None),
            largest_interval_ms=max((row['max_ms'] for row in frames),default=None),
            frames=frames,work=work,tls_handled_range=[min(tls),max(tls)] if tls else None,
            detected_failure_lines=signals)
if args.out:
    args.out.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps({key:value for key,value in report.items() if key not in ('frames','work')},indent=2))
print(json.dumps({'work':{key:{'windows':len(rows),'mean_ms':round(sum(row['mean_ms'] for row in rows)/len(rows),2),'max_ms':max(row['max_ms'] for row in rows)} for key,rows in work.items()}},indent=2))
