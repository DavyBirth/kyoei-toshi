/* Synthetic Windows integration test: actual FS load, two live threads.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_tls.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

void gz_log(const char *fmt, ...) { (void)fmt; }
__attribute__((noreturn)) void gz_stop(int code, const char *fmt, ...) {
    va_list args; va_start(args, fmt); vfprintf(stderr, fmt, args); va_end(args); exit(code);
}
uintptr_t gz_thread_resolve(const char *lib, const char *sym) { (void)lib; (void)sym; return 0; }
uintptr_t kyoei_shared_resolve(const char *lib, const char *sym, int data) {
    (void)lib; (void)sym; (void)data; return 0;
}
static LONG CALLBACK handler(EXCEPTION_POINTERS *e) {
    return kyoei_tls_fault(e) ? EXCEPTION_CONTINUE_EXECUTION : EXCEPTION_CONTINUE_SEARCH;
}
static void *(*fs_load)(void);
static HANDLE ready[2], done;
static unsigned char *blocks[2];
static int results[2];
static DWORD WINAPI worker(void *arg) {
    size_t id = (size_t)arg;
    void *tcb = fs_load();
    blocks[id] = kyoei_tls_address(0);
    int valid = tcb == kyoei_tls_tcb() && *(void **)tcb == tcb &&
        (unsigned char *)tcb - blocks[id] == 40 && !((uintptr_t)blocks[id] & 7);
    for (int i = 0; i < 40; ++i) valid &= blocks[id][i] == (i < 8 ? 0xa5 : 0);
    blocks[id][8] = (unsigned char)(id + 1);
    SetEvent(ready[id]);
    WaitForSingleObject(done, 10000);
    valid &= blocks[id][8] == id + 1;
    results[id] = valid;
    return 0;
}
int main(void) {
    unsigned char *image = VirtualAlloc(NULL, 4096, MEM_RESERVE|MEM_COMMIT, PAGE_READWRITE);
    const unsigned char code[] = {0x64,0x48,0x8b,0x04,0x25,0,0,0,0,0xc3};
    if (!image) return 1;
    memcpy(image, code, sizeof(code)); memset(image+64, 0xa5, 8);
    DWORD old; if (!VirtualProtect(image, 4096, PAGE_EXECUTE_READ, &old)) return 2;
    FlushInstructionCache(GetCurrentProcess(), image, sizeof(code));
    uint64_t layout[] = {64,8,40,8};
    kyoei_tls_configure(image, 4096, layout);
    fs_load = (void *(*)(void))image;
    void *veh = AddVectoredExceptionHandler(1, handler);
    done = CreateEventA(NULL, TRUE, FALSE, NULL);
    HANDLE threads[2];
    for (size_t i = 0; i < 2; ++i) {
        ready[i] = CreateEventA(NULL, TRUE, FALSE, NULL);
        threads[i] = CreateThread(NULL, 0, worker, (void *)i, 0, NULL);
        if (!threads[i]) return 3;
    }
    if (WaitForMultipleObjects(2, ready, TRUE, 10000) != WAIT_OBJECT_0) return 4;
    int distinct = blocks[0] != blocks[1];
    SetEvent(done);
    if (WaitForMultipleObjects(2, threads, TRUE, 10000) != WAIT_OBJECT_0) return 5;
    RemoveVectoredExceptionHandler(veh);
    if (!distinct || !results[0] || !results[1]) return 6;
    puts("TLS PASS: FS load, template, zero fill, alignment, two-thread isolation and cleanup");
    return 0;
}
