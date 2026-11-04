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

static const char *ELLIPSIS = "...";   /* Latin dots; the eboot's own "…" is mapped to "..." too */

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

/* Mission list preview (0xb375f0): texts over 18 characters keep 17 plus "…",
 * via the char->byte helper 0x31ce70 called at 0xb37ad7. This helper keeps whole
 * words that fit the same 17 full-width cells; one long word keeps the native cut. */
static ABI unsigned mission_preview_cut(void *string, unsigned chars) {
    const char *t = *(const char **)string; unsigned len = *(unsigned *)((char *)string + 8);
    size_t best = 0;
    for (size_t i = 1; t && i < len; ++i) {
        if (t[i] != ' ' && t[i] != '\n') continue;
        if (width(t, i) > (float)chars) break;
        best = i;
    }
    while (best && strchr(" ,.:;-\n", t[best - 1])) --best;
    if (best) return (unsigned)best;
    unsigned at = 0;
    for (unsigned n = 0; t && n < chars && at < len; ++n) {
        unsigned char c = (unsigned char)t[at] & 0xf0;
        at += c == 0xe0 ? 3 : (c == 0xc0 || c == 0xd0) ? 2 : 1;
    }
    return at;
}
static void hook_mission_preview(unsigned char *image) {
    static const unsigned char call[5] = {0xe8, 0x94, 0x53, 0x7e, 0xff}, count[5] = {0xbe, 0x11, 0, 0, 0};
    unsigned char *site = image + 0xb37ad7;
    if (memcmp(site, call, 5) || memcmp(site - 8, count, 5)) { gz_log("Kyoei mission preview: signature differs (%02x %02x %02x %02x %02x | %02x %02x); native cut kept",site[0],site[1],site[2],site[3],site[4],site[-5],site[-4]); return; }
    unsigned char *jump = NULL;
    for (unsigned i = 0; i < 16 && !jump; ++i)
        jump = VirtualAlloc(image + 0x5500000 + i * 0x10000, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!jump) return;
    uint64_t target = (uint64_t)(uintptr_t)mission_preview_cut;
    jump[0] = 0x48; jump[1] = 0xb8; memcpy(jump + 2, &target, 8); jump[10] = 0xff; jump[11] = 0xe0;
    DWORD prior, ignored;
    int64_t distance = (int64_t)(uintptr_t)jump - (int64_t)(uintptr_t)(site + 5);
    if (distance < INT32_MIN || distance > INT32_MAX || !VirtualProtect(jump, 4096, PAGE_EXECUTE_READ, &prior) ||
        !VirtualProtect(site, 5, PAGE_EXECUTE_READWRITE, &prior)) return;
    int32_t relative = (int32_t)distance; memcpy(site + 1, &relative, 4);
    VirtualProtect(site, 5, prior, &ignored);
    FlushInstructionCache(GetCurrentProcess(), site, 5);
    gz_log("Kyoei mission preview: cut by width/words");
}

static uint64_t hash(const unsigned char *p, size_t n) {
    uint64_t h = UINT64_C(14695981039346656037);
    for (size_t i = 0; i < n; ++i) h = (h ^ p[i]) * UINT64_C(1099511628211);
    return h;
}
/* 12-character label cutters 0xa6d9a0/0xb034c0 (NPC preview balloons, e.g. the
 * florist): count = characters, cut when >= 13 to 11 + "...". Latin letters are
 * about half a cell, so return the width in full-width cells instead: Western
 * lines are cut only when really wider, and the balloon (sized from the count)
 * fits the text. Japanese counts are unchanged (one cell per character). */
static ABI unsigned label_chars(const char *text) {
    if (!text) return 0;
    float cells = width(text, strlen(text));
    unsigned n = (unsigned)(cells + .999f);
    if (n >= 13) {
        static uint64_t seen[64]; static unsigned count; uint64_t h = hash((const unsigned char *)text, strlen(text));
        unsigned i = 0; while (i < count && seen[i] != h) ++i;
        if (i == count && count < 64) { seen[count++] = h; gz_log("Kyoei label cut (12 cells): %s", text); }
    }
    return n;
}
/* Balloon cut: lines up to 16 cells stay whole (thresholds 13 -> 17 cells, box
 * after a cut 12 -> 16); longer lines keep the whole words within 15 cells,
 * via the substring call that precedes the native "...". */
