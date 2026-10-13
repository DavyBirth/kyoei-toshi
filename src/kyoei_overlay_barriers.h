// Modal composition preserves the game frame and returns the image to WSI.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "video_core/renderer_vulkan/vk_common.h"
namespace KyoeiOverlay {
inline vk::ImageMemoryBarrier BeforeDraw(vk::Image image,vk::ImageSubresourceRange range,bool visible) {
    return {
        .srcAccessMask=vk::AccessFlagBits::eTransferWrite,
        .dstAccessMask=visible?vk::AccessFlagBits::eColorAttachmentRead|vk::AccessFlagBits::eColorAttachmentWrite:vk::AccessFlagBits::eNone,
        .oldLayout=vk::ImageLayout::eTransferDstOptimal,
        .newLayout=visible?vk::ImageLayout::eColorAttachmentOptimal:vk::ImageLayout::ePresentSrcKHR,
        .srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,.dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,
        .image=image,.subresourceRange=range};
}
inline vk::ImageMemoryBarrier AfterDraw(vk::Image image,vk::ImageSubresourceRange range) {
    return {
        .srcAccessMask=vk::AccessFlagBits::eColorAttachmentWrite,.dstAccessMask=vk::AccessFlagBits::eNone,
        .oldLayout=vk::ImageLayout::eColorAttachmentOptimal,.newLayout=vk::ImageLayout::ePresentSrcKHR,
        .srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,.dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,
        .image=image,.subresourceRange=range};
}
inline vk::RenderingAttachmentInfo Attachment(vk::ImageView view) {
    return {.imageView=view,.imageLayout=vk::ImageLayout::eColorAttachmentOptimal,
        .loadOp=vk::AttachmentLoadOp::eLoad,.storeOp=vk::AttachmentStoreOp::eStore};
}
}
