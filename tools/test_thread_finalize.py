"""Run the actual local cleanup against the real thread-key operations."""
from pathlib import Path
import subprocess
root=Path(__file__).resolve().parents[1]
source=(root/'third_party/godzilla-core/src/rt_thread.c').read_text()
start=source.index('#define KEYS ')
end=source.index('/*',source.index('static void keys_cleanup(void)',start)+1)
key_code=source[start:end]
# Keep only complete definitions through keys_cleanup, without copying them.
begin=key_code.index('{',key_code.index('static void keys_cleanup'))
depth=1
at=begin+1
while depth:
    depth += (key_code[at]=='{') - (key_code[at]=='}')
    at+=1
key_code=key_code[:at]
prelude=r'''
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include "gz.h"
#include "kyoei_modules.c"
void gz_log(const char *fmt,...) { (void)fmt; }
void gz_stop(int code,const char *fmt,...) { (void)fmt; exit(code); }
'''
test=r'''
#include "kyoei_thread_finalize.h"
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d failed\n",__LINE__); return 1; } } while(0)
static int called;
static ABI void retained(void *value) { if (value!=(void*)0x1234) abort(); ++called; }
int main(void) {
 uint32_t a,b,c; CHECK(!key_create(&a,(KeyDestructor)0x1001));
 CHECK(!key_create(&b,retained)); CHECK(!key_create(&c,(KeyDestructor)0x2001));
 CHECK(!key_set(a,(void*)0x5678)); CHECK(!key_set(b,(void*)0x1234));
 ModuleInfo info={0}; info.size=sizeof(info); info.segment_count=2;
 info.segments[0]=(ModuleSegment){0x1000,64,5}; info.segments[1]=(ModuleSegment){0x2000,64,6};
 kyoei_pthread_finalize(&info);
 CHECK(keys[a].used && !keys[a].destructor && key_get(a)==(void*)0x5678);
 CHECK(keys[b].used && keys[b].destructor==retained);
 CHECK(keys[c].destructor==(KeyDestructor)0x2001);
 kyoei_pthread_finalize(&info); CHECK(keys[b].destructor==retained);
 keys_cleanup(); CHECK(called==1 && !key_get(b));
 CHECK(!key_delete(a) && !key_delete(b) && !key_delete(c));
 puts("PTHREAD FINALIZE PASS: only finalized code destructors removed; keys/values and foreign/data callbacks retained; remaining cleanup runs");
 return 0;
}
'''
build=root/'build/test-thread-finalize'
build.mkdir(exist_ok=True)
(build/'test.c').write_text(prelude+key_code+test)
subprocess.run(['clang','-O2','-D_CRT_SECURE_NO_WARNINGS','-I'+str(root/'src'),
                '-I'+str(root/'third_party/godzilla-core/src'),str(build/'test.c'),
                '-lbcrypt','-o',str(build/'test.exe')],check=True)
subprocess.run([str(build/'test.exe')],check=True)
