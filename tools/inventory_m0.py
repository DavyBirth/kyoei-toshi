"""M0: calcula SHA-256 de todos os arquivos, sem alterar ou extrair o dump."""
import hashlib,json,sys
from pathlib import Path
root=Path(sys.argv[1]).resolve(strict=True)
out=Path(sys.argv[2]).resolve()
if root==out or root in out.parents:
 raise ValueError('Saída deve ficar fora do dump')
for name in ('eboot.bin','sce_sys/param.sfo','sce_module','DAT'):
 if not (root/name).exists(): raise ValueError('Ausente: '+name)
files=sorted(p for p in root.rglob('*') if p.is_file())
records=[]
for i,p in enumerate(files,1):
 before=p.stat(); h=hashlib.sha256()
 with p.open('rb') as f:
  while block:=f.read(8*1024*1024): h.update(block)
 after=p.stat()
 if (before.st_size,before.st_mtime_ns)!=(after.st_size,after.st_mtime_ns): raise RuntimeError('Arquivo mudou: '+str(p))
 records.append(dict(path=p.relative_to(root).as_posix(),bytes=before.st_size,sha256=h.hexdigest()))
 print(f'{i}/{len(files)} {records[-1]["path"]}',flush=True)
out.mkdir(parents=True,exist_ok=True)
(out/'inventory.json').write_text(json.dumps(dict(source=str(root),algorithm='SHA-256',files=records),indent=2)+'\n',encoding='utf8')
(out/'SHA256SUMS.txt').write_text(''.join(f'{r["sha256"]}  {r["path"]}\n' for r in records),encoding='utf8')
print(f'Concluido: {len(records)} arquivos; {sum(r["bytes"] for r in records)} bytes')
