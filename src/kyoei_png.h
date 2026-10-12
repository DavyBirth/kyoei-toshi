/* PS4 PNG encoder ABI, from the fixed shadPS4 headers. */
#pragma once
#include <stdint.h>
#include <stddef.h>

typedef struct {
    uint32_t this_size, attribute, max_image_width, max_filter_number;
} KyoeiPngCreateParam;
typedef struct {
    const uint8_t *image_mem_addr;
    uint8_t *png_mem_addr;
    uint32_t image_mem_size, png_mem_size, image_width, image_height, image_pitch;
    uint16_t pixel_format, color_space, bit_depth, clut_number, filter_type, compression_level;
} KyoeiPngEncodeParam;
typedef struct { uint32_t data_size, processed_height; } KyoeiPngOutputInfo;
uintptr_t kyoei_png_resolve(const char *symbol);
