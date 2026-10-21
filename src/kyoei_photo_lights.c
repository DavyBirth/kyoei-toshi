/* Native light research: read-only observation, plus a separate opt-in
 * three independent sources. SPDX-License-Identifier: GPL-2.0-or-later */
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
/* 290cf0 consumes three stack arguments in addition to the six SysV registers.
 * Keep the selection/culling inputs opaque and forward every argument. */
typedef uint32_t (ABI *SelectLights)(void*,uint32_t,uint32_t,const void*,const void*,const void*,void**,uint32_t,uint32_t);
static Collect original_collect;
static CacheUpdate original_cache_update;
static SelectLights original_select;
typedef void *(ABI *CreatePoint)(void);
typedef void (ABI *UpdatePoint)(void*);
static CreatePoint create_point;
static UpdatePoint update_point;
static CreatePoint create_spot;
static UpdatePoint update_spot;
/* At most six native allocations (point/spot for each of three slots). Never
 * free a registered unit while native caches/jobs may still reference it. */
static void *owned_system;
static int source_proof,source_failed;
static int source_controls;
static _Atomic int source_available;
typedef struct {int enabled,type;float rgb[3],offset[3],yaw,pitch,cone;} SourceInput;
static SourceInput source_input[KYOEI_PHOTO_LIGHT_COUNT];
typedef struct {void *point,*objects[2];int was_active,object_type;SourceInput previous;float anchor[3],anchor_yaw,anchor_pitch;} OwnedSource;
static OwnedSource owned_sources[KYOEI_PHOTO_LIGHT_COUNT];
static SRWLOCK source_lock=SRWLOCK_INIT;
int kyoei_photo_lights_available(void){return atomic_load_explicit(&source_available,memory_order_acquire);}
void kyoei_photo_lights_input(unsigned slot,int enabled,float red,float green,float blue,float x,float y,float z,int type,float yaw,float pitch,float cone){
    if(slot>=KYOEI_PHOTO_LIGHT_COUNT || type<0 || type>1 || !isfinite(yaw) || fabsf(yaw)>360 ||
       !isfinite(pitch) || fabsf(pitch)>89 || !isfinite(cone) || cone<5 || cone>160)return;
    SourceInput value={!!enabled,type,{red,green,blue},{x,y,z},yaw,pitch,cone};
    for(unsigned i=0;i<3;++i)if(!isfinite(value.rgb[i]) || value.rgb[i]<0 || value.rgb[i]>16 ||
        !isfinite(value.offset[i]) || fabsf(value.offset[i])>50)return;
    AcquireSRWLockExclusive(&source_lock);source_input[slot]=value;ReleaseSRWLockExclusive(&source_lock);
}
static unsigned char *base;
static _Atomic unsigned samples[2];
static _Atomic unsigned cache_samples[2];
static _Atomic unsigned selection_samples[2];
static int readable(const void *p,size_t size){
    MEMORY_BASIC_INFORMATION info;
    if(!p || !VirtualQuery(p,&info,sizeof(info)) || info.State!=MEM_COMMIT ||
       (info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return 0;
    uintptr_t at=(uintptr_t)p-(uintptr_t)info.BaseAddress;
    return at<=info.RegionSize && size<=info.RegionSize-at;
}
static void *pointer_at(const void *p,size_t at){void *v=NULL;if(p && readable((const char*)p+at,sizeof(v)))memcpy(&v,(const char*)p+at,sizeof(v));return v;}
static float scalar_at(const void *p,size_t at){float v;memcpy(&v,(const char*)p+at,4);return v;}
static void set_word(void *p,size_t at,uint32_t value){memcpy((char*)p+at,&value,4);}
static void update_owned_point(void *system,unsigned slot){
    if(!source_proof || source_failed)return;
    if(system!=pointer_at(base,0x1354420) || pointer_at(system,0)!=base+0x1254b48)return;
    /* A changed system ends this proof. Do not follow an old allocator/unit
     * across an unproven scene lifetime or register the same unit twice. */
    if(owned_system && owned_system!=system){source_failed=1;atomic_store(&source_available,0);return;}
    OwnedSource *source=&owned_sources[slot];
    SourceInput request={slot==0,0,{8.f,.2f,.2f},{0,0,0},0,-15,45};
    if(source_controls){AcquireSRWLockShared(&source_lock);request=source_input[slot];ReleaseSRWLockShared(&source_lock);}
    const int wanted=request.enabled && kyoei_photo_hud_hidden() && kyoei_photo_scene_active();
    float xyz[3],forward[3];
    if(wanted && !source->was_active && (!kyoei_photo_scene_light_position(xyz) ||
       !kyoei_photo_scene_light_move(0,0,1,forward)))return;
    if(wanted && (!source->point || source->object_type!=request.type)){
        void *created=source->objects[request.type];
        if(!created){
            owned_system=system;created=request.type?create_spot():create_point();
            if(!readable(created,request.type?0x290:0x270) || pointer_at(created,0)!=base+(request.type?0x1254fc8:0x1254ed0)){
                for(unsigned i=0;i<KYOEI_PHOTO_LIGHT_COUNT;++i)if(owned_sources[i].point){
                    set_word(owned_sources[i].point,0x220,0);set_word(owned_sources[i].point,0x30,0);owned_sources[i].was_active=0;
                }
                source_failed=1;atomic_store(&source_available,0);gz_log("Photo light source proof: native factory failed; no further writes");return;
            }
            source->objects[request.type]=created;
            gz_log("Photo light source: slot=%u created native type=%d object=%p; reusable process-lifetime allocation",slot+1,request.type,created);
        }
        if(source->point && source->point!=created){set_word(source->point,0x220,0);set_word(source->point,0x30,0);}
        source->point=created;
        source->object_type=request.type;
    }
    void *owned_point=source->point;
    if(!owned_point || (wanted==source->was_active && (!wanted || !memcmp(&request,&source->previous,sizeof(request)))))return;
    if(wanted){
        if(!source->was_active){
            memcpy(source->anchor,xyz,sizeof(xyz));
            source->anchor_yaw=atan2f(forward[0],forward[2]);
            source->anchor_pitch=asinf(fmaxf(-1,fminf(1,forward[1])));
        }
        for(unsigned i=0;i<3;++i)xyz[i]=source->anchor[i]+request.offset[i];
        memcpy((char*)owned_point+0x80,xyz,sizeof(xyz));
        memcpy((char*)owned_point+0x230,request.rgb,sizeof(request.rgb));
        /* Main viewport only. Native range calculation is retained;
         * this proof does not enter the ordinary unit-update/culling jobs. */
        set_word(owned_point,0x30,1);set_word(owned_point,0x228,1);
        if(request.type){
            const float yaw=source->anchor_yaw+request.yaw*.01745329252f;
            const float pitch=fmaxf(-1.553343f,fminf(1.553343f,source->anchor_pitch+request.pitch*.01745329252f));
            const float direction[3]={cosf(pitch)*sinf(yaw),sinf(pitch),cosf(pitch)*cosf(yaw)};
            const float cone[2]={request.cone*.01745329252f,request.cone*.75f*.01745329252f};
            memcpy((char*)owned_point+0x250,direction,sizeof(direction));memcpy((char*)owned_point+0x260,cone,sizeof(cone));
            update_spot(owned_point);
        }else update_point(owned_point);
        set_word(owned_point,0x220,1);
        if(!source->was_active || source->previous.type!=request.type){
            atomic_store_explicit(&selection_samples[1],0,memory_order_relaxed);
            gz_log("Photo light source: slot=%u type=%d enabled xyz=%.3f,%.3f,%.3f range=%.3f",slot+1,request.type,xyz[0],xyz[1],xyz[2],scalar_at(owned_point,0x22c));
        }
    }else{
        set_word(owned_point,0x220,0);set_word(owned_point,0x30,0);
        gz_log("Photo light source: slot=%u disabled; allocation retained for outstanding native references",slot+1);
    }
    source->was_active=wanted;
    source->previous=request;
}
typedef struct {void *light;uint32_t type,views;} LightEntry;
_Static_assert(sizeof(LightEntry)==16,"native light cache entry");
static ABI uint32_t select_lights(void *system,uint32_t types,uint32_t views,const void *origin,
                                  const void *camera,const void *filter,void **output,uint32_t capacity,uint32_t viewport){
    const uint32_t count=original_select(system,types,views,origin,camera,filter,output,capacity,viewport);
    if(system!=pointer_at(base,0x1354420) || pointer_at(system,0)!=base+0x1254b48)return count;
    const int photo=kyoei_photo_hud_hidden() && kyoei_photo_scene_active();
    if(atomic_fetch_add_explicit(&selection_samples[photo],1,memory_order_relaxed)>=(photo?8u:4u))return count;
    gz_log("Photo light selection observer: active=%d caller=%p types=%x views=%x viewport=%u capacity=%u count=%u",
        photo,__builtin_return_address(0),types,views,viewport,capacity,count);
    if(count>capacity || count>512 || !readable(output,(size_t)count*sizeof(*output)))return count;
    unsigned points=0,spots=0,owned=0;
    for(uint32_t i=0;i<count;++i){
        void *light=output[i];uint32_t type;
        for(unsigned slot=0;slot<KYOEI_PHOTO_LIGHT_COUNT;++slot)if(light==owned_sources[slot].point){++owned;break;}
        if(!readable(light,0x240))continue;
        memcpy(&type,(char*)light+0x224,4);
        if(type==2)++points;else if(type==3)++spots;else continue;
        if(i>=8)continue;
        gz_log("Photo light selection observer: index=%u type=%u light=%p table=%p xyz=%.3f,%.3f,%.3f rgb=%.3f,%.3f,%.3f range=%.3f",
            i,type,light,pointer_at(light,0),scalar_at(light,0x80),scalar_at(light,0x84),scalar_at(light,0x88),
            scalar_at(light,0x230),scalar_at(light,0x234),scalar_at(light,0x238),scalar_at(light,0x22c));
    }
    gz_log("Photo light selection observer: selected_points=%u selected_spots=%u selected_photo_source=%u",points,spots,owned);
    return count;
}
static ABI void update_light_cache(void *system){
    for(unsigned slot=0;slot<KYOEI_PHOTO_LIGHT_COUNT;++slot)update_owned_point(system,slot);
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
    if(!option || (strcmp(option,"observe") && strcmp(option,"source-proof") && strcmp(option,"preview")) || !image || size<0x1354430 || original_collect)return;
    source_controls=!strcmp(option,"preview");source_proof=source_controls || !strcmp(option,"source-proof");
    if(source_proof){
        const struct {size_t offset,length;uint64_t hash;} factory[]={
            {0x2ac650,121,UINT64_C(0xd49bd3916f22f555)},
            {0x358af0,189,UINT64_C(0x1e90dcb52a2b736f)},
            {0x359b90,404,UINT64_C(0x9469e3350e7b927c)},
            {0x357410,97,UINT64_C(0x55ec9666f5faca7c)},
            {0x2ac830,184,UINT64_C(0x8fc05ab47b4a6a5e)},
            {0x2ac8f0,121,UINT64_C(0x143749240a1a14f3)},
            {0x358e30,239,UINT64_C(0xf053be1115bccbef)},
            {0x2acad0,184,UINT64_C(0x5f434f1c5fd1b0cc)}};
        for(unsigned i=0;i<sizeof(factory)/sizeof(factory[0]);++i)
            if(hash_bytes((unsigned char*)image+factory[i].offset,factory[i].length)!=factory[i].hash){
                gz_log("Photo light source proof: factory signature differs; native lights retained");return;
            }
        create_point=(CreatePoint)((unsigned char*)image+0x2ac650);
        update_point=(UpdatePoint)((unsigned char*)image+0x2ac830);
        create_spot=(CreatePoint)((unsigned char*)image+0x2ac8f0);
        update_spot=(UpdatePoint)((unsigned char*)image+0x2acad0);
    }
    const struct {size_t offset,length,displaced;uint64_t hash;const void *callback;} hooks[]={
        {0x343b10,260,5,UINT64_C(0x05e68dd6b0c7aae0),(const void*)collect_lights},
        {0x290b80,361,6,UINT64_C(0xdd610201f41bd4bc),(const void*)update_light_cache},
        {0x290cf0,1457,6,UINT64_C(0xff3f63c35f31e2dc),(const void*)select_lights}};
    const unsigned hook_count=(unsigned)(sizeof(hooks)/sizeof(hooks[0]));
    for(unsigned i=0;i<hook_count;++i)if(hash_bytes((unsigned char*)image+hooks[i].offset,hooks[i].length)!=hooks[i].hash){
        gz_log("Photo lights observer: signature differs; native collector retained");return;
    }
    unsigned char *relay=NULL;
    for(unsigned i=0;i<16 && !relay;++i)relay=VirtualAlloc((unsigned char*)image+0xd000000+i*0x10000,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!relay)return;
    for(unsigned i=0;i<hook_count;++i){
        unsigned char *code=(unsigned char*)image+hooks[i].offset,*slot=relay+64*i;
        const int64_t delta=(int64_t)(uintptr_t)(slot+32)-(int64_t)(uintptr_t)(code+5);
        if(delta<INT32_MIN || delta>INT32_MAX){VirtualFree(relay,0,MEM_RELEASE);return;}
        passthrough(slot,code,hooks[i].displaced);absolute_jump(slot+32,hooks[i].callback);
    }
    DWORD previous;
    if(!VirtualProtect(relay,4096,PAGE_EXECUTE_READ,&previous) || !FlushInstructionCache(GetCurrentProcess(),relay,64*hook_count)){VirtualFree(relay,0,MEM_RELEASE);return;}
    original_collect=(Collect)relay;original_cache_update=(CacheUpdate)(relay+64);original_select=(SelectLights)(relay+128);base=image;
    for(unsigned i=0;i<hook_count;++i){
        unsigned char *code=base+hooks[i].offset;
        if(!VirtualProtect(code,hooks[i].displaced,PAGE_EXECUTE_READWRITE,&previous)){
            source_proof=0;gz_log("Photo lights observer: partial passthrough hooks; source proof disabled");return;
        }
        const int32_t jump=(int32_t)((intptr_t)(relay+64*i+32)-(intptr_t)(code+5));code[0]=0xe9;memcpy(code+1,&jump,4);
        if(hooks[i].displaced==6)code[5]=0x90;
        DWORD ignored;
        if(!VirtualProtect(code,hooks[i].displaced,previous,&ignored) || !FlushInstructionCache(GetCurrentProcess(),code,hooks[i].displaced))
            gz_stop(21,"STOP: cannot finalize photo light observer");
    }
    atomic_store_explicit(&source_available,source_controls,memory_order_release);
    gz_log("Photo lights: collector/cache/selection hooks installed; source_proof=%d; observer-only=%d",source_proof,!source_proof);
}
