"""Apply the prepared private portable update after its game and launcher close."""
import json,subprocess,sys,time
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]
OUT=ROOT/'build/City-Shrouded-in-Shadow-Portable'
STATUS=ROOT/'build/portable-update-status.json'

def status(state,**fields):
    temporary=STATUS.with_suffix('.json.tmp')
    temporary.write_text(json.dumps(dict(state=state,**fields),indent=2)+'\n',encoding='utf-8')
    temporary.replace(STATUS)

def main():
    manifest=json.loads((ROOT/'docs/m4/runtime-preview.json').read_text(encoding='utf-8'))
    preview=manifest['languages']['en']
    if preview['textures']!=59 or 'runtime-preview-en-14' not in preview['archive']:
        raise ValueError('Expected the corrected 59-texture preview')
    status('waiting_for_game_and_launcher_to_close')
    # No handles are retained; the builder repeats the preflight before writes.
    deadline=time.monotonic()+24*60*60
    while time.monotonic()<deadline:
        try:
            for name in ['runtime/kyoei.exe','City Shrouded in Shadow.exe']:
                with (OUT/name).open('r+b'):pass
        except PermissionError:
            time.sleep(2);continue
        status('updating')
        result=subprocess.run([sys.executable,str(ROOT/'tools/build_portable.py'),str(OUT)],cwd=ROOT)
        if result.returncode:
            status('failed',exit_code=result.returncode);return result.returncode
        check=subprocess.run([str(OUT/'City Shrouded in Shadow.exe'),'--check'],cwd=OUT)
        status('complete' if check.returncode==0 else 'failed',exit_code=check.returncode,
            english_textures=59,launcher_check_passed=check.returncode==0)
        return check.returncode
    status('expired_without_changes');return 2

if __name__=='__main__':
    try:sys.exit(main())
    except Exception as error:
        status('failed',error=str(error));raise
