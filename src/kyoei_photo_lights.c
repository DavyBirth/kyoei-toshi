/* Observe the native light collector before creating photo-owned sources.
 * No light/list/scene modifications. SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_photo_lights.h"
#include "kyoei_photo_hud.h"
#include "kyoei_photo_scene.h"
#include <windows.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <math.h>
typedef uint32_t (ABI *Collect)(void*,uint32_t,uint32_t,void**,uint32_t);
typedef void (ABI *CacheUpdate)(void*);
static Collect original_collect;
static CacheUpdate original_cache_update;
static unsigned char *base;
static _Atomic unsigned samples[2];
static _Atomic unsigned cache_samples[2];
static int readable(const void *p,size_t size){
    MEMORY_BASIC_INFORMATION info;
    if(!p || !VirtualQuery(p,&info,sizeof(info)) || info.State!=MEM_COMMIT ||
       (info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return 0;
    uintptr_t at=(uintptr_t)p-(uintptr_t)info.BaseAddress;
    return at<=info.RegionSize && size<=info.RegionSize-at;
}
static void *pointer_at(const void *p,size_t at){void *v=NULL;if(p && readable((const char*)p+at,sizeof(v)))memcpy(&v,(const char*)p+at,sizeof(v));return v;}
static float scalar_at(const void *p,size_t at){float v;memcpy(&v,(const char*)p+at,4);return v;}
typedef struct {void *light;uint32_t type,views;} LightEntry;
_Static_assert(sizeof(LightEntry)==16,"native light cache entry");
static ABI void update_light_cache(void *system){
    original_cache_update(system);
    if(system!=pointer_at(base,0x1354420) || pointer_at(system,0)!=base+0x1254b48 || !readable(system,0x68))return;
    uint32_t count,capacity;memcpy(&count,(char*)system+0x60,4);memcpy(&capacity,(char*)system+0x64,4);
    LightEntry *entries=pointer_at(system,0x58);
    if(!count || count>capacity || count>4096 || !readable(entries,(size_t)count*sizeof(*entries)))return;
    const int photo=kyoei_photo_hud_hidden() && kyoei_photo_scene_active();
    if(atomic_fetch_add_explicit(&cache_samples[photo],1,memory_order_relaxed)>=(photo?8u:4u))return;
    gz_log("Photo light cache observer: active=%d caller=%p count=%u capacity=%u",photo,__builtin_return_address(0),count,capacity);
    unsigned shown=0;
    for(uint32_t i=0;i<count && shown<4;++i){
        LightEntry entry;memcpy(&entry,entries+i,sizeof(entry));
        if((entry.type!=2 && entry.type!=3) || !readable(entry.light,0x240))continue;
        ++shown;
        gz_log("Photo light cache observer: index=%u type=%u views=%x table=%p xyz=%.3f,%.3f,%.3f rgb=%.3f,%.3f,%.3f range=%.3f",i,entry.type,entry.views,pointer_at(entry.light,0),
            scalar_at(entry.light,0x80),scalar_at(entry.light,0x84),scalar_at(entry.light,0x88),
            scalar_at(entry.light,0x230),scalar_at(entry.light,0x234),scalar_at(entry.light,0x238),scalar_at(entry.light,0x22c));
    }
}
static ABI uint32_t collect_lights(void *system,uint32_t types,uint32_t views,void **output,uint32_t capacity){
    const uintptr_t caller=(uintptr_t)__builtin_return_address(0);
    const uint32_t count=original_collect(system,types,views,output,capacity);
    void *table=pointer_at(system,0);
    if(system!=pointer_at(base,0x1354420) || (table!=base+0x1259ba0 && table!=base+0x1254b48))return count;
    const int photo=kyoei_photo_hud_hidden() && kyoei_photo_scene_active();
    if(atomic_fetch_add_explicit(&samples[photo],1,memory_order_relaxed)>=(photo?12u:4u))return count;
    gz_log("Photo lights observer: active=%d caller=%p types=%x views=%x capacity=%u count=%u",photo,(void*)caller,types,views,capacity,count);
    if(count>capacity || count>512 || !readable(output,(size_t)count*sizeof(void*)))return count;
    for(uint32_t i=0;i<count && i<8;++i){
        void *light=output[i];
        if(pointer_at(light,0)!=base+0x1254ed0 || !readable(light,0x270))continue;
        float x=scalar_at(light,0x80),y=scalar_at(light,0x84),z=scalar_at(light,0x88);
        float r=scalar_at(light,0x230),g=scalar_at(light,0x234),b=scalar_at(light,0x238),range=scalar_at(light,0x22c);
        if(!isfinite(x)||!isfinite(y)||!isfinite(z)||!isfinite(r)||!isfinite(g)||!isfinite(b)||!isfinite(range))continue;
        gz_log("Photo lights observer: point=%p index=%u xyz=%.3f,%.3f,%.3f rgb=%.3f,%.3f,%.3f range=%.3f",light,i,x,y,z,r,g,b,range);
    }
    return count;
}
static uint64_t hash_bytes(const unsigned char *p,size_t n){uint64_t h=UINT64_C(14695981039346656037);for(size_t i=0;i<n;++i)h=(h^p[i])*UINT64_C(1099511628211);return h;}
static void absolute_jump(unsigned char *out,const void *target){
    const unsigned char instruction[6]={0xff,0x25,0,0,0,0};
    memcpy(out,instruction,6);memcpy(out+6,&target,sizeof(target));
}
static void passthrough(unsigned char *out,const unsigned char *code,size_t displaced){
    // Audited whole instructions without RIP references. The collector's
    // five-byte xor/test needs its flags preserved for the following JE;
    // the cache updater instead displaces its six-byte stack prologue.
    memcpy(out,code,displaced);absolute_jump(out+displaced,code+displaced);
}
void kyoei_photo_lights_configure(void *image,uint64_t size){
    const char *option=getenv("KYOEI_PHOTO_LIGHTS");
    if(!option || strcmp(option,"observe") || !image || size<0x1354430 || original_collect)return;
    const struct {size_t offset,length,displaced;uint64_t hash;const void *callback;} hooks[]={
        {0x343b10,260,5,UINT64_C(0x05e68dd6b0c7aae0),(const void*)collect_lights},
        {0x290b80,361,6,UINT64_C(0xdd610201f41bd4bc),(const void*)update_light_cache}};
    for(unsigned i=0;i<2;++i)if(hash_bytes((unsigned char*)image+hooks[i].offset,hooks[i].length)!=hooks[i].hash){
        gz_log("Photo lights observer: signature differs; native collector retained");return;
    }
    unsigned char *relay=NULL;
    for(unsigned i=0;i<16 && !relay;++i)relay=VirtualAlloc((unsigned char*)image+0xd000000+i*0x10000,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!relay)return;
    for(unsigned i=0;i<2;++i){
        unsigned char *code=(unsigned char*)image+hooks[i].offset,*slot=relay+64*i;
        const int64_t delta=(int64_t)(uintptr_t)(slot+32)-(int64_t)(uintptr_t)(code+5);
        if(delta<INT32_MIN || delta>INT32_MAX){VirtualFree(relay,0,MEM_RELEASE);return;}
        passthrough(slot,code,hooks[i].displaced);absolute_jump(slot+32,hooks[i].callback);
    }
    DWORD previous;
    if(!VirtualProtect(relay,4096,PAGE_EXECUTE_READ,&previous) || !FlushInstructionCache(GetCurrentProcess(),relay,110)){VirtualFree(relay,0,MEM_RELEASE);return;}
    original_collect=(Collect)relay;original_cache_update=(CacheUpdate)(relay+64);base=image;
    for(unsigned i=0;i<2;++i){
        unsigned char *code=base+hooks[i].offset;
        if(!VirtualProtect(code,hooks[i].displaced,PAGE_EXECUTE_READWRITE,&previous)){
            gz_log("Photo lights observer: partial passthrough hooks; no light modifications");return;
        }
        const int32_t jump=(int32_t)((intptr_t)(relay+64*i+32)-(intptr_t)(code+5));code[0]=0xe9;memcpy(code+1,&jump,4);
        if(hooks[i].displaced==6)code[5]=0x90;
        DWORD ignored;
        if(!VirtualProtect(code,hooks[i].displaced,previous,&ignored) || !FlushInstructionCache(GetCurrentProcess(),code,hooks[i].displaced))
            gz_stop(21,"STOP: cannot finalize photo light observer");
    }
    gz_log("Photo lights observer: collector/cache passthrough installed; no light/list modifications");
}
