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
frames=[]
for row in re.findall(
    r'Kyoei frame stats: intervals=120 fps=([\d.]+) mean_ms=([\d.]+) p50_ms=([\d.]+) p95_ms=([\d.]+) max_ms=([\d.]+)(?: diagnostic=([01]))?',text):
    frame=dict(zip(('fps','mean_ms','p50_ms','p95_ms','max_ms'),map(float,row[:5])))
    frame['diagnostic']=bool(int(row[5])) if row[5] else None
    frames.append(frame)
work={}
sampling={}
for stage,stride in re.findall(r'Kyoei work stats: stage=([^ ]+) [^\r\n]*sample_stride=(\d+)',text):
    sampling.setdefault(stage,set()).add(int(stride))
for name,mean,p95,maximum,diagnostic in re.findall(r'Kyoei work stats: stage=([^ ]+) calls=120 mean_ms=([\d.]+) p95_ms=([\d.]+) max_ms=([\d.]+)(?: diagnostic=([01]))?',text):
    work.setdefault(name,[]).append(dict(mean_ms=float(mean),p95_ms=float(p95),max_ms=float(maximum),diagnostic=bool(int(diagnostic)) if diagnostic else None))

def partition_summary(rows):
    result={}
    for label,state in [('unflagged',False),('diagnostic',True),('legacy_unknown',None)]:
        selected=[row for row in rows if row['diagnostic'] is state]
        result[label]=dict(windows=len(selected),
            mean_ms=sum(row['mean_ms'] for row in selected)/len(selected) if selected else None,
            largest_interval_ms=max((row['max_ms'] for row in selected),default=None))
    return result
tls=[int(value) for value in re.findall(r'Kyoei TLS stats: handled_total=(\d+)',text)]
display=[line for line in text.splitlines() if 'Kyoei display trace:' in line]
readbacks=[dict(images=int(images),waits=int(waits),bytes=int(size)) for images,waits,size in re.findall(
    r'Kyoei readback batch: images=(\d+) waits=(\d+) bytes=(\d+)',text)]
signals=[line for line in text.splitlines() if re.search(r'STOP:|Kyoei map failure:|Kyoei flip failure:|Assertion Failed|\bVK_ERROR_\w+|GPU .*\[(?:Critical|Error)',line,re.I)]
compile_stalls=[dict(stage=stage,milliseconds=float(milliseconds),diagnostic=bool(int(diagnostic)))
    for stage,milliseconds,diagnostic in re.findall(r'Kyoei compile stall: stage=([^ ]+) milliseconds=([\d.]+) diagnostic=([01])',text)]
deadlines=[dict(zip(('intervals','over_60hz','over_30hz','over_50ms','over_100ms','diagnostic'),map(int,row)))
    for row in re.findall(r'Kyoei frame deadlines: intervals=(\d+) over_60hz=(\d+) over_30hz=(\d+) over_50ms=(\d+) over_100ms=(\d+) diagnostic=([01])',text)]
work_stalls=[dict(stage=stage,milliseconds=float(duration),end_ms=float(end),diagnostic=bool(int(diagnostic)))
    for stage,duration,end,diagnostic in re.findall(
        r'Kyoei work stall: stage=([^ ]+) milliseconds=([\d.]+) end_ms=([\d.]+) diagnostic=([01])',text)]
readback_stalls=[]
for size,width,height,duration,end,address in re.findall(
        r'Kyoei readback stall: bytes=(\d+) width=(\d+) height=(\d+) milliseconds=([\d.]+) end_ms=([\d.]+)(?: address=(0x[0-9a-f]+))?',text):
    record=dict(bytes=int(size),width=int(width),height=int(height),milliseconds=float(duration),end_ms=float(end))
    if address: record['address']=address
    readback_stalls.append(record)
small_images=[dict(address=address,width=int(width),height=int(height),pitch=int(pitch),format=int(fmt),role=int(role))
    for address,width,height,pitch,fmt,role in re.findall(
        r'Kyoei small image: address=(0x[0-9a-f]+) width=(\d+) height=(\d+) pitch=(\d+) format=(\d+) role=(\d+)',text)]
