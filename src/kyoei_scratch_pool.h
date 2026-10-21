#pragma once
#include <bit>
#include <cstdint>
#include <mutex>
#include <optional>
#include <unordered_map>
#include <vector>

// Detile/tile scratch buffers were created and destroyed per texture upload;
// each one is a driver allocation on the GPU command thread. Recycle released
// buffers by power-of-two size class. A buffer only returns to the pool from
// the scheduler's deferred operation, i.e. after the GPU finished with it.
namespace KyoeiScratchPool {
inline constexpr uint64_t MinClass=64*1024;
inline constexpr uint64_t BudgetBytes=256ull*1024*1024;
inline uint64_t Class(uint64_t size){return size<=MinClass?MinClass:std::bit_ceil(size);}

template<class Buffer,class Allocation> struct Pool {
    struct Entry {uint64_t capacity; Buffer buffer; Allocation allocation;};
    std::mutex mutex;
    std::vector<Entry> free;
    uint64_t free_bytes=0;
    std::optional<Entry> Take(uint64_t size){
        const uint64_t want=Class(size);
        std::scoped_lock lock{mutex};
        for(size_t i=free.size();i-->0;) if(free[i].capacity==want){
            Entry e=free[i];free.erase(free.begin()+i);free_bytes-=e.capacity;return e;
        }
        return std::nullopt;
    }
    // Returns entries the caller must destroy (oldest first) to stay in budget.
    std::vector<Entry> Give(const Entry& entry){
        std::vector<Entry> evicted;
        std::scoped_lock lock{mutex};
        if(entry.capacity>BudgetBytes){evicted.push_back(entry);return evicted;}
        free.push_back(entry);free_bytes+=entry.capacity;
        while(free_bytes>BudgetBytes){evicted.push_back(free.front());free_bytes-=free.front().capacity;free.erase(free.begin());}
        return evicted;
    }
    std::vector<Entry> Drain(){
        std::scoped_lock lock{mutex};
        std::vector<Entry> all=std::move(free);free.clear();free_bytes=0;return all;
    }
};
template<class Buffer,class Allocation> Pool<Buffer,Allocation>& Shared(){static Pool<Buffer,Allocation> pool;return pool;}
}
