#include "kyoei_modules.c"
#include <stdlib.h>
#include <stdarg.h>
void gz_log(const char *format, ...) { (void)format; }
void gz_stop(int code, const char *format, ...) { (void)format; exit(code); }
static int calls, fail;
static ABI int32_t finalizer(uint64_t args, const void *argp, const void *options) {
    if (args != 7 || argp != (void *)0x1234 || options) abort();
    ModuleInfo info;
    if (info_from_address(0x1008,2,&info) || info.fini!=(uintptr_t)finalizer) abort();
    ++calls; return fail ? -17 : 0;
}
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d failed\n",__LINE__); return 1; } } while(0)
int main(void) {
    count=1; modules[0].started=1; modules[0].fini=(uintptr_t)finalizer;
    modules[0].info.size=sizeof(ModuleInfo); modules[0].info.fini=(uintptr_t)finalizer;
    modules[0].info.segment_count=1; modules[0].info.segments[0]=(ModuleSegment){0x1000,16,5};
    ModuleInfo info; memset(&info,0x5a,sizeof(info));
    CHECK(!info_from_address(0x100f,2,&info) && info.segments[0].size==16);
    CHECK(elf_match_address(&info,0x1000)==1 && elf_match_address(&info,0x1007)==1);
    CHECK(!elf_match_address(&info,0x1008) && !elf_match_address(&info,0x0fff));
    info.segments[0].protection=6; CHECK(!elf_match_address(&info,0x1000));
    CHECK(!elf_match_address(NULL,0x1000));
    CHECK(info_from_address(0x1010,2,&info)==(int32_t)0x80020003);
    CHECK(info_from_address(0x1000,3,&info)==(int32_t)0x80020016 && !info.size);
    CHECK(info_from_address(0x1000,2,NULL)==(int32_t)0x8002000e);
    int32_t result=99;
    CHECK(stop_module(0x4000,7,(void*)0x1234,1,NULL,&result)==(int32_t)0x80020016);
    CHECK(!calls && result==99 && modules[0].started);
    fail=1; CHECK(stop_module(0x4000,7,(void*)0x1234,0,NULL,&result)==-17);
    CHECK(calls==1 && modules[0].started && result==-17);
    fail=0; CHECK(!stop_module(0x4000,7,(void*)0x1234,0,NULL,&result));
    CHECK(calls==2 && !modules[0].started && !result);
    CHECK(stop_module(0x4000,7,(void*)0x1234,0,NULL,&result)==(int32_t)0x80020003);
    CHECK(stop_module(0x5000,7,(void*)0x1234,0,NULL,&result)==(int32_t)0x80020003);
    CHECK(calls==2);
    puts("MODULE STOP/INFO PASS: exact segment bounds, flags/pointers, query inside finalizer without deadlock, lifecycle/error propagation");
    return 0;
}
