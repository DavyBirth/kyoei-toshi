// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "kyoei_photo.h"
#include "kyoei_photo_settings.h"
#include "kyoei_photo_post.h"
#include "kyoei_photo_notice.h"
#include "video_core/buffer_cache/buffer.h"
#include "video_core/texture_cache/image.h"
#include "kyoei_photo_png.h"
#include <chrono>
namespace KyoeiPhoto {
inline void Capture(Vulkan::Instance& instance,Vulkan::Scheduler& scheduler,VideoCore::Image& image,vk::Format format) {
    if (!TakeRequest()) return;
    const bool bgra=format==vk::Format::eB8G8R8A8Srgb || format==vk::Format::eB8G8R8A8Unorm;
    if (!bgra && format!=vk::Format::eR8G8B8A8Srgb && format!=vk::Format::eR8G8B8A8Unorm) {
        ReportCapture(false);LOG_ERROR(Render_Vulkan,"Photo: unsupported format {}; no image saved",vk::to_string(format));return;
    }
    try {
        const u32 width=image.info.size.width,height=image.info.size.height;
        if (!width || !height || width>16384 || height>16384){ReportCapture(false);return;}
        const u64 size=u64(width)*height*4;
        VideoCore::Buffer readback{instance,scheduler,VideoCore::MemoryUsage::Download,0,vk::BufferUsageFlagBits::eTransferDst,size};
        auto cmd=scheduler.CommandBuffer();
        image.Transit(vk::ImageLayout::eTransferSrcOptimal,vk::AccessFlagBits2::eTransferRead,{},cmd);
        const vk::BufferImageCopy copy{.imageSubresource={vk::ImageAspectFlagBits::eColor,0,0,1},.imageExtent={width,height,1}};
        cmd.copyImageToBuffer(image.GetImage(),vk::ImageLayout::eTransferSrcOptimal,readback.Handle(),copy);
        scheduler.Finish();
        vmaInvalidateAllocation(instance.GetAllocator(),readback.buffer.allocation,0,size);
        std::vector<u8> pixels(readback.mapped_data.begin(),readback.mapped_data.begin()+size);
        if(kyoei_photo_hud_hidden())GradePixels(pixels.data(),pixels.size(),bgra,GetGrade(),width);
        // Private prototype destination. No dump/save changes, no desktop capture.
        const auto directory=std::filesystem::path("photos");std::filesystem::create_directories(directory);
        static u64 sequence{};
        const auto stamp=std::chrono::system_clock::now().time_since_epoch().count();
        const auto path=directory/("Kyoei-"+std::to_string(stamp)+"-"+std::to_string(++sequence)+".png");
        if (SavePng(path,pixels,width,height,bgra)) {
            ReportCapture(true,path.filename().string(),width,height);
            LOG_INFO(Render_Vulkan,"Photo saved: {} (native {}x{}, before host overlays/scaling)",path.string(),width,height);
        } else {ReportCapture(false);LOG_ERROR(Render_Vulkan,"Photo: failed saving {}",path.string());}
    } catch (const std::exception& error) {ReportCapture(false);LOG_ERROR(Render_Vulkan,"Photo: {}",error.what());}
}
}
