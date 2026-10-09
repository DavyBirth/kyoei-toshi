/* Kyoei identity adapter. Fixed Godzilla sources are compiled unchanged.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_modules.h"
#include <string.h>
#include <windows.h>

uintptr_t kyoei_shared_kernel_resolve(const char *library, const char *symbol);
uintptr_t kyoei_shared_resolve(const char *library, const char *symbol, int is_data);
uintptr_t kyoei_ime_resolve(const char *symbol);

static ABI int32_t installed_todo(int32_t handle, void *list, uint32_t capacity, uint32_t *entries) {
    typedef int32_t (ABI *Chunks)(int32_t, uint16_t *, uint32_t, uint32_t *);
    Chunks chunks = (Chunks)kyoei_shared_resolve("libScePlayGo", "scePlayGoGetChunkId", 0);
    uint32_t total;
    int32_t status = chunks(handle, NULL, 0, &total);
    if (status) return status;
    if (!list || !entries) return (int32_t)0x80b2000a;
    if (!capacity) return (int32_t)0x80b2000b;
    *entries = 0; /* Full dump: no download/installation tasks remain. */
    return 0;
}

static ABI int32_t internal_module_args(uint32_t id, int32_t argc, const void *argv, uint64_t options, int32_t *result) {
    if (!id || id > 0xffff || argc || argv || options) return (int32_t)0x805a1000;
    typedef int32_t (ABI *Load)(uint16_t);
    Load load = (Load)kyoei_shared_resolve("libSceSysmodule", "sceSysmoduleLoadModule", 0);
    int32_t status = load((uint16_t)id);
    if (!status && result) *result = 0;
    return status;
}
static ABI int32_t local_disc_location(const char *path, int64_t offset, int64_t bytes, int32_t *flags, int32_t *first, int32_t *second) {
    (void)path; (void)offset; (void)bytes;
    if (!flags || !first || !second) return (int32_t)0x81100001;
    *flags = 0; *first = 0; *second = 0;
    return 0; /* Same local-file location contract as the reference. */
}

/* Layout and behavior from shadPS4 kernel.cpp: persistent writable tables
 * provided to native libc; no tracing callback is enabled. */
typedef struct {
    uint64_t size;
    uint32_t flag, get_segment_info;
    uint64_t *mspace_mask, *mstate_table;
} HeapTraceInfo;
_Static_assert(sizeof(HeapTraceInfo) == 32, "heap trace ABI");
static uint64_t heap_mspace_mask, heap_mstates[64];
static ABI int32_t standard_ps4_mode(void) { return 0; }
/* Debug profiling annotations have no side effects without a Razor collector. */
static ABI int32_t razor_push_marker(const char *name, uint32_t color) { (void)name; (void)color; return 0; }
static ABI int32_t razor_pop_marker(void) { return 0; }
static ABI int32_t recording_unsupported(void) { return (int32_t)0x80a80008; }
static ABI int32_t login_users(int32_t *users) {
    if (!users) return (int32_t)0x80960005;
    users[0] = 1; users[1] = users[2] = users[3] = -1; return 0;
}
static ABI int32_t trophy_game_info(int32_t context, int32_t handle, void *details, void *data) {
    (void)context; (void)handle; (void)details; (void)data;
    /* The inherited runtime does not register a trophy database. Do not
     * manufacture game metadata or advertise an online trophy service. */
    return (int32_t)0x8055160f;
}
static ABI int32_t content_available_space(const void *mount, uint64_t *kilobytes) {
    if (!mount || !kilobytes) return (int32_t)0x80d90002;
    wchar_t path[32768]; ULARGE_INTEGER available;
    if (gz_widen(gz_user_dir(), path, 32768) || !GetDiskFreeSpaceExW(path, &available, NULL, NULL))
        return (int32_t)0x80020005;
    *kilobytes = available.QuadPart / 1024;
    return 0;
}
static ABI int32_t content_addons(uint32_t label, void *list, uint32_t capacity, uint32_t *count) {
    (void)label;
    if ((!list || !capacity) && !count) return (int32_t)0x80d90002;
    /* Only the base dump is mounted by this runtime; no DLC is installed. */
    if (count) *count = 0;
    return 0;
}
/* Local-only runtime: no remote streaming service or connected device. */
static ABI int32_t remote_initialize(void) { gz_log("Kyoei: Remote Play disabled (local session)"); return 0; }
static ABI int32_t remote_connection(int32_t user, int32_t *status) {
    (void)user;
    if (!status) return (int32_t)0x8002000e;
    *status = 0; return 0;
}
static ABI void heap_trace_info(HeapTraceInfo *info) {
    if (!info) gz_stop(21, "STOP: null heap trace information");
    info->mspace_mask = &heap_mspace_mask;
    info->mstate_table = heap_mstates;
    info->get_segment_info = 0;
}

