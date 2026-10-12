/* Windows TLS adapter for Kyoei. Variant-II layout follows bbport's
 * runtime_thread.c; FS loads are handled without changing Windows GS/TEB.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_tls.h"
#include <malloc.h>
#include <stdlib.h>
#include <string.h>

static unsigned char *guest_image;
static uint64_t image_bytes, template_offset, template_bytes, memory_bytes, alignment;
static DWORD tls_slot = FLS_OUT_OF_INDEXES;
typedef struct { unsigned char *allocation; uint64_t *tcb; uint64_t dtv[4]; } LocalTls;

static VOID CALLBACK release_tls(PVOID value) {
    LocalTls *tls = value;
    if (tls) { _aligned_free(tls->allocation); free(tls); }
}

void kyoei_tls_configure(void *image, uint64_t size, const uint64_t layout[4]) {
    if (tls_slot != FLS_OUT_OF_INDEXES) gz_stop(21, "STOP: TLS configured twice");
    if (layout[1] > layout[2] || layout[2] > (1u << 20) || layout[0] > size ||
        layout[1] > size - layout[0] || !layout[3] || layout[3] > 4096 ||
        (layout[3] & (layout[3] - 1))) gz_stop(21, "STOP: invalid main TLS layout");
    guest_image = image; image_bytes = size; template_offset = layout[0];
    template_bytes = layout[1]; memory_bytes = layout[2]; alignment = layout[3];
    tls_slot = FlsAlloc(release_tls);
    if (tls_slot == FLS_OUT_OF_INDEXES) gz_stop(21, "STOP: cannot allocate TLS slot");
    gz_log("Kyoei TLS: %llu bytes per thread, alignment %llu, template %llu bytes",
           (unsigned long long)memory_bytes, (unsigned long long)alignment,
           (unsigned long long)template_bytes);
}

static LocalTls *current_tls(void) {
    if (tls_slot == FLS_OUT_OF_INDEXES) gz_stop(21, "STOP: main TLS not configured");
    LocalTls *tls = FlsGetValue(tls_slot);
    if (tls) return tls;
    tls = calloc(1, sizeof(*tls));
    uint64_t offset = (memory_bytes + alignment - 1) & ~(alignment - 1);
    size_t allocation_alignment = alignment < 64 ? 64 : (size_t)alignment;
    if (!tls || !(tls->allocation = _aligned_malloc((size_t)offset + 256, allocation_alignment)))
        gz_stop(21, "STOP: cannot allocate main TLS block");
    memset(tls->allocation, 0, (size_t)offset + 256);
    memcpy(tls->allocation, guest_image + template_offset, (size_t)template_bytes);
    tls->tcb = (uint64_t *)(tls->allocation + offset);
    tls->tcb[0] = (uintptr_t)tls->tcb;
    tls->dtv[0] = 1; tls->dtv[2] = (uintptr_t)tls->allocation;
    tls->tcb[1] = (uintptr_t)tls->dtv;
    typedef void *(ABI *Self)(void);
    Self self = (Self)gz_thread_resolve("libkernel", "scePthreadSelf");
    tls->tcb[2] = self ? (uintptr_t)self() : 0;
    if (!FlsSetValue(tls_slot, tls)) gz_stop(21, "STOP: cannot publish main TLS");
    return tls;
}

void *kyoei_tls_tcb(void) { return current_tls()->tcb; }
void *kyoei_tls_address(uint64_t offset) {
    if (offset >= memory_bytes) gz_stop(21, "STOP: main TLS offset out of range");
    return current_tls()->allocation + offset;
}

int kyoei_tls_fault(EXCEPTION_POINTERS *exception) {
    if (exception->ExceptionRecord->ExceptionCode != EXCEPTION_ACCESS_VIOLATION ||
        exception->ExceptionRecord->NumberParameters < 2 ||
        exception->ExceptionRecord->ExceptionInformation[0] != 0 ||
        exception->ExceptionRecord->ExceptionInformation[1] != 0) return 0;
    CONTEXT *context = exception->ContextRecord;
    uintptr_t rip = (uintptr_t)context->Rip;
    if (rip < (uintptr_t)guest_image || rip - (uintptr_t)guest_image > image_bytes ||
        image_bytes - (rip - (uintptr_t)guest_image) < 9) return 0;
    const unsigned char *code = (const unsigned char *)rip;
    /* mov r64, fs:[0], absolute SIB addressing. Only the measured instruction
     * is accepted; other accesses remain faults rather than fabricated values. */
    if (code[0] != 0x64 || (code[1] != 0x48 && code[1] != 0x4c) || code[2] != 0x8b ||
        (code[3] & 0xc7) != 4 || code[4] != 0x25 || code[5] || code[6] || code[7] || code[8]) return 0;
    unsigned reg = ((code[3] >> 3) & 7) + ((code[1] & 4) ? 8 : 0);
    DWORD64 *registers[16] = {&context->Rax, &context->Rcx, &context->Rdx, &context->Rbx,
        &context->Rsp, &context->Rbp, &context->Rsi, &context->Rdi, &context->R8, &context->R9,
        &context->R10, &context->R11, &context->R12, &context->R13, &context->R14, &context->R15};
    *registers[reg] = (uintptr_t)kyoei_tls_tcb();
    context->Rip += 9;
    return 1;
}

uintptr_t kyoei_shared_resolve(const char *library, const char *symbol, int is_data);
/* The local session keeps user 1 logged in throughout its lifetime. Keep
 * registrations, but no user-change events are generated in this session. */
