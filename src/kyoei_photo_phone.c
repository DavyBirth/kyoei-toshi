/* Optional augmentation of the photo HUD gate; notification coverage pending.
 * Calls the sprite original unless exact live MailIcon provenance is proven.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_photo_hud.h"
#include "kyoei_hud_trace.h"
#include "kyoei_photo_phone_membership.h"
#include <windows.h>
#include <stdatomic.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

typedef void (ABI *SpriteDraw)(void *sprite);
static SpriteDraw original_sprite;
static unsigned char *base;
static int observe;
static _Atomic unsigned observations=0;
static SRWLOCK observation_lock=SRWLOCK_INIT;
static void *matched[16];
static int memory_read(void *unused,uintptr_t address,void *out,size_t size) {
    (void)unused;MEMORY_BASIC_INFORMATION info;
    if(!address || !VirtualQuery((void*)address,&info,sizeof(info)) || info.State!=MEM_COMMIT ||
       (info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return 0;
    uintptr_t offset=address-(uintptr_t)info.BaseAddress;
    if(offset>info.RegionSize || size>info.RegionSize-offset)return 0;
    memcpy(out,(void*)address,size);return 1;
}
static uintptr_t current_root(void) {
    uintptr_t root=0;memory_read(NULL,(uintptr_t)base+0x1534530,&root,sizeof(root));return root;
}
static void record_match(void *sprite) {
    if(!observe || atomic_load_explicit(&observations,memory_order_relaxed)>=16)return;
    AcquireSRWLockExclusive(&observation_lock);
    unsigned count=atomic_load_explicit(&observations,memory_order_relaxed);
    for(unsigned i=0;i<count;++i)if(matched[i]==sprite){ReleaseSRWLockExclusive(&observation_lock);return;}
    if(count<16) {
        uintptr_t table=0;unsigned flags=0;
        memory_read(NULL,(uintptr_t)sprite,&table,sizeof(table));memory_read(NULL,(uintptr_t)sprite+0x20,&flags,4);
        matched[count]=sprite;atomic_store_explicit(&observations,count+1,memory_order_relaxed);
        gz_log("Photo mail draw match: sprite=%p table=%llx flags20=%x independent=%d",
            sprite,(unsigned long long)table,flags,(flags&0x40)==0);
    }
    ReleaseSRWLockExclusive(&observation_lock);
}
static ABI void sprite_draw(void *sprite) {
    if(kyoei_photo_hud_hidden()) {
        uintptr_t root=current_root();
        if(photo_phone_contains(NULL,memory_read,(uintptr_t)base,root,(uintptr_t)sprite) && root==current_root()) {
            record_match(sprite);return;
        }
    }
    original_sprite(sprite);
}
void kyoei_photo_phone_configure(void *image,uint64_t size) {
    const char *setting=getenv("KYOEI_PHOTO_PHONE");if(!setting || strcmp(setting,"1"))return;
    if(!image || size<0x1534538)return;
    setting=getenv("KYOEI_PHOTO_PHONE_OBSERVE");observe=setting && !strcmp(setting,"1");
    unsigned char *code=(unsigned char*)image+0x324720;
    /* Whole function, from the validated GNU unwind boundary. */
    uint64_t hash=UINT64_C(14695981039346656037);
    for(unsigned i=0;i<1691;++i)hash=(hash^code[i])*UINT64_C(1099511628211);
    if(hash!=UINT64_C(0xb4105a762dea83c6)) { /* Filled from private facts before integration. */
        gz_log("Photo mail draw: sprite signature differs; original retained");return;
    }
    unsigned char *relay=NULL;
    for(unsigned i=0;i<16 && !relay;++i)
        relay=VirtualAlloc((unsigned char*)image+0x8000000+i*0x10000,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!relay)return;
    int64_t distance=(int64_t)(uintptr_t)(relay+32)-(int64_t)(uintptr_t)(code+5);DWORD previous;
    if(distance<INT32_MIN || distance>INT32_MAX){VirtualFree(relay,0,MEM_RELEASE);return;}
    size_t relay_size=kyoei_hud_trace_relay(relay,code+6,sprite_draw);
    if(!VirtualProtect(relay,4096,PAGE_EXECUTE_READ,&previous) ||
       !FlushInstructionCache(GetCurrentProcess(),relay,relay_size) ||
       !VirtualProtect(code,6,PAGE_EXECUTE_READWRITE,&previous)) {
        VirtualFree(relay,0,MEM_RELEASE);return;
    }
    original_sprite=(SpriteDraw)relay;base=image;
    int32_t relative=(int32_t)distance;code[0]=0xe9;memcpy(code+1,&relative,4);code[5]=0x90;
    DWORD ignored;
    if(!VirtualProtect(code,6,previous,&ignored) || !FlushInstructionCache(GetCurrentProcess(),code,6))
        gz_stop(21,"STOP: cannot finalize photo phone code protection/cache");
    gz_log("Photo mail draw: exact live MailIcon descendants only; hidden=0; coverage unverified");
}
