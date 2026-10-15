/* Kyoei-specific offline contracts shared in behavior with kyoei_kernel.c.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include "kyoei_modules.h"
#include <SDL3/SDL.h>
typedef struct { uint64_t size;uint32_t flag,get_segment_info;uint64_t *mask,*states; } HeapTraceInfo;
_Static_assert(sizeof(HeapTraceInfo)==32,"heap trace ABI");
static uint64_t heap_mask,heap_states[64];
static ABI void heap_trace(HeapTraceInfo *info) {
    if(!info) { fputs("STOP: null heap trace information\n",stderr);exit(21); }
    info->mask=&heap_mask;info->states=heap_states;info->get_segment_info=0;
}
static ABI int32_t standard(void) { return 0; }
static ABI int32_t compiled_sdk(int32_t *out) {
    if(!out)return (int32_t)0x80020016;
    typedef void *(ABI *GetParam)(void);
    GetParam get=(GetParam)runtime_resolve("959qrazPIrg#p#J",0);
    void *param=get?get():NULL;
    if(!param)return (int32_t)0x80020016;
    memcpy(out,(char*)param+16,sizeof(*out));return 0;
}
typedef struct { int32_t version,align;uint64_t ids[2]; } Workaround;
typedef struct {
    int32_t id,mmap_flags,attribute_exe,attribute2;
    char cusa[10];unsigned char pad[6];uint64_t preload;
    int32_t attribute1,has_sfo;Workaround workaround;
} AppInfo;
_Static_assert(sizeof(AppInfo)==72,"PS4 app info ABI");
static ABI int32_t app_info(int32_t pid,AppInfo *out) {
    if(pid!=1000)return (int32_t)0x80020001;
    if(out){memset(out,0,sizeof(*out));memcpy(out->cusa,"CUSA03562",9);out->has_sfo=1;}
    return 0;
}
static ABI int32_t workaround(Workaround *tw,int32_t bit,int32_t *result) {
    if(!tw||!result)return (int32_t)0x8002000e;
    if(bit<0||bit>=0x3a)return (int32_t)0x80020016;
    *result=(tw->ids[bit>>6]>>(bit&63))&1;return 0;
}
static ABI const char *sandbox_word(void) {return "sys";}
/* Kyoei uses the default realtime condition clock; the inherited condition
 * implementation accepts this opaque default attribute object. */
