"""Update only a closed private portable's runtime after a verified rebuild."""
import json
from pathlib import Path
from build_portable import ROOT, clean_pe, digest


def main():
    portable=ROOT/'build/City-Shrouded-in-Shadow-Portable'
    manifest_path=portable/'manifest.json'
    manifest=json.loads(manifest_path.read_text(encoding='utf8'))
    if (manifest['title'],manifest['version'])!=('CUSA03562','01.03'):
        raise ValueError('Wrong portable revision')
    for name in ('runtime/kyoei.exe','City Shrouded in Shadow.exe'):
        with (portable/name).open('r+b'):pass
    with (portable/'data/session.lock').open('a+b'):
        temporary=portable/'runtime/kyoei-update.tmp'
        clean_pe(ROOT/'build/kyoei-gow3.exe',temporary)
        temporary.replace(portable/'runtime/kyoei.exe')
        for row in manifest['files']:
            if row['path']=='runtime/kyoei.exe':
                target=portable/row['path']
                row.update(bytes=target.stat().st_size,sha256=digest(target))
        manifest.update(privacy_scan_passed=False,updated_since_finalization=True)
        temp_manifest=manifest_path.with_suffix('.json.tmp')
        temp_manifest.write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf8')
        temp_manifest.replace(manifest_path)
    print('Portable runtime updated; existing saves and language data preserved')


if __name__=='__main__':main()
