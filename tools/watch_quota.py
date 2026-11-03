"""Save this workspace and request Windows shutdown at the owner's quota limit.

Reads only the specified primary conversation's actual rate-limit telemetry.
Never infers usage from elapsed time; stale or unrelated records cannot trigger.
"""
import argparse
import datetime as dt
import hashlib
import json
import math
import os
from pathlib import Path
import subprocess
import time
import zipfile

ROOT=Path(__file__).resolve().parents[1]

def quota_state(session,thread,now=None):
    now=now or dt.datetime.now(dt.timezone.utc)
    with Path(session).open('rb') as stream:
        metadata=json.loads(stream.readline())['payload']
        if (metadata.get('id')!=thread or metadata.get('parent_thread_id') or
                Path(metadata.get('cwd','')).resolve()!=ROOT.resolve()):
            raise ValueError('Expected this workspace main conversation')
        stream.seek(0,2);stream.seek(max(0,stream.tell()-4*1024*1024))
        lines=stream.read().splitlines()
    for raw in reversed(lines):
        try:
            row=json.loads(raw);limits=row.get('payload',{}).get('rate_limits')
            if not limits or limits.get('limit_id')!='codex':continue
            stamp=dt.datetime.fromisoformat(row['timestamp'].replace('Z','+00:00'))
            age=(now-stamp).total_seconds();windows={}
            for key in ('primary','secondary'):
                window=limits.get(key) or {};minutes=window.get('window_minutes')
                name={300:'five_hour',10080:'weekly'}.get(minutes)
                if not name:continue
                used=float(window['used_percent']);reset=float(window['resets_at'])
                if not math.isfinite(used) or not 0<=used<=100 or not math.isfinite(reset):continue
                windows[name]=dict(remaining=100-used,resets_at=reset,
                                   fresh=0<=age<=900 and reset>now.timestamp())
            return dict(checked_at=now.isoformat(),telemetry_at=stamp.isoformat(),
                        telemetry_age_seconds=round(age,1),thread=thread,windows=windows)
        except (ValueError,KeyError,TypeError):continue
    return dict(checked_at=now.isoformat(),thread=thread,windows={})

def exhausted(state,threshold=5):
    return [name for name,window in state['windows'].items()
            if window['fresh'] and window['remaining']<=threshold]

def checkpoint(out):
    def git(*args):return subprocess.check_output(['git','-C',str(ROOT),*args])
    stamp=dt.datetime.now().strftime('%Y%m%d-%H%M%S')
    target=out/('checkpoint-'+stamp+'.zip')
    names=git('diff','--name-only','HEAD','-z').split(b'\0')
    names+=git('ls-files','--others','--exclude-standard','-z').split(b'\0')
    allowed={'src','docs','gpu-gow3','cmake','tools','scripts','translations'}
    records={}
    with zipfile.ZipFile(target,'x',zipfile.ZIP_DEFLATED,compresslevel=3) as archive:
        archive.writestr('head.txt',git('rev-parse','HEAD'))
        archive.writestr('status.txt',git('status','--short'))
        archive.writestr('changes.patch',git('diff','--binary','HEAD'))
        for raw in sorted(set(names)):
            if not raw:continue
            name=raw.decode('utf8');relative=Path(name);path=(ROOT/relative).resolve()
            if not path.is_relative_to(ROOT) or not path.is_file():continue
            if relative.parts[0] not in allowed and name not in ('AGENTS.md','README.md','.gitignore','CMakeLists.txt','LICENSE'):continue
            data=path.read_bytes();archive.writestr('files/'+name,data)
            records[name]=hashlib.sha256(data).hexdigest()
        archive.writestr('files-sha256.json',json.dumps(records,indent=2)+'\n')
    with zipfile.ZipFile(target) as archive:
        if archive.testzip() is not None:raise RuntimeError('Checkpoint verification failed')
        for name,digest in records.items():
            if hashlib.sha256(archive.read('files/'+name)).hexdigest()!=digest:
                raise RuntimeError('Checkpoint content differs')
    with target.open('r+b') as stream:os.fsync(stream.fileno())
    return target

