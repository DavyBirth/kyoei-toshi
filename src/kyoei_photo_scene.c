/* Opt-in photo preview; scene coverage remains under validation.
 * Native pause plus temporary camera-source override during matrix composition.
 * No timestep changes, original camera source pose restored after each call.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_hud_trace.h"
#include "kyoei_photo_hud.h"
#include "kyoei_photo_scene.h"
#include <windows.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

typedef struct {float x,y,z;} Vec3;
typedef struct {uintptr_t unused;const int32_t *begin,*end,*capacity;} Exclusions;
typedef void (ABI *Frame)(void *manager);
typedef void (ABI *Compose)(void *camera);
typedef void (ABI *Pause)(void *manager,const int32_t *paused,const Exclusions *excluded);
static Frame original_frame;
static Compose original_compose;
static Pause original_pause;
static unsigned char *base;
static int preview,observer;
static _Atomic int available=0,requested=0,active=0,owned_pause=0,foreign_pause=0;
static SRWLOCK state_lock=SRWLOCK_INIT,input_lock=SRWLOCK_INIT;
typedef struct {float right,up,forward,yaw,pitch,fov,speed,roll;} Input;
static Input input;
typedef struct {
    void *manager,*camera,*table;
    Vec3 eye,initial_eye,initial_forward,initial_up;
    float distance,yaw,pitch,fov,roll,initial_fov;
    LARGE_INTEGER previous;
} State;
static State state;
static LARGE_INTEGER frequency;
static unsigned observations;
static ABI void compose_hook(void *camera);

static int readable(const void *pointer,size_t size) {
    MEMORY_BASIC_INFORMATION info;
    if(!pointer || !VirtualQuery(pointer,&info,sizeof(info)) || info.State!=MEM_COMMIT ||
       (info.Protect&(PAGE_NOACCESS|PAGE_GUARD)))return 0;
    uintptr_t offset=(uintptr_t)pointer-(uintptr_t)info.BaseAddress;
    return offset<=info.RegionSize && size<=info.RegionSize-offset;
}
static void *pointer_at(const void *object,size_t offset) {
    void *value=NULL;
    if(object && readable((const char*)object+offset,sizeof(value)))
        memcpy(&value,(const char*)object+offset,sizeof(value));
    return value;
}
static unsigned word_at(const void *object,size_t offset) {
    unsigned value=0;if(object && readable((const char*)object+offset,4))memcpy(&value,(const char*)object+offset,4);return value;
}
static void *main_camera(void) {
    void *manager=pointer_at(base,0x1340800);
    void *viewport=pointer_at(manager,0x48);
    return pointer_at(viewport,0x18);
}
static Vec3 add(Vec3 a,Vec3 b){return(Vec3){a.x+b.x,a.y+b.y,a.z+b.z};}
static Vec3 scale(Vec3 a,float s){return(Vec3){a.x*s,a.y*s,a.z*s};}
static float dot(Vec3 a,Vec3 b){return a.x*b.x+a.y*b.y+a.z*b.z;}
static Vec3 cross(Vec3 a,Vec3 b){return(Vec3){a.y*b.z-a.z*b.y,a.z*b.x-a.x*b.z,a.x*b.y-a.y*b.x};}
static int unit(Vec3 *a){float n=dot(*a,*a);if(!isfinite(n)||n<1e-10f)return 0;*a=scale(*a,1.f/sqrtf(n));return 1;}
static int finite3(Vec3 a){return isfinite(a.x)&&isfinite(a.y)&&isfinite(a.z);}
static Vec3 rotate(Vec3 value,Vec3 axis,float radians) {
    float co=cosf(radians),si=sinf(radians);
    return add(add(scale(value,co),scale(cross(axis,value),si)),scale(axis,dot(axis,value)*(1-co)));
}
static void direction(const State *s,Vec3 *forward,Vec3 *right,Vec3 *up) {
    *forward=rotate(s->initial_forward,s->initial_up,s->yaw);
    *right=cross(*forward,s->initial_up);unit(right);
    *forward=rotate(*forward,*right,s->pitch);unit(forward);
    *up=cross(*right,*forward);unit(up);
    *right=rotate(*right,*forward,s->roll);*up=rotate(*up,*forward,s->roll);
}
int kyoei_photo_scene_light_position(float xyz[3]) {
    if(!xyz || !atomic_load_explicit(&active,memory_order_acquire))return 0;
    AcquireSRWLockShared(&state_lock);State pose=state;ReleaseSRWLockShared(&state_lock);
    Vec3 forward,right,up;direction(&pose,&forward,&right,&up);
    Vec3 point=add(add(pose.eye,scale(forward,3.f)),scale(up,1.f));
    if(!finite3(point) || !atomic_load_explicit(&active,memory_order_acquire))return 0;
    xyz[0]=point.x;xyz[1]=point.y;xyz[2]=point.z;return 1;
}
float kyoei_photo_scene_fov_value(void) {
    AcquireSRWLockShared(&state_lock);float value=state.fov;ReleaseSRWLockShared(&state_lock);return value;
}
float kyoei_photo_scene_roll_value(void) {
    AcquireSRWLockShared(&state_lock);float value=state.roll;ReleaseSRWLockShared(&state_lock);return value*57.295779513f;
}
float kyoei_photo_scene_initial_fov(void) {
    AcquireSRWLockShared(&state_lock);float value=state.initial_fov;ReleaseSRWLockShared(&state_lock);return value;
}
void kyoei_photo_scene_roll(float delta) {
    if(!isfinite(delta))return;
    AcquireSRWLockExclusive(&input_lock);input.roll+=delta*.01745329252f;ReleaseSRWLockExclusive(&input_lock);
}
int kyoei_photo_scene_active(void){return atomic_load_explicit(&active,memory_order_relaxed);}
int kyoei_photo_scene_available(void){return atomic_load_explicit(&available,memory_order_acquire);}
void kyoei_photo_scene_input(float right,float up,float forward,float yaw,float pitch,float speed) {
    if(!isfinite(right)||!isfinite(up)||!isfinite(forward)||!isfinite(yaw)||!isfinite(pitch)||!isfinite(speed))return;
    AcquireSRWLockExclusive(&input_lock);
    input.right=fmaxf(-1,fminf(1,right));input.up=fmaxf(-1,fminf(1,up));input.forward=fmaxf(-1,fminf(1,forward));
    input.yaw+=yaw;input.pitch+=pitch;input.speed=fmaxf(.1f,fminf(10,speed));
    ReleaseSRWLockExclusive(&input_lock);
}
void kyoei_photo_scene_fov(float delta) {
    if(!isfinite(delta))return;
    AcquireSRWLockExclusive(&input_lock);input.fov+=delta;ReleaseSRWLockExclusive(&input_lock);
}
static void observe(void *manager,void *camera) {
    if(!observer || observations++>=16)return;
    /* First group10 node only: sampled provenance, not full enumeration. */
    void *group=pointer_at(manager,0x30+10*8);
    void *node=pointer_at(group,8),*entity=pointer_at(node,8);
    gz_log("Photo scene observer: manager=%p paused=%u main_camera=%p table=%p group10_first=%p type=%p flags20=%x flags38=%x",
        manager,word_at(manager,0x44d8),camera,pointer_at(camera,0),entity,pointer_at(entity,0),word_at(entity,0x20),word_at(entity,0x38));
}
static void native_pause(void *manager,int value) {
    const int32_t excluded_group=12; /* Same default exclusion as setUnitPause. */
    const Exclusions exclusions={0,&excluded_group,&excluded_group+1,&excluded_group+1};
    int32_t paused=value;original_pause(manager,&paused,&exclusions);
}
static int enter(void *manager,void *camera) {
    /* Never adopt/resume a pause already owned by the game. */
    if(word_at(manager,0x44d8) || !readable(manager,0x4500) || !readable(camera,0x3f0))return 0;
    State fresh={0};fresh.manager=manager;fresh.camera=camera;fresh.table=pointer_at(camera,0);
    Vec3 target;
    memcpy(&fresh.eye,(char*)camera+0x80,12);memcpy(&target,(char*)camera+0x260,12);
    memcpy(&fresh.initial_up,(char*)camera+0x270,12);memcpy(&fresh.fov,(char*)camera+0x284,4);
    fresh.initial_fov=fresh.fov;
    fresh.initial_forward=add(target,scale(fresh.eye,-1));fresh.distance=sqrtf(dot(fresh.initial_forward,fresh.initial_forward));
    if(!finite3(fresh.eye)||!unit(&fresh.initial_forward)||!unit(&fresh.initial_up)||
       fabsf(dot(fresh.initial_up,fresh.initial_forward))>.999f ||
       !isfinite(fresh.fov)||fresh.fov<1||fresh.fov>179)return 0;
    fresh.initial_eye=fresh.eye;QueryPerformanceCounter(&fresh.previous);
    AcquireSRWLockExclusive(&input_lock);memset(&input,0,sizeof(input));ReleaseSRWLockExclusive(&input_lock);
    AcquireSRWLockExclusive(&state_lock);state=fresh;ReleaseSRWLockExclusive(&state_lock);
    native_pause(manager,1);
    if(word_at(manager,0x44d8)!=1)return 0;
    atomic_store_explicit(&owned_pause,1,memory_order_relaxed);
    atomic_store_explicit(&active,1,memory_order_release);
    gz_log("Photo scene preview: native pause acquired; camera=%p; source pose restored after composition; coverage unverified",camera);
    return 1;
}
static void leave(void *manager,int handoff) {
    atomic_store_explicit(&active,0,memory_order_release);
    kyoei_photo_hud_restore();
    int owned=atomic_exchange_explicit(&owned_pause,0,memory_order_relaxed);
    /* Never follow a stale manager after transition and never undo a newer
     * native pause request; external native calls transfer pause ownership. */
    if(owned && !handoff && manager==state.manager && pointer_at(base,0x1336200)==manager && word_at(manager,0x44d8)==1)
        native_pause(manager,0);
    if(owned && manager!=state.manager)gz_log("Photo scene preview: manager changed; old lifetime unavailable; no stale access");
    atomic_store_explicit(&requested,0,memory_order_relaxed);
    AcquireSRWLockExclusive(&input_lock);memset(&input,0,sizeof(input));ReleaseSRWLockExclusive(&input_lock);
    Vec3 offset=add(state.eye,scale(state.initial_eye,-1));
    gz_log("Photo scene preview: released; native_pause_handoff=%d camera_distance_from_entry=%.2f",
        handoff,sqrtf(dot(offset,offset)));
}
static ABI void pause_hook(void *manager,const int32_t *paused,const Exclusions *excluded) {
    /* Own calls use the trampoline directly. Any original game request wins. */
    AcquireSRWLockShared(&state_lock);int same_manager=manager==state.manager;ReleaseSRWLockShared(&state_lock);
    if(same_manager && atomic_exchange_explicit(&owned_pause,0,memory_order_relaxed)) {
        atomic_store_explicit(&foreign_pause,1,memory_order_relaxed);
        atomic_store_explicit(&active,0,memory_order_release);
        atomic_store_explicit(&requested,0,memory_order_relaxed);
    }
    original_pause(manager,paused,excluded);
}
static ABI void frame_hook(void *manager) {
    int is_active=kyoei_photo_scene_active();
    int hidden=kyoei_photo_hud_hidden();
    /* A rejected scene/foreign pause remains latched off until F10 is closed.
     * Hide-only remains the default without KYOEI_PHOTO_SCENE=preview. */
    if(!hidden)atomic_store_explicit(&requested,1,memory_order_relaxed);
    int wanted=hidden && atomic_load_explicit(&requested,memory_order_relaxed);
    if(!preview && !observer){original_frame(manager);return;}
    void *camera=main_camera();observe(manager,camera);
    int foreign=atomic_exchange_explicit(&foreign_pause,0,memory_order_relaxed);
    if(foreign)leave(manager,1);
    else if(is_active && (!wanted || manager!=state.manager || camera!=state.camera ||
             pointer_at(camera,0)!=state.table || word_at(manager,0x44d8)!=1))leave(manager,0);
    else if(!is_active && wanted && preview) {
        if(!enter(manager,camera)) {
            atomic_store_explicit(&requested,0,memory_order_relaxed);
            kyoei_photo_hud_restore();
            gz_log("Photo scene preview: cannot acquire a stable unpaused scene; request rejected");
        }
    }
    if(kyoei_photo_scene_active()) {
        Input controls;AcquireSRWLockExclusive(&input_lock);controls=input;
        input.yaw=input.pitch=input.fov=input.roll=0;ReleaseSRWLockExclusive(&input_lock);
        LARGE_INTEGER now;QueryPerformanceCounter(&now);
        AcquireSRWLockExclusive(&state_lock);
        float dt=(float)((double)(now.QuadPart-state.previous.QuadPart)/frequency.QuadPart);
        state.previous=now;dt=fmaxf(0,fminf(.1f,dt));
        state.yaw+=fmaxf(-.5f,fminf(.5f,controls.yaw));
        state.pitch=fmaxf(-1.45f,fminf(1.45f,state.pitch+fmaxf(-.5f,fminf(.5f,controls.pitch))));
        state.fov=fmaxf(15,fminf(110,state.fov+controls.fov));
        state.roll=remainderf(state.roll+controls.roll,6.28318530718f);
        Vec3 forward,right,up;direction(&state,&forward,&right,&up);
        Vec3 step=add(add(scale(right,controls.right),scale(up,controls.up)),scale(forward,controls.forward));
        Vec3 next_eye=add(state.eye,scale(step,dt*2.f*fmaxf(.1f,controls.speed)));
        if(finite3(next_eye))state.eye=next_eye;
        ReleaseSRWLockExclusive(&state_lock);
    }
    original_frame(manager);
    /* Native pause can omit the normal camera-update dispatch. Recompose the
     * currently owned main view once here while keeping simulation paused. */
    if(kyoei_photo_scene_active()) {
        void *selected=main_camera();
        AcquireSRWLockShared(&state_lock);
        int same=selected==state.camera && pointer_at(selected,0)==state.table;
        ReleaseSRWLockShared(&state_lock);
        if(same && readable(selected,0x3f0))compose_hook(selected);
    }
}
static ABI void compose_hook(void *camera) {
    if(!atomic_load_explicit(&active,memory_order_acquire)){original_compose(camera);return;}
    State pose;AcquireSRWLockShared(&state_lock);pose=state;ReleaseSRWLockShared(&state_lock);
    if(camera!=pose.camera || camera!=main_camera() || pointer_at(camera,0)!=pose.table) {
        original_compose(camera);return;
    }
    /* Only these source components are overridden for the duration of the
     * original native matrix composer. Computed matrices remain its output. */
    Vec3 eye,target,up;float fov;
    memcpy(&eye,(char*)camera+0x80,12);memcpy(&target,(char*)camera+0x260,12);
    memcpy(&up,(char*)camera+0x270,12);memcpy(&fov,(char*)camera+0x284,4);
    Vec3 forward,right,photo_up;direction(&pose,&forward,&right,&photo_up);
    Vec3 photo_target=add(pose.eye,scale(forward,pose.distance));
    memcpy((char*)camera+0x80,&pose.eye,12);memcpy((char*)camera+0x260,&photo_target,12);
    memcpy((char*)camera+0x270,&photo_up,12);memcpy((char*)camera+0x284,&pose.fov,4);
    original_compose(camera);
    memcpy((char*)camera+0x80,&eye,12);memcpy((char*)camera+0x260,&target,12);
    memcpy((char*)camera+0x270,&up,12);memcpy((char*)camera+0x284,&fov,4);
}

