// Kyoei button prompts on the GoW3 texture cache (kyoei_live_prompts.h): matched icon
// textures upload the keyboard or controller artwork for the current input. Used by the
// adapted texture_cache.cpp (gpu-gow3/CMakeLists.txt), inside TextureCache members.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <cstring>
#include <utility>
#include "kyoei_live_prompts.h"

namespace KyoeiGow3Prompts {
// The artwork is laid out linearly in the guest's mip layout: it is uploaded without detiling.
template <class Stream, class Info>
std::pair<const VideoCore::Buffer*, u64> Upload(Stream& staging, const KyoeiPrompts::Pair& prompt,
                                                unsigned mode, const Info& info) {
    const auto [pixels, at] = staging.Map(info.guest_size, 16);
    ASSERT(pixels);
    std::memset(pixels, 0, info.guest_size);
    const auto& data = mode == KYOEI_INPUT_CONTROLLER ? prompt.Controller() : prompt.keyboard;
    const bool copied = KyoeiPrompts::Rows(prompt, info, [&](size_t to, size_t from, size_t bytes) {
        std::memcpy(pixels + to, data.data() + from, bytes);
        return true;
    });
    ASSERT(copied);
    staging.Commit();
    return {&staging, at};
}
} // namespace KyoeiGow3Prompts

// Start of RefreshImage, before its clean-image return: declares `prompt` and `prompt_mode`.
// Matched images carry KyoeiPromptImage, which keeps them off the clean-image shortcuts, so
// every binding reaches the selection (input changes and the pause-menu state are seen live).
#define KYOEI_PROMPT_SELECT(image)                                                              \
    bool prompt_changed = false;                                                                \
    unsigned prompt_mode = 0;                                                                   \
    const KyoeiPrompts::Pair* prompt =                                                          \
        !(image).usage.render_target && (image).info.guest_samples <= 1 &&                      \
                False((image).flags & ImageFlagBits::GpuModified) &&                            \
                False((image).flags & ImageFlagBits::Aliased)                                   \
            ? KyoeiPrompts::Select(image, True((image).flags & ImageFlagBits::CpuDirty),         \
                                   prompt_changed, prompt_mode)                                 \
            : nullptr;                                                                          \
    if (const auto kyoei_state = KyoeiPrompts::states.find((image).image_uid);                  \
        kyoei_state != KyoeiPrompts::states.end() && kyoei_state->second.pair >= 0) {           \
        (image).flags |= ImageFlagBits::KyoeiPromptImage;                                       \
    }                                                                                           \
    if (prompt_changed) {                                                                       \
        (image).flags |= ImageFlagBits::CpuDirty;                                               \
    }

// Upload source of RefreshImage: declares `in_buffer` and `in_offset` (the adapted line
// supplies the final semicolon).
#define KYOEI_PROMPT_SOURCE(image)                                                              \
    auto kyoei_source =                                                                         \
        buffer_cache.ObtainBufferForImage((image).info.guest_address, (image).info.guest_size); \
    if (prompt) {                                                                               \
        kyoei_source = KyoeiGow3Prompts::Upload(buffer_cache.GetStreamBuffer(), *prompt,        \
                                                prompt_mode, (image).info);                     \
    }                                                                                           \
    const auto [in_buffer, in_offset] = kyoei_source
