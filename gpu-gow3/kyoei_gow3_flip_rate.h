// Kyoei: light flip rate for the GoW3 core (KYOEI_FRAME_STATS=1), comparable with the old
// core's "Kyoei frame stats" line. GOW3_FRAME_STATS collects heavy stall diagnostics that
// slow the game itself; this only timestamps flips. Every 120 flips: rate and worst frame.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <algorithm>
#include <chrono>
#include <cstdlib>
extern "C" void gz_log(const char* format, ...);

namespace KyoeiGow3FlipRate {
inline void Flip() {
    static const bool enabled = [] { const char* v = std::getenv("KYOEI_FRAME_STATS"); return v && v[0] == '1'; }();
    if (!enabled) return;
    using Clock = std::chrono::steady_clock;
    static Clock::time_point start = Clock::now(), last = start;
    static unsigned flips;
    static double worst_ms;
    const auto now = Clock::now();
    worst_ms = std::max(worst_ms, std::chrono::duration<double, std::milli>(now - last).count());
    last = now;
    if (++flips < 120) return;
    const double seconds = std::chrono::duration<double>(now - start).count();
    gz_log("Kyoei frame stats: intervals=120 fps=%.2f mean_ms=%.2f max_ms=%.2f", flips / seconds,
           seconds * 1000 / flips, worst_ms);
    start = now;
    flips = 0;
    worst_ms = 0;
}
} // namespace KyoeiGow3FlipRate
