"""Derive the executable POSIX integration from the pinned local reference.

Only build copies are changed. Imports come from the owner's prepared image.
"""
import argparse
import json
import re
import subprocess
from pathlib import Path

REVISION = 'e5d04e2ba4864ccf40de6c3cf6aa532855320b0d'


def replace(source, before, after):
    if source.count(before) != 1:
        raise ValueError('Review native Linux anchor: ' + before[:90])
    return source.replace(before, after)


def main():
    p = argparse.ArgumentParser(description=__doc__)
    p.add_argument('reference', type=Path)
    p.add_argument('prepared', type=Path)
    p.add_argument('output', type=Path)
    a = p.parse_args()
    revision = subprocess.check_output(['git', '-C', str(a.reference), 'rev-parse', 'HEAD'], text=True).strip()
    if revision != REVISION:
        raise ValueError('Unexpected reference revision')
    a.output.mkdir(parents=True, exist_ok=True)
    sources = {f.name: f.read_text(encoding='utf8') for f in (a.reference/'src').glob('runtime*') if f.is_file()}
    # Extend exact scoped identifiers: scopes are assigned per executable, not global NIDs.
    names = dict(re.findall(r'\{"([^"]+)","([^"]+)"\}', (a.reference/'src/import_names.inc').read_text()))
    names.setdefault('2emaaluWzUw#q#q', '__cxa_guard_abort')
    canonical = {}
    for source in sources.values():
        for nid in re.findall(r'"([A-Za-z0-9+\-]{11}#[A-Za-z0-9+#]+)"', source):
            canonical.setdefault(nid.split('#')[0], nid)
    aliases = []
    for line in (a.prepared/'imports.tsv').read_text().splitlines():
        fields = line.split('\t')
        if len(fields) == 4 and fields[3] and fields[3] != '?':
            names[fields[1]] = fields[3]
            target = canonical.get(fields[1].split('#')[0])
            if target and target != fields[1]:
                aliases.append((fields[1], target))
    sources['import_names.inc'] = ''.join('{'+json.dumps(n)+','+json.dumps(s)+'},\n' for n, s in sorted(names.items()))
    core = sources['runtime.c']
    # Core functions also used hardcoded Bloodborne scopes. Match audited symbol
    # names instead; native libraries remain the fallback for unimplemented libc.
    for nid in re.findall(r'!strcmp\(name, "([^"]+)"\)', core):
        symbol = names.get(nid)
        if not symbol:
            by_hash = {s for n, s in names.items() if n.split('#')[0] == nid.split('#')[0]}
            if len(by_hash) != 1:
                raise ValueError('Unknown core NID: '+nid)
            symbol = by_hash.pop()
        core = replace(core, '!strcmp(name, '+json.dumps(nid)+')',
                       '(runtime_symbol(name) && !strcmp(runtime_symbol(name), '+json.dumps(symbol)+'))')
    sources['runtime.c'] = core
    alias_table = ',\n'.join('{'+json.dumps(n)+','+json.dumps(t)+'}' for n, t in aliases)
    sources['runtime.c'] = replace(core, 'uintptr_t runtime_resolve(const char *name, int is_data) {',
        'uintptr_t runtime_resolve(const char *name, int is_data) {\n'
        '    static const struct { const char *from,*to; } aliases[]={'+alias_table+'};\n'
        '    for(size_t i=0;i<sizeof(aliases)/sizeof(*aliases);++i) if(!strcmp(name,aliases[i].from)) {name=aliases[i].to;break;}\n'
        '    extern uintptr_t kyoei_native_services_resolve(const char*);\n'
        '    uintptr_t kyoei_extra=is_data?0:kyoei_native_services_resolve(name); if(kyoei_extra)return kyoei_extra;')
    root = Path(__file__).resolve().parents[1]
    modules = (root/'src/kyoei_modules.c').read_text()
    modules = replace(modules, '#include "gz.h"', '#include "kyoei_native_compat.h"')
    modules = replace(modules, '#include <windows.h>\n#include <bcrypt.h>', '#include <pthread.h>\n#include <openssl/evp.h>')
    modules = replace(modules, 'static SRWLOCK lock = SRWLOCK_INIT;', 'static pthread_mutex_t lock=PTHREAD_MUTEX_INITIALIZER;')
    modules = modules.replace('AcquireSRWLockExclusive(&lock)', 'pthread_mutex_lock(&lock)').replace('ReleaseSRWLockExclusive(&lock)', 'pthread_mutex_unlock(&lock)')
    start = modules.index('    BCRYPT_ALG_HANDLE algorithm = NULL;')
    end = modules.index('    for (int i = 0; i < 8; ++i) reversed[i]', start)
    modules = modules[:start]+'''    unsigned char digest[20], reversed[8]; unsigned length=0;
    EVP_MD_CTX *ctx=EVP_MD_CTX_new();
    if(!ctx || !EVP_DigestInit_ex(ctx,EVP_sha1(),NULL) ||
       !EVP_DigestUpdate(ctx,name,strlen(name)) || !EVP_DigestUpdate(ctx,salt,sizeof(salt)) ||
       !EVP_DigestFinal_ex(ctx,digest,&length) || length!=20) gz_stop(21,"STOP: cannot hash native symbol");
    EVP_MD_CTX_free(ctx);
'''+modules[end:]
    sources['kyoei_native_modules.c'] = modules
    if sources['runtime_services.c'].count('"Hunter"') != 3:
        raise ValueError('Review default offline user names')
    sources['runtime_services.c'] = sources['runtime_services.c'].replace('"Hunter"', '"Player"')
    probe = (a.reference/'src/probe.c').read_text()
    probe = replace(probe, '    image = allocate(round_page(size));',
        '    image = mmap((void*)UINT64_C(0x800000000),round_page(size),PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS|MAP_FIXED_NOREPLACE,-1,0);\n'
        '    if (image==MAP_FAILED) fail("cannot reserve Kyoei image base");')
    probe = replace(probe, '    if (patch_file) apply_patches(patch_file, segments, ns, relocs, nr);',
        '    kyoei_patch_tls(argv[1],size,segments,ns);\n    kyoei_prepare_modules(argv[1],size);\n    if (patch_file) apply_patches(patch_file, segments, ns, relocs, nr);')
    probe = replace(probe, '            printf("Starting linked module %',
        '            if(kyoei_module_deferred(modules[m].base)) continue;\n            printf("Starting linked module %')
    probe = replace(probe, 'int main(int argc, char **argv) {',
        '#include "kyoei_native_tls.h"\n\nint main(int argc, char **argv) {')
    probe = replace(probe, 'char title[128]="Bloodborne"', 'char title[128]="City Shrouded in Shadow"')
    probe = replace(probe, '    bbgpu_register_kernel();',
        '    setenv("BB_UPSCALER","off",1); setenv("BB_OBJECT_MOTION","0",1);\n'
        '    bbgpu_register_kernel();')
    # Kyoei exposes the base package offline, same explicit values as Windows.
    probe = replace(probe, '    if (content_profile) {',
        '    const uint32_t base_content[5]={3,0,0,0,0};\n'
        '    if (!content_profile) runtime_content_configure(base_content);\n    if (content_profile) {')
    sources['probe.c'] = probe
    sources['vulkan_smoke.c'] = (a.reference/'src/vulkan_smoke.c').read_text()
    for name, source in sources.items():
        (a.output/name).write_text(source, encoding='utf8', newline='\n')
    print('Native POSIX runtime derived from '+revision)


if __name__ == '__main__':
    main()
