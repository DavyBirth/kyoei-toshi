/* Override verified DOF parameters during native effect preparation.
 * Normal filter state is restored on exit; source camera is preserved.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_photo_dof.h"
#include "kyoei_photo_hud.h"
#include "kyoei_photo_scene.h"
#include "kyoei_hud_trace.h"
#include <windows.h>
#include <stdatomic.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>
typedef void (ABI *Update)(void*);
static Update original_update;
static unsigned char *base;
static _Atomic int observed=0;
static SRWLOCK input_lock=SRWLOCK_INIT;
static struct {int mode;float distance,aperture,fov;} input={0,5,2.8f,40};
/* Guest render-preparation thread only; identity is checked against live globals. */
static struct {void *filter,*manager;unsigned enabled;unsigned char parameters[36];} saved;
static unsigned observations;
static int readable(const void *pointer,size_t size) {
    MEMORY_BASIC_INFORMATION info;
    if(!pointer || !VirtualQuery(pointer,&info,sizeof(info)) || info.State!=MEM_COMMIT ||
       (info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return 0;
    uintptr_t offset=(uintptr_t)pointer-(uintptr_t)info.BaseAddress;
    return offset<=info.RegionSize && size<=info.RegionSize-offset;
}
static void *pointer_at(const void *object,size_t offset) {
    void *p=NULL;
    if(object && readable((const char*)object+offset,sizeof(p)))memcpy(&p,(const char*)object+offset,sizeof(p));
    return p;
}
static void put_u(void *object,size_t at,unsigned value){memcpy((char*)object+at,&value,4);}
static void put_f(void *object,size_t at,float value){memcpy((char*)object+at,&value,4);}
static float get_f(const void *object,size_t at){float value;memcpy(&value,(const char*)object+at,4);return value;}
int kyoei_photo_dof_available(void){return original_update && atomic_load_explicit(&observed,memory_order_acquire);}
void kyoei_photo_dof_input(int mode,float distance,float aperture,float fov) {
    if(mode<0 || mode>2 || !isfinite(distance) || !isfinite(aperture) || !isfinite(fov))return;
    AcquireSRWLockExclusive(&input_lock);
    input.mode=mode;input.distance=fmaxf(.2f,fminf(1000,distance));
    input.aperture=fmaxf(1,fminf(32,aperture));input.fov=fmaxf(15,fminf(110,fov));
    ReleaseSRWLockExclusive(&input_lock);
}
static ABI void update_dof(void *filter) {
    void *live=pointer_at(base,0x13425a8),*manager=pointer_at(base,0x1336200);
    uintptr_t table=(uintptr_t)pointer_at(filter,0);
    // SysFilter is the game's registered subclass of GxSysFilter.
    if(filter!=live || !readable(filter,0xd4) ||
       (table!=(uintptr_t)base+0x1251b20 && table!=(uintptr_t)base+0x127bb30)) {
        original_update(filter);return;
    }
    atomic_store_explicit(&observed,1,memory_order_release);
    int mode;float distance,aperture,fov;
    AcquireSRWLockShared(&input_lock);mode=input.mode;distance=input.distance;aperture=input.aperture;fov=input.fov;ReleaseSRWLockShared(&input_lock);
    if(saved.filter && (saved.filter!=filter || saved.manager!=manager))memset(&saved,0,sizeof(saved));
    void *camera_manager=pointer_at(base,0x1340800);
    void *view=pointer_at(camera_manager,0x48),*camera=pointer_at(view,0x18);
    const int camera_valid=readable(camera,0x290) && isfinite(get_f(camera,0x280)) &&
        get_f(camera,0x280)>0 && get_f(camera,0x280)<10 && isfinite(get_f(camera,0x288)) &&
        isfinite(get_f(camera,0x28c)) && get_f(camera,0x288)>0 && get_f(camera,0x28c)>get_f(camera,0x288);
    if(!mode || !kyoei_photo_hud_hidden() || !kyoei_photo_scene_active() || !camera_valid) {
        if(saved.filter) {
            put_u(filter,0xa0,saved.enabled);memcpy((char*)filter+0xb0,saved.parameters,36);
            memset(&saved,0,sizeof(saved));gz_log("Photo DOF: original filter state restored");
        }
        original_update(filter);return;
    }
    if(!saved.filter) {
        saved.filter=filter;saved.manager=manager;
        memcpy(&saved.enabled,(char*)filter+0xa0,4);memcpy(saved.parameters,(char*)filter+0xb0,36);
    }
    put_u(filter,0xa0,mode==1);
    if(mode==1) {
        // Native optical convention: focal mm, focus in scene units. These
        // near/far transition widths follow the native DOF calculation.
        const float focal=17.5f/tanf(fov*.00872664625997f)*.001f;
        const float coc=distance*.001f*aperture*.011916f;
        const float near_width=coc*distance/(focal*focal+coc);
        const float far_width=focal*focal>coc?coc*distance/(focal*focal-coc):1000.f;
        put_f(filter,0xb4,distance-near_width);put_f(filter,0xb8,distance+far_width);
        memcpy((char*)filter+0xbc,(char*)camera+0x288,8);
        put_f(filter,0xc4,distance);put_f(filter,0xc8,aperture);put_f(filter,0xcc,fov);
        memcpy((char*)filter+0xd0,(char*)camera+0x280,4);
    }
    if(observations++<8)gz_log("Photo DOF: native mode=%d focus=%.3f aperture=%.2f fov=%.2f",mode,distance,aperture,fov);
    original_update(filter);
}
static uint64_t hash_bytes(const unsigned char *p,size_t n) {
    uint64_t h=UINT64_C(14695981039346656037);
    for(size_t i=0;i<n;++i)h=(h^p[i])*UINT64_C(1099511628211);return h;
}
void kyoei_photo_dof_configure(void *image,uint64_t size) {
    const char *option=getenv("KYOEI_PHOTO_DOF");
    if(!option || strcmp(option,"1") || !image || size<0x13425b0 || original_update)return;
    unsigned char *code=(unsigned char*)image+0x2458a0;
    if(hash_bytes(code,2723)!=UINT64_C(0x173e9b5337d8a54d)) {
        gz_log("Photo DOF: native signature differs; original retained");return;
    }
    unsigned char *relay=NULL;
    for(unsigned i=0;i<16 && !relay;++i)relay=VirtualAlloc((unsigned char*)image+0xb000000+i*0x10000,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!relay)return;
    int64_t delta=(int64_t)(uintptr_t)(relay+32)-(int64_t)(uintptr_t)(code+5);DWORD previous;
    if(delta<INT32_MIN || delta>INT32_MAX){VirtualFree(relay,0,MEM_RELEASE);return;}
    size_t n=kyoei_hud_trace_relay(relay,code+6,update_dof);
    if(!VirtualProtect(relay,4096,PAGE_EXECUTE_READ,&previous) || !FlushInstructionCache(GetCurrentProcess(),relay,n) ||
       !VirtualProtect(code,6,PAGE_EXECUTE_READWRITE,&previous)){VirtualFree(relay,0,MEM_RELEASE);return;}
    original_update=(Update)relay;base=image;
    int32_t jump=(int32_t)delta;code[0]=0xe9;memcpy(code+1,&jump,4);code[5]=0x90;
    DWORD ignored;
    if(!VirtualProtect(code,6,previous,&ignored) || !FlushInstructionCache(GetCurrentProcess(),code,6))
        gz_stop(21,"STOP: cannot finalize photo DOF hook protection/cache");
    gz_log("Photo DOF: native parameter hook installed; starts in original mode; scene validation pending");
}
