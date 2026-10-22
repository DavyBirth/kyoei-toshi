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
    if (!enabled) return 0;
    // KYOEI_UPLOAD_WATCH=<hex address>: every GPU write that touches the 16 MiB from there.
    static const u64 watch = [] { const char* v = std::getenv("KYOEI_UPLOAD_WATCH"); return v ? std::strtoull(v, nullptr, 16) : 0ull; }();
    if (watch && address < watch + (16u << 20) && watch < address + size) {
        std::printf("Kyoei watched write: shader %#llx at %#llx, %llu bytes\n", (unsigned long long)shader,
                    (unsigned long long)address, (unsigned long long)size);
    }
    if (size < (1u << 20)) return 0;
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

/// Who removes write tracking from large images (>= 4 MB): caller address, symbolized every 5 s.
int NoteUntrackSymbolize(void* caller, char* out, size_t size);
inline int NoteUntrack(const VideoCore::Image& image, void* caller, int kind) {
    static const bool enabled = [] { const char* v = std::getenv("KYOEI_UPLOAD_REPORT"); return v && v[0] == '1'; }();
    if (!enabled || image.info.guest_size < (4u << 20)) return 0;
    static std::mutex mutex;
    static std::unordered_map<void*, std::pair<int, u32>> callers;
    static auto window = std::chrono::steady_clock::now();
    std::scoped_lock lock{mutex};
    auto& entry = callers[caller];
    entry.first = kind;
    ++entry.second;
    if (std::chrono::steady_clock::now() - window < std::chrono::seconds(5)) return 0;
    std::printf("Kyoei untrack of large images (5 s):\n");
    for (const auto& [address, stat] : callers) {
        char name[256];
        NoteUntrackSymbolize(address, name, sizeof(name));
        std::printf("  %s from %s: %u\n", stat.first == 0 ? "whole" : stat.first == 1 ? "head" : "tail", name, stat.second);
    }
    callers.clear();
    window = std::chrono::steady_clock::now();
    return 0;
}
}

namespace KyoeiUploadReport {
inline bool CopyEnabled() {
    static const bool enabled = [] { const char* v = std::getenv("KYOEI_UPLOAD_REPORT"); return v && v[0] == '1'; }();
    return enabled;
}
/// Rasterizer::CopyBuffer (CP DMA) of 256 KiB or more: where from, where to, and whether the
/// source was GPU-written or an image (otherwise the copy runs as a host memcpy).
inline bool NoteCopy(u64 dst, u64 src, u32 bytes, bool gds, bool src_gpu, bool src_image) {
    if (bytes < (256u << 10)) return false;
    std::printf("Kyoei copy: %#llx -> %#llx, %u bytes%s%s%s\n", (unsigned long long)src,
                (unsigned long long)dst, bytes, gds ? ", GDS" : "", src_gpu ? ", source GPU-written" : "",
                src_image ? ", source image" : "");
    return false;
}
} // namespace KyoeiUploadReport

// KYOEI_UPLOAD_REPORT=1, in TextureCache::RefreshImage: an image of 4 MiB or more uploaded from
// guest memory although a GPU-written image overlaps it (which one, and how they differ).
#define KYOEI_UPLOAD_OVERLAPS(image)                                                              \
    if (KyoeiUploadReport::CopyEnabled() && (image).info.guest_size >= (4u << 20) &&            \
        True((image).flags & ImageFlagBits::CpuDirty)) {                                         \
        ForEachImageInRegion((image).info.guest_address, (image).info.guest_size,                \
                             [&](ImageId other_id, Image& other) {                               \
            if (&other == &(image) || False(other.flags & ImageFlagBits::GpuModified)) return;   \
            std::printf("Kyoei overlap: upload %#llx %ux%u fmt %u tiled %u/%u/%u size %u | "      \
                        "GPU image %#llx %ux%u fmt %u tiled %u/%u/%u size %u\n",                 \
                        (unsigned long long)(image).info.guest_address,                          \
                        (image).info.size.width, (image).info.size.height,                       \
                        unsigned((image).info.pixel_format), unsigned((image).info.props.is_tiled),\
                        unsigned((image).info.tile_mode), unsigned((image).info.array_mode),     \
                        (image).info.guest_size, (unsigned long long)other.info.guest_address,   \
                        other.info.size.width, other.info.size.height,                           \
                        unsigned(other.info.pixel_format), unsigned(other.info.props.is_tiled),  \
                        unsigned(other.info.tile_mode), unsigned(other.info.array_mode),         \
                        other.info.guest_size);                                                  \
        });                                                                                      \
    }

namespace KyoeiUploadReport {
/// Rasterizer::ReadMemory (guest CPU read of GPU-tracked memory): 1 MiB pages read, every 5 s.
inline bool NoteRead(u64 address, u64 size) {
    if (!CopyEnabled()) return false;
    static std::mutex mutex;
    static std::unordered_map<u64, u64> pages;
    static auto window = std::chrono::steady_clock::now();
    std::scoped_lock lock{mutex};
    pages[address >> 20] += size;
    if (std::chrono::steady_clock::now() - window < std::chrono::seconds(5)) return false;
    std::vector<std::pair<u64, u64>> top(pages.begin(), pages.end());
    std::sort(top.begin(), top.end(), [](auto& a, auto& b) { return a.second > b.second; });
    std::printf("Kyoei CPU reads of GPU memory (5 s): %zu MiB pages\n", top.size());
    for (size_t i = 0; i < std::min<size_t>(top.size(), 8); ++i) {
        std::printf("  %#llx: %llu bytes\n", (unsigned long long)(top[i].first << 20), (unsigned long long)top[i].second);
    }
    pages.clear();
    window = std::chrono::steady_clock::now();
    return false;
}
} // namespace KyoeiUploadReport

namespace KyoeiUploadReport {
/// TextureCache::FindRenderTarget: each 4 MiB+ render target the first time (address, size,
/// format, tiling, and whether another image shares the address).
inline int NoteTarget(const VideoCore::Image& image) {
    if (!CopyEnabled() || image.info.guest_size < (4u << 20)) return 0;
    static std::mutex mutex;
    static std::unordered_map<u64, bool> seen;
    std::scoped_lock lock{mutex};
    const u64 key = image.info.guest_address ^ (u64(image.info.pixel_format) << 56) ^ (u64(image.info.props.is_tiled) << 55);
    if (!seen.try_emplace(key, true).second) return 0;
    std::printf("Kyoei target: %#llx %ux%u fmt %u tiled %u/%u/%u size %u flags %#x\n",
                (unsigned long long)image.info.guest_address, image.info.size.width, image.info.size.height,
                unsigned(image.info.pixel_format), unsigned(image.info.props.is_tiled), unsigned(image.info.tile_mode),
                unsigned(image.info.array_mode), image.info.guest_size, unsigned(image.flags));
    return 0;
}
} // namespace KyoeiUploadReport
