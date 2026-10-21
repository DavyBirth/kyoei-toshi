/* Experimental HUD-set draw gate, opt-in until scene coverage is validated.
 * Hidden state is host-only and starts disabled for every process.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_photo_hud.h"
#include "kyoei_hud_trace.h"
#include <windows.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef void (ABI *HudDraw)(void *root);
static HudDraw original_draw; /* published before any guest/GPU threads start */
static _Atomic int hidden=0,available=0;
static int observer; /* immutable after boot */
static _Atomic uint64_t observed_calls=0;
static _Atomic unsigned observed_roots=0;
static SRWLOCK observer_lock=SRWLOCK_INIT;
static void *roots[16];

int kyoei_photo_hud_available(void) {return atomic_load_explicit(&available,memory_order_acquire);}
int kyoei_photo_hud_hidden(void) {return atomic_load_explicit(&hidden,memory_order_relaxed);}
void kyoei_photo_hud_restore(void) {
    atomic_store_explicit(&hidden,0,memory_order_relaxed);
    kyoei_photo_player_restore();
}
int kyoei_photo_hud_toggle(void) {
    if(!kyoei_photo_hud_available())return 0;
    /* SDL is the only toggler; other host code can only request restore. */
    int next=!atomic_load_explicit(&hidden,memory_order_relaxed);
    atomic_store_explicit(&hidden,next,memory_order_relaxed);
    if(!next)kyoei_photo_player_restore();
    return 1;
}
uint64_t kyoei_photo_hud_observed_calls(void) {
    return atomic_load_explicit(&observed_calls,memory_order_relaxed);
}
static void observe_root(void *root) {
    atomic_fetch_add_explicit(&observed_calls,1,memory_order_relaxed);
    if(atomic_load_explicit(&observed_roots,memory_order_relaxed)>=16)return;
    AcquireSRWLockExclusive(&observer_lock);
    unsigned count=atomic_load_explicit(&observed_roots,memory_order_relaxed);
    for(unsigned i=0;i<count;++i)if(roots[i]==root){ReleaseSRWLockExclusive(&observer_lock);return;}
    if(count<16) {
        uintptr_t table=0;unsigned children=0,flags=0;
        memcpy(&table,root,sizeof(table));memcpy(&children,(char *)root+0x58,4);
        memcpy(&flags,(char *)root+0x170,4);roots[count]=root;
        atomic_store_explicit(&observed_roots,count+1,memory_order_relaxed);
        gz_log("Photo HUD observer: root=%p table=%llx children=%u flags=%x",root,(unsigned long long)table,children,flags);
    }
    ReleaseSRWLockExclusive(&observer_lock);
}
static ABI void draw_children(void *root) {
    if(observer)observe_root(root);
    if(!atomic_load_explicit(&hidden,memory_order_relaxed))original_draw(root);
}
void kyoei_photo_hud_configure(void *image,uint64_t size) {
    /* Research feature opt-in until coverage has been proven. Observer separate. */
    const char *option=getenv("KYOEI_PHOTO_HUD");
    if(!option || strcmp(option,"1"))return;
    option=getenv("KYOEI_PHOTO_HUD_OBSERVE");observer=option && !strcmp(option,"1");
    if(!image || size<0x2f0250 || original_draw) {
        gz_log("Photo HUD: unavailable (image/state); F10 disabled");return;
    }
    unsigned char *code=(unsigned char *)image+0x2f01f0;
    uint64_t hash=UINT64_C(14695981039346656037);
    for(unsigned i=0;i<96;++i)hash=(hash^code[i])*UINT64_C(1099511628211);
    if(hash!=UINT64_C(0x52fde02d01a9e568)) {
        gz_log("Photo HUD: traversal signature differs; F10 disabled; original HUD retained");return;
    }
    unsigned char *relay=NULL;
    for(unsigned i=0;i<16 && !relay;++i)
        relay=VirtualAlloc((unsigned char *)image+0x6000000+i*0x10000,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!relay){gz_log("Photo HUD: relay unavailable; F10 disabled");return;}
    int64_t delta=(int64_t)(uintptr_t)(relay+32)-(int64_t)(uintptr_t)(code+5);
    DWORD previous;
    if(delta<INT32_MIN || delta>INT32_MAX)goto fail;
    /* Existing helper copies the known six-byte push/mov/push prologue and
     * emits absolute resume/callback jumps at0/32. Existing SysV ABI retained. */
    size_t relay_size=kyoei_hud_trace_relay(relay,code+6,draw_children);
    if(!VirtualProtect(relay,4096,PAGE_EXECUTE_READ,&previous) ||
       !FlushInstructionCache(GetCurrentProcess(),relay,relay_size) ||
       !VirtualProtect(code,6,PAGE_EXECUTE_READWRITE,&previous))goto fail;
    original_draw=(HudDraw)relay;
    int32_t relative=(int32_t)delta;
    code[0]=0xe9;memcpy(code+1,&relative,4);code[5]=0x90;
    DWORD ignored;
    if(!VirtualProtect(code,6,previous,&ignored) || !FlushInstructionCache(GetCurrentProcess(),code,6))
        gz_stop(21,"STOP: cannot finalize photo HUD gate code protection/cache");
    atomic_store_explicit(&available,1,memory_order_release);
    kyoei_photo_tips_configure(image,size);
    gz_log("Photo HUD: experimental draw traversal available; hidden=0 observer=%d; full coverage unverified",observer);
    return;
fail:
    VirtualFree(relay,0,MEM_RELEASE);gz_log("Photo HUD: relay/protection unavailable; F10 disabled");
}
