/* Kyoei identity adapter. Fixed Godzilla sources are compiled unchanged.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include <string.h>

uintptr_t kyoei_shared_kernel_resolve(const char *library, const char *symbol);

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
    if (!strcmp(library, "libkernel") && !strcmp(symbol, "sceKernelIsNeoMode"))
        return (uintptr_t)standard_ps4_mode;
    if (!strcmp(library, "libSceLibcInternalExt") && !strcmp(symbol, "sceLibcHeapGetTraceInfo"))
        return (uintptr_t)heap_trace_info;
    uintptr_t result = kyoei_shared_kernel_resolve(library, symbol);
    if (result && !strcmp(symbol, "sceKernelGetAppInfo")) return (uintptr_t)kyoei_app_info;
    return result;
}
