#pragma once

// A sidecar keeps the fixed backend's Scheduler ABI intact. Normal builds
// use the no-op entry points. Preview builds may opt out at runtime with
// KYOEI_VULKAN_RECORDING=0.
#include "video_core/renderer_vulkan/vk_common.h"

namespace KyoeiVulkanRecording {
#ifdef KYOEI_VULKAN_RECORDING_ENABLED
void Initialize();
void Attach(const void* owner, vk::CommandBuffer command);
void Drain(const void* owner);
// External renderers with private Vulkan function tables must enter directly
// only after prior commands in this buffer have finished recording.
void DrainCommand(vk::CommandBuffer command);
void Detach(const void* owner);
void Report();
#else
inline void Initialize() {}
inline void Attach(const void*, vk::CommandBuffer) {}
inline void Drain(const void*) {}
inline void DrainCommand(vk::CommandBuffer) {}
inline void Detach(const void*) {}
inline void Report() {}
#endif
}
