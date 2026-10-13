#pragma once
#include <atomic>
#include <cstdint>

// Identify measurements that overlap F9 work. Keep their raw timings; never
// discard slow game frames to make a performance report look better.
namespace KyoeiDiagnosticActivity {
inline std::atomic<uint64_t> state{};
inline void Change(bool active) {
    auto previous=state.load();
    while(!state.compare_exchange_weak(previous,((previous+2)&~uint64_t(1))|uint64_t(active))) {}
}
inline void Begin() { Change(true); }
inline void End() { Change(false); }
struct Stamp { uint64_t generation;bool active; };
inline Stamp Now() { auto value=state.load();return {value>>1,bool(value&1)}; }
inline bool Overlaps(Stamp before,Stamp after) {
    return before.active || after.active || before.generation!=after.generation;
}
}
