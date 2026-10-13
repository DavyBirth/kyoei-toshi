"""Refresh the private image gallery with portable review import/export."""
import argparse
import html
import json
from pathlib import Path
from text_tool import output_path

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('inventory',type=Path)
    parser.add_argument('--output',type=Path)
    args=parser.parse_args(); target=output_path(args.output or args.inventory.parent/'review-portable.html')
    assets=json.loads(args.inventory.read_text(encoding='utf8')); cards=[]
    for asset in assets:
        picture=f'<img loading="lazy" src="{html.escape(asset["preview"],quote=True)}">' if 'preview' in asset else '<p>Prévia indisponível; formato pendente.</p>'
        cards.append(f'<article data-id="{html.escape(asset["id"],quote=True)}">{picture}<h3>{html.escape(asset["resource"])}</h3>'
            f'<p>{asset.get("width","?")}×{asset.get("height","?")}</p>'
            '<select><option value="pending">Revisão pendente</option><option value="translate">Traduzir texto de interface/trama</option><option value="preserve">Preservar imagem</option></select><textarea placeholder="Contexto e observações"></textarea></article>')
    metadata=[{k:a[k] for k in ('id','archive','resource','source_sha256','width','height') if k in a} for a in assets]
    serialized=json.dumps(metadata,ensure_ascii=False).replace('<','\\u003c')
    script=r'''
const assets=JSON.parse(document.querySelector('#assets').textContent), known=new Map(assets.map(a=>[a.id,a]));
const labels=['pending','translate','preserve'];
function key(id){return 'kyoei-image-review-'+known.get(id).source_sha256+'-'+id}
function save(card){localStorage.setItem(key(card.dataset.id),JSON.stringify({decision:card.querySelector('select').value,notes:card.querySelector('textarea').value}))}
for(const card of document.querySelectorAll('article')){
 let old={};try{old=JSON.parse(localStorage.getItem(key(card.dataset.id))||'{}')}catch(e){}
 card.querySelector('select').value=labels.includes(old.decision)?old.decision:'pending';card.querySelector('textarea').value=old.notes||'';
 card.querySelector('select').onchange=()=>{save(card);filter()};card.querySelector('textarea').oninput=()=>save(card);
}
function filter(){let count=0;const query=document.querySelector('#filter').value.toLowerCase(),pending=document.querySelector('#pending').checked;
 for(const card of document.querySelectorAll('article')){card.hidden=!card.querySelector('h3').textContent.toLowerCase().includes(query)||(pending&&card.querySelector('select').value!=='pending');if(!card.hidden)++count}
 document.querySelector('#count').textContent=count+' imagens exibidas';}
document.querySelector('#filter').oninput=filter;document.querySelector('#pending').onchange=filter;filter();
document.querySelector('#export').onclick=()=>{
 const records=[...document.querySelectorAll('article')].map(card=>({...known.get(card.dataset.id),decision:card.querySelector('select').value,notes:card.querySelector('textarea').value}));
 const result={format:'kyoei-image-review-v1',title:'CUSA03562',version:'01.03',images:records};
 const url=URL.createObjectURL(new Blob([JSON.stringify(result,null,2)+'\n'],{type:'application/json'})),a=document.createElement('a');
 a.href=url;a.download='kyoei-image-review.json';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);
};
document.querySelector('#import').onchange=async function(){try{
 const doc=JSON.parse(await this.files[0].text());
 if(doc.format!=='kyoei-image-review-v1'||doc.title!=='CUSA03562'||doc.version!=='01.03'||!Array.isArray(doc.images))throw Error('Documento diferente');
 const seen=new Set();for(const row of doc.images){const source=known.get(row.id);if(!source||seen.has(row.id)||!labels.includes(row.decision)||typeof row.notes!=='string'||['archive','resource','source_sha256','width','height'].some(k=>row[k]!==source[k]))throw Error('Imagem, hash ou classificação diferente');seen.add(row.id)}
 for(const row of doc.images)localStorage.setItem(key(row.id),JSON.stringify({decision:row.decision,notes:row.notes}));location.reload();
}catch(e){alert('Importação recusada: '+e.message)}this.value='';};
'''
    page='''<!doctype html><html lang="pt-BR"><meta charset="utf-8"><title>Kyoei — imagens para revisão</title>
<style>body{background:#161b23;color:#eee;font:15px system-ui;margin:24px}header{position:sticky;top:0;background:#161b23;padding:12px;z-index:1}main{display:grid;grid-template-columns:repeat(auto-fill,minmax(280px,1fr));gap:18px}article{padding:14px;background:#252d39;border-radius:8px;overflow:hidden}article[hidden]{display:none}img{max-width:100%;max-height:250px;object-fit:contain;background:#555}h3{overflow-wrap:anywhere;font-size:14px}select,textarea{width:100%;box-sizing:border-box;padding:8px;margin:6px 0}button,input{padding:8px;margin:4px}</style>
<header><h1>Imagens da interface</h1><p>Preservar propagandas e placas do cenário japonês. Esta página não modifica o jogo. Exporte o JSON para guardar a revisão fora do navegador.</p>
<input id="filter" placeholder="Filtrar caminho"><label><input id="pending" type="checkbox">Só pendentes</label><button id="export">Exportar revisão</button><label>Importar <input id="import" type="file" accept=".json"></label><p id="count"></p></header><main>'''+''.join(cards)+'''</main><script type="application/json" id="assets">'''+serialized+'''</script><script>'''+script+'''</script></html>'''
    target.write_text(page,encoding='utf8'); print(f'Review gallery refreshed: {len(assets)} images; {target}')

if __name__=='__main__': main()
