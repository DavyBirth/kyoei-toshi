/* Included in the local thread adapter where the real key table is visible.
 * Mirrors module-unload TSD cleanup: keep keys/values, remove only destructors
 * pointing into the finalized executable segments. No fork callbacks can be
 * registered by this runtime, so there is no atfork registry to purge.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include "kyoei_modules.h"
static ABI void kyoei_pthread_finalize(const void *info) {
    unsigned removed=0;
    AcquireSRWLockExclusive(&key_lock);
    for (unsigned i=1;i<KEYS;++i) {
        if (keys[i].used && keys[i].destructor &&
            kyoei_module_code_contains(info,(uintptr_t)keys[i].destructor)) {
            keys[i].destructor=NULL; ++removed;
        }
    }
    ReleaseSRWLockExclusive(&key_lock);
    gz_log("Kyoei pthread finalize: removed %u module-owned key destructors",removed);
}
