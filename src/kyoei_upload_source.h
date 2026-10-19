// Prefer the accessible guest view for CPU-only image uploads on Windows.
// The caller holds the runtime VMA read lock, and runs on the GPU command thread
// after excluding GPU-modified data. Mapping/protection changes from the guest
// require the VMA write lock; this thread owns creation of GPU read watchers.
// Never probe by reading a protected page: retain the physical backing fallback.
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <windows.h>
static inline const void* kyoei_upload_source(uintptr_t guest,const void* backing,
                                            size_t bytes,int enabled) {
    if (!enabled || bytes < 256u*1024u) return backing;
    MEMORY_BASIC_INFORMATION info;
    if (!VirtualQuery((const void*)guest,&info,sizeof(info)) ||
        info.State!=MEM_COMMIT || info.Type!=MEM_MAPPED ||
        (info.Protect & PAGE_GUARD)) return backing;
    const DWORD protection=info.Protect & 0xff;
    if (protection!=PAGE_READONLY && protection!=PAGE_READWRITE &&
        protection!=PAGE_EXECUTE_READ && protection!=PAGE_EXECUTE_READWRITE)
        return backing;
    const uintptr_t base=(uintptr_t)info.BaseAddress;
    if (guest<base || guest-base>info.RegionSize ||
        bytes>info.RegionSize-(guest-base)) return backing;
    return (const void*)guest;
}
