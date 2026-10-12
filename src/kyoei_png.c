/* PNG encoding for the native Kyoei runtime. Uses the existing miniz library.
 * API layouts/errors follow the fixed shadPS4 reference.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_png.h"
#include <windows.h>
#define MINIZ_NO_ZLIB_COMPATIBLE_NAMES
#include <miniz.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#define PNG_ADDR ((int32_t)0x80690101)
#define PNG_SIZE ((int32_t)0x80690102)
#define PNG_PARAM ((int32_t)0x80690103)
#define PNG_HANDLE ((int32_t)0x80690104)
#define PNG_OVERFLOW ((int32_t)0x80690110)
#define PNG_FATAL ((int32_t)0x80690120)

typedef struct { uint64_t tag; uint32_t width, filters; } Encoder;
static SRWLOCK encoder_lock = SRWLOCK_INIT;
static Encoder *encoders[32];
_Static_assert(sizeof(KyoeiPngCreateParam) == 16, "PNG create ABI");
_Static_assert(sizeof(KyoeiPngEncodeParam) == 48, "PNG encode ABI");
_Static_assert(offsetof(KyoeiPngEncodeParam, image_pitch) == 32, "PNG pitch ABI");

static ABI int32_t png_query(const KyoeiPngCreateParam *p) {
    if (!p) return PNG_ADDR;
    if (p->attribute || p->max_filter_number > 5) return PNG_PARAM;
    if (!p->max_image_width || p->max_image_width > 1000001) return PNG_SIZE;
    return sizeof(Encoder);
}

static ABI int32_t png_create(const KyoeiPngCreateParam *p, void *memory, uint32_t size, void **handle) {
    int32_t need = png_query(p);
    if (need < 0) return need;
    if (!memory || !handle) return PNG_ADDR;
    if (size < (uint32_t)need) return PNG_SIZE;
    AcquireSRWLockExclusive(&encoder_lock);
    int free_slot = -1;
    for (unsigned i = 0; i < 32; ++i) {
        if (encoders[i] == memory) { ReleaseSRWLockExclusive(&encoder_lock); return PNG_HANDLE; }
        if (!encoders[i] && free_slot < 0) free_slot = i;
    }
    if (free_slot < 0) { ReleaseSRWLockExclusive(&encoder_lock); return PNG_FATAL; }
    Encoder *encoder = memory;
    *encoder = (Encoder){UINT64_C(0x4b594f504e47454e), p->max_image_width, p->max_filter_number};
    encoders[free_slot] = encoder;
    *handle = encoder;
    ReleaseSRWLockExclusive(&encoder_lock);
    gz_log("Kyoei PNG: encoder created, max_width=%u filters=%u", p->max_image_width, p->max_filter_number);
    return 0;
}

static ABI int32_t png_delete(void *handle) {
    AcquireSRWLockExclusive(&encoder_lock);
    for (unsigned i = 0; i < 32; ++i) if (handle && encoders[i] == handle) {
        encoders[i]->tag = 0;
        encoders[i] = NULL;
        ReleaseSRWLockExclusive(&encoder_lock);
        return 0;
    }
    ReleaseSRWLockExclusive(&encoder_lock);
    return PNG_HANDLE;
}

static int paeth(int left, int above, int corner) {
    int p = left + above - corner;
    int a = abs(p - left), b = abs(p - above), c = abs(p - corner);
    return a <= b && a <= c ? left : b <= c ? above : corner;
}
static void big32(uint8_t *out, uint32_t value) {
    out[0] = value >> 24; out[1] = value >> 16; out[2] = value >> 8; out[3] = value;
}
static size_t chunk(uint8_t *out, const char *type, const uint8_t *data, uint32_t size) {
    big32(out, size); memcpy(out + 4, type, 4);
    if (size) memcpy(out + 8, data, size);
    big32(out + 8 + size, (uint32_t)mz_crc32(0, out + 4, (size_t)size + 4));
    return (size_t)size + 12;
}

static ABI int32_t png_encode(void *handle, const KyoeiPngEncodeParam *p, KyoeiPngOutputInfo *info) {
    if (info) *info = (KyoeiPngOutputInfo){0, 0};
    if (!p) return PNG_PARAM;
    if (!p->image_mem_addr || !p->png_mem_addr) return PNG_ADDR;
    if (!p->image_width || !p->image_height || !p->image_mem_size || !p->png_mem_size) return PNG_SIZE;
    if (p->pixel_format > 1 || (p->color_space != 3 && p->color_space != 19) ||
        p->bit_depth != 8 || p->clut_number || p->filter_type > 15) return PNG_PARAM;
    uint64_t input_row = (uint64_t)p->image_width * 4;
    if (p->image_pitch < input_row || (uint64_t)(p->image_height - 1) * p->image_pitch + input_row > p->image_mem_size)
        return PNG_SIZE;
    AcquireSRWLockExclusive(&encoder_lock);
    Encoder *encoder = NULL;
    for (unsigned i = 0; i < 32; ++i) if (handle && encoders[i] == handle) encoder = handle;
    if (!encoder) { ReleaseSRWLockExclusive(&encoder_lock); return PNG_HANDLE; }
    if (p->image_width > encoder->width) { ReleaseSRWLockExclusive(&encoder_lock); return PNG_SIZE; }
    const unsigned channels = p->color_space == 19 ? 4 : 3;
    const size_t row = (size_t)p->image_width * channels;
    const uint64_t raw_size64 = (uint64_t)(row + 1) * p->image_height;
    if (raw_size64 > ULONG_MAX) { ReleaseSRWLockExclusive(&encoder_lock); return PNG_SIZE; }
    const size_t raw_size = (size_t)raw_size64;
    uint8_t *raw = malloc(raw_size), *current = malloc(row), *previous = calloc(1, row), *candidate = malloc(row);
    uint8_t *compressed = NULL;
    int32_t result = PNG_FATAL;
    if (!raw || !current || !previous || !candidate) goto done;
    for (uint32_t y = 0; y < p->image_height; ++y) {
        const uint8_t *input = p->image_mem_addr + (size_t)y * p->image_pitch;
        for (uint32_t x = 0; x < p->image_width; ++x) {
            uint8_t *pixel = current + (size_t)x * channels;
            pixel[0] = input[x * 4 + (p->pixel_format ? 2 : 0)];
            pixel[1] = input[x * 4 + 1];
            pixel[2] = input[x * 4 + (p->pixel_format ? 0 : 2)];
            if (channels == 4) pixel[3] = input[x * 4 + 3];
        }
        uint8_t *output = raw + (size_t)y * (row + 1);
        uint64_t best = UINT64_MAX;
        for (unsigned filter = 0; filter < 5; ++filter) {
            if (filter == 0 ? (p->filter_type != 0 && p->filter_type != 15) : !(p->filter_type & (1u << (filter - 1)))) continue;
            uint64_t score = 0;
            for (size_t x = 0; x < row; ++x) {
                int left = x >= channels ? current[x - channels] : 0;
                int above = previous[x], corner = x >= channels ? previous[x - channels] : 0;
                int predict = filter == 1 ? left : filter == 2 ? above :
                              filter == 3 ? (left + above) / 2 : filter == 4 ? paeth(left, above, corner) : 0;
                candidate[x] = (uint8_t)(current[x] - predict);
                int signed_value = candidate[x] < 128 ? candidate[x] : (int)candidate[x] - 256;
                score += abs(signed_value);
            }
            if (score < best) { best = score; output[0] = filter; memcpy(output + 1, candidate, row); }
        }
        memcpy(previous, current, row);
    }
    mz_ulong compressed_size = mz_compressBound((mz_ulong)raw_size);
    if (compressed_size < raw_size) { result = PNG_SIZE; goto done; }
    compressed = malloc(compressed_size);
    if (!compressed) goto done;
    if (mz_compress2(compressed, &compressed_size, raw, (mz_ulong)raw_size,
                     p->compression_level > 9 ? 9 : p->compression_level) != MZ_OK) goto done;
    const uint64_t total = (uint64_t)compressed_size + 57;
    if (total > p->png_mem_size || total > INT32_MAX) { result = PNG_OVERFLOW; goto done; }
    uint8_t header[13] = {0};
    big32(header, p->image_width); big32(header + 4, p->image_height);
    header[8] = 8; header[9] = channels == 4 ? 6 : 2;
    uint8_t *output = p->png_mem_addr;
    memcpy(output, "\x89PNG\r\n\x1a\n", 8);
    size_t offset = 8;
    offset += chunk(output + offset, "IHDR", header, 13);
    offset += chunk(output + offset, "IDAT", compressed, (uint32_t)compressed_size);
    offset += chunk(output + offset, "IEND", NULL, 0);
    if (info) *info = (KyoeiPngOutputInfo){(uint32_t)offset, p->image_height};
    result = (int32_t)offset;
    gz_log("Kyoei PNG: encoded %ux%u format=%u color=%u bytes=%d", p->image_width, p->image_height, p->pixel_format, p->color_space, result);
done:
    free(compressed); free(candidate); free(previous); free(current); free(raw);
    ReleaseSRWLockExclusive(&encoder_lock);
    return result;
}

uintptr_t kyoei_png_resolve(const char *symbol) {
    if (!strcmp(symbol, "scePngEncQueryMemorySize")) return (uintptr_t)png_query;
    if (!strcmp(symbol, "scePngEncCreate")) return (uintptr_t)png_create;
    if (!strcmp(symbol, "scePngEncEncode")) return (uintptr_t)png_encode;
    if (!strcmp(symbol, "scePngEncDelete")) return (uintptr_t)png_delete;
    return 0;
}
