#pragma once
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>
#include <cstdint>
#include "kyoei_diagnostic_activity.h"
extern "C" uint64_t kyoei_tls_fault_count(void);

// Intervals between completed game presents, excluding idle redraws.
// Opt-in; reports every 120 intervals, with no per-frame log traffic.
namespace KyoeiFrameStats {
inline bool Enabled() {
    static const bool enabled = [] {
        const char* value = std::getenv("KYOEI_FRAME_STATS");
        return value && value[0] == '1';
    }();
    return enabled;
}
class Work {
    unsigned category;
    std::chrono::steady_clock::time_point start;
    KyoeiDiagnosticActivity::Stamp diagnostic_start{};
public:
    explicit Work(unsigned category_) : category(category_) {
        if (Enabled()) { start = std::chrono::steady_clock::now();diagnostic_start=KyoeiDiagnosticActivity::Now(); }
    }
    ~Work() {
        if (!Enabled()) return;
        // Deferred publication may run on the scheduler worker. Keep samples
        // per thread rather than racing or mixing overlapping CPU scopes.
        static thread_local std::array<std::array<double, 120>, 6> samples{};
        static thread_local std::array<size_t, 6> counts{};
        static thread_local std::array<bool,6> diagnostics{};
        static thread_local std::array<uint64_t,6> previous_generation{};
        auto& values = samples[category]; auto& count = counts[category];
        auto diagnostic_end=KyoeiDiagnosticActivity::Now();
        diagnostics[category]|=KyoeiDiagnosticActivity::Overlaps(diagnostic_start,diagnostic_end) ||
            (count && previous_generation[category]!=diagnostic_start.generation);
        previous_generation[category]=diagnostic_end.generation;
        values[count++] = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
        if (count != values.size()) return;
        double total = 0; for (double value : values) total += value;
        auto sorted = values; std::sort(sorted.begin(), sorted.end());
        const char* names[] = {"prepare", "present", "linear-downloads",
                               "readback-finish", "readback-publish", "readback-map"};
        gz_log("Kyoei work stats: stage=%s calls=120 mean_ms=%.2f p95_ms=%.2f max_ms=%.2f diagnostic=%u",
               names[category], total/count, sorted[113], sorted.back(),unsigned(diagnostics[category]));
        count = 0;diagnostics[category]=false;
    }
};
inline void Presented() {
    if (!Enabled()) return;
    using Clock = std::chrono::steady_clock;
    static auto previous = Clock::now();
    static bool started = false;
    static std::array<double, 120> intervals{};
    static size_t count = 0;
    static bool diagnostic_window=false;
    static auto previous_diagnostic=KyoeiDiagnosticActivity::Now();
    static uint64_t previous_faults=0;
    const auto now = Clock::now();
    const double milliseconds = std::chrono::duration<double, std::milli>(now - previous).count();
    previous = now;
    auto diagnostic_now=KyoeiDiagnosticActivity::Now();
    if (!started) { started = true;previous_diagnostic=diagnostic_now;previous_faults=kyoei_tls_fault_count();return; }
    diagnostic_window|=KyoeiDiagnosticActivity::Overlaps(previous_diagnostic,diagnostic_now);
    previous_diagnostic=diagnostic_now;
    if (milliseconds <= 0) return;
    intervals[count++] = milliseconds;
    if (count != intervals.size()) return;
    double total = 0;
    for (double value : intervals) total += value;
    auto sorted = intervals;
    std::sort(sorted.begin(), sorted.end());
    gz_log("Kyoei frame stats: intervals=120 fps=%.2f mean_ms=%.2f p50_ms=%.2f p95_ms=%.2f max_ms=%.2f diagnostic=%u",
           count * 1000.0 / total, total / count, sorted[59], sorted[113], sorted.back(),unsigned(diagnostic_window));
    const uint64_t faults = kyoei_tls_fault_count();
    gz_log("Kyoei TLS stats: handled_total=%llu delta=%llu per_present=%.2f",
           (unsigned long long)faults, (unsigned long long)(faults - previous_faults),
           (faults - previous_faults) / 120.0);
    previous_faults = faults;
    count = 0;diagnostic_window=false;
}
}
