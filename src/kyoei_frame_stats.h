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
inline constexpr unsigned CategoryCount = 19;
inline bool Enabled() {
    static const bool enabled = [] {
        const char* value = std::getenv("KYOEI_FRAME_STATS");
        return value && value[0] == '1';
    }();
    return enabled;
}
inline void SmallImage(uint64_t address, unsigned width, unsigned height,
                       unsigned pitch, unsigned format, unsigned role) {
    if (width!=1 || height!=1 || !Enabled()) return;
    struct Key { uint64_t address; unsigned format,role; };
    static thread_local std::array<Key,16> seen{};
    static thread_local unsigned count=0;
    for (unsigned i=0;i<count;++i)
        if (seen[i].address==address && seen[i].format==format && seen[i].role==role) return;
    if (count==seen.size()) return;
    seen[count++]={address,format,role};
    gz_log("Kyoei small image: address=0x%llx width=%u height=%u pitch=%u format=%u role=%u",
        (unsigned long long)address,width,height,pitch,format,role);
}
inline void GpuWait(uint64_t requested, uint64_t submitted_next,
                    uint64_t known_before, uint64_t known_after, bool forced_flush,
                    double milliseconds, KyoeiDiagnosticActivity::Stamp before) {
    if (!Enabled() || milliseconds < 50.) return;
    static thread_local unsigned reports=0;
    if (reports>=8) return;
    ++reports;
    const auto after=KyoeiDiagnosticActivity::Now();
    const double end_ms=std::chrono::duration<double,std::milli>(
        std::chrono::steady_clock::now().time_since_epoch()).count();
    gz_log("Kyoei GPU wait: requested=%llu next=%llu known_before=%llu known_after=%llu forced_flush=%u milliseconds=%.2f end_ms=%.2f diagnostic=%u",
        (unsigned long long)requested,(unsigned long long)submitted_next,
        (unsigned long long)known_before,(unsigned long long)known_after,
        unsigned(forced_flush),milliseconds,end_ms,
        unsigned(KyoeiDiagnosticActivity::Overlaps(before,after)));
}
class Work {
    unsigned category;
    bool active = false;
    uint64_t transfer_bytes = 0;
    uint64_t transfer_address = 0;
    unsigned transfer_width = 0, transfer_height = 0;
    std::chrono::steady_clock::time_point start;
    KyoeiDiagnosticActivity::Stamp diagnostic_start{};
public:
    explicit Work(unsigned category_, uint64_t bytes=0, unsigned width=0, unsigned height=0, uint64_t address=0)
        : category(category_), transfer_bytes(bytes), transfer_address(address), transfer_width(width), transfer_height(height) {
        if (!Enabled()) return;
        // Millions of draw calls per session: sample before reading clocks or
        // publishing logs. Other categories retain every call.
        if (category == 16 || category == 17) {
            static thread_local std::array<uint64_t,2> calls{};
            if ((calls[category-16]++ % 128) != 0) return;
        }
        active = true;
        start = std::chrono::steady_clock::now();diagnostic_start=KyoeiDiagnosticActivity::Now();
    }
    ~Work() {
        if (!active) return;
        // Deferred publication may run on the scheduler worker. Keep samples
        // per thread rather than racing or mixing overlapping CPU scopes.
        static thread_local std::array<std::array<double, 120>, CategoryCount> samples{};
        static thread_local std::array<size_t, CategoryCount> counts{};
        static thread_local std::array<bool,CategoryCount> diagnostics{};
        static thread_local std::array<uint64_t,CategoryCount> previous_generation{};
        auto& values = samples[category]; auto& count = counts[category];
        auto diagnostic_end=KyoeiDiagnosticActivity::Now();
        diagnostics[category]|=KyoeiDiagnosticActivity::Overlaps(diagnostic_start,diagnostic_end) ||
            (count && previous_generation[category]!=diagnostic_start.generation);
        previous_generation[category]=diagnostic_end.generation;
        const double elapsed=std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start).count();
        values[count++] = elapsed;
        const char* names[] = {"prepare", "present", "linear-downloads",
                               "readback-finish", "readback-publish", "readback-map",
                               "shader-compile", "graphics-pipeline", "compute-pipeline",
                               "scheduler-submit", "scheduler-wait", "queue-lock",
                               "command-batch", "present-pacing", "command-callback",
                               "video-label-wait", "raster-draw", "raster-dispatch", "cp-sync"};
        // Sparse evidence survives partial 120-call windows. This is wall time
        // on the calling thread, including nested waits, not GPU execution time.
        static thread_local std::array<unsigned,CategoryCount> work_stalls{};
        if((category>=12 || category==3 || category==10) && elapsed>=50. && work_stalls[category]<8) {
            ++work_stalls[category];
            const double end_ms=std::chrono::duration<double,std::milli>(
                std::chrono::steady_clock::now().time_since_epoch()).count();
            gz_log("Kyoei work stall: stage=%s milliseconds=%.2f end_ms=%.2f diagnostic=%u",
                names[category],elapsed,end_ms,
                unsigned(KyoeiDiagnosticActivity::Overlaps(diagnostic_start,diagnostic_end)));
            if (transfer_bytes)
                gz_log("Kyoei readback stall: bytes=%llu width=%u height=%u milliseconds=%.2f end_ms=%.2f address=0x%llx",
                    (unsigned long long)transfer_bytes,transfer_width,transfer_height,elapsed,end_ms,(unsigned long long)transfer_address);
        }
        // A cold start may compile fewer than 120 shaders. Preserve evidence of
        // its slow calls, with a strict log cap instead of per-frame traffic.
        static thread_local std::array<unsigned,3> slow_reports{};
        if(category>=6 && category<9 && elapsed>=1000./60. && slow_reports[category-6]++<8)
            gz_log("Kyoei compile stall: stage=%s milliseconds=%.2f diagnostic=%u",names[category],elapsed,
                unsigned(KyoeiDiagnosticActivity::Overlaps(diagnostic_start,diagnostic_end)));
        if (count != values.size()) return;
        double total = 0; for (double value : values) total += value;
        auto sorted = values; std::sort(sorted.begin(), sorted.end());
        gz_log("Kyoei work stats: stage=%s calls=120 mean_ms=%.2f p95_ms=%.2f max_ms=%.2f diagnostic=%u sample_stride=%u",
               names[category], total/count, sorted[113], sorted.back(),unsigned(diagnostics[category]),
               category==16 || category==17 ? 128u : 1u);
        count = 0;diagnostics[category]=false;
    }
};
inline void PresentedAt(std::chrono::steady_clock::time_point now) {
    if (!Enabled()) return;
    using Clock = std::chrono::steady_clock;
    static auto previous = Clock::now();
    static bool started = false;
    static std::array<double, 120> intervals{};
    static size_t count = 0;
    static bool diagnostic_window=false;
    static auto previous_diagnostic=KyoeiDiagnosticActivity::Now();
    static uint64_t previous_faults=0;
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
    // Count actual intervals before rounding. These are presentation deadlines,
    // not simulation ticks; never infer game speed from an FPS counter alone.
    unsigned over_60=0, over_30=0, over_50ms=0, over_100ms=0;
    for (double value : intervals) {
        over_60 += value > 1000.0/60;
        over_30 += value > 1000.0/30;
        over_50ms += value > 50;
        over_100ms += value > 100;
    }
    gz_log("Kyoei frame deadlines: intervals=120 over_60hz=%u over_30hz=%u over_50ms=%u over_100ms=%u diagnostic=%u",
           over_60,over_30,over_50ms,over_100ms,unsigned(diagnostic_window));
    const uint64_t faults = kyoei_tls_fault_count();
    gz_log("Kyoei TLS stats: handled_total=%llu delta=%llu per_present=%.2f",
           (unsigned long long)faults, (unsigned long long)(faults - previous_faults),
           (faults - previous_faults) / 120.0);
    previous_faults = faults;
    count = 0;diagnostic_window=false;
}
inline void Presented() {
    if (Enabled()) PresentedAt(std::chrono::steady_clock::now());
}
}
