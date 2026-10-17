#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>

// Fetch decoding depends only on instruction bytes. Descriptors, addresses
// read by those instructions and bound resources are never cached here.
namespace KyoeiFetchCache {
template<class Value, size_t Slots = 128, size_t Ways = 2> class Cache {
    static_assert(Slots && Ways);
    static constexpr size_t MaxBytes = 512;
    struct Entry {
        const void* code = nullptr;
        size_t size = 0;
        bool valid = false;
        Value value{};
        std::array<unsigned char, MaxBytes> bytes{};
    };
    std::array<std::array<Entry, Ways>, Slots> entries{};
    std::array<size_t, Slots> next_victim{};
    std::array<unsigned char, MaxBytes> before{};
    Entry* pending = nullptr;
    const void* pending_code = nullptr;
    bool before_valid = false;
public:
    static bool Fits(const void* code, size_t size) {
        return code && size && size <= MaxBytes &&
               size <= 4096 - (reinterpret_cast<uintptr_t>(code) & 4095);
    }
    static size_t SlotFor(const void* code) {
        const auto p = reinterpret_cast<uintptr_t>(code);
        return ((p >> 2) ^ (p >> 12) ^ (p >> 22)) % Slots;
    }
    const Value* Probe(const void* code) {
        const size_t slot=SlotFor(code);
        auto& bucket=entries[slot];
        pending=nullptr;
        for(auto& candidate:bucket)if(candidate.code==code){pending=&candidate;break;}
        if(!pending){
            size_t victim=next_victim[slot];
            for(size_t i=0;i<Ways;++i)if(!bucket[i].code){victim=i;break;}
            pending=&bucket[victim];next_victim[slot]=(victim+1)%Ways;
        }
        pending_code=code;
        before_valid = false;
        auto& entry = *pending;
        if (entry.code != code || !Fits(code, entry.size)) return nullptr;
        if (entry.valid && std::memcmp(code, entry.bytes.data(), entry.size) == 0)
            return &entry.value;
        std::memcpy(before.data(), code, entry.size);
        before_valid = true;
        return nullptr;
    }
    void Record(const void* code, size_t size, const Value& value) {
        if (!pending || pending_code != code) return;
        auto& entry = *pending;
        if (!Fits(code, size)) {
            entry.valid = false;
            entry.code = nullptr;
            return;
        }
        // A cold parse only establishes a bounded length. Another slow parse
        // must be enclosed by equal byte snapshots before it can be reused.
        const bool confirmed = before_valid && entry.code == code && entry.size == size &&
                               std::memcmp(code, before.data(), size) == 0;
        entry.code = code;
        entry.size = size;
        entry.value = value;
        entry.valid = confirmed;
        std::memcpy(entry.bytes.data(), confirmed ? before.data() : code, size);
    }
};
}
