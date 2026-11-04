"""Test helper: while a --warp-stage gallery runs, capture the window after each
NPC visit printed in the log (Guest: Chr<stage>_<id>), pressing Enter when a
message stops the tour. Paths are this machine's test session."""
import time,re,subprocess,json,sys
log='G:/kyoei-toshi/build/items12-20261103/candidate.log';out='G:/kyoei-toshi/build/items12-20261103/tour201'
cap=r'D:/Programas/Dados/Temp/User/claude/4e768ff6d224fd4b/64e49a75-e5ea-49b0-8c5a-ff8849d53f72/scratchpad/cap.ps1'
pid=json.load(open('G:/kyoei-toshi/build/items12-20261103/session.json'))['pid']
seen=int(sys.argv[1]) if len(sys.argv)>1 else 0;idle=0;stuck=0
while stuck<12:
    lines=[l for l in open(log,encoding='utf8',errors='ignore') if re.search(r'Guest: Chr\d+_\d+',l)]
    if len(lines)>seen:
        idle=0;stuck=0
        for l in lines[seen:]:
            name=re.search(r'(Chr\d+_\d+)',l).group(1)
        seen=len(lines);time.sleep(2.0)
        subprocess.run(['powershell','-NoProfile','-ExecutionPolicy','Bypass','-File','G:/kyoei-toshi/tools/capture_native.ps1','-TargetId',str(pid),'-Out',f'{out}/{seen:03d}-{name}.png'],capture_output=True)
    else:
        idle+=1;time.sleep(0.5)
        if idle%8==0:
            open('G:/kyoei-toshi/build/items12-20261103/input.txt','a').write('tap enter 120'+chr(10));stuck+=1
print('captured',seen)
