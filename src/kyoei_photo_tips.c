/* Experimental independent tip-font gate. Explicit opt-in; visual proof pending.
 * No guest writes except boot-time hooks, no retained scene pointers.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_photo_hud.h"
#include "kyoei_photo_scene.h"
#include "kyoei_hud_trace.h"
#include "kyoei_photo_tips_membership.h"
#include <windows.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

typedef void (ABI *FontDraw)(void *font);
static FontDraw originals[2];
static uintptr_t tips_image;
static _Atomic unsigned suppressed;

static int tips_read(void *context,uintptr_t address,void *out,size_t size) {
    (void)context;SIZE_T copied=0;
    /* Unlike VirtualQuery + memcpy, a released region between check and copy
     * reports failure. This does not promise a coherent guest scene snapshot. */
    return address && size && address<=UINTPTR_MAX-size &&
        ReadProcessMemory(GetCurrentProcess(),(const void*)address,out,size,&copied) && copied==size;
}
static int tips_hide(void *font) {
    if(!kyoei_photo_hud_hidden() || !kyoei_photo_scene_active())return 0;
    if(!photo_tips_contains(NULL,tips_read,tips_image,(uintptr_t)font))return 0;
    if(!kyoei_photo_hud_hidden() || !kyoei_photo_scene_active())return 0;
    unsigned n=atomic_load_explicit(&suppressed,memory_order_relaxed);
    /* Bounded telemetry, no object-pointer cache or per-frame log spam. */
    while(n<8 && !atomic_compare_exchange_weak_explicit(&suppressed,&n,n+1,
            memory_order_relaxed,memory_order_relaxed)){}
    if(n<8)gz_log("Photo tips: suppressed independent font=%p sample=%u (visual proof pending)",font,n+1);
    return 1;
}
static ABI void tips_extended_draw(void *font) {
    if(!tips_hide(font))originals[0](font);
}
static ABI void tips_dummy_draw(void *font) {
    if(!tips_hide(font))originals[1](font);
}
static uint64_t tips_hash(const unsigned char *code,size_t size) {
    uint64_t hash=UINT64_C(14695981039346656037);
    for(size_t i=0;i<size;++i)hash=(hash^code[i])*UINT64_C(1099511628211);
    return hash;
}
void kyoei_photo_tips_configure(void *image,uint64_t size) {
    const char *setting=getenv("KYOEI_PHOTO_TIPS");
    if(!setting || strcmp(setting,"1") || !kyoei_photo_hud_available())return;
    if(!image || size<0x152cbf8 || originals[0] || originals[1])return;
    unsigned char *base=image;
    const size_t entries[2]={0xae6820,0x2e56e0};
    const size_t lengths[2]={3681,588};
    const uint64_t hashes[2]={UINT64_C(0x168be630d35f5764),UINT64_C(0xb1205c06d881b81c)};
    const FontDraw callbacks[2]={tips_extended_draw,tips_dummy_draw};
    for(unsigned i=0;i<2;++i)if(tips_hash(base+entries[i],lengths[i])!=hashes[i]) {
        gz_log("Photo tips: font signature differs; both original paths retained");return;
    }
    unsigned char *relay=NULL;
    for(unsigned i=0;i<16 && !relay;++i)
        relay=VirtualAlloc(base+0xa000000+i*0x10000,4096,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);
    if(!relay){gz_log("Photo tips: relay unavailable; original fonts retained");return;}
    int32_t branches[2];DWORD previous[2],ignored;
    for(unsigned i=0;i<2;++i) {
        int64_t distance=(int64_t)(uintptr_t)(relay+i*64+32)-(int64_t)(uintptr_t)(base+entries[i]+5);
        if(distance<INT32_MIN || distance>INT32_MAX)goto fail;
        branches[i]=(int32_t)distance;
        /* Both verified functions start with the same complete six bytes. */
        kyoei_hud_trace_relay(relay+i*64,base+entries[i]+6,callbacks[i]);
    }
    if(!VirtualProtect(relay,4096,PAGE_EXECUTE_READ,&ignored) ||
       !FlushInstructionCache(GetCurrentProcess(),relay,128))goto fail;
    if(!VirtualProtect(base+entries[0],6,PAGE_EXECUTE_READWRITE,&previous[0]))goto fail;
    if(!VirtualProtect(base+entries[1],6,PAGE_EXECUTE_READWRITE,&previous[1])) {
        if(!VirtualProtect(base+entries[0],6,previous[0],&ignored))
            gz_stop(21,"STOP: cannot restore photo tips protection before installation");
        goto fail;
    }
    /* Called during boot, before guest threads. Publish both originals before
     * changing either entry. No partial installation on preparatory failures. */
    tips_image=(uintptr_t)image;originals[0]=(FontDraw)relay;originals[1]=(FontDraw)(relay+64);
    for(unsigned i=0;i<2;++i) {
        unsigned char *code=base+entries[i];
        code[0]=0xe9;memcpy(code+1,&branches[i],4);code[5]=0x90;
        if(!VirtualProtect(code,6,previous[i],&ignored) || !FlushInstructionCache(GetCurrentProcess(),code,6))
            gz_stop(21,"STOP: cannot finalize photo tips gate protection/cache");
    }
    gz_log("Photo tips: two experimental font gates installed; inactive outside photo; visual coverage unverified");
    return;
fail:
    VirtualFree(relay,0,MEM_RELEASE);gz_log("Photo tips: preparation failed; original fonts retained");
}
