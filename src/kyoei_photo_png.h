// Photo PNG writer through the runtime's PNG encoder, shared by both GPU cores.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <vector>
extern "C" {
#include "kyoei_png.h"
}
namespace KyoeiPhoto {
inline bool SavePng(const std::filesystem::path& path,const std::vector<uint8_t>& pixels,uint32_t width,uint32_t height,bool bgra) {
    using Create=int32_t (__attribute__((sysv_abi)) *)(const KyoeiPngCreateParam*,void*,uint32_t,void**);
    using Encode=int32_t (__attribute__((sysv_abi)) *)(void*,const KyoeiPngEncodeParam*,KyoeiPngOutputInfo*);
    using Delete=int32_t (__attribute__((sysv_abi)) *)(void*);
    auto create=reinterpret_cast<Create>(kyoei_png_resolve("scePngEncCreate"));
    auto encode=reinterpret_cast<Encode>(kyoei_png_resolve("scePngEncEncode"));
    auto destroy=reinterpret_cast<Delete>(kyoei_png_resolve("scePngEncDelete"));
    if (!width || !height || width>16384 || height>16384 || pixels.size()!=uint64_t(width)*height*4) return false;
    alignas(16) unsigned char memory[64]{};
    KyoeiPngCreateParam options{sizeof(options),0,width,0};void* handle{};
    // Deflate worst-case plus PNG/chunk overhead; bounded above by the size limit.
    std::vector<uint8_t> output(pixels.size()+pixels.size()/100+height+4096);
    if (create(&options,memory,sizeof(memory),&handle)<0) return false;
    KyoeiPngEncodeParam input{};
    input.image_mem_addr=pixels.data();input.png_mem_addr=output.data();
    input.image_mem_size=static_cast<uint32_t>(pixels.size());input.png_mem_size=static_cast<uint32_t>(output.size());
    input.image_width=width;input.image_height=height;input.image_pitch=width*4;
    input.pixel_format=bgra?1:0;input.color_space=3;input.bit_depth=8;input.compression_level=3;
    KyoeiPngOutputInfo result{};const int32_t status=encode(handle,&input,&result);destroy(handle);
    if (status<0) return false;
    std::ofstream file(path,std::ios::binary);file.write(reinterpret_cast<const char*>(output.data()),result.data_size);
    file.close();return !file.fail();
}
}
