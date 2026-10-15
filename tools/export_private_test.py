"""Export a clean private test folder, excluding every writable user artifact."""
import hashlib
import json
import os
import shutil
from pathlib import Path
from build_portable import ROOT, personal_text_hits


def main():
    source=ROOT/'build/City-Shrouded-in-Shadow-Portable'
    target=ROOT/'build/City-Shrouded-in-Shadow-Friends-Test'
    if target.exists():raise ValueError('Choose a fresh export; existing output is preserved')
    manifest=json.loads((source/'manifest.json').read_text(encoding='utf8'))
    inventory=json.loads((ROOT/'docs/m0/inventory.json').read_text(encoding='utf8'))
    originals={r['path'].replace('\\','/'):r['sha256'] for r in inventory['files']}
    tokens={b'davy',b'davyd',b'codex',b'claude',b'chatgpt',b'openai'}
    for value in (os.environ.get('USERNAME',''),Path(os.environ.get('USERPROFILE','')).name):
        if len(value)>2:tokens.add(value.lower().encode())
    records=[];findings=[];coincidences=0
    target.mkdir()
    for row in manifest['files']:
        name=row['path'];relative=Path(name)
        if relative.is_absolute() or '..' in relative.parts or name.startswith('data/'):
            raise ValueError('Unexpected manifest path')
        before,after=source/relative,target/relative
        if not before.is_file() or before.stat().st_size!=row['bytes']:raise ValueError('Source differs: '+name)
        after.parent.mkdir(parents=True,exist_ok=True)
        if after.suffix=='.psarc':os.link(before,after)
        else:shutil.copyfile(before,after)
        h=hashlib.sha256();tail=b''
        immutable=name.startswith('game/') and after.suffix=='.psarc'
        with after.open('rb') as f:
            for chunk in iter(lambda:f.read(8<<20),b''):
                h.update(chunk)
                if not immutable:
                    hits,random=personal_text_hits(tail+chunk,list(tokens),after.suffix in {'.txt','.json','.sh','.tsv','.ini'})
                    if hits:findings.append(dict(path=name,tokens=hits))
                    coincidences+=random;tail=chunk[-256:]
        checksum=h.hexdigest()
        if checksum!=row['sha256']:raise ValueError('Source hash differs: '+name)
        if name.startswith('game/') and originals.get(name[5:])!=checksum:
            raise ValueError('Game file differs from own dump: '+name)
        records.append(dict(path=name,bytes=after.stat().st_size,sha256=checksum))
    proof=ROOT/'build/friends-export-privacy.json'
    report=dict(passed=not findings,findings=findings,files=len(records),
                compressed_binary_coincidences=coincidences,personal_data_copied=False)
    proof.write_text(json.dumps(report,indent=2)+'\n',encoding='utf8')
    if findings:raise ValueError('Privacy scan failed; inspect private report, do not share output')
    manifest.update(files=records,personal_saves_included=False,host_configuration_included=False,
        privacy_scan_passed=True,updated_since_finalization=False,
        privacy_scope='Clean export: all file hashes verified; original game matches disc inventory. No saves, logs, settings, account names, caches or host source/debug paths. Personal identifiers scanned in text, UTF16 and non-original binaries; compressed coincidences recorded separately.')
    (target/'manifest.json').write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf8')
    print(json.dumps(dict(output=str(target),**report)),flush=True)


if __name__=='__main__':main()
