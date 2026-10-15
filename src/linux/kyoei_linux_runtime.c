/* First POSIX runtime modules; keep unsupported imports unresolved.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include <string.h>

uintptr_t gz_lookup(const GzExport *table, size_t count,
                    const char *library, const char *symbol) {
    for (size_t i = 0; i < count; ++i)
        if ((!table[i].library || !strcmp(table[i].library, library)) &&
            !strcmp(table[i].symbol, symbol)) return (uintptr_t)table[i].function;
    return 0;
}

uintptr_t gz_resolve(const char *library, const char *symbol, int is_data) {
    if (is_data) return 0;
    uintptr_t address = gz_rtc_resolve(library, symbol);
    return address ? address : gz_sema_resolve(library, symbol);
}
