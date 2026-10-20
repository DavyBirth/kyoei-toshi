/* Verify the real observer preserves outputs and the five-byte relay preserves flags. */
#include "kyoei_photo_lights.c"
#include <assert.h>
#include <stdio.h>
static int calls,hud=1,active=1;
int kyoei_photo_hud_hidden(void){return hud;}
int kyoei_photo_scene_active(void){return active;}
void gz_log(const char *format,...){(void)format;}
void gz_stop(int code,const char *format,...){(void)format;exit(code);}
static void put_pointer(void *p,size_t at,void *v){memcpy((char*)p+at,&v,sizeof(v));}
static ABI uint32_t spy(void *sys,uint32_t types,uint32_t views,void **out,uint32_t cap){
    (void)sys;assert(types==4 && views==1 && cap==4);++calls;out[0]=(void*)0x1234;return 1;
}
int main(void){
    base=VirtualAlloc(NULL,0x1354430,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);assert(base);
    unsigned char system[0x68]={0};put_pointer(base,0x1354420,system);put_pointer(system,0,base+0x1259ba0);
    original_collect=spy;void *output[4]={0};
    assert(collect_lights(system,4,1,output,4)==1 && calls==1 && output[0]==(void*)0x1234);
    put_pointer(system,0,base+0x1254b48);
    assert(collect_lights(system,4,1,output,4)==1 && calls==2 && output[0]==(void*)0x1234);
    hud=0;assert(collect_lights(system,4,1,output,4)==1 && calls==3);
    active=0;assert(collect_lights(NULL,4,1,output,4)==1 && calls==4);
    VirtualFree(base,0,MEM_RELEASE);
    unsigned char *memory=VirtualAlloc(NULL,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);assert(memory);
    const unsigned char function[]={0x31,0xc0,0x45,0x85,0xc0,0x74,0x06,0xb8,77,0,0,0,0xc3,0xb8,33,0,0,0,0xc3};
    memcpy(memory,function,sizeof(function));passthrough(memory+64,memory);
    DWORD protection;assert(VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&protection));
    assert(FlushInstructionCache(GetCurrentProcess(),memory,128));
    Collect trampoline=(Collect)(memory+64);
    assert(trampoline(NULL,0,0,NULL,0)==33);assert(trampoline(NULL,0,0,NULL,1)==77);
    VirtualFree(memory,0,MEM_RELEASE);
    puts("PHOTO LIGHT OBSERVER PASS: native outputs unchanged, invalid pointers ignored, relay preserves branch flags");
}
