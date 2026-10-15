#include "gz.h"
#include "kyoei_gpu_wait.h"
#include <windows.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>

void gz_log(const char *format, ...) {
    va_list args; va_start(args, format); vprintf(format, args); va_end(args); puts("");
}
void gz_stop(int code, const char *format, ...) { (void)format; ExitProcess(code); }

static volatile LONG64 label;
static DWORD WINAPI complete_later(void *unused) {
    (void)unused; Sleep(40); InterlockedExchange64(&label, 123); return 0;
}
static uintptr_t args_seen[3];
static ABI void clobber_arguments(void *context, uint64_t expected, const volatile uint64_t *pointer) {
    args_seen[0] = (uintptr_t)context; args_seen[1] = expected; args_seen[2] = (uintptr_t)pointer;
    __asm__ volatile("xor %%rdi, %%rdi; xor %%rsi, %%rsi; xor %%rdx, %%rdx" ::: "rdi", "rsi", "rdx");
}

int main(void) {
    HANDLE thread = CreateThread(NULL, 0, complete_later, NULL, 0, NULL);
    if (!thread || !kyoei_wait_real_label((const volatile uint64_t *)&label, 123, 1000)) return 1;
    WaitForSingleObject(thread, INFINITE); CloseHandle(thread);
    if (label != 123) return 2;
    ULONGLONG started = GetTickCount64();
    if (kyoei_wait_real_label((const volatile uint64_t *)&label, 456, 30)) return 3;
    if (label != 123 || GetTickCount64() - started > 1000) return 4;
    if (!kyoei_wait_real_label((const volatile uint64_t *)&label, 123, 0)) return 5;
    DWORD handles_before,handles_after;
    if (!GetProcessHandleCount(GetCurrentProcess(),&handles_before)) return 10;
    for (unsigned i=0;i<100;++i)
        if (kyoei_wait_real_label((const volatile uint64_t *)&label,456,2) || label!=123) return 11;
    if (!GetProcessHandleCount(GetCurrentProcess(),&handles_after) || handles_before!=handles_after) return 12;

    unsigned char *page = VirtualAlloc(NULL, 4096, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!page) return 6;
    const unsigned char original[] = {
        0x55, 0x48, 0x89, 0xe5, 0x41, 0x57,
        0x48, 0x89, 0xf0,                   // RAX = RSI
        0x48, 0x01, 0xf8,                   // RAX += RDI
        0x48, 0x01, 0xd0,                   // RAX += RDX
        0x41, 0x5f, 0x5d, 0xc3
    };
    memcpy(page, original, sizeof(original));
    unsigned char *relay = page + 128;
    if (kyoei_build_wait_relay(relay, clobber_arguments, page) != 29) return 7;
    page[0] = 0xe9;
    const int32_t distance = (int32_t)(relay - (page + 5));
    memcpy(page + 1, &distance, sizeof(distance)); page[5] = 0x90;
    DWORD prior;
    if (!VirtualProtect(page, 4096, PAGE_EXECUTE_READ, &prior)) return 8;
    FlushInstructionCache(GetCurrentProcess(), page, 4096);
    typedef uint64_t (ABI *GuestFunction)(void *, uint64_t, const volatile uint64_t *);
    uint64_t value = ((GuestFunction)page)((void *)11, 22, (const volatile uint64_t *)33);
    if (value != 66 || args_seen[0] != 11 || args_seen[1] != 22 || args_seen[2] != 33) return 9;
    VirtualFree(page, 0, MEM_RELEASE);
    puts("GPU WAIT PASS: delayed real fence, bounded timeout without label writes, SysV relay preserves arguments and original return");
    return 0;
}
