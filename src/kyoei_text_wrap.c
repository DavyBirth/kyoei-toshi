/* Word wrap for Western text in the game's bitmap-font line measurer.
 * CUSA03562 0x2e2b10 measures one line from params->start and, when the next
 * glyph would exceed the width, ends the line at that glyph (correct for
 * Japanese, mid-word for English). After the original call, a mid-word
 * overflow is re-measured up to the last space so the line ends between
 * words; a line that ends on a space starts the next line after it. Lines
 * without spaces (Japanese, long tokens) keep the original result.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "kyoei_text_wrap.h"
#include <string.h>
#ifndef KYOEI_TEXT_WRAP_TEST
#include "gz.h"
#include "kyoei_hud_trace.h"
#include <windows.h>
#include <limits.h>
#include <stdlib.h>
#endif

#ifdef KYOEI_TEXT_WRAP_TEST
#define ABI
#endif
typedef struct { uint32_t start; } MeasureParams;          /* +0: first byte of the line */
typedef struct {
    uint32_t end; uint8_t newline, reserved[3];
    uint32_t glyphs, overflow;
    int32_t height, line_height, align;
    uint32_t style;
} MeasureOut; /* Verified stores at 0x2e31d3..0x2e3256; overflow is cached by the caller. */
typedef float (ABI *Measure)(void *, const char *, uint32_t, uint32_t, void *, const MeasureParams *, MeasureOut *);
static Measure original;

/* Last space usable as a break in [start,end), outside [..] tags. */
static uint32_t last_break(const char *text, uint32_t start, uint32_t end) {
    uint32_t found = UINT32_MAX; int tag = 0;
    for (uint32_t i = start; i < end; ++i) {
        char c = text[i];
        if (c == '[') tag = 1;
        else if (c == ']') tag = 0;
        else if (c == ' ' && !tag && i > start) found = i;
    }
    return found;
}

float kyoei_text_wrap_measure(void *font, const char *text, uint32_t index, uint32_t length, void *state,
                              const MeasureParams *params, MeasureOut *out) {
    float width = original(font, text, index, length, state, params, out);
#ifndef KYOEI_TEXT_WRAP_TEST
    static volatile LONG reports;
    if (getenv("KYOEI_WRAP_TRACE") && text && params && out && length>40 && out->end<length &&
        InterlockedIncrement(&reports)<=16) {
        gz_log("Kyoei wrap probe: length=%u start=%u end=%u explicit=%u index=%u",length,params->start,out->end,out->newline,index);
    }
#endif
    if (!text || !params || !out || out->newline || out->end >= length || out->end <= params->start) return width;
    uint32_t end = out->end;
    if (text[end] == ' ') { out->end = end + 1; return width; }
    if (text[end - 1] == ' ') return width;                          /* next word starts here */
    uint32_t space = last_break(text, params->start, end);
    if (space == UINT32_MAX) return width;                           /* no space: keep native wrap */
    /* Measure only up to the space; the following line starts after it. */
    float shorter = original(font, text, index, space, state, params, out);
    if (out->end == space) { out->end = space + 1; out->newline = 0; return shorter; }
    /* Unexpected: restore the original measurement. */
    return original(font, text, index, length, state, params, out);
}

#ifdef KYOEI_TEXT_WRAP_TEST
void kyoei_text_wrap_test_original(void *measure) { original = (Measure)measure; }
#else
static ABI float wrap_measure(void *font, const char *text, uint32_t index, uint32_t length, void *state,
                              const MeasureParams *params, MeasureOut *out) {
    return kyoei_text_wrap_measure(font, text, index, length, state, params, out);
}
static uint64_t code_hash(const unsigned char *bytes, size_t count) {
    uint64_t hash = UINT64_C(14695981039346656037);
    for (size_t i = 0; i < count; ++i) hash = (hash ^ bytes[i]) * UINT64_C(1099511628211);
    return hash;
}
void kyoei_text_wrap_configure(void *image, uint64_t size) {
    extern void kyoei_ui_text_configure(void *,uint64_t);
    kyoei_ui_text_configure(image,size);
    const char *language = getenv("KYOEI_LANGUAGE");
    if (!language || (strcmp(language, "en") && strcmp(language, "pt-BR"))) return;
    const char *setting = getenv("KYOEI_WORD_WRAP");
    if (setting && !strcmp(setting, "0")) return;
    const size_t entry = 0x2e2b10, length = 0x780;
    if (!image || size < entry + length) { gz_log("Kyoei wrap: image too small; native wrap kept"); return; }
    unsigned char *code = (unsigned char *)image + entry;
    if (code_hash(code, length) != UINT64_C(0xa142832280f2669b)) { gz_log("Kyoei wrap: routine signature differs; native wrap kept"); return; }
    unsigned char *relay = NULL;
    for (unsigned i = 0; i < 16 && !relay; ++i)
        relay = VirtualAlloc((unsigned char *)image + 0x5200000 + i * 0x10000, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!relay) { gz_log("Kyoei wrap: no relay allocation; native wrap kept"); return; }
    kyoei_hud_trace_relay(relay, code + 6, wrap_measure);
    int64_t distance = (int64_t)(uintptr_t)(relay + 32) - (int64_t)(uintptr_t)(code + 5);
    DWORD prior;
    if (distance < INT32_MIN || distance > INT32_MAX || !VirtualProtect(relay, 4096, PAGE_EXECUTE_READ, &prior) ||
        !VirtualProtect(code, 6, PAGE_EXECUTE_READWRITE, &prior)) {
        VirtualFree(relay, 0, MEM_RELEASE); gz_log("Kyoei wrap: relay unavailable; native wrap kept"); return;
    }
    original = (Measure)relay;
    int32_t relative = (int32_t)distance;
    code[0] = 0xe9; memcpy(code + 1, &relative, 4); code[5] = 0x90;
    DWORD ignored;
    if (!VirtualProtect(code, 6, prior, &ignored)) gz_stop(21, "STOP: cannot restore wrap code protection");
    FlushInstructionCache(GetCurrentProcess(), relay, 46);
    FlushInstructionCache(GetCurrentProcess(), code, 6);
    gz_log("Kyoei wrap: line measurer verified; words kept whole for %s", language);
}
#endif
