/* Give the PC renderer time to compile a newly encountered pipeline before
 * entering the original guest polling routine. Its check and trap stay intact.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_gpu_wait.h"
#include <windows.h>
#include <string.h>
#include <limits.h>

static unsigned char *guest_image;

static int readable(const void *pointer, size_t bytes) {
    MEMORY_BASIC_INFORMATION info;
    if (!pointer || !VirtualQuery(pointer, &info, sizeof(info)) || info.State != MEM_COMMIT ||
        (info.Protect & (PAGE_NOACCESS | PAGE_GUARD))) return 0;
    const uintptr_t offset = (uintptr_t)pointer - (uintptr_t)info.BaseAddress;
    return offset <= info.RegionSize && bytes <= info.RegionSize - offset;
}

int kyoei_wait_real_label(const volatile uint64_t *label, uint64_t expected, unsigned timeout_ms) {
    const ULONGLONG started = GetTickCount64();
    for (;;) {
        if (__atomic_load_n(label, __ATOMIC_ACQUIRE) == expected) return 1;
        if (GetTickCount64() - started >= timeout_ms) return 0;
        Sleep(1);
    }
}

static ABI void wait_before_guest(void *context, uint64_t expected, const volatile uint64_t *label) {
    if (!label) {
        // Same fallback used by this verified guest routine (frame-local label).
        if (!readable(context, 0x90)) return;
        uintptr_t frames;
        uint32_t frame;
        memcpy(&frames, (const unsigned char *)context + 0x88, sizeof(frames));
        memcpy(&frame, guest_image + 0x1393150, sizeof(frame));
        const void *slot = (const void *)(frames + (uint64_t)frame * 0xcc00 + 0xcbb8);
        if (!readable(slot, sizeof(label))) return;
        memcpy(&label, slot, sizeof(label));
    }
    if (!readable((const void *)label, sizeof(*label))) return;
    const ULONGLONG started = GetTickCount64();
    const int reached = kyoei_wait_real_label(label, expected, 3000);
    const ULONGLONG elapsed = GetTickCount64() - started;
    if (elapsed >= 25 || !reached) {
        gz_log("Kyoei GPU wait: label=%p expected=%llu actual=%llu waited=%llu ms reached=%d",
               (const void *)label, (unsigned long long)expected,
               (unsigned long long)__atomic_load_n(label, __ATOMIC_ACQUIRE), elapsed, reached);
    }
    // On timeout, still enter the original routine. Do not skip its trap,
    // report success, write a fence value, or manufacture a completed frame.
}

size_t kyoei_build_wait_relay(unsigned char *relay, const void *callback, const unsigned char *original) {
    // SysV entry RSP is 8 mod 16. Three argument pushes align the callback.
    const unsigned char code[] = {
        0x57, 0x56, 0x52,                       // save RDI, RSI, RDX
        0x48, 0xb8, 0,0,0,0,0,0,0,0,          // absolute callback address
        0xff, 0xd0,                            // call RAX (SysV)
        0x5a, 0x5e, 0x5f,                     // restore the three guest arguments
        0x55, 0x48, 0x89, 0xe5, 0x41, 0x57,   // displaced guest prologue
        0xe9, 0,0,0,0                         // resume original at entry + 6
    };
    const int64_t distance = (int64_t)(uintptr_t)(original + 6) - (int64_t)(uintptr_t)(relay + sizeof(code));
    if (distance < INT32_MIN || distance > INT32_MAX) return 0;
    memcpy(relay, code, sizeof(code));
    const uintptr_t target = (uintptr_t)callback;
    const int32_t displacement = (int32_t)distance;
    memcpy(relay + 5, &target, sizeof(target));
    memcpy(relay + 25, &displacement, sizeof(displacement));
    return sizeof(code);
}

void kyoei_gpu_wait_configure(void *image, uint64_t size) {
    const size_t entry = 0x4330b0, length = 0x13c;
    if (size < 0x1393154) gz_stop(21, "STOP: GPU wait adapter image too small");
    unsigned char *original = (unsigned char *)image + entry;
    uint64_t hash = UINT64_C(14695981039346656037);
    for (size_t i = 0; i < length; ++i) hash = (hash ^ original[i]) * UINT64_C(1099511628211);
    if (hash != UINT64_C(0xfa40be72aa1369e9))
        gz_stop(21, "STOP: unknown guest GPU polling routine; review adapter");
    unsigned char *relay = NULL;
    for (unsigned i = 0; i < 16 && !relay; ++i) {
        relay = VirtualAlloc((unsigned char *)image + 0x4000000 + i * 0x10000,
                             4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    }
    if (!relay || !kyoei_build_wait_relay(relay, wait_before_guest, original))
        gz_stop(21, "STOP: cannot allocate guest GPU wait relay");
    DWORD prior;
    if (!VirtualProtect(relay, 4096, PAGE_EXECUTE_READ, &prior))
        gz_stop(21, "STOP: cannot protect GPU wait relay");
    const int64_t distance = (int64_t)(uintptr_t)relay - (int64_t)(uintptr_t)(original + 5);
    if (distance < INT32_MIN || distance > INT32_MAX ||
        !VirtualProtect(original, 6, PAGE_EXECUTE_READWRITE, &prior))
        gz_stop(21, "STOP: cannot install GPU wait relay");
    const int32_t displacement = (int32_t)distance;
    original[0] = 0xe9;
    memcpy(original + 1, &displacement, sizeof(displacement));
    original[5] = 0x90;
    DWORD ignored;
    if (!VirtualProtect(original, 6, prior, &ignored))
        gz_stop(21, "STOP: cannot restore GPU wait code protection");
    FlushInstructionCache(GetCurrentProcess(), relay, 29);
    FlushInstructionCache(GetCurrentProcess(), original, 6);
    guest_image = image;
    gz_log("Kyoei GPU wait: verified routine, cooperative wait up to 3000 ms; original poll and trap preserved");
}
