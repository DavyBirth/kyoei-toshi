#pragma once
#include <algorithm>
#include <array>
#include <chrono>
#include <cstdlib>

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
public:
    explicit Work(unsigned category_) : category(category_) {
        if (Enabled()) start = std::chrono::steady_clock::now();
    }
    ~Work() {
        if (!Enabled()) return;
        // Each category runs on a single owning renderer thread.
        static std::array<std::array<double, 120>, 3> samples{};
        static std::array<size_t, 3> counts{};
        auto& values = samples[category]; auto& count = counts[category];
        values[count++] = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
        if (count != values.size()) return;
        double total = 0; for (double value : values) total += value;
        auto sorted = values; std::sort(sorted.begin(), sorted.end());
        const char* names[] = {"prepare", "present", "linear-downloads"};
        gz_log("Kyoei work stats: stage=%s calls=120 mean_ms=%.2f p95_ms=%.2f max_ms=%.2f",
               names[category], total/count, sorted[113], sorted.back());
        count = 0;
    }
};
inline void Presented() {
    if (!Enabled()) return;
    using Clock = std::chrono::steady_clock;
    static auto previous = Clock::now();
    static bool started = false;
    static std::array<double, 120> intervals{};
    static size_t count = 0;
    const auto now = Clock::now();
    const double milliseconds = std::chrono::duration<double, std::milli>(now - previous).count();
    previous = now;
    if (!started) { started = true; return; }
    if (milliseconds <= 0) return;
    intervals[count++] = milliseconds;
    if (count != intervals.size()) return;
    double total = 0;
    for (double value : intervals) total += value;
    auto sorted = intervals;
    std::sort(sorted.begin(), sorted.end());
    gz_log("Kyoei frame stats: intervals=120 fps=%.2f mean_ms=%.2f p50_ms=%.2f p95_ms=%.2f max_ms=%.2f",
           count * 1000.0 / total, total / count, sorted[59], sorted[113], sorted.back());
    count = 0;
}
}
