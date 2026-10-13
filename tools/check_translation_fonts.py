"""Check fallback font cmaps offline; does not certify the game's own renderer."""
import hashlib
import json
from pathlib import Path
from fontTools.ttLib import TTFont

ROOT=Path(__file__).resolve().parents[1]

def main():
    required=set(chr(i) for i in range(32,127))
    required.update('ÀÁÂÃÄÅàáâãäåÈÉÊËèéêëÌÍÎÏìíîïÒÓÔÕÖòóôõöÙÚÛÜùúûüÇçÑñ“”‘’…–—')
    for path in (ROOT/'translations').glob('pilot-*.json'):
        draft=json.loads(path.read_text(encoding='utf8'))
        for resource in draft['resources']:
            for entry in resource['entries']:
                for language in ('en','pt-BR'):
                    value=entry.get(language)
                    if value is not None:
                        if not isinstance(value,str): raise ValueError('Invalid translation field')
                        required.update(value)
    required.difference_update('\n\r\t')
    fonts=ROOT/'third_party/godzilla-core/third_party/shadPS4/src/imgui/renderer/fonts'
    rows=[]; combined=set()
    for name in ('NotoSans-Regular.ttf','NotoSansCJK-Regular.ttc'):
        path=fonts/name
        with TTFont(path,fontNumber=0,lazy=True) as font:
            available=set((font.getBestCmap() or {}).keys()); combined.update(available)
        missing=sorted(c for c in required if ord(c) not in available)
        rows.append(dict(path=path.relative_to(ROOT).as_posix(),collection_face=0,
            sha256=hashlib.sha256(path.read_bytes()).hexdigest(),missing=missing,
            covered=len(required)-len(missing),required=len(required)))
    report=dict(scope='Configured fallback font files only; game font/layout unverified',
        required_characters=''.join(sorted(required)),fonts=rows,
        combined_missing=sorted(c for c in required if ord(c) not in combined),
        runtime_verified=False)
    target=ROOT/'docs/m3/fallback-coverage.json'; target.parent.mkdir(parents=True,exist_ok=True)
    target.write_text(json.dumps(report,ensure_ascii=False,indent=2)+'\n',encoding='utf8')
    print(json.dumps(report,indent=2))
    if report['combined_missing']: raise SystemExit(1)

if __name__=='__main__': main()
