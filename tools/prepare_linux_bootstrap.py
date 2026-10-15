"""Derive a non-executing POSIX loader stage from the fixed Godzilla loader.

Shares its BBPROBE5 parser, segment checks, relocations and native bindings.
Guest execution is deliberately absent until POSIX runtime/TLS/GPU exist.
"""
import argparse, hashlib
from pathlib import Path

def adapt(source):
    source=source.replace('\r\n','\n')
    def replace(old,new):
        nonlocal source
        if source.count(old)!=1:raise ValueError('Review Linux loader anchor: '+old[:80])
        source=source.replace(old,new)
    replace('#include <windows.h>','#include "bootstrap_host.h"')
    replace('static const char *module_names[] = {"libc.prx", "libSceFios2.prx"};','')
    start=source.index('/* ---- where an address is, for messages ---- */')
    end=source.index('/* ---- CPU:',start)
    source=source[:start]+'static void gz_stub_thunk(void) { gz_stop(21,"Linux bootstrap cannot execute guest imports"); }\n\n'+source[end:]
    start=source.index('/* ---- guest main thread ---- */')
    end=source.index('int main(',start)
    source=source[:start]+source[end:]
    replace('    if (code != 1) { gz_memory_report(); gz_file_report(); }','')
    replace('    SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);','')
    for line in ('gz_kernel_start();','gz_runtime_start();','gz_memory_start();','gz_file_configure(app0, user_dir);','gz_savedata_configure(app0);','AddVectoredExceptionHandler(1, on_exception);','SetUnhandledExceptionFilter(on_unhandled);'):
        replace('    '+line,'')
    replace('    if (!out) {','    if (!list_stubs || stubs_return_zero || gpu_test) { fprintf(stderr,"Linux bootstrap requires --list-stubs; guest execution is not implemented.\\n"); return 2; }\n    if (!out) {')
    replace('    if (main_tls[2]) fail("this eboot has static TLS; the Windows loader does not support it yet");',
        '    if (main_tls[1] > main_tls[2] || main_tls[2] > (1u<<20) || main_tls[0] > image_size || main_tls[1] > image_size-main_tls[0] || !main_tls[3] || main_tls[3]>4096 || (main_tls[3]&(main_tls[3]-1))) fail("invalid main TLS metadata");\n    gz_log("Linux bootstrap: main TLS metadata validated; TLS execution not implemented");')
    start=source.index('    for (uint64_t m = 0; m < module_count; ++m)\n        if (modules[m].tls_module)\n            gz_set_module_tls')
    source=source[:start]+'    fail("Linux bootstrap cannot enter guest code");\n}\n'
    replace('    if (list_stubs) { fflush(NULL); return 0; }','    if (list_stubs) { gz_log("LINUX BOOTSTRAP PASS: real image mapped and relocated; runtime, TLS execution and GPU remain unavailable; no guest code executed"); fflush(NULL); return 0; }')
    replace('int main(int argc, char **argv) {','#include "bootstrap_tls.h"\n\nint main(int argc, char **argv) {')
    replace('    protect(traps, round_page(import_count * 32), 5);','    prepare_main_tls(out,main_tls);\n    protect(traps, round_page(import_count * 32), 5);')
    source=source.replace('\\\\','/')
    return source

def main():
    parser=argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source',type=Path);parser.add_argument('output',type=Path)
    a=parser.parse_args();data=a.source.read_bytes()
    text=adapt(data.decode('utf8'))
    a.output.write_text(text,encoding='utf8')
    print('Linux loader derived from SHA256 '+hashlib.sha256(data).hexdigest())
if __name__=='__main__':main()
