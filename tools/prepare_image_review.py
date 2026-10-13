"""Inventory every TEX; restore/preview only understanding-related UI assets."""
import argparse
import hashlib
import html
import json
from pathlib import Path
import subprocess
import sys
from tex_preview import preview

ROOT=Path(__file__).resolve().parents[1]
GROUPS={'game_title','menu_option','hud','controller_operation','loading','game_over',
 'pause','savedata','stage_clear','intermission','item_reading','play_menu','stage_title',
 'menu_item','menu_mail','menu_mission','menu_collection','menu_hair','treasure_change','2D'}
def scope(path):
    parts=path.split('/')
    if len(parts)>=3 and parts[0]=='texture' and parts[1] in GROUPS:
        if any(word in Path(path).stem.lower() for word in ('cloud','background','_back','_bg')):
            return 'ui_decorative_review','preserve_unless_text_present'
        return 'ui_or_story_review','check_visible_text'
    return 'environment_or_nontext','excluded_by_default'

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('output',type=Path)
    parser.add_argument('--restore',action='store_true')
    parser.add_argument('--preview',action='store_true')
    args=parser.parse_args(); output=args.output.resolve()
    ledger=ROOT/'work/m1/catalog.json'; expected=json.loads((ROOT/'docs/m1/formats.json').read_text())['catalog_sha256']
    if hashlib.sha256(ledger.read_bytes()).hexdigest()!=expected: raise ValueError('M1 catalogue changed')
    catalog=json.loads(ledger.read_text(encoding='utf8'))
    if output.is_relative_to(ROOT) or output.is_relative_to(Path(catalog['source']).resolve()):
        raise ValueError('Original image previews must remain outside repository/dump')
    output.mkdir(parents=True,exist_ok=True)
    assets=[]; selected={}
    for archive in catalog['archives']:
        for row in archive['files']:
            if not row['path'].endswith('.tex'): continue
            group,policy=scope(row['path']); identifier=hashlib.sha256((archive['archive']+'/'+row['path']).encode()).hexdigest()[:16]
            asset=dict(id=identifier,archive=archive['archive'],resource=row['path'],source_sha256=row['sha256'],
                stored_bytes=row['bytes'],category_hint=group,policy=policy,classification_verified=False,
                text_review='pending' if group!='environment_or_nontext' else 'out_of_scope',
                ai_edit_status='not_started',reinsertion_verified=False)
            assets.append(asset)
            if group!='environment_or_nontext': selected.setdefault(archive['archive'],[]).append(row['path'])
    for archive,entries in selected.items():
        entries_file=output/(archive+'.entries.json'); entries_file.write_text(json.dumps(entries,indent=2)+'\n')
        if args.restore:
            subprocess.run([sys.executable,str(ROOT/'tools/restore_m1.py'),archive,'--entries-file',str(entries_file)],check=True)
    preview_dir=output/'previews'; preview_dir.mkdir(exist_ok=True)
    reviewed=[]; errors=[]
    for number,asset in enumerate((asset for asset in assets if asset['text_review']=='pending'),1):
        path=Path(catalog['extraction'])/Path(asset['archive']).stem/asset['resource']
        if path.exists() and args.preview:
            data=path.read_bytes()
            if hashlib.sha256(data).hexdigest()!=asset['source_sha256']: raise ValueError('Restored image hash differs')
            try:
                image,info=preview(data); asset.update(info)
                image.thumbnail((640,640)); image.save(preview_dir/(asset['id']+'.png'))
                asset['preview']='previews/'+asset['id']+'.png'; asset['preview_status']='decoded_'+info['decoded_format']
            except ValueError as error:
                asset['preview_status']='unsupported_format'; errors.append(dict(id=asset['id'],resource=asset['resource'],error=str(error)))
        else: asset['preview_status']='not_generated'
        reviewed.append(asset)
        if number%200==0: print(f'Interface previews processed: {number}',flush=True)
    (output/'all-textures.json').write_text(json.dumps(assets,indent=2)+'\n',encoding='utf8')
    (output/'interface-images.json').write_text(json.dumps(reviewed,indent=2)+'\n',encoding='utf8')
    cards=[]
    for asset in reviewed:
        image=f'<img loading="lazy" src="{html.escape(asset["preview"])}">' if 'preview' in asset else '<p>Preview unavailable</p>'
        cards.append(f'<article data-id="{asset["id"]}">{image}<h3>{html.escape(asset["resource"])}</h3>'
            f'<p>{html.escape(asset["category_hint"])} · {asset.get("width","?")}×{asset.get("height","?")}</p>'
            '<select><option value="pending">Review needed</option><option value="translate">Translate UI/story text</option>'
            '<option value="preserve">Preserve art / Japanese setting</option></select></article>')
    page='''<!doctype html><meta charset="utf-8"><title>Kyoei interface translation review</title>
<style>body{background:#161b23;color:#eee;font:15px sans-serif;margin:24px}main{display:grid;grid-template-columns:repeat(auto-fill,minmax(280px,1fr));gap:18px}article{padding:14px;background:#252d39;border-radius:8px;overflow:hidden}img{max-width:100%;max-height:250px;object-fit:contain;background:#555}h3{overflow-wrap:anywhere;font-size:14px}select{width:100%;padding:8px}</style>
<h1>City Shrouded in Shadow: interface review</h1><p>Local previews only. Selecting a review label stores it in this browser; it does not modify the game. Preserve scenery advertisements and signs.</p><main>'''+''.join(cards)+'''</main><script>document.querySelectorAll('article').forEach(a=>{let s=a.querySelector('select'),k='kyoei-review-'+a.dataset.id;s.value=localStorage.getItem(k)||'pending';s.onchange=()=>localStorage.setItem(k,s.value)})</script>'''
    (output/'review.html').write_text(page,encoding='utf8')
    report=dict(private_output=str(output),textures_in_catalog=len(assets),interface_review_candidates=len(reviewed),
        excluded_environment_or_nontext=len(assets)-len(reviewed),candidate_stored_bytes=sum(a['stored_bytes'] for a in reviewed),
        decoded_previews=sum('preview' in a for a in reviewed),unsupported_previews=errors,
        japanese_ocr_available=False,all_visible_text_identified=False,ai_edits_applied=False)
    target=ROOT/'docs/m4'; target.mkdir(exist_ok=True)
    (target/'image-inventory.json').write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    print(json.dumps({k:v for k,v in report.items() if k!='unsupported_previews'},indent=2))

if __name__=='__main__': main()
