/* Kyoei identity adapter. Fixed Godzilla sources are compiled unchanged.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include <string.h>

uintptr_t kyoei_shared_kernel_resolve(const char *library, const char *symbol);

static ABI int32_t kyoei_app_info(int32_t pid, void *info) {
    typedef int32_t (ABI *GetInfo)(int32_t, void *);
    GetInfo original = (GetInfo)kyoei_shared_kernel_resolve("libkernel", "sceKernelGetAppInfo");
    int32_t result = original(pid, info);
    /* PS4 AppInfo: four i32 fields before the 10-byte title ID. */
    if (!result && info) memcpy((char *)info + 16, "CUSA03562", 10);
    return result;
}

uintptr_t gz_kernel_resolve(const char *library, const char *symbol) {
    uintptr_t result = kyoei_shared_kernel_resolve(library, symbol);
    if (result && !strcmp(symbol, "sceKernelGetAppInfo")) return (uintptr_t)kyoei_app_info;
    return result;
}
