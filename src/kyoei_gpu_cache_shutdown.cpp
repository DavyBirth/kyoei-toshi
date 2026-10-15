// Finish optional cache IO on its owning GPU command thread, never on a live
// pipeline compilation thread. Error shutdowns do not attempt cache writes.
#include <atomic>
#include <chrono>
#include <memory>
#include <thread>
#include "video_core/amdgpu/liverpool.h"
#include "video_core/renderer_vulkan/vk_rasterizer.h"
#include "core/memory.h"
extern "C" {
#include "gz.h"
}
extern std::unique_ptr<AmdGpu::Liverpool> liverpool;
extern "C" void kyoei_gpu_cache_shutdown(void) {
    if(!std::getenv("KYOEI_PIPELINE_CACHE_DIR") || !liverpool)return;
    auto *rasterizer=Core::Memory::Instance()->GetRasterizer();if(!rasterizer)return;
    static std::atomic_flag requested=ATOMIC_FLAG_INIT;if(requested.test_and_set())return;
    auto done=std::make_shared<std::atomic<bool>>(false);
    liverpool->SendCommand<false>([done,rasterizer] {
        rasterizer->GetPipelineCache().Sync();done->store(true,std::memory_order_release);
    });
    const auto until=std::chrono::steady_clock::now()+std::chrono::seconds(3);
    while(!done->load(std::memory_order_acquire)&&std::chrono::steady_clock::now()<until)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    gz_log("Kyoei driver cache: normal-exit checkpoint %s",done->load()?"completed":"timed out; previous cache retained");
}
