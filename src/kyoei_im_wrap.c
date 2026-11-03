/* Intermission (KYOEI NEWS) text for Western languages.
 * CUSA03562 lays these out by character count, sized for full-width Japanese:
 *  - 0xac6210 split(obj, text, chars_per_line, max_lines) cuts a line every N
 *    characters and at "\n"; if the text needs more lines, the last kept line
 *    is cut with "…" when it is full;
 *  - 0xac78c0 shorten(label, text, max_chars) keeps max-1 characters plus "…".
 * Latin letters are about half a cell wide, so counting characters wasted half
 * of every line and split words. Here the limit is read as N full-width cells
 * (native advances from the KHF1 cache), text is broken only between words
 * (or after a hyphen), and the originals are called with a count they will
 * never reach. Japanese is untouched (only en/pt-BR install the hooks).
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "kyoei_im_wrap.h"
#include <string.h>
#include <stdint.h>
#include <stdio.h>
#ifndef KYOEI_IM_WRAP_TEST
#include "gz.h"
#include "kyoei_hud_trace.h"
#include <windows.h>
#include <limits.h>
#include <stdlib.h>
#endif

static float advance[95];          /* ASCII 32..126, in full-width cells */
static int metrics_ready;

void kyoei_im_set_metrics(const float *equivalent) {
    memcpy(advance, equivalent, sizeof advance); metrics_ready = 1;
}

/* Width of bytes [s, s+n) in full-width cells. */
static float width(const char *s, size_t n) {
    float w = 0;
    for (size_t i = 0; i < n;) {
        unsigned char c = (unsigned char)s[i];
        if (c >= 32 && c < 127) { w += metrics_ready ? advance[c - 32] : .55f; ++i; }
        else if (c >= 0xc0 && c < 0xe0) { w += .6f; i += 2; }          /* accented Latin */
        else if (c >= 0xe0 && c < 0xf0) { w += 1.f; i += 3; }          /* "…", CJK: one cell */
        else { w += 1.f; ++i; }
    }
    return w;
}

static const char *ELLIPSIS = "\xe2\x80\xa6";

size_t kyoei_im_rewrap(const char *text, float cells, unsigned max_lines, char *out, size_t cap) {
    size_t o = 0, n = strlen(text), lines = 1;
    float line = 0;
    if (cells < 2 || n * 2 + 8 > cap) return 0;
    for (size_t i = 0; i < n;) {
        if (text[i] == '\n') {
            if (max_lines && lines == max_lines) goto cut;
            out[o++] = '\n'; line = 0; ++lines; ++i; continue;
        }
        if (text[i] == ' ' && line == 0) { ++i; continue; }
        size_t end = i; while (end < n && text[end] != ' ' && text[end] != '\n') ++end;
        float w = width(text + i, end - i), space = line ? width(" ", 1) : 0;
        if (line && line + space + w > cells) {
            if (max_lines && lines == max_lines) goto cut;
            out[o++] = '\n'; line = 0; ++lines; space = 0;
        }
        if (!line && w > cells) {                       /* a word wider than the line */
            size_t hy = 0;                              /* break after a hyphen if one fits */
            for (size_t j = i; j < end; ++j) if (text[j] == '-' && width(text + i, j + 1 - i) <= cells) hy = j + 1;
            if (hy) {
                memcpy(out + o, text + i, hy - i); o += hy - i;
                if (max_lines && lines == max_lines) goto cut;
                out[o++] = '\n'; ++lines; i = hy; continue;
            }
        }
        if (space > 0) out[o++] = ' ';
        memcpy(out + o, text + i, end - i); o += end - i; line += space + w;
        i = end; while (i < n && text[i] == ' ') ++i;
    }
    out[o] = 0;
    return o;
cut:                                                    /* more text than lines: end with "…" */
    while (o && out[o - 1] == ' ') --o;
    {
        size_t start = o; while (start && out[start - 1] != '\n') --start;
        while (o > start && width(out + start, o - start) + 1.f > cells) {
            --o; while (o > start && ((unsigned char)out[o] & 0xc0) == 0x80) --o;
        }
        while (o > start && strchr(" ,.:;-", out[o - 1])) --o;
    }
    memcpy(out + o, ELLIPSIS, 4);
    return o + 3;
}

size_t kyoei_im_shorten(const char *text, float cells, char *out, size_t cap) {
    size_t n = strlen(text);
    if (n + 4 > cap) return 0;
    if (width(text, n) <= cells) { memcpy(out, text, n + 1); return n; }   /* fits as is */
    size_t last_space = 0;
    for (size_t i = 1; i < n; ++i) {
        if (text[i] != ' ') continue;
        if (width(text, i) + 1.f > cells) break;
        last_space = i;
    }
    if (!last_space) return 0;                          /* one long word: native cut */
    size_t end = last_space; while (end && strchr(" ,.:;-", text[end - 1])) --end;
    memcpy(out, text, end); memcpy(out + end, ELLIPSIS, 4);
    return end + 3;
}

