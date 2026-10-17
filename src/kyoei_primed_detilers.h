#pragma once
#include <atomic>
#include <bitset>
#include <mutex>
#include <unordered_map>

namespace KyoeiPrimedDetilers {
inline constexpr unsigned SlotCount=160;
inline std::mutex mutex;
inline std::unordered_map<const void*,std::bitset<SlotCount>> pending;
inline std::atomic<bool> any{false};
inline void Mark(const void* owner,unsigned slot) {
    if(slot>=SlotCount)return;
    std::scoped_lock lock{mutex};
    pending[owner].set(slot);
    any.store(true,std::memory_order_release);
}
// Only the first real use can discard an unused primed pipeline. Once claimed,
// the original cache/lifetime rules apply; never destroy a submitted pipeline.
inline bool Claim(const void* owner,unsigned slot) {
    if(slot>=SlotCount || !any.load(std::memory_order_acquire))return false;
    std::scoped_lock lock{mutex};
    auto it=pending.find(owner);
    if(it==pending.end())return false;
    const bool primed=it->second.test(slot);
    it->second.reset(slot);
    if(it->second.none())pending.erase(it);
    any.store(!pending.empty(),std::memory_order_release);
    return primed;
}
inline void Forget(const void* owner) {
    std::scoped_lock lock{mutex};
    pending.erase(owner);
    any.store(!pending.empty(),std::memory_order_release);
}
}
