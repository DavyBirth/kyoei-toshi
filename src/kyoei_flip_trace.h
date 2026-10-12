// Local diagnostic history. Does not change flip scheduling or surface labels.
#pragma once

#include <array>
#include <atomic>
#include <cstdlib>
#include <mutex>
#include <windows.h>

namespace KyoeiFlipTrace {
struct Event {
    unsigned long long sequence, request, tick, label;
    unsigned long thread, buffer;
    const char* action;
    uintptr_t address;
    unsigned long long detail;
};
inline std::mutex mutex;
inline std::array<Event, 256> history{};
inline unsigned long long sequence = 0, requests = 0;
inline std::atomic<uintptr_t> labels_begin{0}, labels_end{0};

inline void Bind(const void* labels, size_t count) {
    const auto begin = reinterpret_cast<uintptr_t>(labels);
    labels_end.store(begin + count * 8);
    labels_begin.store(begin);
}

inline int LabelIndex(const void* label) {
    const auto address = reinterpret_cast<uintptr_t>(label);
    const auto begin = labels_begin.load();
    if (!begin || address < begin || address >= labels_end.load() || (address - begin) % 8) return -1;
    return static_cast<int>((address - begin) / 8);
}

inline void StressAfterWrite() {
    static const bool enabled = [] {
        const auto* value = std::getenv("KYOEI_FLIP_STRESS");
        return value && value[0] == '1';
    }();
    if (enabled) Sleep(2);
}

inline void Action(const char* action, unsigned long buffer, unsigned long long label,
                   const void* address, unsigned long long detail = 0) {
    std::lock_guard lock(mutex);
    const auto serial = ++sequence;
    history[(serial - 1) % history.size()] =
        {serial, 0, GetTickCount64(), label, GetCurrentThreadId(), buffer, action,
         reinterpret_cast<uintptr_t>(address), detail};
}

inline unsigned long long Record(bool callback, unsigned long long request,
                                 unsigned long buffer, unsigned long long label) {
    std::lock_guard lock(mutex);
    if (!callback) request = ++requests;
    const auto serial = ++sequence;
    history[(serial - 1) % history.size()] =
        {serial, request, GetTickCount64(), label, GetCurrentThreadId(), buffer,
         callback ? "irq" : "register", 0, 0};
    return request;
}

inline void Dump() {
    decltype(history) copy;
    unsigned long long last;
    {
        std::lock_guard lock(mutex);
        copy = history;
        last = sequence;
    }
    const auto first = last > copy.size() ? last - copy.size() + 1 : 1;
    gz_log("Kyoei flip history: last %llu events (registration order precedes IRQ queue insertion)",
           last - first + 1);
    for (auto serial = first; serial <= last; ++serial) {
        const auto& event = copy[(serial - 1) % copy.size()];
        gz_log("flip trace: seq=%llu request=%llu event=%s thread=%lu tick=%llu buffer=%lu label=0x%llx address=0x%llx detail=0x%llx",
               event.sequence, event.request, event.action,
               event.thread, event.tick, event.buffer, event.label,
               static_cast<unsigned long long>(event.address), event.detail);
    }
}
}
