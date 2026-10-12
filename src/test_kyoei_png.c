#include "gz.h"
#include "kyoei_png.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void gz_log(const char *format, ...) { (void)format; }
typedef int32_t (ABI *Query)(const KyoeiPngCreateParam *);
typedef int32_t (ABI *Create)(const KyoeiPngCreateParam *, void *, uint32_t, void **);
typedef int32_t (ABI *Encode)(void *, const KyoeiPngEncodeParam *, KyoeiPngOutputInfo *);
typedef int32_t (ABI *Delete)(void *);

int main(int argc, char **argv) {
    if (argc != 2) return 1;
    Query query = (Query)kyoei_png_resolve("scePngEncQueryMemorySize");
    Create create = (Create)kyoei_png_resolve("scePngEncCreate");
    Encode encode = (Encode)kyoei_png_resolve("scePngEncEncode");
    Delete destroy = (Delete)kyoei_png_resolve("scePngEncDelete");
    KyoeiPngCreateParam config = {16, 0, 3, 5};
    int32_t size = query(&config);
    if (size <= 0 || query(NULL) >= 0) return 2;
    void *memory = malloc(size), *handle = NULL;
    if (!memory || create(&config, memory, size - 1, &handle) >= 0 || handle) return 3;
    if (create(&config, memory, size, &handle) || !handle) return 4;
    uint8_t input[32] = {255,0,0,10, 0,255,0,20, 0,0,255,30, 222,222,222,222,
                         10,20,30,40, 50,60,70,80, 90,100,110,120, 222,222,222,222};
    uint8_t original[32], output[1024];
    memcpy(original, input, sizeof(input));
    KyoeiPngEncodeParam params = {input, output, 28, sizeof(output), 3, 2, 16, 0, 19, 8, 0, 0, 6};
    KyoeiPngOutputInfo info;
    for (unsigned format = 0; format < 2; ++format) {
        for (unsigned alpha = 0; alpha < 2; ++alpha) {
            params.pixel_format = format; params.color_space = alpha ? 19 : 3;
            for (unsigned filter = 0; filter < 16; ++filter) {
                params.filter_type = filter;
                memset(output, 0xcc, sizeof(output));
                int32_t written = encode(handle, &params, &info);
                if (written <= 0 || info.data_size != (uint32_t)written || info.processed_height != 2 ||
                    output[written] != 0xcc || memcmp(input, original, sizeof(input))) return 5;
                char path[1024];
                snprintf(path, sizeof(path), "%s/png-%u-%u-%u.png", argv[1], format, alpha, filter);
                FILE *file = fopen(path, "wb");
                if (!file || fwrite(output, 1, written, file) != (size_t)written) return 6;
                fclose(file);
            }
        }
    }
    memset(output, 0xcc, sizeof(output));
    params.png_mem_size = 10;
    if (encode(handle, &params, &info) != (int32_t)0x80690110 || info.data_size || info.processed_height || output[0] != 0xcc) return 7;
    params.png_mem_size = sizeof(output); params.image_mem_size = 27;
    if (encode(handle, &params, &info) != (int32_t)0x80690102) return 8;
    params.image_mem_size = 28; params.pixel_format = 2;
    if (encode(handle, &params, &info) != (int32_t)0x80690103) return 9;
    params.pixel_format = 0;
    if (destroy(handle) || destroy(handle) != (int32_t)0x80690104 || encode(handle, &params, &info) != (int32_t)0x80690104) return 10;
    free(memory);
    puts("PNG ABI PASS: 64 encodes, padded input, guarded output, overflow/errors and handle lifecycle");
    return 0;
}
