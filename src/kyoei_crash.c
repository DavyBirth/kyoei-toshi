/* Local crash snapshots contain guest memory: write only to ignored build,
 * never distribute or commit the resulting dump. */
#include "gz.h"
#include "kyoei_crash.h"
#include <dbghelp.h>
#include <stdio.h>
#include <string.h>

void kyoei_capture_failure(const char *reason, EXCEPTION_POINTERS *exception) {
    static LONG capturing;
    if (InterlockedCompareExchange(&capturing, 1, 0)) return;
    char path[MAX_PATH];
    DWORD length = GetModuleFileNameA(NULL, path, sizeof(path));
    if (!length || length >= sizeof(path)) goto done;
    char *name = strrchr(path, '\\');
    if (!name) goto done;
    SYSTEMTIME now;
    GetLocalTime(&now);
    snprintf(name + 1, sizeof(path) - (size_t)(name + 1 - path),
             "kyoei-%s-%04u%02u%02u-%02u%02u%02u-%lu.dmp", reason,
             now.wYear, now.wMonth, now.wDay, now.wHour, now.wMinute,
             now.wSecond, GetCurrentThreadId());
    HANDLE file = CreateFileA(path, GENERIC_WRITE, 0, NULL, CREATE_NEW,
                              FILE_ATTRIBUTE_NORMAL, NULL);
    if (file == INVALID_HANDLE_VALUE) {
        gz_log("Kyoei dump: cannot create file, error=%lu", GetLastError());
        goto done;
    }
    MINIDUMP_EXCEPTION_INFORMATION info = {GetCurrentThreadId(), exception, FALSE};
    BOOL saved = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), file,
                                  MiniDumpWithThreadInfo | MiniDumpWithIndirectlyReferencedMemory,
                                  exception ? &info : NULL, NULL, NULL);
    DWORD error = saved ? 0 : GetLastError();
    CloseHandle(file);
    gz_log("Kyoei dump: reason=%s saved=%d error=%lu path=%s", reason, saved, error, path);
done:
    InterlockedExchange(&capturing, 0);
}

uintptr_t kyoei_shared_resolve(const char *library, const char *symbol, int is_data);
ABI int32_t kyoei_render_debug_status(void) {
    // The guest calls this after its flush polling limit, before int 0x41.
    kyoei_capture_failure("render-stall", NULL);
    typedef int32_t (ABI *Status)(void);
    Status original = (Status)kyoei_shared_resolve("libSceGnmDriver", "sceGnmDebugHardwareStatus", 0);
    if (!original) gz_stop(21, "STOP: missing original sceGnmDebugHardwareStatus");
    return original();
}
