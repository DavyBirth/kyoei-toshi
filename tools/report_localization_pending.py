"""Triage empty localization entries without changing them or exporting original prose.

These are content-type candidates, not evidence that a string is unreachable.
No adjusted completion percentage is inferred from them.
"""
import argparse,json,re
from collections import Counter
from pathlib import Path

def classify(entry):
    text=entry['original'];category=entry.get('category','unknown')
    if text=='仮登録':return 'temporary_registration_placeholder'
    if re.fullmatch(r'(見出し|本文)[０-９0-9]+',text):return 'news_template_placeholder'
    if category=='credits':return 'credits_require_name_review'
    if any(tag in text for tag in ('[開発中]','【開発メッセージ】','※開発用','実装遅れ','現在作成中')):
        return 'explicit_development_annotation'
    if re.fullmatch(r'(テスト[０-９0-9]*|ステージ[０-９0-9]+(?:タイトル|テキスト)|目標地点[０-９0-9]+|進捗テキスト[０-９0-9]+|場所[０-９0-9]+|説明[０-９0-9]+)',text):
        return 'numbered_test_placeholder'
    if '（入手不可）' in text or '（仮）' in text or '（仮仮）' in text:return 'provisional_or_unobtainable_candidate'
    if re.fullmatch(r'.+[＿_][０-９0-9]+',text):return 'technical_label_candidate'
    return 'requires_individual_review'

def main():
    p=argparse.ArgumentParser(description=__doc__);p.add_argument('workspace',type=Path);p.add_argument('--out',type=Path,required=True);a=p.parse_args()
    counts=Counter();rows=[];files=sorted((a.workspace/'working').glob('translate-en-*.json'))
    if len(files)!=173:raise ValueError('Expected the complete 173-file workspace')
    for file in files:
        for resource in json.loads(file.read_text(encoding='utf-8'))['resources']:
            for entry in resource['entries']:
                if entry.get('en'):continue
                group=classify(entry);counts[group]+=1
                rows.append(dict(file=file.name,resource=resource['path'],id=entry['id'],
                    source_text_sha256=entry['source_text_sha256'],category=entry.get('category'),triage=group))
    report=dict(empty_entries=len(rows),groups=dict(counts),
        note='Triage only. Provisional/technical/development candidates are not proved unreachable. Credits and other player-facing gaps remain under review; no adjusted completion percentage.',entries=rows)
    a.out.parent.mkdir(parents=True,exist_ok=True);a.out.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf-8')
    print(json.dumps({k:v for k,v in report.items() if k!='entries'}))
if __name__=='__main__':main()