#ifndef KYOEI_IM_WRAP_TEST
typedef uintptr_t (ABI *Split)(void *, const char *, unsigned, unsigned);
typedef uintptr_t (ABI *Shorten)(void *, const char *, unsigned);
static Split original_split;
static Shorten original_shorten;
static __declspec(thread) char split_buffer[16384], short_buffer[2048];

static ABI uintptr_t split_words(void *object, const char *text, unsigned per_line, unsigned max_lines) {
    if (text && per_line >= 4 && kyoei_im_rewrap(text, (float)per_line, max_lines, split_buffer, sizeof split_buffer))
        return original_split(object, split_buffer, 4096, max_lines);
    return original_split(object, text, per_line, max_lines);
}
static ABI uintptr_t shorten_words(void *label, const char *text, unsigned max_chars) {
    if (text && max_chars >= 3 && kyoei_im_shorten(text, (float)max_chars, short_buffer, sizeof short_buffer))
        return original_shorten(label, short_buffer, 4096);
    return original_shorten(label, text, max_chars);
}

static uint64_t hash(const unsigned char *p, size_t n) {
    uint64_t h = UINT64_C(14695981039346656037);
    for (size_t i = 0; i < n; ++i) h = (h ^ p[i]) * UINT64_C(1099511628211);
    return h;
}
static int hook(unsigned char *image, size_t entry, uint64_t expected, unsigned char *relay) {
    unsigned char *code = image + entry;
    if (hash(code, 0x80) != expected) return 0;
    int64_t distance = (int64_t)(uintptr_t)(relay + 32) - (int64_t)(uintptr_t)(code + 5);
    DWORD prior, ignored;
    if (distance < INT32_MIN || distance > INT32_MAX || !VirtualProtect(code, 6, PAGE_EXECUTE_READWRITE, &prior)) return 0;
    int32_t relative = (int32_t)distance;
    code[0] = 0xe9; memcpy(code + 1, &relative, 4); code[5] = 0x90;
    if (!VirtualProtect(code, 6, prior, &ignored)) gz_stop(21, "STOP: cannot restore intermission text code protection");
    FlushInstructionCache(GetCurrentProcess(), code, 6);
    return 1;
}

static void load_metrics(void) {
    const char *path = getenv("KYOEI_HUD_METRICS");
    FILE *file = path ? fopen(path, "rb") : NULL;
    if (!file) return;
    unsigned char header[8]; float values[95];
    int ok = fread(header, 1, 8, file) == 8 && !memcmp(header, "KHF1\x5f\0\0\0", 8) &&
             fread(values, 4, 95, file) == 95;
    fclose(file);
    for (unsigned i = 0; ok && i < 95; ++i) ok = values[i] > 0 && values[i] <= 4;
    if (ok) kyoei_im_set_metrics(values);
}

void kyoei_im_wrap_configure(void *image_base, uint64_t size) {
    const char *language = getenv("KYOEI_LANGUAGE");
    if (!language || (strcmp(language, "en") && strcmp(language, "pt-BR"))) return;
    const char *setting = getenv("KYOEI_NEWS_WRAP");
    if (setting && !strcmp(setting, "0")) return;
    unsigned char *image = image_base;
    if (!image || size < 0xac7a10) return;
    load_metrics();
    unsigned char *relay = NULL;
    for (unsigned i = 0; i < 16 && !relay; ++i)
        relay = VirtualAlloc(image + 0x5400000 + i * 0x10000, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!relay) { gz_log("Kyoei news text: no relay; native layout kept"); return; }
    original_split = (Split)relay; original_shorten = (Shorten)(relay + 128);
    kyoei_hud_trace_relay(relay, image + 0xac6210 + 6, split_words);
    kyoei_hud_trace_relay(relay + 128, image + 0xac78c0 + 6, shorten_words);
    DWORD prior;
    if (!VirtualProtect(relay, 4096, PAGE_EXECUTE_READ, &prior)) { gz_log("Kyoei news text: relay not executable"); return; }
    FlushInstructionCache(GetCurrentProcess(), relay, 256);
    int split_ok = hook(image, 0xac6210, UINT64_C(0xdfea76622a7ee823), relay);
    int short_ok = hook(image, 0xac78c0, UINT64_C(0x194fca4c175ee645), relay + 128);
    gz_log("Kyoei news text: line split %s, title cut %s, metrics %s", split_ok ? "by width/words" : "native",
           short_ok ? "by width/words" : "native", metrics_ready ? "native" : "estimated");
}
#endif
