#include "gz.h"
#include "kyoei_hud_trace.h"
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
void gz_log(const char *format,...) { va_list args; va_start(args,format); vprintf(format,args); va_end(args); puts(""); }
void gz_stop(int code,const char *format,...) { (void)format; ExitProcess(code); }
static ABI uintptr_t callback(void *first,const void *second) {
    return (uintptr_t)first+2*(uintptr_t)second;
}
int main(int argc,char **argv) {
    _putenv_s("KYOEI_HUD_TRACE","0"); kyoei_hud_trace_configure(NULL,0);
    unsigned char *page=VirtualAlloc(NULL,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
    if (!page) return 1;
    const unsigned char code[]={0x55,0x48,0x89,0xe5,0x41,0x57,
        0x48,0x89,0xf0,0x48,0x01,0xf8,0x41,0x5f,0x5d,0xc3};
    memcpy(page,code,sizeof(code));
    if (kyoei_hud_trace_relay(page+128,page+6,callback)!=46) return 2;
    DWORD prior;
    if (!VirtualProtect(page,4096,PAGE_EXECUTE_READ,&prior)) return 3;
    FlushInstructionCache(GetCurrentProcess(),page,4096);
    typedef uintptr_t (ABI *Call)(void *,const void *);
    if (((Call)(page+128))((void *)11,(void *)22)!=33) return 4;
    if (((Call)(page+160))((void *)11,(void *)22)!=55) return 5;
    VirtualFree(page,0,MEM_RELEASE);
    _putenv_s("KYOEI_HUD_TRACE","1"); kyoei_hud_trace_configure(NULL,0);
    if (argc==2) {
        FILE *source=fopen(argv[1],"rb");
        const size_t entry=0xa6d9a0,length=0x1da,size=entry+length;
        unsigned char *image=VirtualAlloc(NULL,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
        if (!source || !image || fseek(source,(long)(entry+0x4000),SEEK_SET)) return 6;
        if (fread(image+entry,1,length,source)!=length) return 7;
        fclose(source);
        unsigned char saved[6]; memcpy(saved,image+entry,6);
        image[entry+10]^=1;
        kyoei_hud_trace_configure(image,size);
        if (memcmp(saved,image+entry,6)) return 8;
        image[entry+10]^=1;
        kyoei_hud_trace_configure(image,size);
        if (image[entry]!=0xe9 || image[entry+5]!=0x90) return 9;
        // Only installation is tested. Never call any original game code.
        VirtualFree(image,0,MEM_RELEASE);
    }
    puts("HUD TRACE PASS: opt-in guard, SysV arguments/return, original prologue relay and checked installation; no game execution");
    return 0;
}
