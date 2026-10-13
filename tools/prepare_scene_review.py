"""Build private per-resource review pages; exports contain translations only."""
import argparse
import collections
import hashlib
import html
import json
import re
from pathlib import Path
from text_tool import REPO,output_path,export
from text_formats import JAPANESE

SCRIPT=r'''
const data=JSON.parse(document.getElementById('scene-data').textContent);
const savedKey='kyoei-scene-'+data.source_sha256;
let saved={};try{saved=JSON.parse(localStorage.getItem(savedKey)||'{}')}catch(e){}
const container=document.querySelector('main');
function save(){try{localStorage.setItem(savedKey,JSON.stringify(saved))}catch(e){document.querySelector('#status').textContent='Não foi possível salvar no navegador. Exporte o JSON.'}}
for(const row of data.entries){
 const card=document.createElement('article');card.dataset.id=row.id;
 const title=document.createElement('h3');title.textContent=row.id+' · '+row.tokens.join(' ');card.append(title);
 const original=document.createElement('p');original.className='original';original.textContent=row.original;card.append(original);
 const values=saved[row.id]||{en:row.en||'', 'pt-BR':row['pt-BR']||'',notes:row.notes||'',review:'pending'};
 saved[row.id]=values;
 for(const [key,label] of [['en','English'],['pt-BR','Português do Brasil'],['notes','Contexto / dúvidas / nomes']]){
  const heading=document.createElement('label');heading.textContent=label;
  const input=document.createElement('textarea');input.value=values[key]||'';
  input.oninput=()=>{values[key]=input.value;values.review='pending';select.value='pending';save()};
  heading.append(input);card.append(heading);
 }
 const select=document.createElement('select');
 for(const [value,label] of [['pending','Revisão pendente'],['context','Falta contexto'],['reviewed','Revisão textual concluída']]){
  const option=document.createElement('option');option.value=value;option.textContent=label;select.append(option);
 }
 select.value=values.review||'pending';select.onchange=()=>{values.review=select.value;save()};card.append(select);container.append(card);
}
document.querySelector('#filter').oninput=function(){
 const query=this.value.toLowerCase();for(const card of container.children)card.hidden=!card.textContent.toLowerCase().includes(query);
};
document.querySelector('#export').onclick=()=>{
 const entries=[];
 for(const row of data.entries){const value=saved[row.id];if(!value.en && !value['pt-BR'])continue;
  if(!value.en || !value['pt-BR']){alert('Preencha os dois idiomas antes de exportar: '+row.id);return;}
  entries.push({id:row.id,source_text_sha256:row.source_text_sha256,en:value.en,'pt-BR':value['pt-BR'],notes:value.notes,editorial_status:value.review});
 }
 const draft={format:'kyoei-translation-draft-v1',title:'CUSA03562',version:'01.03',status:'draft_requires_human_review',runtime_verified:false,
  resources:[{path:data.resource,source_sha256:data.source_sha256,entries}]};
 const url=URL.createObjectURL(new Blob([JSON.stringify(draft,null,2)+'\n'],{type:'application/json'}));
 const a=document.createElement('a');a.href=url;a.download=data.filename+'.draft.json';a.click();setTimeout(()=>URL.revokeObjectURL(url),1000);
};
document.querySelector('#import').onchange=async function(){try{
 const draft=JSON.parse(await this.files[0].text());
 if(draft.format!=='kyoei-translation-draft-v1'||draft.title!=='CUSA03562'||draft.version!=='01.03'||draft.resources.length!==1)throw Error('Formato ou jogo diferente');
 const resource=draft.resources[0];if(resource.path!==data.resource||resource.source_sha256!==data.source_sha256)throw Error('Recurso ou versão diferente');
 const known=new Map(data.entries.map(row=>[row.id,row])),seen=new Set();
 for(const row of resource.entries){const source=known.get(row.id);if(!source||seen.has(row.id)||row.source_text_sha256!==source.source_text_sha256||typeof row.en!=='string'||typeof row['pt-BR']!=='string')throw Error('IDs, textos ou campos inválidos');seen.add(row.id);}
 for(const row of resource.entries)saved[row.id]={en:row.en,'pt-BR':row['pt-BR'],notes:row.notes||'',review:row.editorial_status||'pending'};
 save();location.reload();
}catch(e){alert('Importação recusada: '+e.message)}this.value='';};
'''

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output',type=Path); parser.add_argument('--all-stages',action='store_true')
    args=parser.parse_args(); output=output_path(args.output)
    if output.exists(): raise ValueError('Choose a new private directory')
    audit=json.loads((REPO/'docs/m2/audit.json').read_text(encoding='utf8'))
    pilot={}
    for path in (REPO/'translations').glob('pilot-*.json'):
        draft=json.loads(path.read_text(encoding='utf8'))
        for resource in draft['resources']:
            entries=pilot.setdefault(resource['path'],{})
            for entry in resource['entries']:
                if entry['id'] in entries: raise ValueError('Duplicate pilot entry')
                entries[entry['id']]=entry
    output.mkdir(parents=True); reports=[]; links=[]
    for row in audit['records']:
        if row['path'] not in pilot and not (args.all_stages and 'text_stage_' in row['path'].lower()): continue
        data=(Path(audit['source'])/row['path']).read_bytes()
        if hashlib.sha256(data).hexdigest()!=row['sha256']: raise ValueError('M2 source changed')
        canonical=export(data); entries=[];internal=[]
        for string in canonical['strings']:
            if not JAPANESE.search(string['original']): continue
            if re.match(r'^(?:テスト用|主人公・[男女][＿_])',string['original']):
                internal.append(string['id']);continue
            from prepare_translation_catalog import TOKENS
            entry=dict(id=string['id'],original=string['original'],
                       source_text_sha256=hashlib.sha256(string['original'].encode('utf8')).hexdigest(),
                       tokens=TOKENS.findall(string['original']))
            entry.update({k:v for k,v in pilot.get(row['path'],{}).get(string['id'],{}).items() if k!='id'})
            entries.append(entry)
        filename=hashlib.sha256(row['path'].encode('utf8')).hexdigest()[:16]
        info=dict(resource=row['path'],source_sha256=row['sha256'],filename=filename,entries=entries)
        serialized=json.dumps(info,ensure_ascii=False).replace('<','\\u003c').replace('&','\\u0026')
        page='''<!doctype html><html lang="pt-BR"><meta charset="utf-8"><title>Kyoei — revisão de tradução</title>
<style>body{font:16px system-ui;background:#151b25;color:#eee;margin:24px}header{position:sticky;top:0;background:#151b25;padding:12px}article{background:#242e3f;padding:18px;margin:18px 0;border-radius:8px}.original{white-space:pre-wrap;font-size:20px}label{display:block;margin:12px 0}textarea{display:block;box-sizing:border-box;width:100%;min-height:80px;font:16px system-ui}button,input,select{padding:8px;margin:5px}h3{font-size:14px;color:#b6c9e0}</style>
<header><h1>Revisão de tradução</h1><p>'''+html.escape(row['path'])+'''</p>
<p>Não altera o jogo. Revisão textual não certifica layout. Guarde o JSON exportado; o navegador sozinho não é backup.</p>
<input id="filter" placeholder="Filtrar texto ou ID"><button id="export">Exportar rascunho EN + PT-BR</button><label>Importar rascunho <input id="import" type="file" accept=".json"></label><p id="status"></p></header><main></main>
<script type="application/json" id="scene-data">'''+serialized+'''</script><script>'''+SCRIPT+'''</script></html>'''
        (output/(filename+'.html')).write_text(page,encoding='utf8')
        reports.append(dict(resource=row['path'],source_sha256=row['sha256'],entries=len(entries),private_page=filename+'.html',excluded_internal_ids=internal))
        links.append(f'<li><a href="{filename}.html">{html.escape(row["path"])}</a> — {len(entries)} trechos</li>')
    (output/'index.html').write_text('<!doctype html><meta charset="utf-8"><h1>Kyoei: cenas para revisão</h1><p>Originais privados; rascunhos ainda precisam de contexto e validação.</p><ul>'+''.join(links)+'</ul>',encoding='utf8')
    (REPO/'docs/m4/scene-review.json').write_text(json.dumps(dict(private_output=str(output),resources=reports,
        layout_verified=False,chronological_order_verified=False),indent=2)+'\n',encoding='utf8')
    print(f'Private review pages: {len(reports)}; entries: {sum(r["entries"] for r in reports)}; output: {output}')

if __name__=='__main__': main()
