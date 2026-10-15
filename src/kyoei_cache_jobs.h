// CPU-only cache writes. Vulkan handles never leave their owning thread.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <chrono>
#include <cstdint>
#include <condition_variable>
#include <functional>
#include <mutex>
#include <optional>
#include <thread>
namespace KyoeiPipelineCache {
class Jobs {
    std::mutex mutex;
    std::condition_variable changed;
    std::optional<std::function<bool()>> pending;
    bool stopping=false,busy=false,last_success=true;
    unsigned completed=0,failed=0;
    std::thread worker;
    void Run() {
        std::unique_lock lock{mutex};
        for(;;) {
            changed.wait(lock,[&] {return stopping || pending.has_value();});
            if(!pending && stopping)return;
            auto job=std::move(*pending);pending.reset();busy=true;
            lock.unlock();bool success=false;
            try {success=job();} catch(...) {success=false;}
            lock.lock();busy=false;last_success=success;++completed;if(!success)++failed;
            changed.notify_all();
        }
    }
public:
    struct Status {unsigned completed,failed;bool last_success;};
    Jobs():worker([this] {Run();}) {}
    ~Jobs() {
        {std::lock_guard lock{mutex};stopping=true;}changed.notify_all();worker.join();
    }
    bool Submit(std::function<bool()> job) {
        std::lock_guard lock{mutex};if(stopping)return false;
        // One active write plus the most recent pending snapshot; bound memory
        // and avoid writing superseded driver blobs during heavy compilation.
        pending=std::move(job);changed.notify_all();return true;
    }
    bool Flush(std::chrono::milliseconds timeout) {
        std::unique_lock lock{mutex};
        return changed.wait_for(lock,timeout,[&] {return !busy && !pending;});
    }
    Status GetStatus() {std::lock_guard lock{mutex};return {completed,failed,last_success};}
};
inline Jobs& Writer() {static Jobs jobs;return jobs;}
struct CheckpointGate {
    uint64_t saved_generation=0,next_ms=0;
    unsigned polls=0;
    bool Poll(uint64_t generation) {return generation!=saved_generation && (++polls%256)==0;}
    bool Due(uint64_t now_ms)const{return now_ms>=next_ms;}
    void Record(uint64_t generation,uint64_t now_ms,bool queued) {
        next_ms=now_ms+30000;if(queued)saved_generation=generation;
    }
};
}