gpu_waits=[dict(requested=int(requested),next=int(next_tick),known_before=int(before),known_after=int(after),
                forced_flush=bool(int(flush)),milliseconds=float(duration),end_ms=float(end),diagnostic=bool(int(diagnostic)))
    for requested,next_tick,before,after,flush,duration,end,diagnostic in re.findall(
        r'Kyoei GPU wait: requested=(\d+) next=(\d+) known_before=(\d+) known_after=(\d+) forced_flush=([01]) milliseconds=([\d.]+) end_ms=([\d.]+) diagnostic=([01])',text)]
deadline_summary={}
for label,state in [('unflagged',0),('diagnostic',1)]:
    selected=[row for row in deadlines if row['diagnostic']==state]
    deadline_summary[label]={key:sum(row[key] for row in selected)
        for key in ('intervals','over_60hz','over_30hz','over_50ms','over_100ms')}
report=dict(log=str(args.log.resolve()),start_byte=args.start_byte,end_byte=args.start_byte+len(data),
            presentation_windows=len(frames),summarized_intervals=120*len(frames),
            estimated_presentation_seconds=round(sum(row['mean_ms']*120/1000 for row in frames),3),
            note='Aggregate summaries of 120 intervals; no raw frame trace. Menus/loading may be mixed unless scene is observed.',
            fps_over_summarized_intervals=round(1000*len(frames)/sum(row['mean_ms'] for row in frames),2) if frames else None,
            slowest_window_fps=min((row['fps'] for row in frames),default=None),
            fastest_window_fps=max((row['fps'] for row in frames),default=None),
            largest_interval_ms=max((row['max_ms'] for row in frames),default=None),
            frames=frames,work=work,
            work_sampling_strides={stage:sorted(strides) for stage,strides in sampling.items()},
            work_sampling_note='sample_stride=128 measures one in 128 calls; statistics and slow records cover selected calls only. Missing stride means legacy every-call instrumentation. Do not infer total work from sampled calls.',
            measured_deadlines=deadline_summary,
            measured_deadlines_note='Counts from individual presentation intervals before rounding; only windows carrying deadline records. Not simulation speed or raw frame timestamps.',
            diagnostic_partitions=partition_summary(frames),
            work_diagnostic_partitions={name:partition_summary(rows) for name,rows in work.items()},
            target_60fps=dict(budget_ms=1000/60,
                windows_mean_over_budget=sum(row['mean_ms']>1000/60 for row in frames),
                windows_p95_over_budget=sum(row['p95_ms']>1000/60 for row in frames),
                note='Window summaries cannot establish minimum FPS or raw frame deadline counts. Unflagged does not imply gameplay or absence of other profiling.'),
            tls_handled_range=[min(tls),max(tls)] if tls else None,
            detected_failure_lines=signals,display_trace=display,readback_batch_samples=readbacks,
            compile_stalls=compile_stalls,
            work_stalls=work_stalls,
            readback_stalls=readback_stalls,
            small_images=small_images,
            small_images_note='At most 16 unique (address,format,role) records per thread, 1x1 images only; roles 1=storage binding, 2=render target lookup, 3=download. Virtual addresses can be reused. Bindings do not prove a particular shader executed or identify a game subsystem.',
            gpu_waits=gpu_waits,
            gpu_waits_note='Up to eight Scheduler::Wait calls >=50ms per thread. Known ticks are cached completion counters, not live GPU timestamps or workload durations. next is the logical tick before Wait; forced_flush reports the existing unsent-tick path. No added Vulkan queries.',
            work_stalls_note='At most eight calls >=50ms per category/thread. end_ms uses a process-local monotonic clock; nested scopes overlap and must not be summed. Not GPU timestamps or a complete trace.',
            compile_stalls_note='Capped at eight slow calls per compiler category/thread; not a complete stall count.')
if args.out:
    args.out.write_text(json.dumps(report,indent=2)+'\n',encoding='utf-8')
print(json.dumps({key:value for key,value in report.items() if key not in ('frames','work')},indent=2))
print(json.dumps({'work':{key:{'windows':len(rows),'mean_ms':round(sum(row['mean_ms'] for row in rows)/len(rows),2),'max_ms':max(row['max_ms'] for row in rows)} for key,rows in work.items()}},indent=2))
