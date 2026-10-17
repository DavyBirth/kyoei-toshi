#pragma once
#include "kyoei_cooperative_readback.h"
#include "kyoei_frame_stats.h"
#include "core/memory.h"
#include "video_core/buffer_cache/buffer.h"
#include "video_core/renderer_vulkan/vk_instance.h"
#include "video_core/renderer_vulkan/vk_scheduler.h"
#include "video_core/texture_cache/image.h"
#include <vk_mem_alloc.h>

namespace KyoeiTinyDownload {
inline bool Eligible(const VideoCore::ImageInfo& info) {
    return info.size.width==1 && info.size.height==1 && info.size.depth==1 &&
        info.resources.layers==1 && info.resources.levels==1 && info.num_samples==1 &&
        !info.props.is_tiled && !info.props.is_block && !info.props.is_depth &&
        info.pixel_format==vk::Format::eR8G8B8A8Unorm && info.num_bits==32 &&
        info.guest_address && info.pitch && info.pitch<=1024 &&
        info.guest_size>=uint64_t(info.pitch)*4;
}
inline void ToHost(vk::CommandBuffer command,vk::Buffer buffer) {
    const vk::BufferMemoryBarrier2 barrier{
        .srcStageMask=vk::PipelineStageFlagBits2::eTransfer,
        .srcAccessMask=vk::AccessFlagBits2::eTransferWrite,
        .dstStageMask=vk::PipelineStageFlagBits2::eHost,
        .dstAccessMask=vk::AccessFlagBits2::eHostRead,
        .srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,
        .buffer=buffer,.offset=0,.size=4,
    };
    command.pipelineBarrier2(vk::DependencyInfo{.bufferMemoryBarrierCount=1,.pBufferMemoryBarriers=&barrier});
}
inline std::shared_ptr<VideoCore::Buffer> Allocate(const Vulkan::Instance& instance,Vulkan::Scheduler& scheduler,uint64_t bytes) {
    return std::make_shared<VideoCore::Buffer>(instance,scheduler,VideoCore::MemoryUsage::Download,
        0,vk::BufferUsageFlagBits::eTransferDst,bytes);
}
inline void Invalidate(const VideoCore::Buffer& buffer) {
    if(!buffer.is_coherent) {
        const auto result=vmaInvalidateAllocation(buffer.instance->GetAllocator(),buffer.buffer.allocation,0,4);
        ASSERT_MSG(result==VK_SUCCESS,"Tiny readback invalidation failed");
    }
}
inline void RecordImageCopy(vk::CommandBuffer command,vk::Image image,unsigned pitch,const VideoCore::Buffer& storage) {
    const vk::BufferImageCopy copy{
        .bufferOffset=0,.bufferRowLength=pitch,.bufferImageHeight=1,
        .imageSubresource={.aspectMask=vk::ImageAspectFlagBits::eColor,.mipLevel=0,.baseArrayLayer=0,.layerCount=1},
        .imageOffset={0,0,0},.imageExtent={1,1,1},
    };
    command.copyImageToBuffer(image,vk::ImageLayout::eTransferSrcOptimal,storage.Handle(),copy);
    ToHost(command,storage.Handle());
}
inline bool Queue(VideoCore::Image& image,const Vulkan::Instance& instance,Vulkan::Scheduler& scheduler) {
    auto* context=KyoeiCooperativeReadback::current;
    if(!context || !context->recording || !Eligible(image.info))return false;
    const auto storage=Allocate(instance,scheduler,uint64_t(image.info.pitch)*4);
    scheduler.EndRendering();
    image.Transit(vk::ImageLayout::eTransferSrcOptimal,vk::AccessFlagBits2::eTransferRead,{});
    const auto command=scheduler.CommandBuffer();
    RecordImageCopy(command,image.GetImage(),image.info.pitch,*storage);
    context->publications.emplace_back([storage,address=image.info.guest_address] {
        KyoeiFrameStats::Work measurement(4);
        Invalidate(*storage);
        // Only one RGBA pixel was written by the GPU. Row padding is not data.
        Core::Memory::Instance()->TryWriteBacking(std::bit_cast<u8*>(address),storage->mapped_data.data(),4);
    });
    return true;
}
}