def save_commit(out,state):
    """Checkpoint all eligible work, then commit tracked changes without adding game data."""
    with (ROOT/'docs/DIARIO.md').open('a',encoding='utf8') as stream:
        remaining=state['windows']['weekly']['remaining']
        stream.write(f'\n\n### {dt.datetime.now():%d/%m/%Y %H:%M} ? commit por cota semanal\n'
                     f'- Telemetria real desta conversa: {remaining:g}% semanal restante; commit imediato solicitado por Davy.\n'
                     '- Checkpoint verificado preserva tamb?m arquivos novos eleg?veis; commit inclui apenas arquivos j? versionados.\n')
        stream.flush();os.fsync(stream.fileno())
    saved=checkpoint(out)
    subprocess.run(['git','-C',str(ROOT),'add','--update'],check=True)
    subprocess.run(['git','-C',str(ROOT),'commit','-m','Save progress at weekly quota reserve'],check=True)
    head=subprocess.check_output(['git','-C',str(ROOT),'rev-parse','HEAD'],text=True).strip()
    return dict(commit=head,checkpoint=str(saved),quota=state)

def write_json(path,value):
    temporary=path.with_suffix(path.suffix+'.tmp')
    temporary.write_text(json.dumps(value,indent=2)+'\n',encoding='utf8')
    temporary.replace(path)

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--session',type=Path,required=True)
    parser.add_argument('--thread',required=True)
    parser.add_argument('--threshold',type=float,default=5)
    parser.add_argument('--weekly-commit-threshold',type=float,help='Commit tracked work at this weekly reserve; replaces weekly shutdown')
    parser.add_argument('--poll',type=float,default=5)
    parser.add_argument('--once',action='store_true')
    parser.add_argument('--output',type=Path,default=ROOT/'build/quota-watch-20261102')
    args=parser.parse_args();out=args.output.resolve()
    if not out.is_relative_to(ROOT/'build') or not 0<args.threshold<100 or args.poll<1:
        raise ValueError('Invalid private output or limit')
    if args.weekly_commit_threshold is not None and not 0<args.weekly_commit_threshold<100:
        raise ValueError('Invalid weekly commit limit')
    out.mkdir(parents=True,exist_ok=True)
    previous=None
    while not (out/'stop').exists():
        try:
            state=quota_state(args.session,args.thread)
            state['threshold_remaining']=args.threshold
            state['triggered_windows']=exhausted(state,args.threshold)
            weekly_commit=(args.weekly_commit_threshold is not None and
                           'weekly' in exhausted(state,args.weekly_commit_threshold))
            state['weekly_commit_threshold']=args.weekly_commit_threshold
            state['weekly_commit_due']=weekly_commit
            if args.weekly_commit_threshold is not None:
                state['triggered_windows']=[name for name in state['triggered_windows'] if name!='weekly']
            write_json(out/'current.json',state)
            if state.get('telemetry_at')!=previous:
                with (out/'history.jsonl').open('a',encoding='utf8') as stream:
                    stream.write(json.dumps(state)+'\n')
                previous=state.get('telemetry_at')
            if args.once:print(json.dumps(state),flush=True);return
            if weekly_commit:
                saved=save_commit(out,state)
                write_json(out/'committed.json',saved)
                return
            if state['triggered_windows']:
                write_json(out/'shutdown-request.json',state)
                details=', '.join(f'{name}: {state["windows"][name]["remaining"]:g}% restantes'
                                  for name in state['triggered_windows'])
                with (ROOT/'docs/DIARIO.md').open('a',encoding='utf8') as stream:
                    stream.write(f'\n\n### {dt.datetime.now():%d/%m/%Y %H:%M} — desligamento por cota\n'
                                 f'- Telemetria real da conversa principal: {details}. Limite autorizado: {args.threshold:g}% ou menos na janela de 5 horas ou semanal.\n'
                                 f'- Trabalho salvo em disco e checkpoint ZIP verificado em {out.relative_to(ROOT)}. Desligamento normal solicitado, sem forçar outros aplicativos.\n')
                    stream.flush();os.fsync(stream.fileno())
                saved=checkpoint(out)
                subprocess.run(['git','-C',str(ROOT),'add','--update'],check=True)
                subprocess.run(['git','-C',str(ROOT),'commit','-m','Save progress before authorized quota shutdown'],check=True)
                write_json(out/'saved.json',dict(checkpoint=str(saved),quota=state))
                subprocess.run(['shutdown.exe','/s','/t','0','/d','p:0:0','/c',
                                f'Kyoei: trabalho salvo; cota em {args.threshold:g}% ou menos, conforme pedido.'],check=True)
                return
        except Exception as error:
            write_json(out/'error.json',dict(at=dt.datetime.now(dt.timezone.utc).isoformat(),error=str(error)))
            if args.once:raise
        time.sleep(args.poll)

if __name__=='__main__':main()
