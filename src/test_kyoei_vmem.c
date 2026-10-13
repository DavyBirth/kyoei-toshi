#include "gz.h"
#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#ifdef KYOEI_VMEM_REFERENCE
#include "../third_party/godzilla-core/src/rt_vmem.c"
#else
#include "kyoei_vmem.c"
#endif
void gz_stop(int code,const char *fmt,...) {
    va_list args; va_start(args,fmt); vfprintf(stderr,fmt,args); va_end(args); exit(code);
}
void gz_log(const char *fmt,...) { va_list args; va_start(args,fmt); vfprintf(stderr,fmt,args); fputc('\n',stderr); va_end(args); }
int main(void) {
    const uintptr_t base=UINT64_C(0x113d000000); const unsigned slots=768, page=16384;
    if (gz_vmem_init(base,base+(uint64_t)slots*page,(uint64_t)slots*page)) return 1;
    unsigned char used[768]={0}; unsigned state=1234567;
    for (unsigned step=0;step<20000;++step) {
        state=state*1664525u+1013904223u; unsigned start=(state>>8)%(slots-8);
        state=state*1664525u+1013904223u; unsigned length=1+(state>>8)%8;
        int clear=1; for (unsigned i=start;i<start+length;++i) clear &= !used[i];
        if (clear) {
            gz_vmem_map(base+(uint64_t)start*page,(uint64_t)length*page,(uint64_t)start*page,PAGE_READWRITE);
            for (unsigned i=start;i<start+length;++i) { used[i]=1; *(unsigned char *)(base+(uint64_t)i*page)=(unsigned char)i; }
        } else {
            gz_vmem_unmap(base+(uint64_t)start*page,(uint64_t)length*page);
            for (unsigned i=start;i<start+length;++i) used[i]=0;
        }
        if (step%100==0) for (unsigned i=0;i<slots;++i)
            if (used[i] && *(unsigned char *)(base+(uint64_t)i*page)!=(unsigned char)i) return 2;
    }
    puts("VMEM PASS: 20000 fragmented map/unmap operations and live contents preserved");
    return 0;
}