static SRWLOCK user_callback_lock = SRWLOCK_INIT;
static struct { uintptr_t callback, argument; } user_callbacks[16];
static uintptr_t np_callback, np_argument;
static ABI int32_t register_np_callback(uintptr_t callback, uintptr_t argument) {
    if (!callback) return (int32_t)0x80550003;
    AcquireSRWLockExclusive(&user_callback_lock);
    np_callback = callback; np_argument = argument;
    ReleaseSRWLockExclusive(&user_callback_lock); return 0;
}
static ABI int32_t unregister_np_callback(void) {
    AcquireSRWLockExclusive(&user_callback_lock);
    if (!np_callback) { ReleaseSRWLockExclusive(&user_callback_lock); return (int32_t)0x80550009; }
    np_callback = np_argument = 0;
    ReleaseSRWLockExclusive(&user_callback_lock); return 0;
}
static ABI int32_t register_user_callback(uintptr_t callback, uintptr_t argument, int32_t *id) {
    if (!callback || !id) return (int32_t)0x80960005;
    AcquireSRWLockExclusive(&user_callback_lock);
    for (int i = 0; i < 16; ++i) if (!user_callbacks[i].callback) {
        user_callbacks[i].callback = callback; user_callbacks[i].argument = argument;
        *id = i + 1; ReleaseSRWLockExclusive(&user_callback_lock); return 0;
    }
    ReleaseSRWLockExclusive(&user_callback_lock);
    return (int32_t)0x80960006;
}
static ABI int32_t unregister_user_callback(int32_t id) {
    if (id < 1 || id > 16) return (int32_t)0x80960005;
    AcquireSRWLockExclusive(&user_callback_lock);
    memset(&user_callbacks[id - 1], 0, sizeof(user_callbacks[0]));
    ReleaseSRWLockExclusive(&user_callback_lock); return 0;
}
static ABI void *main_tls_get_addr(const uint64_t *index) {
    if (!index) gz_stop(21, "STOP: null TLS index");
    if (index[0] == 1) return kyoei_tls_address(index[1]);
    typedef void *(ABI *Get)(const uint64_t *);
    Get original = (Get)kyoei_shared_resolve("libkernel", "__tls_get_addr", 0);
    return original(index);
}
uintptr_t gz_resolve(const char *library, const char *symbol, int is_data) {
#ifdef KYOEI_CRASH_CAPTURE
    if (!is_data && !strcmp(library, "libSceGnmDriver") && !strcmp(symbol, "sceGnmDebugHardwareStatus")) {
        extern ABI int32_t kyoei_render_debug_status(void);
        return (uintptr_t)kyoei_render_debug_status;
    }
#endif
    /* aerolib appends underscores to several VideoOut names; the native GPU
     * table uses the public API spelling. Resolve only existing functions. */
    if (!is_data && !strcmp(library, "libSceVideoOut")) {
        size_t length = strlen(symbol);
        if (length > 1 && length < 128 && symbol[length - 1] == '_') {
            char public_name[128]; memcpy(public_name, symbol, length - 1); public_name[length - 1] = 0;
            uintptr_t implementation = kyoei_shared_resolve(library, public_name, 0);
            if (implementation) return implementation;
        }
    }
    if (!is_data && !strcmp(library, "libSceScreenShot") && !strcmp(symbol, "sceScreenShotSetOverlayImageWithOrigin"))
        return kyoei_shared_resolve(library, "sceScreenShotSetOverlayImage", 0);
    if (!is_data && !strcmp(library, "libSceNpManagerForToolkit")) {
        if (!strcmp(symbol, "sceNpRegisterStateCallbackForToolkit")) return (uintptr_t)register_np_callback;
        if (!strcmp(symbol, "sceNpUnregisterStateCallbackForToolkit")) return (uintptr_t)unregister_np_callback;
        if (!strcmp(symbol, "sceNpCheckCallbackForToolkit") || !strcmp(symbol, "sceNpCheckCallbackForLib")) return kyoei_shared_resolve("libSceNpManager", "sceNpCheckCallback", 0);
    }
    if (!is_data && !strcmp(library, "libSceUserServiceForNpToolkit")) {
        if (!strcmp(symbol, "sceUserServiceRegisterEventCallback")) return (uintptr_t)register_user_callback;
        if (!strcmp(symbol, "sceUserServiceUnregisterEventCallback")) return (uintptr_t)unregister_user_callback;
    }
    if (!is_data && !strcmp(library, "libSceNetCtlForNpToolkit")) {
        static const char *toolkit[] = {"sceNetCtlRegisterCallbackForNpToolkit", "sceNetCtlCheckCallbackForNpToolkit", "sceNetCtlUnregisterCallbackForNpToolkit"};
        static const char *ordinary[] = {"sceNetCtlRegisterCallback", "sceNetCtlCheckCallback", "sceNetCtlUnregisterCallback"};
        for (unsigned i = 0; i < 3; ++i)
            if (!strcmp(symbol, toolkit[i])) return kyoei_shared_resolve("libSceNetCtl", ordinary[i], 0);
    }
    if (!is_data && !strcmp(library, "libSceVideoOut") && !strcmp(symbol, "sceVideoOutGetDeviceCapabilityInfo_"))
        return kyoei_shared_resolve(library, "sceVideoOutGetDeviceCapabilityInfo", 0);
    if (!is_data && !strcmp(library, "libkernel") && !strcmp(symbol, "__tls_get_addr"))
        return (uintptr_t)main_tls_get_addr;
    return kyoei_shared_resolve(library, symbol, is_data);
}