typedef void *(ABI *Substring)(void *, void *, unsigned, unsigned);
static Substring original_substring;
static ABI void *balloon_substring(void *out, void *string, unsigned start, unsigned bytes) {
    const char *t = *(const char **)string; unsigned len = *(unsigned *)((char *)string + 8);
    size_t best = 0;
    for (size_t i = 1; t && i < len; ++i) {
        if (t[i] != ' ' && t[i] != '\n') continue;
        if (width(t, i) > 15.f) break;
        best = i;
    }
    while (best && strchr(" ,.:;-\n", t[best - 1])) --best;
    return original_substring(out, string, start, best ? (unsigned)best : bytes);
}
static int patch_bytes(unsigned char *site, const void *expect, const void *value, size_t n) {
    DWORD prior, ignored;
    if (memcmp(site, expect, n) || !VirtualProtect(site, n, PAGE_EXECUTE_READWRITE, &prior)) return 0;
    memcpy(site, value, n); VirtualProtect(site, n, prior, &ignored); FlushInstructionCache(GetCurrentProcess(), site, n);
    return 1;
}
static void hook_balloon_words(unsigned char *image, unsigned char *jump) {
    original_substring = (Substring)(image + 0x31cf70);
    uint64_t target = (uint64_t)(uintptr_t)balloon_substring;
    jump[0] = 0x48; jump[1] = 0xb8; memcpy(jump + 2, &target, 8); jump[10] = 0xff; jump[11] = 0xe0;
    int ok = patch_bytes(image + 0xa6d9fa, "\x41\x83\xff\x0d", "\x41\x83\xff\x11", 4) &
             patch_bytes(image + 0xa6da72, "\x41\xbf\x0c\x00\x00\x00", "\x41\xbf\x10\x00\x00\x00", 6) &
             patch_bytes(image + 0xb03517, "\x83\xf8\x0d", "\x83\xf8\x11", 3);
    static const size_t sites[2] = {0xa6da40, 0xb03560};
    static const unsigned char calls[2][5] = {{0xe8, 0x2b, 0xf5, 0x8a, 0xff}, {0xe8, 0x0b, 0x9a, 0x81, 0xff}};
    for (int k = 0; k < 2; ++k) {
        int64_t distance = (int64_t)(uintptr_t)jump - (int64_t)(uintptr_t)(image + sites[k] + 5);
        unsigned char call[5] = {0xe8}; int32_t rel = (int32_t)distance; memcpy(call + 1, &rel, 4);
        ok &= distance >= INT32_MIN && distance <= INT32_MAX && patch_bytes(image + sites[k], calls[k], call, 5);
    }
    gz_log("Kyoei NPC balloon: whole words, 16 cells %s", ok ? "enabled" : "partly native");
}

static void hook_label_report(unsigned char *image) {
    static const size_t sites[2] = {0xa6d9f2, 0xb03512};
    static const unsigned char calls[2][5] = {{0xe8, 0x99, 0xf3, 0x8a, 0xff}, {0xe8, 0x79, 0x98, 0x81, 0xff}};
    unsigned char *jump = NULL;
    for (unsigned i = 0; i < 16 && !jump; ++i)
        jump = VirtualAlloc(image + 0x5600000 + i * 0x10000, 4096, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!jump) { gz_log("Kyoei NPC balloon: no relay"); return; }
    uint64_t target = (uint64_t)(uintptr_t)label_chars;
    jump[0] = 0x48; jump[1] = 0xb8; memcpy(jump + 2, &target, 8); jump[10] = 0xff; jump[11] = 0xe0;
    DWORD prior, ignored;
    hook_balloon_words(image, jump + 64);
    if (!VirtualProtect(jump, 4096, PAGE_EXECUTE_READ, &prior)) return;
    for (int k = 0; k < 2; ++k) {
        unsigned char *site = image + sites[k];
        int64_t distance = (int64_t)(uintptr_t)jump - (int64_t)(uintptr_t)(site + 5);
        if (memcmp(site, calls[k], 5) || distance < INT32_MIN || distance > INT32_MAX) { gz_log("Kyoei NPC balloon: label %d signature %02x %02x %02x %02x %02x", k, site[0], site[1], site[2], site[3], site[4]); continue; }
        if (!VirtualProtect(site, 5, PAGE_EXECUTE_READWRITE, &prior)) continue;
        int32_t relative = (int32_t)distance; memcpy(site + 1, &relative, 4);
        VirtualProtect(site, 5, prior, &ignored); FlushInstructionCache(GetCurrentProcess(), site, 5);
        gz_log("Kyoei NPC balloon: label %d measured by width", k);
    }
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
    hook_mission_preview(image);
    hook_label_report(image);
    gz_log("Kyoei news text: line split %s, title cut %s, metrics %s", split_ok ? "by width/words" : "native",
           short_ok ? "by width/words" : "native", metrics_ready ? "native" : "estimated");
}
#endif
