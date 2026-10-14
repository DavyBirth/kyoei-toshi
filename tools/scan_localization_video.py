"""Index 2 fps text-region changes; candidates are not verified dialogue.

Keeps scores/timestamps, not thousands of redundant frame files. Original video
is never changed. Use native-resolution snapshots for actual semantic review.
"""
import argparse
import hashlib
import json
from pathlib import Path
import subprocess
from PIL import Image, ImageChops


def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('video',type=Path)
    parser.add_argument('output',type=Path)
    args=parser.parse_args()
    probe=json.loads(subprocess.check_output(['ffprobe','-v','error','-select_streams','v:0',
        '-show_entries','stream=width,height,codec_name,avg_frame_rate:format=duration',
        '-of','json',str(args.video)]))
    args.output.parent.mkdir(parents=True,exist_ok=True)
    if args.output.exists(): raise ValueError('Choose a new index; preserve earlier scans')
    command=['ffmpeg','-v','error','-threads','2','-i',str(args.video),'-an',
             '-vf','fps=2:start_time=0,scale=320:180,format=gray','-f','rawvideo','pipe:1']
    rows=[]; previous=None; regions=((0,110,320,180),(210,0,320,90))
    with subprocess.Popen(command,stdout=subprocess.PIPE,stderr=subprocess.PIPE) as process:
        while True:
            raw=process.stdout.read(320*180)
            if not raw: break
            if len(raw)!=320*180: raise ValueError('Truncated video frame')
            image=Image.frombytes('L',(320,180),raw)
            scores=[]
            for region in regions:
                current=image.crop(region)
                if previous is None: scores.append(1.0)
                else:
                    hist=ImageChops.difference(current,previous.crop(region)).histogram()
                    scores.append(sum(hist[25:])/sum(hist))
            rows.append(dict(sample=len(rows),sample_seconds=len(rows)/2,
                             bottom_change=round(scores[0],6),hud_change=round(scores[1],6),
                             candidate=previous is None or max(scores)>=0.02))
            previous=image
            if len(rows)%1200==0: print(f'{args.video.name}: {len(rows)} samples',flush=True)
        errors=process.stderr.read().decode('utf-8',errors='replace')
        if process.wait(): raise ValueError(errors)
    result=dict(video=str(args.video.resolve()),source_bytes=args.video.stat().st_size,
                source_sha256=hashlib.file_digest(args.video.open('rb'),'sha256').hexdigest(),
                probe=probe,sample_fps=2,timestamp_note='Nominal fps-filter sample grid; source frames quantized to source rate. Seek native frames for verification.',
                semantic_review_complete=False,threshold=0.02,regions_scaled_320x180=regions,
                samples=len(rows),candidates=sum(r['candidate'] for r in rows),frames=rows)
    args.output.write_text(json.dumps(result,indent=2)+'\n',encoding='utf-8')
    print(f'{args.video.name}: {len(rows)} samples; {result["candidates"]} candidates; no semantic completion claimed',flush=True)


if __name__=='__main__': main()
