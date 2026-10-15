/* Calendar ABI, host clock and timezone tests without entering game code.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

typedef struct { uint16_t year, month, day, hour, minute, second; uint32_t us; } Date;
void gz_log(const char *format, ...) { (void)format; }
void gz_stop(int code, const char *format, ...) { (void)format; exit(code); }
#define RESOLVE(name, type) ((type)gz_resolve("libSceRtc", name, 0))
int main(void) {
    int32_t (*get_tick)(const Date *, uint64_t *) = RESOLVE("sceRtcGetTick", int32_t (*)(const Date *, uint64_t *));
    int32_t (*set_tick)(Date *, const uint64_t *) = RESOLVE("sceRtcSetTick", int32_t (*)(Date *, const uint64_t *));
    int32_t (*current)(uint64_t *) = RESOLVE("sceRtcGetCurrentTick", int32_t (*)(uint64_t *));
    int32_t (*format)(char *, const uint64_t *) = RESOLVE("sceRtcFormatRFC3339LocalTime", int32_t (*)(char *, const uint64_t *));
    int32_t (*to_local)(const uint64_t *, uint64_t *) = RESOLVE("sceRtcConvertUtcToLocalTime", int32_t (*)(const uint64_t *, uint64_t *));
    int32_t (*to_utc)(const uint64_t *, uint64_t *) = RESOLVE("sceRtcConvertLocalTimeToUtc", int32_t (*)(const uint64_t *, uint64_t *));
    assert(get_tick && set_tick && current && format && to_local && to_utc);
    assert(!gz_resolve("wrong-library", "sceRtcGetTick", 0));
    assert(!gz_resolve("libSceRtc", "sceRtcGetTick", 1));
    assert(!gz_resolve("libSceRtc", "unsupported", 0));
    assert(current(NULL) == (int32_t)0x80B50002);
    Date epoch = {1970, 1, 1, 0, 0, 0, 0}, roundtrip;
    uint64_t tick, local, utc;
    assert(!get_tick(&epoch, &tick) && tick == UINT64_C(0xdcbffeff2bc000));
    assert(!set_tick(&roundtrip, &tick) && !memcmp(&epoch, &roundtrip, sizeof(epoch)));
    Date leap = {2000, 2, 29, 23, 59, 59, 999999};
    assert(!get_tick(&leap, &tick));
    assert(!set_tick(&roundtrip, &tick) && !memcmp(&leap, &roundtrip, sizeof(leap)));
    leap.year = 1900;
    assert(get_tick(&leap, &tick) == (int32_t)0x80B5000A);
    struct timespec before, after;
    assert(!clock_gettime(CLOCK_REALTIME, &before));
    assert(!current(&tick));
    assert(!clock_gettime(CLOCK_REALTIME, &after));
    uint64_t epoch_tick = UINT64_C(0xdcbffeff2bc000);
    assert(tick >= epoch_tick + (uint64_t)before.tv_sec * 1000000 + (uint64_t)before.tv_nsec / 1000);
    assert(tick <= epoch_tick + (uint64_t)after.tv_sec * 1000000 + (uint64_t)after.tv_nsec / 1000);
    assert(!get_tick(&epoch, &tick));
    char text[64];
    assert(!setenv("TZ", "UTC0", 1)); tzset();
    assert(!format(text, &tick) && !strcmp(text, "1970-01-01T00:00:00.00+00:00"));
    assert(!setenv("TZ", "BRT3", 1)); tzset();
    assert(!format(text, &tick) && !strcmp(text, "1969-12-31T21:00:00.00-03:00"));
    assert(!to_local(&tick, &local) && local == tick - UINT64_C(10800000000));
    assert(!to_utc(&local, &utc) && utc == tick);
    assert(!setenv("TZ", "NPT-5:45", 1)); tzset();
    assert(!format(text, &tick) && !strcmp(text, "1970-01-01T05:45:00.00+05:45"));
    puts("LINUX RTC PASS: calendar, epoch, leap years, realtime, timezone and import filtering");
    return 0;
}
