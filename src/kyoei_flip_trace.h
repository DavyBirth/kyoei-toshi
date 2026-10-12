// Local diagnostic history. Does not change flip scheduling or surface labels.
#pragma once

#include <array>
#include <mutex>
#include <windows.h>

namespace KyoeiFlipTrace {
struct Event {
    unsigned long long sequence, request, tick, label;
    unsigned long thread, buffer;
    bool callback;
};
inline std::mutex mutex;
inline std::array<Event, 64> history{};
inline unsigned long long sequence = 0, requests = 0;

inline unsigned long long Record(bool callback, unsigned long long request,
                                 unsigned long buffer, unsigned long long label) {
    std::lock_guard lock(mutex);
    if (!callback) request = ++requests;
    const auto serial = ++sequence;
    history[(serial - 1) % history.size()] =
        {serial, request, GetTickCount64(), label, GetCurrentThreadId(), buffer, callback};
    return request;
}

inline void Dump() {
    std::array<Event, 64> copy;
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
        gz_log("flip trace: seq=%llu request=%llu event=%s thread=%lu tick=%llu buffer=%lu label=0x%llx",
               event.sequence, event.request, event.callback ? "irq" : "register",
               event.thread, event.tick, event.buffer, event.label);
    }
}
}
