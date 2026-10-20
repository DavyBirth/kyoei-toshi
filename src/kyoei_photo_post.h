// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "video_core/renderer_vulkan/vk_common.h"
namespace Vulkan {class Instance;}
namespace KyoeiPhoto {
void ShutdownPost();
// Slot's presentation fence must have completed before its descriptor is updated.
void RenderPost(const Vulkan::Instance&,vk::CommandBuffer,void* slot,vk::ImageView source,
    unsigned source_width,unsigned source_height,vk::ImageView target,vk::Extent2D extent,vk::Format format);
}
