// Kyoei: light flip rate for the GoW3 core (KYOEI_FRAME_STATS=1), comparable with the old
// core's "Kyoei frame stats" line. GOW3_FRAME_STATS collects heavy stall diagnostics that
// slow the game itself; this only timestamps flips. Every 120 flips: rate and worst frame.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <algorithm>
#include <chrono>
#include <atomic>
#include <cstdlib>
#include "gow3_toggles.h"
extern "C" void gz_log(const char* format, ...);
extern "C" void kyoei_thread_sampler_slow_flip(double interval_ms);

namespace KyoeiGow3FlipRate {
// Submit-to-flip latency: the guest's SubmitAndFlip calls, matched in order with the flips.
inline std::chrono::steady_clock::time_point submits[8];
inline std::atomic<unsigned> submitted, flipped;
inline void Submit() {
    submits[submitted.fetch_add(1) % 8] = std::chrono::steady_clock::now();
}
// Stage marks for the oldest unflipped frame: 0 = flip reached the recording thread,
// 1 = its request is queued (after PrepareFrame).
inline std::chrono::steady_clock::time_point stage_at[2][8];
inline std::atomic<unsigned> staged[2];
inline bool Stage(int stage) {
    stage_at[stage][staged[stage].fetch_add(1) % 8] = std::chrono::steady_clock::now();
    return true;
}
inline void Flip() {
    static const bool enabled = [] { const char* v = std::getenv("KYOEI_FRAME_STATS"); return v && v[0] == '1'; }();
    if (!enabled) return;
    using Clock = std::chrono::steady_clock;
    static Clock::time_point start = Clock::now(), last = start;
    static unsigned flips;
    static std::uint64_t last_draws = Gow3Stats::draws.load(std::memory_order_relaxed);
    static double worst_ms, latency_sum, latency_max, part_sum[3];
    const auto now = Clock::now();
    if (flipped < submitted) {
        const unsigned index = flipped.fetch_add(1) % 8;
        const auto ms = [](Clock::time_point a, Clock::time_point b) {
            return std::chrono::duration<double, std::milli>(b - a).count();
        };
        const double latency = ms(submits[index], now);
        latency_sum += latency;
        latency_max = std::max(latency_max, latency);
        part_sum[0] += ms(submits[index], stage_at[0][index]);
        part_sum[1] += ms(stage_at[0][index], stage_at[1][index]);
        part_sum[2] += ms(stage_at[1][index], now);
    }
    const double interval_ms = std::chrono::duration<double, std::milli>(now - last).count();
    kyoei_thread_sampler_slow_flip(interval_ms);
    worst_ms = std::max(worst_ms, interval_ms);
    last = now;
    if (++flips < 120) return;
    const double seconds = std::chrono::duration<double>(now - start).count();
    const auto draws = Gow3Stats::draws.load(std::memory_order_relaxed);
    gz_log("Kyoei frame stats: intervals=120 fps=%.2f mean_ms=%.2f max_ms=%.2f submit_to_flip_ms=%.1f/%.1f "
           "to_recorder=%.1f prepare=%.1f to_flip=%.1f draws_per_frame=%.1f",
           flips / seconds, seconds * 1000 / flips, worst_ms, latency_sum / flips, latency_max,
           part_sum[0] / flips, part_sum[1] / flips, part_sum[2] / flips,
           double(draws - last_draws) / flips);
    last_draws = draws;
    latency_sum = latency_max = part_sum[0] = part_sum[1] = part_sum[2] = 0;
    start = now;
    flips = 0;
    worst_ms = 0;
}
} // namespace KyoeiGow3FlipRate
