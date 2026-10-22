// Kyoei experiments on the GoW3 core, off unless their variable is set.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <cstdlib>

namespace KyoeiGow3Experiments {
// KYOEI_SYNC_READS=1: every read-only buffer binding that starts on an image copies the image
// into the buffer first (the core does it only for texel buffers).
inline bool SyncAllReads() {
    static const bool enabled = [] { const char* v = std::getenv("KYOEI_SYNC_READS"); return v && v[0] == '1'; }();
    return enabled;
}
} // namespace KyoeiGow3Experiments
