// Kyoei diagnostics: symbol names for kyoei_upload_report.h (dbghelp, diagnostics only).
// SPDX-License-Identifier: GPL-2.0-or-later
#include <cstdio>
#include <mutex>
#include <windows.h>
#include <dbghelp.h>

namespace KyoeiUploadReport {
int NoteUntrackSymbolize(void* caller, char* out, size_t size) {
    static std::mutex mutex;
    static const bool ready = [] {
        SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
        return SymInitialize(GetCurrentProcess(), nullptr, TRUE) != FALSE;
    }();
    std::scoped_lock lock{mutex};
    alignas(SYMBOL_INFO) unsigned char storage[sizeof(SYMBOL_INFO) + 256]{};
    auto* symbol = reinterpret_cast<SYMBOL_INFO*>(storage);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = 255;
    DWORD64 displacement = 0;
    if (ready && SymFromAddr(GetCurrentProcess(), DWORD64(caller), &displacement, symbol))
        std::snprintf(out, size, "%s+0x%llx", symbol->Name, (unsigned long long)displacement);
    else
        std::snprintf(out, size, "%p", caller);
    return 0;
}
}
