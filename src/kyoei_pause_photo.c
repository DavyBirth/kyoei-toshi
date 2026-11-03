/* A host-owned seventh pause row. Native categories/arrays remain unchanged.
 * Photo entry sends the normal pause button, then waits for BOTH the menu bit
 * and native unit pause to clear. It never adopts or clears another owner's pause.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "kyoei_pause_photo.h"
#include <stdatomic.h>
#include <string.h>
#ifdef KYOEI_PAUSE_PHOTO_TEST
static uint64_t clock_ms;static int closed,paused,same=1;
static uint64_t now_ms(void){return clock_ms;}
void kyoei_pause_photo_test_scene(int a,int b,int c){closed=a;paused=b;same=c;}
void kyoei_pause_photo_test_clock(uint64_t now){clock_ms=now;}
#else
#include "gz.h"
#include "kyoei_hud_trace.h"
#include "kyoei_photo_scene.h"
#include "kyoei_photo_hud.h"
#include <windows.h>
#include <limits.h>
static unsigned char *base;
typedef void (ABI *Update)(void *);
static Update original_update;
static uint64_t now_ms(void){return GetTickCount64();}
static uintptr_t unit_identity;
static _Atomic uintptr_t menu_identity;
static int read_value(uintptr_t address,void *out,size_t n){
    MEMORY_BASIC_INFORMATION info;
    if(!address || !VirtualQuery((void*)address,&info,sizeof(info)) || info.State!=MEM_COMMIT ||
       (info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return 0;
    size_t offset=address-(uintptr_t)info.BaseAddress;
    if(offset>info.RegionSize || n>info.RegionSize-offset)return 0;
    memcpy(out,(void*)address,n);return 1;
}
static uintptr_t pointer(uintptr_t address){uintptr_t p=0;read_value(address,&p,8);return p;}
static unsigned word(uintptr_t address){unsigned p=0;read_value(address,&p,4);return p;}
#endif
static _Atomic uint64_t seen,deadline,close_until;
static _Atomic int native_cursor,selected,swallow;
static unsigned previous;
static int direction;
int kyoei_pause_photo_visual(float *offset,float *opacity){
#ifdef KYOEI_PAUSE_PHOTO_TEST
    *offset=0;*opacity=1;return kyoei_pause_photo_visible();
#else
    if(!base)return 0;
    uintptr_t menu=pointer(pointer((uintptr_t)base+0x1543dd0)+0xa10);
    if(!menu || !word(menu+0x90) || word(menu+0x58)!=13)return 0;
    uintptr_t nodes=pointer(menu+0x50),last=pointer(nodes+11*8),text=pointer(nodes+12*8);
    if(!(word(last+0x20)&2))return 0;
    float x=0;read_value(last+0x60,&x,4);
    if(!(x>=-3000 && x<=3000))return 0;
    // The native Load row already contains the authored easing and staggering.
    // Follow its position throughout opening/closing instead of waiting for idle.
    *offset=x-135.f;*opacity=(word(text+0x94)>>24)/255.f;
    return *opacity>0;
#endif
}
int kyoei_pause_photo_visible(void){
    if(atomic_load(&deadline))return 0;
#ifdef KYOEI_PAUSE_PHOTO_TEST
    uint64_t t=atomic_load(&seen);return t && now_ms()-t<200;
#else
    uintptr_t menu=atomic_load(&menu_identity);
    if(!menu || pointer(pointer((uintptr_t)base+0x1543dd0)+0xa10)!=menu ||
       !word(menu+0x90) || word(menu+0x190)!=2 || word(menu+0x298) || word(menu+0x58)!=13)return 0;
    uintptr_t nodes=pointer(menu+0x50),last=pointer(nodes+11*8);float x=0;read_value(last+0x60,&x,4);
    return x>=100 && x<=200 && word(menu+0x288)<=5;
#endif
}
int kyoei_pause_photo_selected(void){return atomic_load(&selected);}
void kyoei_pause_photo_cancel(void){
    int ours=atomic_exchange(&selected,0);uint64_t pending=atomic_exchange(&deadline,0);
    if(ours||pending)atomic_store(&swallow,1);
}
void kyoei_pause_photo_request(void){
    if(!kyoei_pause_photo_visible())return;
#ifndef KYOEI_PAUSE_PHOTO_TEST
    unit_identity=pointer((uintptr_t)base+0x1336200);
    if(!unit_identity)return;
    gz_log("Kyoei pause photo: closing native pause before camera entry");
#endif
    uint64_t now=now_ms();atomic_store(&deadline,now+5000);atomic_store(&close_until,now+120);
    atomic_store(&selected,0);atomic_store(&swallow,1);
}
int kyoei_pause_photo_take_entry(void){
    uint64_t end=atomic_load(&deadline);if(!end)return 0;
    if(now_ms()>=end){kyoei_pause_photo_cancel();return 0;}
#ifdef KYOEI_PAUSE_PHOTO_TEST
    if(!same){kyoei_pause_photo_cancel();return 0;}
    if(!closed || paused)return 0;
#else
    uintptr_t units=pointer((uintptr_t)base+0x1336200),state=pointer((uintptr_t)base+0x13361f0);
    if(!units || units!=unit_identity || !state){kyoei_pause_photo_cancel();return 0;}
    if((word(state+0x317c)&1) || word(units+0x44d8) || now_ms()-atomic_load(&seen)<200)return 0;
    gz_log("Kyoei pause photo: native menu and unit pause released; camera entry ready");
#endif
    atomic_store(&deadline,0);return 1;
}
int kyoei_pause_photo_input(uint32_t *buttons,unsigned stick_y){
    unsigned b=*buttons,pressed=b&~previous;previous=b;
    int next=(b&0x40)||stick_y>=200?1:(b&0x10)||stick_y<=56?-1:0;
    int edge=next && next!=direction;direction=next;
    if(atomic_load(&deadline)){
        *buttons=now_ms()<atomic_load(&close_until)?8:0;return 0;
    }
    if(atomic_load(&swallow)){if(b||next)return 1;atomic_store(&swallow,0);}
    if(!kyoei_pause_photo_visible()){atomic_store(&selected,0);return 0;}
    if(!atomic_load(&selected)){
        if(atomic_load(&native_cursor)==5 && edge && next==1){atomic_store(&selected,1);return 1;}
        return 0;
    }
    if(pressed&0x2000){kyoei_pause_photo_request();return 1;}
    if(pressed&(8|0x4000)){atomic_store(&selected,0);return 0;}
    if(edge && next<0){atomic_store(&selected,0);atomic_store(&swallow,1);}
    return 1;
}
#ifdef KYOEI_PAUSE_PHOTO_TEST
void kyoei_pause_photo_test_frame(int cursor,uint64_t now){clock_ms=now;atomic_store(&native_cursor,cursor);atomic_store(&seen,now);}
#else
static ABI void update(void *self){
    static uintptr_t muted;static unsigned color;
    uintptr_t nodes=pointer((uintptr_t)self+0x50),cursor=pointer(nodes+10*8);
    if(muted && muted==cursor && !atomic_load(&selected)){memcpy((void*)(cursor+0x94),&color,4);muted=0;}
    original_update(self);
    if(!kyoei_photo_hud_available() || !kyoei_photo_scene_available() ||
       !word((uintptr_t)self+0x90) ||
       word((uintptr_t)self+0x298) || word((uintptr_t)self+0x58)!=13)return;
    int value=(int)word((uintptr_t)self+0x288);float x=0;
    uintptr_t last=pointer(nodes+11*8);read_value(last+0x60,&x,4);
    if(value<0 || value>5 || x<100 || x>200)return;
    atomic_store(&menu_identity,(uintptr_t)self);
    atomic_store(&native_cursor,value);atomic_store(&seen,now_ms());
    if(atomic_load(&selected) && cursor){if(muted!=cursor)color=word(cursor+0x94);unsigned hidden=color&0xffffff;memcpy((void*)(cursor+0x94),&hidden,4);muted=cursor;}
}
void kyoei_pause_photo_configure(void *image,uint64_t size){
    const size_t entry=0xb296b0,length=2619;
    if(!image || size<0x1543dd8)return;
    unsigned char *code=(unsigned char*)image+entry;uint64_t hash=UINT64_C(14695981039346656037);
    for(size_t i=0;i<length;++i)hash=(hash^code[i])*UINT64_C(1099511628211);
    const unsigned char prologue[]={0x55,0x48,0x89,0xe5,0x41,0x57};
    if(hash!=UINT64_C(0xfc85cea13d1fc742) || memcmp(code,prologue,6)){gz_log("Kyoei pause photo: unsupported menu signature; original retained");return;}
    unsigned char *relay=NULL;
    for(unsigned i=0;i<16&&!relay;++i)relay=VirtualAlloc((char*)image+0x9000000+i*0x10000,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
    if(!relay)return;
    size_t bytes=kyoei_hud_trace_relay(relay,code+6,update);
    int64_t delta=(intptr_t)(relay+32)-(intptr_t)(code+5);DWORD old;
    if(delta<INT32_MIN||delta>INT32_MAX||!VirtualProtect(relay,4096,PAGE_EXECUTE_READ,&old)||!VirtualProtect(code,6,PAGE_EXECUTE_READWRITE,&old)){VirtualFree(relay,0,MEM_RELEASE);return;}
    original_update=(Update)relay;base=image;int32_t rel=(int32_t)delta;code[0]=0xe9;memcpy(code+1,&rel,4);code[5]=0x90;
    DWORD ignored;if(!VirtualProtect(code,6,old,&ignored)||!FlushInstructionCache(GetCurrentProcess(),code,6)||!FlushInstructionCache(GetCurrentProcess(),relay,bytes))gz_stop(21,"STOP: pause photo hook finalization failed");
    gz_log("Kyoei pause photo: host seventh row enabled; native categories and pause ownership preserved");
}
#endif
