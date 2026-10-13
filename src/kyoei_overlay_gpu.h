// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "video_core/renderer_vulkan/vk_common.h"
union SDL_Event;
struct SDL_Window;
namespace Vulkan { class Instance; }
namespace KyoeiOverlay {
void Initialize(const Vulkan::Instance &instance,vk::Format format,unsigned image_count);
void Shutdown();
bool HandleEvent(const SDL_Event &event);
void UpdateTextInput(SDL_Window *window);
void Render(vk::CommandBuffer command,vk::ImageView view,vk::Extent2D extent);
}
