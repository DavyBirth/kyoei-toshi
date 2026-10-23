// Kyoei photo mode on the GoW3 presenter: a requested photo copies the guest display buffer
// (native resolution, before post-processing, the Kyoei overlay and window scaling) and is saved once the GPU is done, graded like the
// old core (kyoei_photo_gpu.h). Used by the adapted vk_presenter.cpp (gpu-gow3/CMakeLists.txt).
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <chrono>
#include <string>
#include <vector>
#include <vk_mem_alloc.h>
#include "kyoei_photo.h"
#include "kyoei_photo_notice.h"
#include "kyoei_photo_png.h"
#include "kyoei_photo_settings.h"

#ifdef MemoryBarrier
#undef MemoryBarrier // <windows.h>: a fence macro, not vk::MemoryBarrier
#endif

namespace KyoeiGow3Photo {
/// In PrepareFrame, on the guest display image: moves it to TransferSrcOptimal (the presenter
/// transitions it for sampling right after). `view_format` is the display view's format (the
/// channel order the screen shows); `to_rgba` is the presenter's screenshot
/// conversion to 8-bit RGBA.
template <class Runtime, class Image, class ToRgba>
void Capture(const Vulkan::Instance& instance, Vulkan::Scheduler& scheduler, vk::CommandBuffer cmdbuf,
             Runtime& runtime, Image& image, vk::Format view_format, ToRgba to_rgba) {
    if (!KyoeiPhoto::TakeRequest()) return;
    struct {
        vk::Image image;
        u32 width, height;
        vk::Format format;
        bool is_hdr;
    } const frame{image.GetImage(), image.info.size.width, image.info.size.height, view_format, false};
    if (!frame.width || !frame.height || frame.width > 16384 || frame.height > 16384) {
        KyoeiPhoto::ReportCapture(false);
        return;
    }
    runtime.Transit(&image, vk::ImageLayout::eTransferSrcOptimal, vk::PipelineStageFlagBits2::eTransfer,
                    vk::AccessFlagBits2::eTransferRead);
    runtime.FlushBarriers();
    const VkBufferCreateInfo buffer_ci{.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
                                       .size = VkDeviceSize(frame.width) * frame.height * 4,
                                       .usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT};
    const VmaAllocationCreateInfo alloc_ci{
        .flags = VMA_ALLOCATION_CREATE_MAPPED_BIT | VMA_ALLOCATION_CREATE_HOST_ACCESS_RANDOM_BIT,
        .usage = VMA_MEMORY_USAGE_AUTO_PREFER_HOST};
    VkBuffer buffer{};
    VmaAllocation allocation{};
    VmaAllocationInfo info{};
    if (vmaCreateBuffer(instance.GetAllocator(), &buffer_ci, &alloc_ci, &buffer, &allocation, &info) !=
        VK_SUCCESS) {
        KyoeiPhoto::ReportCapture(false);
        return;
    }
    cmdbuf.copyImageToBuffer(frame.image, vk::ImageLayout::eTransferSrcOptimal, buffer,
                             vk::BufferImageCopy{.imageSubresource = {vk::ImageAspectFlagBits::eColor, 0, 0, 1},
                                                 .imageExtent = {frame.width, frame.height, 1}});
    const vk::MemoryBarrier to_host{.srcAccessMask = vk::AccessFlagBits::eTransferWrite,
                                    .dstAccessMask = vk::AccessFlagBits::eHostRead};
    cmdbuf.pipelineBarrier(vk::PipelineStageFlagBits::eTransfer, vk::PipelineStageFlagBits::eHost,
                           vk::DependencyFlags{}, to_host, {}, {});
    // The grade on screen when the photo was taken (photo HUD open).
    const bool graded = kyoei_photo_hud_hidden() != 0;
    scheduler.DeferPriorityOperation([allocator = instance.GetAllocator(), buffer, allocation, info,
                                      width = frame.width, height = frame.height, format = frame.format,
                                      hdr = frame.is_hdr, graded, grade = KyoeiPhoto::GetGrade(), to_rgba] {
        vmaInvalidateAllocation(allocator, allocation, 0, VK_WHOLE_SIZE);
        std::vector<uint8_t> rgba;
        const bool converted = to_rgba(static_cast<const uint8_t*>(info.pMappedData), width, height, format, hdr, rgba);
        vmaDestroyBuffer(allocator, buffer, allocation);
        if (!converted) {
            KyoeiPhoto::ReportCapture(false);
            std::printf("Photo: unsupported frame format %s\n", vk::to_string(format).c_str());
            return;
        }
        if (graded) KyoeiPhoto::GradePixels(rgba.data(), rgba.size(), false, grade, width);
        // Same destination and naming as the old core.
        static uint64_t sequence{};
        std::error_code error;
        const auto directory = std::filesystem::path("photos");
        std::filesystem::create_directories(directory, error);
        const auto stamp = std::chrono::system_clock::now().time_since_epoch().count();
        const auto path = directory / ("Kyoei-" + std::to_string(stamp) + "-" + std::to_string(++sequence) + ".png");
        if (KyoeiPhoto::SavePng(path, rgba, width, height, false)) {
            KyoeiPhoto::ReportCapture(true, path.filename().string(), width, height);
            std::printf("Photo saved: %s (%ux%u, before host overlays/scaling)\n", path.string().c_str(), width, height);
        } else {
            KyoeiPhoto::ReportCapture(false);
            std::printf("Photo: failed saving %s\n", path.string().c_str());
        }
    });
}
} // namespace KyoeiGow3Photo
