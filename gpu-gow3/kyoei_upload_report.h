// Kyoei diagnostics (KYOEI_UPLOAD_REPORT=1): every 5 s, the images TextureCache::RefreshImage
// uploaded most, by bytes. Off by default; one flag check per upload otherwise.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <algorithm>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <mutex>
#include <unordered_map>
#include <vector>
#include "video_core/texture_cache/image.h"

namespace KyoeiUploadReport {
inline int Note(const VideoCore::Image& image) {
    static const bool enabled = [] { const char* v = std::getenv("KYOEI_UPLOAD_REPORT"); return v && v[0] == '1'; }();
    if (!enabled) return 0;
    struct Stat { u32 count; u64 bytes; u32 width, height, format; bool cpu_dirty; u32 flags; };
    static std::mutex mutex;
    static std::unordered_map<VAddr, Stat> stats;
    static auto window = std::chrono::steady_clock::now();
    std::scoped_lock lock{mutex};
    auto& s = stats[image.info.guest_address];
    ++s.count;
    s.bytes += image.info.guest_size;
    s.width = image.info.size.width;
    s.height = image.info.size.height;
    s.format = u32(image.info.pixel_format);
    s.cpu_dirty = True(image.flags & VideoCore::ImageFlagBits::CpuDirty);
    s.flags |= u32(image.flags);
    if (std::chrono::steady_clock::now() - window < std::chrono::seconds(5)) return 0;
    std::vector<std::pair<VAddr, Stat>> top(stats.begin(), stats.end());
    std::sort(top.begin(), top.end(), [](auto& a, auto& b) { return a.second.bytes > b.second.bytes; });
    u64 total = 0;
    for (auto& e : top) total += e.second.bytes;
    std::printf("Kyoei uploads (5 s): %.1f MB in %zu images\n", total / 1048576.0, top.size());
    for (size_t i = 0; i < std::min<size_t>(top.size(), 10); ++i) {
        const auto& [address, stat] = top[i];
        std::printf("  %#llx %ux%u fmt %u flags %#x: %u uploads, %.1f MB%s\n", (unsigned long long)address,
                    stat.width, stat.height, stat.format, stat.flags, stat.count, stat.bytes / 1048576.0,
                    stat.cpu_dirty ? ", CPU-written" : "");
    }
    stats.clear();
    window = std::chrono::steady_clock::now();
    return 0;
}

/// GPU writes through buffers that land on an image's base address (they mark it GpuDirty):
/// shader hash (0 = CopyBuffer/DMA), count and bytes, every 5 s.
inline int NoteWrite(u64 shader, u64 address, u64 size) {
    static const bool enabled = [] { const char* v = std::getenv("KYOEI_UPLOAD_REPORT"); return v && v[0] == '1'; }();
    if (!enabled || size < (1u << 20)) return 0;
    struct Stat { u32 count; u64 bytes; u64 address; };
    static std::mutex mutex;
    static std::unordered_map<u64, Stat> stats;
    static auto window = std::chrono::steady_clock::now();
    std::scoped_lock lock{mutex};
    auto& s = stats[shader];
    ++s.count;
    s.bytes += size;
    s.address = address;
    if (std::chrono::steady_clock::now() - window < std::chrono::seconds(5)) return 0;
    std::printf("Kyoei GPU buffer writes >= 1 MB (5 s):\n");
    for (const auto& [hash, stat] : stats)
        std::printf("  shader %#llx: %u writes, %.1f MB, last %#llx\n", (unsigned long long)hash, stat.count,
                    stat.bytes / 1048576.0, (unsigned long long)stat.address);
    stats.clear();
    window = std::chrono::steady_clock::now();
    return 0;
}
}
