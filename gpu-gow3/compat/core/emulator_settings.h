// Kyoei: the GoW3 core's settings surface, read from the GZ_* environment variables the
// Kyoei launcher and scripts already set (GoW3 reads GOW3_*), plus the audio settings
// Godzilla's AudioOut uses. Defaults follow the GoW3 core.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <algorithm>
#include <cstdlib>
#include <string>
#include "common/types.h"

u32 Gow3DisplayRefreshHz(); // primary display refresh rate, 60 when unknown

enum GpuReadbacksMode : int { Disabled, Relaxed, Precise };

class EmulatorSettingsImpl {
public:
    static EmulatorSettingsImpl* GetInstance() { static EmulatorSettingsImpl s; return &s; }
    static bool Flag(const char* name, bool fallback) {
        const char* v = std::getenv(name);
        return v ? (v[0] == '1' || v[0] == 'y' || v[0] == 't') : fallback;
    }
    static long Number(const char* name, long fallback) {
        const char* v = std::getenv(name);
        return v ? std::strtol(v, nullptr, 10) : fallback;
    }
    int GetVolumeSlider() { return 100; }
    std::string GetSDLMainOutputDevice() { return {}; }
    std::string GetSDLPadSpkOutputDevice() { return {}; }
    s32 GetGpuId() { static const auto value = s32(Number("GZ_GPU_ID", -1)); return value; }
    u32 GetInternalScreenWidth() { static const auto value = u32(Number("GZ_INTERNAL_WIDTH", 1920)); return value; }
    u32 GetInternalScreenHeight() { static const auto value = u32(Number("GZ_INTERNAL_HEIGHT", 1080)); return value; }
    std::string GetPresentMode() { const char* v = std::getenv("GZ_PRESENT_MODE"); return v ? v : "Mailbox"; }
    u32 GetReadbacksMode() { static const auto value = u32(Number("GZ_READBACKS", GpuReadbacksMode::Relaxed)); return value; }
    // PS4 timing by default: 60 Hz vblank. GZ_VBLANK_HZ=0 runs vblank at 480 Hz and limits
    // presents to the display rate (at most 120), as in the GoW3 core.
    u32 GetVblankFrequency() {
        static const u32 value = [] {
            const long hz = Number("GZ_VBLANK_HZ", 60);
            return hz > 0 ? u32(hz) : 480u;
        }();
        return value;
    }
    u32 GetFrameLimit() {
        static const u32 value = [] {
            const long limit = Number("GZ_FPS_LIMIT", -1);
            if (limit >= 0) return u32(limit);
            return Number("GZ_VBLANK_HZ", 60) > 0 ? 0u : std::min<u32>(Gow3DisplayRefreshHz(), 120);
        }();
        return value;
    }
    bool IsCopyGpuBuffers() { static const auto value = Flag("GZ_COPY_GPU_BUFFERS", false); return value; }
    bool IsDirectMemoryAccessEnabled() { static const auto value = Flag("GZ_DIRECT_MEMORY_ACCESS", false); return value; }
    bool IsDumpShaders() { static const auto value = Flag("GZ_DUMP_SHADERS", false); return value; }
    bool IsHdrAllowed() { static const auto value = Flag("GZ_HDR", false); return value; }
    bool IsNullGPU() { static const auto value = Flag("GZ_NULL_GPU", false); return value; }
    bool IsPatchShaders() { return false; }
    bool IsPipelineCacheArchived() { return false; }
    bool IsPipelineCacheEnabled() { static const auto value = Flag("GZ_PIPELINE_CACHE", true); return value; }
    bool IsReadbackLinearImagesEnabled() { static const auto value = Flag("GZ_READBACK_LINEAR", false); return value; }
    bool IsRenderdocEnabled() { return false; }
    bool IsShaderCollect() { return false; }
    bool IsUserfaultfdTracking() { return false; }
    bool IsVkCrashDiagnosticEnabled() { return false; }
    bool IsVkGuestMarkersEnabled() { static const auto value = Flag("GZ_VK_MARKERS", false); return value; }
    bool IsVkHostMarkersEnabled() { static const auto value = Flag("GZ_VK_MARKERS", false); return value; }
    bool IsVkValidationCoreEnabled() { return true; }
    bool IsVkValidationEnabled() { static const auto value = Flag("GZ_VK_VALIDATION", false); return value; }
    bool IsVkValidationGpuEnabled() { return false; }
    bool IsVkValidationSyncEnabled() { static const auto value = Flag("GZ_VK_VALIDATION_SYNC", false); return value; }
};
#define EmulatorSettings (*EmulatorSettingsImpl::GetInstance())
