"""Assemble the owner's private offline test copy. Never add its output to Git."""
import argparse, hashlib, json, os, re, shutil, struct, subprocess, sys, tempfile
from pathlib import Path

ROOT=Path(__file__).resolve().parents[1]

def digest(path):
    h=hashlib.sha256()
    with path.open('rb') as f:
        for chunk in iter(lambda:f.read(8<<20),b''):h.update(chunk)
    return h.hexdigest()

def clean_pe(source, target):
    """Erase build-only debug records and source-file diagnostics, not code.

    Existing imports, code, relocations and file offsets stay byte-identical.
    No source names or account identifiers are needed to run the binary.
    """
    data=bytearray(source.read_bytes()); pe=struct.unpack_from('<I',data,0x3c)[0]
    if data[pe:pe+4]!=b'PE\0\0':raise ValueError('Expected PE')
    sections=struct.unpack_from('<H',data,pe+6)[0]; opt_size=struct.unpack_from('<H',data,pe+20)[0];opt=pe+24
    if struct.unpack_from('<H',data,opt)[0]!=0x20b:raise ValueError('Expected x64 PE')
    table=opt+opt_size; regions=[]
    for n in range(sections):
        s=table+n*40; vs,va,rs,raw=struct.unpack_from('<IIII',data,s+8);flags=struct.unpack_from('<I',data,s+36)[0]
        regions.append((va,vs,raw,rs,flags))
    debug_rva,debug_size=struct.unpack_from('<II',data,opt+112+6*8)
    for va,vs,raw,rs,flags in regions:
        if va<=debug_rva<va+max(vs,rs):
            debug=raw+debug_rva-va
            for p in range(debug,debug+debug_size,28):
                size=struct.unpack_from('<I',data,p+16)[0]
                ptr=struct.unpack_from('<I',data,p+24)[0]
                if ptr+size>len(data):raise ValueError('Bad PE debug range')
                data[ptr:ptr+size]=b'\0'*size
            data[debug:debug+debug_size]=b'\0'*debug_size
    struct.pack_into('<II',data,opt+112+6*8,0,0)
    struct.pack_into('<I',data,opt+64,0) # checksum after removing optional debug metadata
    scrubbed=0
    for va,vs,raw,rs,flags in regions:
        if not flags&0x40000000 or flags&0xa0000000:continue # read-only, non-code only
        for match in re.finditer(rb'[ -~]{8,}\x00',bytes(data[raw:raw+rs])):
            value=match.group()[:-1]
            if re.search(rb'[A-Za-z]:[/\\].*\.(?:cpp|hpp|c|h|pdb|ipp|inl|cc|hh|hxx|tpp)$',value,re.I):
                name=re.split(rb'[/\\]',value)[-1];replacement=b'source/'+name
                if len(replacement)<=len(value):
                    begin=raw+match.start();data[begin:begin+len(value)]=replacement.ljust(len(value),b' ');scrubbed+=1
    target.write_bytes(data)
    return scrubbed

def copy_file(source,target):
    target.parent.mkdir(parents=True,exist_ok=True)
    if not target.exists() or source.stat().st_size!=target.stat().st_size or source.stat().st_mtime_ns>target.stat().st_mtime_ns:
        # Atomic replacement also preserves clean exports sharing immutable
        # archive links with this local test copy.
        with tempfile.NamedTemporaryFile(dir=target.parent,prefix=target.name+'.',suffix='.copying',delete=False) as f:
            temporary=Path(f.name)
        try:
            shutil.copyfile(source,temporary)
            temporary.replace(target)
        finally:
            if temporary.exists():temporary.unlink()

def copy_reviewed_delta(record,target):
    """Prefer a roundtrip-verified direct build over an older full preview."""
    if not record.get('delta'):return False
    source=ROOT/record['delta']
    if not source.is_file() or digest(source)!=record.get('delta_sha256'):
        raise ValueError('Reviewed language delta is missing or changed; rebuild it first')
    if not target.exists() or digest(target)!=record['delta_sha256']:
        target.parent.mkdir(parents=True,exist_ok=True)
        with tempfile.NamedTemporaryFile(dir=target.parent,prefix=target.name+'.',suffix='.copying',delete=False) as stream:
            temporary=Path(stream.name)
        try:
            shutil.copyfile(source,temporary)
            if digest(temporary)!=record['delta_sha256']:raise ValueError('Reviewed delta copy differs')
            temporary.replace(target)
        finally:
            if temporary.exists():temporary.unlink()
    return True