typedef struct {size_t offset,length;uint64_t hash;void *callback;unsigned char *relay;} Hook;
static int signature(void *image,const Hook *hook) {
    uint64_t hash=UINT64_C(14695981039346656037);
    const unsigned char *code=(unsigned char*)image+hook->offset;
    for(size_t i=0;i<hook->length;++i)hash=(hash^code[i])*UINT64_C(1099511628211);
    return hash==hook->hash;
}
void kyoei_photo_scene_configure(void *image,uint64_t size) {
    const char *option=getenv("KYOEI_PHOTO_SCENE");
    if(!option || (strcmp(option,"observe") && strcmp(option,"preview")))return;
    preview=!strcmp(option,"preview");observer=!preview;
    if(!image || size<0x1340810 || !QueryPerformanceFrequency(&frequency))return;
    Hook hooks[]={
        {0x3542e0,209,UINT64_C(0x3d0956ac898b2106),frame_hook,NULL},
        {0x326300,805,UINT64_C(0x43df88be73088560),compose_hook,NULL},
        {0x6f4600,793,UINT64_C(0xfe87963393250a56),pause_hook,NULL}};
    for(unsigned i=0;i<3;++i)if(!signature(image,&hooks[i])) {
        gz_log("Photo scene: signature differs; unavailable; normal scene retained");return;
    }
    unsigned char *memory=NULL;
    for(unsigned i=0;i<16 && !memory;++i)
        memory=VirtualAlloc((unsigned char*)image+0x7000000+i*0x10000,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if(!memory){gz_log("Photo scene: no relay; unavailable");return;}
    for(unsigned i=0;i<3;++i) {
        Hook *hook=&hooks[i];hook->relay=memory+i*64;
        unsigned char *code=(unsigned char*)image+hook->offset;
        int64_t delta=(int64_t)(uintptr_t)(hook->relay+32)-(int64_t)(uintptr_t)(code+5);
        if(delta<INT32_MIN || delta>INT32_MAX){VirtualFree(memory,0,MEM_RELEASE);return;}
        kyoei_hud_trace_relay(hook->relay,code+6,hook->callback);
        /* Existing helper hardcodes pushR15; camera composer instead pushesR14.
         * All selected first6 bytes were decoded at unwind function entries:
         * push rbp;mov rbp,rsp;push r15/r14. No RIP-relative relocation. */
        memcpy(hook->relay,code,6);
    }
    DWORD previous;
    if(!VirtualProtect(memory,4096,PAGE_EXECUTE_READ,&previous) || !FlushInstructionCache(GetCurrentProcess(),memory,192)) {
        VirtualFree(memory,0,MEM_RELEASE);return;
    }
    original_frame=(Frame)hooks[0].relay;original_compose=(Compose)hooks[1].relay;original_pause=(Pause)hooks[2].relay;base=image;
    for(unsigned i=0;i<3;++i) {
        Hook *hook=&hooks[i];unsigned char *code=base+hook->offset;
        if(!VirtualProtect(code,6,PAGE_EXECUTE_READWRITE,&previous)) {
            /* Earlier hooks remain pure passthrough; no guest or GPU threads
             * have started, no requests can be accepted, relay must stay alive. */
            preview=observer=0;gz_log("Photo scene: hook protection unavailable; requests disabled");return;
        }
        int32_t relative=(int32_t)((intptr_t)(hook->relay+32)-(intptr_t)(code+5));
        code[0]=0xe9;memcpy(code+1,&relative,4);code[5]=0x90;
        DWORD ignored;
        if(!VirtualProtect(code,6,previous,&ignored)||!FlushInstructionCache(GetCurrentProcess(),code,6))
            gz_stop(21,"STOP: cannot finalize photo scene hook protection/cache");
    }
    atomic_store_explicit(&available,preview,memory_order_release);
    atomic_store_explicit(&requested,1,memory_order_relaxed);
    gz_log("Photo scene: private %s candidate installed at boot; inactive; runtime coverage unverified",preview?"preview":"observer");
}
