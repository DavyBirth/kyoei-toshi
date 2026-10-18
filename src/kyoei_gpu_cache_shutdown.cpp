// Flush already captured CPU cache blobs. Never wait for a new GPU queue task
// while closing: that queue can be occupied by guest work or a driver wait.
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include "video_core/amdgpu/liverpool.h"
#include "video_core/renderer_vulkan/vk_rasterizer.h"
#include "core/memory.h"
#include "kyoei_pipeline_cache.h"
#include "kyoei_tiny_download.h"
#include "kyoei_vulkan_recording.h"
#include "kyoei_stutter_trace.h"
extern "C" {
#include "gz.h"
}
extern std::unique_ptr<AmdGpu::Liverpool> liverpool;
static std::atomic<unsigned> observed_phase{};
extern "C" void kyoei_gpu_cache_shutdown(void) {
    if(KyoeiStutterTrace::Path()){
        const auto trace=KyoeiStutterTrace::Flush();
        gz_log("Kyoei stutter trace: written=%u records=%llu dropped=%llu",unsigned(trace.written),
            (unsigned long long)trace.records,(unsigned long long)trace.dropped);
    }
    KyoeiVulkanRecording::Report();
    if(!std::getenv("KYOEI_PIPELINE_CACHE_DIR") || !liverpool)return;
    auto *rasterizer=Core::Memory::Instance()->GetRasterizer();if(!rasterizer)return;
    static std::atomic_flag requested=ATOMIC_FLAG_INIT;if(requested.test_and_set())return;
    auto &writer=KyoeiPipelineCache::Writer();
    const bool settled=writer.Flush(std::chrono::seconds(3));
    const auto status=writer.GetStatus();
    const bool saved=settled && status.completed && status.last_success;
    observed_phase.store(saved?2:1);
    gz_log("Kyoei driver cache: exit flush settled=%u completed=%u failed=%u latest_saved=%u; no GPU queue wait",
        unsigned(settled),status.completed,status.failed,unsigned(saved));
}
extern "C" void kyoei_gpu_cache_self_test(void) {
    if(!Core::Memory::Instance()->GetRasterizer())gz_stop(24,"GPU CACHE TEST: no rasterizer");
    if(const char *busy=getenv("KYOEI_CACHE_TEST_BUSY");busy && busy[0]=='1') {
        auto entered=std::make_shared<std::atomic<bool>>(false);
        liverpool->SendCommand<false>([entered] {
            entered->store(true,std::memory_order_release);
            std::this_thread::sleep_for(std::chrono::milliseconds(4200));
        });
        const auto until=std::chrono::steady_clock::now()+std::chrono::seconds(2);
        while(!entered->load(std::memory_order_acquire) && std::chrono::steady_clock::now()<until)
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        if(!entered->load())gz_stop(24,"GPU CACHE TEST: worker did not enter busy fixture");
        gz_log("GPU CACHE TEST: real GPU command thread occupied for 4200 ms");
    }
    const auto started=std::chrono::steady_clock::now();
    kyoei_gpu_cache_shutdown();
    if(observed_phase.load()!=2)gz_stop(24,"GPU CACHE TEST: checkpoint failed, phase=%u",observed_phase.load());
    const double elapsed=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-started).count();
    if(getenv("KYOEI_CACHE_TEST_BUSY") && elapsed>=1000)gz_stop(24,"GPU CACHE TEST: exit still waits on busy GPU");
    gz_stop(0,"GPU CACHE TEST: checkpoint completed in %.2f ms without game execution",elapsed);
}

