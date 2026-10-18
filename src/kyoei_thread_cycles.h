#pragma once
#include <cstdint>
#include <cstdlib>
#include <optional>
#ifdef _WIN32
#pragma push_macro("NOMINMAX")
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#pragma pop_macro("NOMINMAX")
#endif

namespace KyoeiThreadCycles {
struct Sample { uint64_t cycles{}, thread{}; bool valid{}; };
inline bool Enabled() {
    static const bool enabled=[] {
        const auto* value=std::getenv("KYOEI_STUTTER_CYCLES");
        return value && value[0]=='1';
    }();
    return enabled;
}
inline Sample Read() {
#ifdef _WIN32
    ULONG64 cycles=0;
    const bool valid=QueryThreadCycleTime(GetCurrentThread(), &cycles)!=0;
    return {cycles, GetCurrentThreadId(), valid};
#else
    return {};
#endif
}
inline std::optional<uint64_t> Difference(Sample before, Sample after) {
    if (!before.valid || !after.valid || before.thread!=after.thread || after.cycles<before.cycles)
        return std::nullopt;
    return after.cycles-before.cycles;
}
}
