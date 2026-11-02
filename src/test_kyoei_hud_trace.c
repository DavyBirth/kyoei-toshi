#include "gz.h"
#include "kyoei_hud_trace.h"
#include "kyoei_hud_layout.h"
#include <windows.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
void gz_log(const char *format,...) { va_list args; va_start(args,format); vprintf(format,args); va_end(args); puts(""); }
void gz_stop(int code,const char *format,...) { (void)format; ExitProcess(code); }
/* This test uses host fixtures rather than the guest allocation registry. */
uint64_t gz_memory_clamp(uintptr_t address,uint64_t size) {
    MEMORY_BASIC_INFORMATION info;
    if(!address || !VirtualQuery((void*)address,&info,sizeof(info)) || info.State!=MEM_COMMIT ||
       (info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return 0;
    size_t available=info.RegionSize-(address-(uintptr_t)info.BaseAddress);
    return size<available?size:available;
}
static ABI uintptr_t callback(void *first,const void *second) {
    return (uintptr_t)first+2*(uintptr_t)second;
}
typedef struct { char *pointer; unsigned length,capacity; char buffer[116]; } EngineString;
typedef uintptr_t (ABI *Marker)(void *,const void *);
typedef void (ABI *Assign)(void *,const char *);
void kyoei_hud_test_callbacks(Marker,Assign,const KyoeiHudMetrics *);
ABI uintptr_t kyoei_hud_test_marker(void *,const void *);
ABI uintptr_t kyoei_hud_test_npc(Marker,void *,const void *);
typedef unsigned (ABI *Count)(const char *);
unsigned kyoei_hud_test_map(Count,const char *);
static ABI unsigned fake_count(const char *text) { return (unsigned)strlen(text); }
static ABI unsigned fake_unicode_count(const char *text) {
    unsigned count=0;for(;*text;++text) if (((unsigned char)*text&0xc0)!=0x80) ++count;return count;
}
static unsigned original_calls,assign_calls;
static unsigned char node[0x300],plate[0x100],object[0x300];
static ABI void fake_assign(void *target,const char *source) {
    EngineString *string=target;
    size_t bytes=strlen(source);
    if (bytes>115) ExitProcess(22);
    memcpy(string->buffer,source,bytes+1);string->pointer=string->buffer;
    string->length=(unsigned)bytes;string->capacity=116;++assign_calls;
}
static ABI uintptr_t fake_original(void *unused,const void *source) {
    (void)unused;(void)source;
    fake_assign(node+0x110,"Listen to t...");++original_calls;
    return 0xc0de;
}
static ABI uintptr_t fake_npc(void *result,const void *source) {
    (void)source;*(unsigned *)result=1;fake_assign(node+0x110,"Senhora...");return (uintptr_t)result;
}
static int check_fit_callback(void) {
    void *node_pointer=node,*plate_pointer=plate;
    memcpy(object+0x278,&node_pointer,8);memcpy(object+0x238,&plate_pointer,8);
    KyoeiHudMetrics metrics;
    for (unsigned i=0;i<95;++i) metrics.equivalent[i]=.6f;
    kyoei_hud_test_callbacks(fake_original,fake_assign,&metrics);
    EngineString source={0};fake_assign(&source,"Listen to their conversation");
    assign_calls=0;
    if (kyoei_hud_test_marker(object,&source)!=0xc0de || original_calls!=1 || assign_calls!=2) return 10;
    EngineString *actual=(void *)(node+0x110);
    if (strcmp(actual->pointer,source.pointer) || actual->length!=source.length) return 11;
    KyoeiHudFit fit;
    kyoei_hud_fit(&metrics,source.pointer,source.length,0,&fit);
    float scale;memcpy(&scale,plate+0x80,4);
    if (fabsf(scale-fit.scale_x)>.001f) return 12;
    unsigned direction=1;memcpy(object+0x2c8,&direction,4);
    if (kyoei_hud_test_marker(object,&source)!=0xc0de) return 13;
    float x;memcpy(&x,node+0x60,4);
    if (fabsf(x-(32-36*scale))>.001f) return 14;
    fake_assign(&source,"[i4]");assign_calls=0;
    kyoei_hud_test_marker(object,&source);
    if (assign_calls!=1 || strcmp(actual->pointer,"Listen to t...")) return 15;
    metrics.equivalent['i'-32]=.2f;metrics.equivalent['W'-32]=1.f;
    kyoei_hud_test_callbacks(fake_original,fake_assign,&metrics);
    if (kyoei_hud_test_map(fake_count,"iiii")!=1 || kyoei_hud_test_map(fake_count,"WWWW")!=4) return 16;
    if (kyoei_hud_test_map(fake_count,"[i4]")!=4) return 17;
    const char *curved="i\xe2\x80\x99i";
    if (kyoei_hud_test_map(fake_unicode_count,curved)!=2) return 20;
    if (kyoei_hud_test_map(fake_unicode_count,"\xc3\xa9")!=1) return 21;
    fake_assign(&source,curved);kyoei_hud_test_marker(object,&source);
    if (strcmp(actual->pointer,curved)) return 22;
    const char *portuguese="Mo\xc3\xa7\x61 da floricultura";
    fake_assign(&source,portuguese);assign_calls=0;
    kyoei_hud_test_marker(object,&source);
    if (assign_calls!=2 || strcmp(actual->pointer,portuguese)) return 24;
    void *npc_label=node;memcpy(object+0x240,&npc_label,8);
    fake_assign(object+0x190,"Senhora da floricultura");
    float one=1;memcpy(node+0x80,&one,4);memcpy(node+0x84,&one,4);
    unsigned result=0;
    if (kyoei_hud_test_npc(fake_npc,&result,object)!=(uintptr_t)&result || result!=1 ||
        strcmp(actual->pointer,"Senhora da floricultura")) return 25;
    memcpy(&scale,node+0x80,4);float vertical;memcpy(&vertical,node+0x84,4);
    if (!(scale>0 && scale<1) || vertical!=1) return 26;
    fake_assign(object+0x190,"Yuki");kyoei_hud_test_npc(fake_npc,&result,object);
    memcpy(&scale,node+0x80,4);
    if (strcmp(actual->pointer,"Yuki") || scale!=1) return 27;
    KyoeiHudFit star;
    if (!kyoei_hud_fit(&metrics,"\xe2\x98\x85W",4,0,&star) ||
        fabsf(star.scale_x-(2*29.f/36.f+32.f/36.f))>.001f) return 23;
    return 0;
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
    int fit_result=check_fit_callback();if (fit_result) return fit_result;
    _putenv_s("KYOEI_HUD_TRACE","1"); kyoei_hud_trace_configure(NULL,0);
    if (argc>=2) {
        FILE *source=fopen(argv[1],"rb");
        const size_t entry=0xa6d9a0,length=0x1da,size=0xaeab8c;
        unsigned char *image=VirtualAlloc(NULL,size,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
        if (!source || !image || fseek(source,(long)(entry+0x4000),SEEK_SET)) return 6;
        if (fread(image+entry,1,length,source)!=length) return 7;
        const size_t blocks[][2]={{0x31e090,0xc2},{0x31cd90,0x4d},{0xa5bfe8,0x45},{0xa5f499,0x29},{0xaea710,0x47c}};
        for (unsigned i=0;i<sizeof(blocks)/sizeof(blocks[0]);++i) {
            if (fseek(source,(long)(blocks[i][0]+0x4000),SEEK_SET) ||
                fread(image+blocks[i][0],1,blocks[i][1],source)!=blocks[i][1]) return 18;
        }
        fclose(source);
        if (argc==3) {
            _putenv_s("KYOEI_HUD_FIT","1");_putenv_s("KYOEI_LANGUAGE","en");
            _putenv_s("KYOEI_HUD_METRICS",argv[2]);
        }
        unsigned char map_before[5];memcpy(map_before,image+0xa5bff0,5);
        unsigned char saved[6]; memcpy(saved,image+entry,6);
        image[entry+10]^=1;
        kyoei_hud_trace_configure(image,size);
        if (memcmp(saved,image+entry,6)) return 8;
        image[entry+10]^=1;
        kyoei_hud_trace_configure(image,size);
        if (image[entry]!=0xe9 || image[entry+5]!=0x90) return 9;
        if (argc==3 && (image[0xa5bff0]!=0xe8 || !memcmp(map_before,image+0xa5bff0,5))) return 19;
        if (argc==3 && (image[0xaea710]!=0xe9 || image[0xaea715]!=0x90)) return 28;
        // Only installation is tested. Never call any original game code.
        VirtualFree(image,0,MEM_RELEASE);
    }
    puts("HUD TRACE PASS: opt-in guard, SysV arguments/return, original prologue relay and checked installation; no game execution");
    return 0;
}