// Exercise the production image-copy path on a separate scheduler before guest_main.
extern "C" void kyoei_cooperative_readback_self_test(void) {
    auto* rasterizer=Core::Memory::Instance()->GetRasterizer();
    if(!rasterizer || !KyoeiCooperativeReadback::Enabled())gz_stop(24,"COOPERATIVE READBACK TEST: unavailable");
    const auto& instance=*rasterizer->GetBufferCache().GetUtilityBuffer(VideoCore::MemoryUsage::Download).instance;
#ifdef KYOEI_VULKAN_RECORDING_ENABLED
    if(const auto* test=std::getenv("KYOEI_STREAM_SELF_TEST");test && test[0]=='1'){
        extern void KyoeiStreamSelfTest(const Vulkan::Instance&);
        KyoeiStreamSelfTest(instance);
    }
#endif
    std::array<u8,256> destination;destination.fill(0xCA);
    VideoCore::ImageInfo eligible{};
    eligible.pixel_format=vk::Format::eR8G8B8A8Unorm;
    eligible.resources.layers=eligible.resources.levels=1;
    eligible.num_bits=32;eligible.pitch=64;eligible.guest_size=256;eligible.guest_address=1;
    if(!KyoeiTinyDownload::Eligible(eligible))gz_stop(24,"COOPERATIVE READBACK TEST: valid layout rejected");
    eligible.num_samples=4;
    if(KyoeiTinyDownload::Eligible(eligible))gz_stop(24,"COOPERATIVE READBACK TEST: MSAA accepted");
    eligible.num_samples=1;eligible.props.is_tiled=1;
    if(KyoeiTinyDownload::Eligible(eligible))gz_stop(24,"COOPERATIVE READBACK TEST: tiled accepted");
    eligible.props.is_tiled=0;eligible.guest_size=3;
    if(KyoeiTinyDownload::Eligible(eligible))gz_stop(24,"COOPERATIVE READBACK TEST: short layout accepted");
    std::weak_ptr<VideoCore::Buffer> lifetime;
    {
        Vulkan::Scheduler scheduler{instance};
        const VkImageCreateInfo image_info{
            .sType=VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,.imageType=VK_IMAGE_TYPE_2D,
            .format=VK_FORMAT_R8G8B8A8_UNORM,.extent={1,1,1},.mipLevels=1,.arrayLayers=1,
            .samples=VK_SAMPLE_COUNT_1_BIT,.tiling=VK_IMAGE_TILING_OPTIMAL,
            .usage=VK_IMAGE_USAGE_TRANSFER_DST_BIT|VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
            .sharingMode=VK_SHARING_MODE_EXCLUSIVE,.initialLayout=VK_IMAGE_LAYOUT_UNDEFINED,
        };
        const VmaAllocationCreateInfo allocation_info{.usage=VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE};
        struct ImageOwner {
            VmaAllocator allocator;
            VkImage image{};
            VmaAllocation allocation{};
            ~ImageOwner(){if(image)vmaDestroyImage(allocator,image,allocation);}
        } image{instance.GetAllocator()};
        if(vmaCreateImage(image.allocator,&image_info,&allocation_info,&image.image,&image.allocation,nullptr)!=VK_SUCCESS)
            gz_stop(24,"COOPERATIVE READBACK TEST: image allocation failed");
        auto storage=KyoeiTinyDownload::Allocate(instance,scheduler,256);
        lifetime=storage;
        const auto command=scheduler.CommandBuffer();
        const vk::ImageSubresourceRange range{.aspectMask=vk::ImageAspectFlagBits::eColor,.baseMipLevel=0,.levelCount=1,.baseArrayLayer=0,.layerCount=1};
        const auto transition=[&](vk::ImageLayout old_layout,vk::ImageLayout new_layout,
            vk::AccessFlags2 source,vk::AccessFlags2 target) {
            const vk::ImageMemoryBarrier2 barrier{
                .srcStageMask=vk::PipelineStageFlagBits2::eTransfer,.srcAccessMask=source,
                .dstStageMask=vk::PipelineStageFlagBits2::eTransfer,.dstAccessMask=target,
                .oldLayout=old_layout,.newLayout=new_layout,
                .srcQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,.dstQueueFamilyIndex=VK_QUEUE_FAMILY_IGNORED,
                .image=vk::Image{image.image},.subresourceRange=range,
            };
            command.pipelineBarrier2(vk::DependencyInfo{.imageMemoryBarrierCount=1,.pImageMemoryBarriers=&barrier});
        };
        transition(vk::ImageLayout::eUndefined,vk::ImageLayout::eTransferDstOptimal,{},vk::AccessFlagBits2::eTransferWrite);
        vk::ClearColorValue green{};green.float32[1]=1.f;green.float32[3]=1.f;
        command.clearColorImage(vk::Image{image.image},vk::ImageLayout::eTransferDstOptimal,green,range);
        transition(vk::ImageLayout::eTransferDstOptimal,vk::ImageLayout::eTransferSrcOptimal,
            vk::AccessFlagBits2::eTransferWrite,vk::AccessFlagBits2::eTransferRead);
        KyoeiTinyDownload::RecordImageCopy(command,vk::Image{image.image},64,*storage);
        auto completion=KyoeiCooperativeReadback::Process(scheduler,[&] {
            auto* context=KyoeiCooperativeReadback::current;
            if(!context)gz_stop(24,"COOPERATIVE READBACK TEST: no scope");
            context->publications.emplace_back([storage,&destination] {
                KyoeiTinyDownload::Invalidate(*storage);
                std::memcpy(destination.data(),storage->mapped_data.data(),4);
            });
        });
        storage.reset();
        if(!completion)gz_stop(24,"COOPERATIVE READBACK TEST: no completion");
        completion->Wait();
        if(destination[0]!=0 || destination[1]!=255 || destination[2]!=0 || destination[3]!=255)
            gz_stop(24,"COOPERATIVE READBACK TEST: wrong RGBA result");
        for(size_t i=4;i<destination.size();++i)
            if(destination[i]!=0xCA)gz_stop(24,"COOPERATIVE READBACK TEST: padding overwritten");
    }
    if(!lifetime.expired())gz_stop(24,"COOPERATIVE READBACK TEST: staging leaked");
    gz_stop(0,"COOPERATIVE READBACK TEST: GPU image, owned staging, publication and padding PASS; no game execution");
}
