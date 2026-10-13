/* Synchronous publication of several GPU downloads with one fence wait.
 * A ring-buffer overlap must publish the old range before it is overwritten.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <cstdint>
#include <functional>
#include <vector>
#include <utility>
#include <cstdlib>
namespace KyoeiReadback {
class Batch {
    struct Pending { uint64_t offset, size; std::function<void()> publish; };
    std::function<void()> finish;
    std::vector<Pending> pending;
public:
    unsigned images=0, waits=0;
    uint64_t bytes=0;
    explicit Batch(std::function<void()> finish_) : finish(std::move(finish_)) {}
    void Flush() {
        if (pending.empty()) return;
        finish(); ++waits;
        for (auto& item:pending) item.publish();
        pending.clear();
    }
    void BeforeReuse(uint64_t offset,uint64_t size) {
        for (const auto& item:pending)
            if (offset < item.offset+item.size && item.offset < offset+size) { Flush(); return; }
    }
    void Add(uint64_t offset,uint64_t size,std::function<void()> publish) {
        pending.push_back({offset,size,std::move(publish)}); ++images; bytes+=size;
    }
    void RecordSingle(uint64_t size) { ++images; ++waits; bytes+=size; }
    ~Batch() { Flush(); }
};
inline thread_local Batch* active=nullptr;
inline thread_local Batch* observing=nullptr;
inline void RecordSingle(uint64_t size) { if (observing) observing->RecordSingle(size); }
inline bool Enabled() {
    // Runtime A/B validation is pending: retain the established path by default.
    static const bool enabled=[] { const char *v=std::getenv("KYOEI_BATCH_READBACK"); return v && v[0]=='1'; }();
    return enabled;
}
class Activation {
    Batch* previous;
    Batch* previous_observer;
public:
    explicit Activation(Batch& batch) : previous(active),previous_observer(observing) {
        observing=&batch;
        if (Enabled()) active=&batch;
    }
    ~Activation() { active=previous; observing=previous_observer; }
    Activation(const Activation&)=delete;
    Activation& operator=(const Activation&)=delete;
};
}
