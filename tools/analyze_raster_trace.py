"""Read bounded raster captures; report clipping candidates, never assert a GPU cause."""
import argparse
import json
import re
from pathlib import Path


def analyze(text):
    captures=[];current=None
    for line in text.splitlines():
        if 'Kyoei raster capture begin:' in line:
            values=dict(re.findall(r'(\w+)=([^\s]+)',line))
            current=dict(id=int(values['id']),frames=int(values['frames']),draws=int(values['draws']),
                         expected_profiles=int(values['profiles']),dropped=int(values['dropped']),
                         complete=False,profiles={})
            captures.append(current)
        elif current and 'Kyoei raster draw:' in line:
            values=dict(re.findall(r'(\w+)=([^\s]+)',line))
            if int(values['id'])!=current['id']:continue
            current['profiles'][int(values['p'])]=values
        elif current and ('Kyoei raster viewport:' in line or 'Kyoei raster bounds:' in line):
            values=dict(re.findall(r'(\w+)=([^\s]+)',line));index=int(values['p'])
            if index in current['profiles']:current['profiles'][index].update(values)
        elif current and 'Kyoei raster vertex:' in line:
            values=dict(re.findall(r'(\w+)=([^\s]+)',line));index=int(values['p'])
            if index in current['profiles']:current['profiles'][index].setdefault('vertex_samples',[]).append(values)
        elif current and 'Kyoei raster parameters:' in line:
            values=dict(re.findall(r'(\w+)=([^\s]+)',line));index=int(values['p'])
            if index in current['profiles']:current['profiles'][index].setdefault('shader_parameters',[]).append(values)
        elif current and 'Kyoei raster uniform:' in line:
            values=dict(re.findall(r'(\w+)=([^\s]+)',line));index=int(values['p'])
            if index in current['profiles']:current['profiles'][index].setdefault('uniform_samples',[]).append(values)
        elif current and 'Kyoei raster capture end:' in line:
            end=int(re.search(r'id=(\d+)',line).group(1))
            required={'xywh','scissor_xywh','screen','window','generic','viewport'}
            current['complete']=end==current['id'] and len(current['profiles'])==current['expected_profiles'] and all(
                required<=row.keys() for row in current['profiles'].values())
    for capture in captures:
        capture['log_complete']=capture['complete']
        capture['sampling_complete']=capture['complete'] and capture['dropped']==0
        rows=list(capture['profiles'].values());capture['profiles']=rows
        capture['constant_capture_present']=any(row.get('uniform_samples') for row in rows)
        candidates=[]
        for row in rows:
            if 'xywh' not in row or 'scissor_xywh' not in row:continue
            viewport=[float(v) for v in row['xywh'].split(',')]
            scissor=[int(v) for v in row['scissor_xywh'].split(',')]
            if viewport[2]>0 and viewport[0]<scissor[0]<viewport[0]+viewport[2]:
                candidates.append(dict(profile=int(row['p']),target=row['target'],ps=row['ps'],
                    viewport=viewport,scissor=scissor,
                    left_cut_fraction=(scissor[0]-viewport[0])/viewport[2],
                    note='Partial left coverage; may be intentional. Compare before/after and draw target.'))
        capture['left_cut_candidates']=candidates
    return dict(scope='Raster states only, no texture/shader contents. Candidates are not proof of a defect.',captures=captures)


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('log',type=Path);parser.add_argument('--output',type=Path,required=True)
    args=parser.parse_args()
    report=analyze(args.log.read_text(encoding='utf8',errors='replace'))
    if args.output.exists():raise ValueError('Choose a new report file')
    args.output.write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print(f'{len(report["captures"])} captures; completeness/drop counts retained; {args.output}')


if __name__=='__main__':main()