def personal_text_hits(data,tokens,text_file=False):
    """Find identifiers in text, not coincidental case-folded compressed bytes."""
    lower=data.lower();hits=[];binary_coincidences=0
    for token in tokens:
        start=0
        while (position:=lower.find(token,start))>=0:
            start=position+len(token)
            left=position
            while left and 32<=data[left-1]<=126:left-=1
            right=start
            while right<len(data) and 32<=data[right]<=126:right+=1
            nul_bounded=(left==0 or data[left-1]==0) and (right==len(data) or data[right]==0)
            if text_file or right-left>=12 or nul_bounded:hits.append(token.decode())
            else:binary_coincidences+=1
        if token.decode().encode('utf-16-le') in lower:hits.append(token.decode()+':utf16')
    return hits,binary_coincidences

def main():
    ap=argparse.ArgumentParser(description=__doc__);ap.add_argument('output',type=Path);ap.add_argument('--finalize',action='store_true');a=ap.parse_args()
    out=a.output.resolve()
    if not out.is_relative_to(ROOT/'build') or out==ROOT/'build':raise ValueError('Private output must be a child of ignored build/')
    # A loaded PE cannot be opened for writing on Windows. Check both before
    # copying anything, including archives that a running game may be reading.
    for name in ['runtime/kyoei.exe','City Shrouded in Shadow.exe']:
        current=out/name
        if current.exists():
            try:
                with current.open('r+b'):pass
            except PermissionError as error:
                raise RuntimeError('Close the game and portable launcher before updating; no files copied: '+name) from error
    # Keep this handle alive for all of main(). The launcher uses FileShare.None,
    # so it cannot start another session while an existing private copy updates.
    profile_guard=None
    if (out/'data').is_dir():
        try:profile_guard=(out/'data/session.lock').open('a+b')
        except PermissionError as error:
            raise RuntimeError('Portable session is active; no files copied') from error
    proof=json.loads((ROOT/'docs/native-prepare.json').read_text(encoding='utf-8'))
    publication=json.loads((ROOT/'docs/m4/runtime-preview.json').read_text(encoding='utf-8'))
    preview=publication['languages']['en']
    game=Path(proof['source']);prepared=Path(proof['prepared_out'])
    from prepare_portable_image import validate_prepared_stages
    validate_prepared_stages(prepared)
    out.mkdir(parents=True,exist_ok=True)
    for source in game.rglob('*'):
        if source.is_file():copy_file(source,out/'game'/source.relative_to(game))
    for name in ['boot-linked.bin','imports.tsv','deferred.tsv','tls-sites.tsv']:
        copy_file(prepared/name,out/'runtime/out'/name)
    english=Path(preview['archive'])
    # Return to Desktop title item, drawn in the native menu style from the dump.
    subprocess.run([str(ROOT/'build/image-tools/Scripts/python.exe'),str(ROOT/'tools/make_title_exit_art.py'),
                    str(out/'game/DAT/common.psarc'),str(out/'assets')],check=True)
    # Paired button payloads switch live; UI corrections stay a read-time delta.
    imaging=ROOT/'build/image-tools/Scripts/python.exe'
    # The whole translation (archive resources + HUD corrections) is one small
    # read-time delta over the untouched disc archive; no 6 GB English copy.
    delta=out/'languages/en/translation.kydelta'
    if not copy_reviewed_delta(preview,delta):
        overlay=ROOT/'build/ui-corrections-en.kydelta';overlay.unlink(missing_ok=True)
        subprocess.run([str(sys.executable),str(ROOT/'tools/build_ui_corrections.py'),str(english),str(overlay)],check=True)
        delta.unlink(missing_ok=True)
        subprocess.run([str(sys.executable),str(ROOT/'tools/build_translation_delta.py'),str(out/'game/DAT/common.psarc'),
                        str(english),str(delta),'--overlay',str(overlay)],check=True)
    for obsolete in ('common.psarc','ui-corrections.kydelta'):(out/'languages/en'/obsolete).unlink(missing_ok=True)
    # Partial pt-BR preview (English fallback) when one has been built.
    portuguese=publication['languages'].get('pt-BR')
    if portuguese and (portuguese.get('delta') or Path(portuguese['archive']).exists()):
        subprocess.run([str(imaging if imaging.exists() else sys.executable),str(ROOT/'tools/make_title_exit_art.py'),
                        str(out/'game/DAT/common.psarc'),str(out/'languages/pt-BR'),'--language','pt-BR'],check=True)
        delta=out/'languages/pt-BR/translation.kydelta'
        if not copy_reviewed_delta(portuguese,delta):
            overlay=ROOT/'build/ui-corrections-pt-BR.kydelta';overlay.unlink(missing_ok=True)
            subprocess.run([str(sys.executable),str(ROOT/'tools/build_ui_corrections.py'),portuguese['archive'],str(overlay),'--language','pt-BR'],check=True)
            delta.parent.mkdir(parents=True,exist_ok=True);delta.unlink(missing_ok=True)
            subprocess.run([str(sys.executable),str(ROOT/'tools/build_translation_delta.py'),str(out/'game/DAT/common.psarc'),
                            portuguese['archive'],str(delta),'--overlay',str(overlay)],check=True)
        subprocess.run([str(imaging if imaging.exists() else sys.executable),str(ROOT/'tools/build_prompt_pack.py'),
                        str(out/'game/DAT/common.psarc'),str(out/'languages/pt-BR/prompts.kypack'),
                        '--delta',str(delta),'--diagrams','--language','pt-BR'],check=True)
    for language,base in (('en',english),('ja',out/'game/DAT/common.psarc')):
        subprocess.run([str(imaging if imaging.exists() else sys.executable),str(ROOT/'tools/build_prompt_pack.py'),
                        str(base),str(out/'languages'/language/'prompts.kypack')]+(['--diagrams'] if language=='en' else []),check=True)
        # Replaced by the verified paired pack above; no runtime reads this.
        obsolete=out/'languages'/language/'keyboard-icons.kydelta'
        if obsolete.exists():obsolete.unlink()
    # English display strings of the executable (translations/binary-en.json), and the
    # game-styled art of the PC screens (photo mode, saves).
    subprocess.run([str(sys.executable),str(ROOT/'tools/build_eboot_text.py'),str(ROOT/'translations/binary-en.json'),
                    str(out/'languages/en/eboot-text.kytext'),'--eboot',str(game/'eboot.bin'),'--elf',str(prepared/'eboot.elf')],check=True)
    if portuguese and Path(portuguese['archive']).exists():
        subprocess.run([str(sys.executable),str(ROOT/'tools/build_eboot_text.py'),str(ROOT/'translations/binary-en.json'),
                        str(out/'languages/pt-BR/eboot-text.kytext'),'--language','pt-BR','--eboot',str(game/'eboot.bin'),'--elf',str(prepared/'eboot.elf')],check=True)
    subprocess.run([str(imaging if imaging.exists() else sys.executable),str(ROOT/'tools/make_ui_art.py'),
                    str(out/'game/DAT/common.psarc'),str(out/'assets/ui-art.kyart')],check=True)
    fonts=ROOT/'third_party/godzilla-core/third_party/shadPS4/src/imgui/renderer/fonts'
    for name in ['NotoSans-Regular.ttf','NotoSansCJK-Regular.ttc']:copy_file(fonts/name,out/'assets/fonts'/name)
    for name in ['artwork.png','game.ico']:copy_file(ROOT/'build/launcher-assets'/name,out/'assets'/name)
    copy_file(ROOT/'build/hud-native-metrics-01.bin',out/'assets/hud-metrics.bin')
    scrubbed=clean_pe(ROOT/'build/kyoei.exe',out/'runtime/kyoei.exe')
    for name in ['msvcp140.dll','msvcp140_atomic_wait.dll','vcruntime140.dll','vcruntime140_1.dll']:
        copy_file(Path(os.environ['WINDIR'])/'System32'/name,out/'runtime'/name)
    copy_file(ROOT/'LICENSE',out/'licenses/GPL-2.0.txt')
    # Preserve upstream license notices without shipping developer workspaces.
    dependencies=Path('G:/godzilla-port-codex/build/_deps')
    for folder in dependencies.glob('*-src'):
        for source in folder.iterdir():
            if source.is_file() and source.name.upper().startswith(('LICENSE','COPYING','COPYRIGHT','OFL')):
                copy_file(source,out/'licenses'/folder.name/source.name)
    for name in ['imgui','shadPS4']:
        folder=ROOT/'third_party/godzilla-core/third_party'/name
        if folder.exists():
            for source in folder.iterdir():
                if source.is_file() and source.name.upper().startswith(('LICENSE','COPYING','COPYRIGHT','OFL')):
                    copy_file(source,out/'licenses'/name/source.name)
    for source in (ROOT/'build/overnight/notices').glob('*.txt'):
        copy_file(source,out/'licenses'/source.name)
    csc=Path(os.environ['WINDIR'])/'Microsoft.NET/Framework64/v4.0.30319/csc.exe'
    subprocess.run([str(csc),'/nologo','/codepage:65001','/target:winexe','/platform:x64','/optimize+',
                    '/r:System.Windows.Forms.dll','/r:System.Drawing.dll','/win32icon:'+str(out/'assets/game.ico'),
                    '/out:'+str(out/'City Shrouded in Shadow.exe'),str(ROOT/'src/kyoei_portable_launcher.cs')],check=True)
    shutil.copyfile(ROOT/'tools/start-on-linux.sh',out/'Start on Linux.sh')
    (out/'README.txt').write_text('CITY SHROUDED IN SHADOW\n\nOpen "City Shrouded in Shadow.exe". Choose English or Japanese.\nKeep all folders together. No installation or administrator account is needed.\n\nWindows 10/11 x64, compatible Vulkan graphics driver and AVX2-capable CPU required.\nThe complete folder must be on NTFS or exFAT (individual files exceed 4 GB).\nSaves and new logs stay in data/. No existing personal saves are included.\n\nKEYBOARD AND MOUSE\nWASD: move. Mouse: camera. Shift: sprint. Ctrl: dodge.\nE / Enter / left click: interact, confirm. Space: back, cancel.\nF: read mail. Q / V: camera view. G / right click: Giant Shadow Camera.\nR / middle click: reset camera. H: HUD. M: map. Esc / Tab: pause menu.\nArrow keys: menus and target selection. F7 releases/recaptures the mouse.\nThe launcher\'s Controls window lists every action, sets mouse sensitivity\nand chooses keyboard or PlayStation button prompts.\nAn SDL-compatible gamepad can also be used.\n\nEnglish localization and runtime are experimental. Some text/images and layout\nremain under review. Performance depends on the computer; 60 FPS is a target,\nnot a guarantee. Rendering, game timing and original Japanese remain intact.\n\nPrivate test copy made from the owner\'s disc. Do not publish game files.\nRuntime contains GPL software; see licenses/.\n',encoding='utf-8')
    readme=out/'README.txt'
    readme.write_text(readme.read_text(encoding='utf-8').replace('H: HUD.', 'H: control guide.').replace(
        'An SDL-compatible gamepad can also be used.',
        'F12 saves a native-resolution PNG in photos/.\nEnable Photo preview in Controls for F10: native pause, hidden HUD and free camera.\nPhoto camera: WASD move, Space/Ctrl up/down, mouse look, wheel zoom, Shift faster.\nF10 exits and restores the game camera. Photo preview remains experimental.\nAn SDL-compatible gamepad can also be used.').replace(
        'and chooses keyboard or PlayStation button prompts.',
        'and chooses Automatic, keyboard or PlayStation button prompts.\nAutomatic follows the last device used without restarting the game.\nPause: Left/Right change pages on the main pause screen; A/D change pages\ninside the menus, leaving arrow keys available for item grids.'),encoding='utf-8')
    if not a.finalize and (out/'manifest.json').exists():
        # Updating an already-tested private copy must not leave stale hashes or
        # retain a claim that these new artifacts passed the full privacy scan.
        manifest=json.loads((out/'manifest.json').read_text(encoding='utf-8'))
        if manifest.get('title')!='CUSA03562' or manifest.get('version')!='01.03':
            raise ValueError('Existing portable manifest targets another game')
        manifest['files']=[row for row in manifest['files'] if not row['path'].endswith('/keyboard-icons.kydelta') and row['path'] not in {'languages/en/common.psarc','languages/en/ui-corrections.kydelta'}]
        prompt_paths={p.relative_to(out).as_posix() for p in (out/'languages').rglob('*') if p.suffix in {'.kypack','.kydelta','.kytext','.kyart'}}
        prompt_paths.add('Start on Linux.sh')
        for row in manifest['files']:
            path=out/row['path']
            if not path.is_file():raise ValueError('Portable manifest file missing: '+row['path'])
            if row['path'] in {'runtime/kyoei.exe','City Shrouded in Shadow.exe','README.txt'} | prompt_paths:
                row.update(bytes=path.stat().st_size,sha256=digest(path))
        recorded={row['path'] for row in manifest['files']}
        for name in sorted(prompt_paths-recorded):
            path=out/name
            manifest['files'].append(dict(path=name,bytes=path.stat().st_size,sha256=digest(path)))
        manifest.update(english_entries=publication.get('entries_by_language',{}).get('en',publication.get('entries_per_language')),
            english_textures=preview.get('textures'),privacy_scan_passed=False,updated_since_finalization=True,
            privacy_scope='Private runtime update; full distribution privacy scan must be repeated before redistribution. Existing data/ and saves preserved.')
        temporary=out/'manifest.json.tmp'
        temporary.write_text(json.dumps(manifest,indent=2)+'\n',encoding='utf-8')
        temporary.replace(out/'manifest.json')
    if a.finalize:
        forbidden=[os.environ.get('USERNAME','').lower().encode(),Path(os.environ['USERPROFILE']).name.lower().encode(),
                   b'codex',b'claude',b'chatgpt',b'openai']
        forbidden=list({s for s in forbidden if len(s)>2})
        findings=[];records=[];coincidences=0;originals_verified=0
        inventory=json.loads((ROOT/'docs/m0/inventory.json').read_text(encoding='utf-8'))
        original_hashes={item['path'].replace('\\','/'):item['sha256'] for item in inventory['files']}
        for path in sorted(out.rglob('*')):
            if not path.is_file():continue
            relative=path.relative_to(out).as_posix()
            if relative.startswith('data/'):raise ValueError('Test data must be removed before finalization')
            file_hash=digest(path)
            if relative.startswith('game/'):
                if original_hashes.get(relative[5:])!=file_hash:raise ValueError('Portable original differs from disc inventory: '+relative)
                originals_verified+=1
                if path.suffix.lower()=='.psarc':
                    # These immutable compressed files predate this project and
                    # match the disc inventory exactly. Random compressed bytes
                    # are not personal text; never rewrite a disc asset to hide
                    # a coincidental four-letter sequence.
                    records.append(dict(path=relative,bytes=path.stat().st_size,sha256=file_hash))
                    continue
            if relative=='languages/en/common.psarc' and file_hash!=preview['sha256']:
                raise ValueError('Portable English archive differs from audited preview')
            with path.open('rb') as f:
                tail=b''
                for chunk in iter(lambda:f.read(8<<20),b''):
                    data=tail+chunk
                    hits,random_hits=personal_text_hits(data,forbidden,path.suffix.lower() in {'.json','.txt','.log','.ini','.xml','.tsv','.ps1','.cmd'})
                    coincidences+=random_hits
                    if hits:findings.append(relative)
                    tail=data[-256:]
            if path.name!='manifest.json':records.append(dict(path=relative,bytes=path.stat().st_size,sha256=file_hash))
        if findings:raise ValueError('Privacy scan failed: '+str(sorted(set(findings))))
        (out/'manifest.json').write_text(json.dumps(dict(format='kyoei-private-portable-v1',title='CUSA03562',version='01.03',
            languages=['ja','en']+(['pt-BR'] if (out/'languages/pt-BR/translation.kydelta').exists() else []),english_entries=publication.get('entries_by_language',{}).get('en',publication.get('entries_per_language')),
            english_textures=preview.get('textures'),personal_saves_included=False,
            host_configuration_included=False,privacy_scan_passed=True,original_disc_files_verified=originals_verified,
            compressed_binary_coincidences=coincidences,
            privacy_scope='Runtime/configuration text, UTF16 identifiers and PE debug/source metadata scanned. Original PSARC files byte-identical to disc inventory; compressed binary is not treated as personal text. No personal saves, logs, account files or host configuration.',
            files=records),indent=2)+'\n',encoding='utf-8')
    print(json.dumps(dict(output=str(out),source_diagnostic_paths_removed=scrubbed,finalized=a.finalize)),flush=True)

if __name__=='__main__':main()
