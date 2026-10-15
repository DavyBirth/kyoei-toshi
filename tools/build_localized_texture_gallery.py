"""Private, offline comparison gallery for localized TEX assets from the owner's dump."""
import argparse,hashlib,html,json
from pathlib import Path
from psarc import Psarc
from tex_preview import preview

ROOT=Path(__file__).resolve().parents[1]

def main():
    p=argparse.ArgumentParser(description=__doc__)
    p.add_argument('--textures',type=Path,default=ROOT/'build/overnight/textures-en')
    p.add_argument('--output',type=Path,default=ROOT/'build/texture-review')
    p.add_argument('--previous',type=Path,help='Private TEX directory to compare against the revision')
    p.add_argument('--approved-prefix',action='append',default=[],help='Path prefix explicitly approved by the owner')
    a=p.parse_args();out=a.output.resolve();textures=a.textures.resolve()
    if not out.is_relative_to(ROOT/'build') or out==ROOT/'build':raise ValueError('Private gallery must remain under build/')
    files=sorted(textures.rglob('*.tex'))
    if not files:raise ValueError('No localized TEX files')
    source=Path(json.loads((ROOT/'docs/m0/inventory.json').read_text(encoding='utf-8'))['source'])/'DAT/common.psarc'
    (out/'images').mkdir(parents=True,exist_ok=True)
    rows=[];cards=[]
    with Psarc(source) as archive:
        entries={e.name:e for e in archive.entries}
        for number,file in enumerate(files):
            name=file.relative_to(textures).as_posix();original=b''.join(archive.chunks(entries[name]));translated=file.read_bytes()
            row=dict(id=hashlib.sha256(name.encode()).hexdigest()[:16],resource=name,
                original_sha256=hashlib.sha256(original).hexdigest(),localized_sha256=hashlib.sha256(translated).hexdigest(),decision='pending')
            if any(name.startswith(prefix) for prefix in a.approved_prefix):row['decision']='accept'
            for version,data in [('original',original),('en',translated)]:
                picture,info=preview(data)
                filename=f'images/{number:02d}-{version}.png';picture.save(out/filename)
                row[version]=filename;row['width']=info['width'];row['height']=info['height']
            versions=[('original','Original japonês'),('en','Versão inglesa')]
            if a.previous:
                previous=(a.previous/name).read_bytes()
                picture,_=preview(previous)
                if picture.size!=(row['width'],row['height']):raise ValueError('Previous texture dimensions differ: '+name)
                filename=f'images/{number:02d}-previous.png';picture.save(out/filename)
                row['previous']=filename;row['previous_sha256']=hashlib.sha256(previous).hexdigest()
                versions=[('original','Original japonês'),('previous','Inglês anterior'),('en','Inglês revisado')]
            rows.append(row)
            pictures=''.join(f'<figure><figcaption>{label}</figcaption><button class="picture" data-src="{row[key]}"><img loading="lazy" src="{row[key]}" alt="{label}"></button></figure>' for key,label in versions)
            cards.append(f'<article data-id="{row["id"]}" data-resource="{html.escape(name,quote=True)}"><h2>{html.escape(name)}</h2><p>{row["width"]} × {row["height"]} · clique para ver em tamanho original</p><div class="comparison">{pictures}</div><label>Revisão <select><option value="pending">Pendente</option><option value="accept">Aprovada por mim</option><option value="revise">Precisa de ajustes</option><option value="defer">Decidir depois</option></select></label><textarea placeholder="O que você mudaria?"></textarea></article>')
    data=json.dumps(rows,ensure_ascii=False).replace('<','\\u003c')
    page='''<!doctype html><html lang="pt-BR"><meta charset="utf-8"><meta name="viewport" content="width=device-width"><title>Kyoei · revisão de texturas</title>
<style>*{box-sizing:border-box}body{margin:0;background:#14171d;color:#eee;font:16px system-ui}header{padding:22px 4%;position:sticky;top:0;background:#14171df5;z-index:1;border-bottom:1px solid #424955}h1{margin:0 0 8px;font-size:25px}p{color:#c0c6d0}input,select,button,textarea{font:inherit;border:1px solid #596372;border-radius:5px;padding:8px;background:#252b35;color:#fff}header input{width:min(480px,100%)}main{padding:20px 4%;display:grid;gap:25px}article{background:#202630;padding:18px;border-radius:8px}article[hidden]{display:none}h2{overflow-wrap:anywhere;font-size:17px}article p{font-size:13px}.comparison{display:grid;grid-template-columns:1fr 1fr;gap:16px}figure{margin:0;min-width:0}figcaption{margin-bottom:8px}.picture{display:block;width:100%;height:300px;cursor:zoom-in;background-color:#555;background-image:linear-gradient(45deg,#666 25%,transparent 25%),linear-gradient(-45deg,#666 25%,transparent 25%),linear-gradient(45deg,transparent 75%,#666 75%),linear-gradient(-45deg,transparent 75%,#666 75%);background-size:24px 24px;background-position:0 0,0 12px,12px -12px,-12px 0}.picture img{width:100%;height:100%;object-fit:contain}article label{display:block;margin-top:18px}textarea{width:100%;margin-top:8px}dialog{max-width:96vw;max-height:96vh;background:#242b35;color:white;border:1px solid #748298}dialog::backdrop{background:#000c}.full-image{overflow:auto;max-height:75vh;margin-top:12px;background:#555}dialog img{display:block;max-width:none}dialog button{cursor:pointer}@media(max-width:700px){.comparison{grid-template-columns:1fr}}</style>
<header><h1>Texturas para você revisar</h1><p>Original e inglês lado a lado. Nenhuma imagem é considerada aprovada de início. Esta galeria não modifica o jogo.</p><input id="filter" placeholder="Buscar: menu_option, stage_title…"><label><input style="width:auto" type="checkbox" id="pending"> Só pendentes</label> <button id="export">Salvar minha revisão</button> <span id="count"></span></header><main>'''+''.join(cards)+'''</main><dialog><button id="close">Fechar</button> <button id="actual">Tamanho original</button> <button id="fit">Ajustar à tela</button><div class="full-image"><img id="full" alt="Textura isolada"></div></dialog><script type="application/json" id="assets">'''+data+'''</script><script>
const rows=JSON.parse(document.querySelector('#assets').textContent),byId=new Map(rows.map(r=>[r.id,r]));
function key(row){return 'kyoei-localized-texture-'+row.localized_sha256}
for(const card of document.querySelectorAll('article')){const row=byId.get(card.dataset.id);let old={};try{old=JSON.parse(localStorage.getItem(key(row))||'{}')}catch(e){}const select=card.querySelector('select'),notes=card.querySelector('textarea');select.value=['pending','accept','revise','defer'].includes(old.decision)?old.decision:'pending';notes.value=typeof old.notes==='string'?old.notes:'';function save(){localStorage.setItem(key(row),JSON.stringify({decision:select.value,notes:notes.value}));filter()}select.onchange=save;notes.oninput=save}
function filter(){let n=0;for(const card of document.querySelectorAll('article')){card.hidden=!card.dataset.resource.toLowerCase().includes(document.querySelector('#filter').value.toLowerCase())||(document.querySelector('#pending').checked&&card.querySelector('select').value!=='pending');if(!card.hidden)n++}document.querySelector('#count').textContent=n+' de '+rows.length+' texturas'}document.querySelector('#filter').oninput=filter;document.querySelector('#pending').onchange=filter;filter();
const dialog=document.querySelector('dialog'),full=document.querySelector('#full');for(const button of document.querySelectorAll('.picture'))button.onclick=()=>{full.src=button.dataset.src;full.style.maxWidth='none';dialog.showModal()};document.querySelector('#close').onclick=()=>dialog.close();document.querySelector('#actual').onclick=()=>full.style.maxWidth='none';document.querySelector('#fit').onclick=()=>full.style.maxWidth='85vw';
document.querySelector('#export').onclick=()=>{const records=[...document.querySelectorAll('article')].map(card=>({...byId.get(card.dataset.id),decision:card.querySelector('select').value,notes:card.querySelector('textarea').value}));const blob=new Blob([JSON.stringify({format:'kyoei-localized-texture-review-v1',images:records},null,2)],{type:'application/json'}),url=URL.createObjectURL(blob),a=document.createElement('a');a.href=url;a.download='kyoei-texture-review.json';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000)};
</script></html>'''
    if a.previous:page=page.replace('grid-template-columns:1fr 1fr;', 'grid-template-columns:repeat(3,minmax(0,1fr));')
    page=page.replace("select.value=['pending','accept','revise','defer'].includes(old.decision)?old.decision:'pending'", "select.value=row.decision==='accept'?'accept':['pending','accept','revise','defer'].includes(old.decision)?old.decision:row.decision")
    (out/'index.html').write_text(page,encoding='utf-8')
    (out/'inventory.json').write_text(json.dumps(rows,indent=2)+'\n',encoding='utf-8')
    print(json.dumps(dict(gallery=str(out/'index.html'),textures=len(rows),images=len(rows)*(3 if a.previous else 2),review_status='pending')))

if __name__=='__main__':main()
