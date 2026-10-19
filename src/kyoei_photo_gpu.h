// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "kyoei_photo.h"
#include "video_core/buffer_cache/buffer.h"
#include "video_core/texture_cache/image.h"
#include <filesystem>
#include <fstream>
#include <chrono>
extern "C" {
#include "kyoei_png.h"
}
namespace KyoeiPhoto {
inline bool SavePng(const std::filesystem::path& path,const std::vector<u8>& pixels,u32 width,u32 height,bool bgra) {
    using Create=int32_t (__attribute__((sysv_abi)) *)(const KyoeiPngCreateParam*,void*,uint32_t,void**);
    using Encode=int32_t (__attribute__((sysv_abi)) *)(void*,const KyoeiPngEncodeParam*,KyoeiPngOutputInfo*);
    using Delete=int32_t (__attribute__((sysv_abi)) *)(void*);
    auto create=reinterpret_cast<Create>(kyoei_png_resolve("scePngEncCreate"));
    auto encode=reinterpret_cast<Encode>(kyoei_png_resolve("scePngEncEncode"));
    auto destroy=reinterpret_cast<Delete>(kyoei_png_resolve("scePngEncDelete"));
    if (!width || !height || width>16384 || height>16384 || pixels.size()!=uint64_t(width)*height*4) return false;
    alignas(16) unsigned char memory[64]{};
    KyoeiPngCreateParam options{sizeof(options),0,width,0};void* handle{};
    // Deflate worst-case plus PNG/chunk overhead; bounded above by the size limit.
    std::vector<u8> output(pixels.size()+pixels.size()/100+height+4096);
    if (create(&options,memory,sizeof(memory),&handle)<0) return false;
    KyoeiPngEncodeParam input{};
    input.image_mem_addr=pixels.data();input.png_mem_addr=output.data();
    input.image_mem_size=static_cast<u32>(pixels.size());input.png_mem_size=static_cast<u32>(output.size());
    input.image_width=width;input.image_height=height;input.image_pitch=width*4;
    input.pixel_format=bgra?1:0;input.color_space=3;input.bit_depth=8;input.compression_level=3;
    KyoeiPngOutputInfo result{};const int32_t status=encode(handle,&input,&result);destroy(handle);
    if (status<0) return false;
    std::ofstream file(path,std::ios::binary);file.write(reinterpret_cast<const char*>(output.data()),result.data_size);
    file.close();return !file.fail();
}
inline void Capture(Vulkan::Instance& instance,Vulkan::Scheduler& scheduler,VideoCore::Image& image,vk::Format format) {
    if (!TakeRequest()) return;
    const bool bgra=format==vk::Format::eB8G8R8A8Srgb || format==vk::Format::eB8G8R8A8Unorm;
    if (!bgra && format!=vk::Format::eR8G8B8A8Srgb && format!=vk::Format::eR8G8B8A8Unorm) {
        LOG_ERROR(Render_Vulkan,"Photo: unsupported format {}; no image saved",vk::to_string(format));return;
    }
    try {
        const u32 width=image.info.size.width,height=image.info.size.height;
        if (!width || !height || width>16384 || height>16384) return;
        const u64 size=u64(width)*height*4;
        VideoCore::Buffer readback{instance,scheduler,VideoCore::MemoryUsage::Download,0,vk::BufferUsageFlagBits::eTransferDst,size};
        auto cmd=scheduler.CommandBuffer();
        image.Transit(vk::ImageLayout::eTransferSrcOptimal,vk::AccessFlagBits2::eTransferRead,{},cmd);
        const vk::BufferImageCopy copy{.imageSubresource={vk::ImageAspectFlagBits::eColor,0,0,1},.imageExtent={width,height,1}};
        cmd.copyImageToBuffer(image.GetImage(),vk::ImageLayout::eTransferSrcOptimal,readback.Handle(),copy);
        scheduler.Finish();
        vmaInvalidateAllocation(instance.GetAllocator(),readback.buffer.allocation,0,size);
        std::vector<u8> pixels(readback.mapped_data.begin(),readback.mapped_data.begin()+size);
        // Private prototype destination. No dump/save changes, no desktop capture.
        const auto directory=std::filesystem::path("photos");std::filesystem::create_directories(directory);
        static u64 sequence{};
        const auto stamp=std::chrono::system_clock::now().time_since_epoch().count();
        const auto path=directory/("Kyoei-"+std::to_string(stamp)+"-"+std::to_string(++sequence)+".png");
        if (SavePng(path,pixels,width,height,bgra)) LOG_INFO(Render_Vulkan,"Photo saved: {} (native {}x{}, before host overlays/scaling)",path.string(),width,height);
        else LOG_ERROR(Render_Vulkan,"Photo: failed saving {}",path.string());
    } catch (const std::exception& error) { LOG_ERROR(Render_Vulkan,"Photo: {}",error.what()); }
}
}