static ABI int32_t condattr_init(void **out) {
    if(!out)return (int32_t)0x80020016;
    *out=calloc(1,16);return *out?0:(int32_t)0x8002000c;
}
static ABI int32_t condattr_destroy(void **attr) {
    if(!attr||!*attr)return (int32_t)0x80020016;
    free(*attr);*attr=NULL;return 0;
}
static int error_dialog;
static ABI int32_t error_init(void) {
    if(error_dialog)return (int32_t)0x80b80004;
    error_dialog=1;return 0;
}
static ABI int32_t error_status(void) {return error_dialog;}
static ABI int32_t error_term(void) {
    if(!error_dialog)return (int32_t)0x80b80001;
    error_dialog=0;return 0;
}
static ABI int32_t error_open(const int32_t *param) {
    if(!error_dialog)return (int32_t)0x80b80001;
    if(!param)return (int32_t)0x80b8000d;
    if(param[0]!=16)return (int32_t)0x80b8000a;
    char message[128];snprintf(message,sizeof(message),"The game reported error 0x%08x.",(unsigned)param[1]);
    fprintf(stderr,"Kyoei ErrorDialog: %s\n",message);error_dialog=2;
    if(!SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,"City Shrouded in Shadow",message,NULL))return (int32_t)0x80b80009;
    error_dialog=3;return 0;
}
static ABI int32_t marker(const char *name,uint32_t color) { (void)name;(void)color;return 0; }
static ABI int32_t login_users(int32_t *users) {
    if(!users)return (int32_t)0x80960005;
    users[0]=1;users[1]=users[2]=users[3]=-1;return 0;
}
static ABI int32_t available_space(const void *mount,uint64_t *kb) {
    if(!mount||!kb)return (int32_t)0x80d90002;
    struct statvfs info;
    if(statvfs(runtime_file_user_dir(),&info))return (int32_t)0x80020005;
    *kb=(uint64_t)info.f_bavail*info.f_frsize/1024;return 0;
}
static ABI int32_t addons(uint32_t label,void *list,uint32_t capacity,uint32_t *count) {
    (void)label;if((!list||!capacity)&&!count)return (int32_t)0x80d90002;
    if(count)*count=0;return 0;
}
static ABI int32_t recording_unsupported(void) { return (int32_t)0x80a80008; }
static ABI int32_t remote_status(int32_t user,int32_t *status) {
    (void)user;if(!status)return (int32_t)0x8002000e;*status=0;return 0;
}
static ABI int32_t disc_location(const char *path,int64_t offset,int64_t size,int32_t *flags,int32_t *first,int32_t *second) {
    (void)path;(void)offset;(void)size;
    if(!flags||!first||!second)return (int32_t)0x81100001;
    *flags=*first=*second=0;return 0;
}
static ABI int32_t module_args(uint32_t id,int32_t argc,const void *argv,uint64_t options,int32_t *result) {
    if(!id||id>0xffff||argc||argv||options)return (int32_t)0x805a1000;
    typedef int32_t (ABI *Load)(uint16_t);
    Load load=(Load)runtime_resolve("g8cM39EUZ6o#M#N",0);
    if(!load)return (int32_t)0x805a1001;
    int32_t status=load((uint16_t)id);if(!status&&result)*result=0;return status;
}
uintptr_t kyoei_native_services_resolve(const char *name) {
    const char *symbol=runtime_symbol(name);
    uintptr_t module=symbol?kyoei_modules_resolve(symbol):0;
    if(module)return module;
    static const RuntimeExport table[]={
        {"sceLibcHeapGetTraceInfo",heap_trace},
        {"sceKernelGetCompiledSdkVersion",compiled_sdk},
        {"sceKernelGetAppInfo",app_info},{"sceKernelTitleWorkaroundIsEnabled",workaround},
        {"sceKernelGetFsSandboxRandomWord",sandbox_word},
        {"scePthreadCondattrInit",condattr_init},{"scePthreadCondattrDestroy",condattr_destroy},
        {"sceErrorDialogInitialize",error_init},{"sceErrorDialogOpen",error_open},
        {"sceErrorDialogGetStatus",error_status},{"sceErrorDialogUpdateStatus",error_status},
        {"sceErrorDialogTerminate",error_term},
        {"sceKernelMlock",standard}, /* Same advisory no-op as the PS4 reference. */
        {"sceKernelIsNeoMode",standard},{"sceKernelIsAddressSanitizerEnabled",standard},
        {"sceRazorCpuPushMarker",marker},{"sceRazorCpuPopMarker",standard},
        {"sceUserServiceGetLoginUserIdList",login_users},
        {"sceAppContentDownloadDataGetAvailableSpaceKb",available_space},
        {"sceAppContentGetAddcontInfoList",addons},
        {"sceRemoteplayInitialize",standard},{"sceRemoteplayTerminate",standard},
        {"sceRemoteplayProhibitStreaming",standard},{"sceRemoteplayGetConnectionStatus",remote_status},
        {"sceVideoRecordingOpen",recording_unsupported},{"sceVideoRecordingStart",recording_unsupported},
        {"sceVideoRecordingClose",standard},{"sceVideoRecordingStop",standard},
        {"sceDiscMap_7C980FFB0AA27E7A",disc_location},
        {"sceSysmoduleLoadModuleInternalWithArg",module_args},
    };
    return RUNTIME_LOOKUP(table,name);
}
