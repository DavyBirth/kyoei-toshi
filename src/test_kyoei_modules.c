#include "kyoei_modules.c"
#include <stdlib.h>
#include <stdarg.h>
void gz_log(const char *format, ...) { (void)format; }
void gz_stop(int code, const char *format, ...) { (void)format; exit(code); }
static int calls, fail;
static ABI int32_t finalizer(uint64_t args, const void *argp, const void *options) {
    if (args != 7 || argp != (void *)0x1234 || options) abort();
    ++calls; return fail ? -17 : 0;
}
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d failed\n",__LINE__); return 1; } } while(0)
int main(void) {
    count=1; modules[0].started=1; modules[0].fini=(uintptr_t)finalizer;
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
    puts("MODULE STOP PASS: real finalizer arguments/result, failed stop retained, no double stop, invalid handles");
    return 0;
}
