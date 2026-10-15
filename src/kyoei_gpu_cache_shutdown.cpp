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
extern "C" {
#include "gz.h"
}
extern std::unique_ptr<AmdGpu::Liverpool> liverpool;
static std::atomic<unsigned> observed_phase{};
extern "C" void kyoei_gpu_cache_shutdown(void) {
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
