// Kyoei experiments on the GoW3 core, off unless their variable is set.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <cstdlib>
#include "video_core/texture_cache/image.h"

namespace KyoeiGow3Experiments {
// KYOEI_SYNC_READS=1: every read-only buffer binding that starts on an image copies the image
// into the buffer first (the core does it only for texel buffers).
inline bool SyncAllReads() {
    static const bool enabled = [] { const char* v = std::getenv("KYOEI_SYNC_READS"); return v && v[0] == '1'; }();
    return enabled;
}
} // namespace KyoeiGow3Experiments

// Rasterizer::Resolve: the resolved MRT1 holds the GPU's result, yet keeps the flags of a fresh
// image (CPU-dirty), so its first use as a texture may re-upload guest memory over the
// resolve. Opt-in (KYOEI_RESOLVE_WRITTEN=1): did not fix the pause background (23/10).
namespace KyoeiGow3Experiments {
template <class Image>
void ResolveWritten(Image& image) {
    static const bool enabled = std::getenv("KYOEI_RESOLVE_WRITTEN") != nullptr;
    if (!enabled) return;
    image.flags &= ~VideoCore::ImageFlagBits::Dirty;
    image.flags |= VideoCore::ImageFlagBits::GpuModified;
}
} // namespace KyoeiGow3Experiments
