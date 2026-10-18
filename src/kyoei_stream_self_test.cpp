#include <array>
#include <algorithm>
#include "video_core/buffer_cache/buffer.h"
#include "video_core/renderer_vulkan/vk_instance.h"
#include "video_core/renderer_vulkan/vk_scheduler.h"
#include <vk_mem_alloc.h>
extern "C" {
#include "gz.h"
}

extern "C" void KyoeiStreamSelfTest(const Vulkan::Instance& instance){
    constexpr size_t Count=2048,Bytes=128;
    Vulkan::Scheduler scheduler{instance};
    const auto alignment=std::max<uint64_t>(16,instance.NonCoherentAtomSize());
    const auto stride=(Bytes+alignment-1)/alignment*alignment;
    VideoCore::StreamBuffer stream{instance,scheduler,VideoCore::MemoryUsage::Stream,16*stride};
    VideoCore::Buffer output{instance,scheduler,VideoCore::MemoryUsage::Download,0,
                            VideoCore::AllFlags,Count*Bytes};
    std::array<u8,Bytes> data;
    const auto expected=[](size_t block,size_t byte){return u8((block*29+byte*7)&255);};
    uint64_t previous=0;unsigned wraps=0;
    for(size_t block=0;block<Count;++block){
        for(size_t byte=0;byte<Bytes;++byte)data[byte]=expected(block,byte);
        const auto offset=stream.Copy(data.data(),Bytes,alignment);
        if(offset<previous)++wraps;
        previous=offset;
        const vk::BufferCopy copy{.srcOffset=offset,.dstOffset=block*Bytes,.size=Bytes};
        // Map may have submitted a full ring and begun another command buffer.
        scheduler.CommandBuffer().copyBuffer(stream.Handle(),output.Handle(),copy);
        if(block%37==36)scheduler.Flush();
    }
    const vk::BufferMemoryBarrier2 barrier{
        .srcStageMask=vk::PipelineStageFlagBits2::eTransfer,
        .srcAccessMask=vk::AccessFlagBits2::eTransferWrite,
        .dstStageMask=vk::PipelineStageFlagBits2::eHost,
        .dstAccessMask=vk::AccessFlagBits2::eHostRead,
        .srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,
        .dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,
        .buffer=output.Handle(),.offset=0,.size=Count*Bytes,
    };
    scheduler.CommandBuffer().pipelineBarrier2(vk::DependencyInfo{
        .bufferMemoryBarrierCount=1,.pBufferMemoryBarriers=&barrier});
    scheduler.Finish();
    if(!output.is_coherent)
        vmaInvalidateAllocation(instance.GetAllocator(),output.buffer.allocation,0,Count*Bytes);
    for(size_t block=0;block<Count;++block)
        for(size_t byte=0;byte<Bytes;++byte)
            if(output.mapped_data[block*Bytes+byte]!=expected(block,byte))
                gz_stop(24,"STREAM WATCH TEST: overwritten GPU data at block=%llu byte=%llu",
                        (unsigned long long)block,(unsigned long long)byte);
    if(wraps!=127)gz_stop(24,"STREAM WATCH TEST: expected 127 wraps, got %u",wraps);
    gz_log("STREAM WATCH TEST: 2048 GPU transfers, 127 ring wraps and partial submissions PASS");
}
