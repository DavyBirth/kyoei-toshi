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
static Collect original_collect;
static unsigned char *base;
static _Atomic unsigned samples[2];
static int readable(const void *p,size_t size){
    MEMORY_BASIC_INFORMATION info;
    if(!p || !VirtualQuery(p,&info,sizeof(info)) || info.State!=MEM_COMMIT ||
       (info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return 0;
    uintptr_t at=(uintptr_t)p-(uintptr_t)info.BaseAddress;
    return at<=info.RegionSize && size<=info.RegionSize-at;
}
static void *pointer_at(const void *p,size_t at){void *v=NULL;if(p && readable((const char*)p+at,sizeof(v)))memcpy(&v,(const char*)p+at,sizeof(v));return v;}
static float scalar_at(const void *p,size_t at){float v;memcpy(&v,(const char*)p+at,4);return v;}
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
static void passthrough(unsigned char *out,const unsigned char *code){
    // xor eax,eax; test r8d,r8d. Exactly five bytes, no RIP references.
    // Jump without clobbering EAX or flags: original JE follows at code+5.
    memcpy(out,code,5);absolute_jump(out+5,code+5);
}
void kyoei_photo_lights_configure(void *image,uint64_t size){
    const char *option=getenv("KYOEI_PHOTO_LIGHTS");
    if(!option || strcmp(option,"observe") || !image || size<0x1354430 || original_collect)return;
    unsigned char *code=(unsigned char*)image+0x343b10;
    if(hash_bytes(code,260)!=UINT64_C(0x05e68dd6b0c7aae0)){
        gz_log("Photo lights observer: signature differs; native collector retained");return;
    }
    unsigned char *relay=NULL;
    for(unsigned i=0;i<16 && !relay;++i)relay=VirtualAlloc((unsigned char*)image+0xd000000+i*0x10000,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!relay)return;
    const int64_t delta=(int64_t)(uintptr_t)(relay+32)-(int64_t)(uintptr_t)(code+5);
    if(delta<INT32_MIN || delta>INT32_MAX){VirtualFree(relay,0,MEM_RELEASE);return;}
    passthrough(relay,code);absolute_jump(relay+32,collect_lights);
    DWORD previous;
    if(!VirtualProtect(relay,4096,PAGE_EXECUTE_READ,&previous) || !FlushInstructionCache(GetCurrentProcess(),relay,46) ||
       !VirtualProtect(code,5,PAGE_EXECUTE_READWRITE,&previous)){VirtualFree(relay,0,MEM_RELEASE);return;}
    original_collect=(Collect)relay;base=image;
    const int32_t jump=(int32_t)delta;code[0]=0xe9;memcpy(code+1,&jump,4);
    DWORD ignored;
    if(!VirtualProtect(code,5,previous,&ignored) || !FlushInstructionCache(GetCurrentProcess(),code,5))
        gz_stop(21,"STOP: cannot finalize photo light observer");
    gz_log("Photo lights observer: native collector passthrough installed; no light/list modifications");
}