static ABI int32_t kyoei_app_info(int32_t pid, void *info) {
    typedef int32_t (ABI *GetInfo)(int32_t, void *);
    GetInfo original = (GetInfo)kyoei_shared_kernel_resolve("libkernel", "sceKernelGetAppInfo");
    int32_t result = original(pid, info);
    /* PS4 AppInfo: four i32 fields before the 10-byte title ID. */
    if (!result && info) memcpy((char *)info + 16, "CUSA03562", 10);
    return result;
}

uintptr_t gz_kernel_resolve(const char *library, const char *symbol) {
    if (!strcmp(library, "libSceImeDialog")) return kyoei_ime_resolve(symbol);
    if (!strcmp(library, "libSceRazorCpu")) {
        if (!strcmp(symbol, "sceRazorCpuPushMarkerStatic")) return (uintptr_t)razor_push_marker;
        if (!strcmp(symbol, "sceRazorCpuPopMarker")) return (uintptr_t)razor_pop_marker;
    }
    if (!strcmp(library, "libSceVideoRecording")) {
        if (!strcmp(symbol, "sceVideoRecordingGetStatus")) return (uintptr_t)standard_ps4_mode; /* None */
        if (!strcmp(symbol, "sceVideoRecordingOpen2") || !strcmp(symbol, "sceVideoRecordingStart")) return (uintptr_t)recording_unsupported;
        if (!strcmp(symbol, "sceVideoRecordingClose") || !strcmp(symbol, "sceVideoRecordingStop")) return (uintptr_t)standard_ps4_mode;
    }
    if (!strcmp(library, "libSceUserService") && !strcmp(symbol, "sceUserServiceGetLoginUserIdList")) return (uintptr_t)login_users;
    if (!strcmp(library, "libSceNpTrophy") && !strcmp(symbol, "sceNpTrophyGetGameInfo")) return (uintptr_t)trophy_game_info;
    if (!strcmp(library, "libSceAppContent") && !strcmp(symbol, "sceAppContentDownloadDataGetAvailableSpaceKb"))
        return (uintptr_t)content_available_space;
    if (!strcmp(library, "libSceAppContent") && !strcmp(symbol, "sceAppContentGetAddcontInfoList"))
        return (uintptr_t)content_addons;
    if (!strcmp(library, "libSceRemoteplay")) {
        if (!strcmp(symbol, "sceRemoteplayInitialize")) return (uintptr_t)remote_initialize;
        if (!strcmp(symbol, "sceRemoteplayGetConnectionStatus")) return (uintptr_t)remote_connection;
        if (!strcmp(symbol, "sceRemoteplayTerminate") || !strcmp(symbol, "sceRemoteplayProhibitStreaming")) return (uintptr_t)standard_ps4_mode;
    }
    if (!strcmp(library, "libSceDiscMap") && !strcmp(symbol, "sceDiscMap_7C980FFB0AA27E7A"))
        return (uintptr_t)local_disc_location;
    if (!strcmp(library, "libSceSysmodule") && !strcmp(symbol, "sceSysmoduleLoadModuleInternalWithArg"))
        return (uintptr_t)internal_module_args;
    if (!strcmp(library, "libScePlayGo") && !strcmp(symbol, "scePlayGoGetToDoList"))
        return (uintptr_t)installed_todo;
    uintptr_t module_function;
    if (!strcmp(library, "libkernel") && (module_function = kyoei_modules_resolve(symbol))) return module_function;
    if (!strcmp(library, "libkernel") && (!strcmp(symbol, "sceKernelIsNeoMode") ||
        !strcmp(symbol, "sceKernelIsAddressSanitizerEnabled")))
        return (uintptr_t)standard_ps4_mode;
    if (!strcmp(library, "libSceLibcInternalExt") && !strcmp(symbol, "sceLibcHeapGetTraceInfo"))
        return (uintptr_t)heap_trace_info;
    uintptr_t result = kyoei_shared_kernel_resolve(library, symbol);
    if (result && !strcmp(symbol, "sceKernelGetAppInfo")) return (uintptr_t)kyoei_app_info;
    return result;
}
