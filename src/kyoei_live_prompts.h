/* Select icon variants at upload time; guest memory/archive stay intact.
 * Microtile address calculation adapted from shadPS4 tiling.comp,
 * SPDX-FileCopyrightText: Copyright 2025 shadPS4 Emulator Project
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <algorithm>
#include <bit>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <vector>
#include <unordered_map>
#include "kyoei_input_mode.h"
#include "kyoei_prompt_state.h"
extern "C" int gz_memory_copy(uintptr_t, void *, uint64_t, int);
extern "C" void gz_log(const char *, ...);
namespace KyoeiPrompts {
struct Pair {
    uint32_t width{}, height{}, levels{}, storage{}, channels{}, kind{}, bytes{};
    std::vector<unsigned char> controller, keyboard, american;
    const std::vector<unsigned char> &Controller() const {
        return american.empty() ? controller : american;
    }
};
inline std::vector<Pair> pairs;
inline bool initialized{};
inline void Initialize() {
    if (initialized) return;
    initialized = true;
    const char *path = std::getenv("KYOEI_PROMPT_PACK");
    if (!path || !*path) return;
    FILE *file = std::fopen(path, "rb");
    if (!file) return;
    char magic[8]; uint32_t count{};
    bool ok = std::fread(magic, 1, 8, file) == 8;
    const bool american = ok && !std::memcmp(magic, "KYPROMP2", 8);
    ok = ok && (american || !std::memcmp(magic, "KYPROMP1", 8)) &&
              std::fread(&count, 4, 1, file) == 1 && count <= 128;
    size_t total{};
    for (uint32_t i = 0; ok && i < count; ++i) {
        uint32_t fields[7];
        if (std::fread(fields, 4, 7, file) != 7) { ok = false; break; }
        Pair p{fields[0], fields[1], fields[2], fields[3], fields[4], fields[5], fields[6], {}, {}};
        total += size_t(p.bytes) * (american ? 3 : 2);
        if (!p.width || !p.height || p.width > 4096 || p.height > 4096 || !p.levels ||
            p.levels > 14 || !p.bytes || total > 32 * 1024 * 1024 || (p.storage != 1 && p.storage != 13)) {
            ok = false; break;
        }
        p.controller.resize(p.bytes); p.keyboard.resize(p.bytes);
        ok = std::fread(p.controller.data(), 1, p.bytes, file) == p.bytes &&
             std::fread(p.keyboard.data(), 1, p.bytes, file) == p.bytes;
        if (ok && american) {
            p.american.resize(p.bytes);
            ok = std::fread(p.american.data(), 1, p.bytes, file) == p.bytes;
        }
        if (ok) pairs.push_back(std::move(p));
    }
    ok = ok && std::fgetc(file) == EOF;
    std::fclose(file);
    if (!ok) pairs.clear();
    if (ok && !american) gz_log("Kyoei prompts: legacy artwork; rebuild pack for physical American buttons");
    gz_log("Kyoei prompts: %zu paired textures; live input selection", pairs.size());
}
struct State { int pair{-1}; unsigned mode{~0u}; };
inline std::unordered_map<uint64_t, State> states;
template<class Info> bool Dimensions(const Pair &p,const Info &info) {
    return (p.width==info.size.width || std::bit_ceil(p.width)==info.size.width) &&
           (p.height==info.size.height || std::bit_ceil(p.height)==info.size.height);
}

/* Compact TEX mip rows mapped onto the guest's padded linear image layout. */
template<class Info, class Row>
bool Rows(const Pair &p, const Info &info, Row row, bool guest_tiling=false) {
    size_t compact{};
    if (info.resources.levels > p.levels) return false;
    for (uint32_t m = 0; m < info.resources.levels; ++m) {
        auto [size, pitch, height, offset] = info.mips_layout[m];
        (void)size; (void)height;
        const uint32_t w = std::max(1u, p.width >> m), h = std::max(1u, p.height >> m);
        const uint32_t unit = p.storage == 13 ? 16 : 4;
        const uint32_t columns = p.storage == 13 ? (w + 3) / 4 : w;
        const uint32_t rows = p.storage == 13 ? (h + 3) / 4 : h;
        const uint32_t stride = (p.storage == 13 ? (std::max(pitch, w) + 3) / 4 : std::max(pitch, w)) * unit;
        const size_t bytes = size_t(columns) * unit;
        for (uint32_t y = 0; y < rows; ++y) {
            if (guest_tiling && info.props.is_tiled) {
                // Thin1D/Display1D: 8x8 microtiles, BC blocks for BC3.
                // Address bits follow the fixed shadPS4 tiling.comp.
                if (unsigned(info.array_mode)!=2 || (unsigned(info.tile_mode)!=9 && unsigned(info.tile_mode)!=13)) return false;
                const uint32_t block_pitch=stride/unit;
                for (uint32_t x=0;x<columns;++x) {
                    uint32_t pixel;
                    if (unsigned(info.tile_mode)==9 && unit==4)
                        pixel=(x&3)|((y&1)<<2)|((x&4)<<1)|((y&6)<<3);
                    else if (unsigned(info.tile_mode)==13)
                        pixel=(x&1)|((y&1)<<1)|((x&2)<<1)|((y&2)<<2)|((x&4)<<2)|((y&4)<<3);
                    else return false;
                    const size_t at=size_t(offset)+((y/8)*(block_pitch/8)+x/8)*64*unit+pixel*unit;
                    if (compact>p.bytes || unit>p.bytes-compact || at>info.guest_size || unit>info.guest_size-at ||
                        !row(at,compact,unit)) return false;
                    compact+=unit;
                }
                continue;
            }
            const size_t at = size_t(offset) + size_t(y) * stride;
            if (compact > p.bytes || bytes > p.bytes - compact || at > info.guest_size || bytes > info.guest_size - at ||
                !row(at, compact, bytes)) return false;
            compact += bytes;
        }
    }
    return compact <= p.bytes;
}
template<class Image>
const Pair *Select(Image &image, bool dirty, bool &changed, unsigned &mode) {
    Initialize(); changed = false;
    if (pairs.empty()) return nullptr;
    const auto &info = image.info;
    static unsigned initial_reports=0;
    static std::unordered_map<uint64_t,bool> initial_shapes;
    const uint64_t shape=uint64_t(info.size.width)<<32|uint64_t(info.size.height)<<8|info.resources.levels;
    if (std::getenv("KYOEI_INPUT_TEST") && initial_reports<160 && info.size.width>1 && info.size.width<512 && info.size.height<600 && initial_shapes.try_emplace(shape,true).second) {
        ++initial_reports;
        gz_log("Kyoei prompt image: %ux%u tiled=%u bits=%u mips=%u layers=%u block=%u bytes=%u",
            info.size.width,info.size.height,unsigned(info.props.is_tiled),info.num_bits,
            info.resources.levels,info.resources.layers,unsigned(info.props.is_block),info.guest_size);
    }
    bool candidate = false;
    for (const auto &p : pairs) if (Dimensions(p,info) &&
        info.resources.levels <= p.levels && (p.storage == 13) == bool(info.props.is_block)) { candidate = true; break; }
    if (!candidate) return nullptr;
    static unsigned reports=0;
    static std::unordered_map<uint64_t,bool> reported;
    if (std::getenv("KYOEI_INPUT_TEST") && reports<24 && reported.try_emplace(image.image_uid,true).second) {
        ++reports;
        gz_log("Kyoei prompt candidate: %ux%u tiled=%u bits=%u mips=%u layers=%u bytes=%u pitch=%u",
            info.size.width,info.size.height,unsigned(info.props.is_tiled),info.num_bits,
            info.resources.levels,info.resources.layers,info.guest_size,info.pitch);
    }
    if (info.resources.layers != 1 || info.size.depth != 1 || info.num_samples != 1) return nullptr;
    if (info.props.is_tiled && (unsigned(info.array_mode)!=2 || (unsigned(info.tile_mode)!=9 && unsigned(info.tile_mode)!=13))) return nullptr;
    auto [it, fresh] = states.try_emplace(image.image_uid);
    State &state = it->second;
    if (fresh || dirty) {
        state.pair = -1;
        if (info.guest_size>16*1024*1024) return nullptr;
        std::vector<unsigned char> guest(info.guest_size);
        if (!gz_memory_copy(info.guest_address,guest.data(),guest.size(),0)) return nullptr;
        for (size_t i = 0; i < pairs.size(); ++i) {
            const Pair &p = pairs[i];
            if (!Dimensions(p,info) || info.resources.levels > p.levels ||
                (p.storage == 13) != bool(info.props.is_block) || info.num_bits != (p.storage == 13 ? 128u : 32u)) continue;
            bool controller=true,keyboard=true;
            bool valid = Rows(p, info, [&](size_t at, size_t from, size_t bytes) {
                controller=controller && !std::memcmp(guest.data()+at,p.controller.data()+from,bytes);
                keyboard=keyboard && !std::memcmp(guest.data()+at,p.keyboard.data()+from,bytes);
                return controller || keyboard;
            },true);
            if (valid && (controller || keyboard)) { state.pair = int(i); break; }
        }
    }
    if (state.pair < 0) return nullptr;
    if (fresh) gz_log("Kyoei prompt matched: %ux%u index=%d",info.size.width,info.size.height,state.pair);
    const Pair &p = pairs[size_t(state.pair)];
    if (p.kind==1) kyoei_prompt_pause_seen();
    if (p.kind==2) { kyoei_prompt_status_seen(); return nullptr; }
    const char *forced = std::getenv("KYOEI_PROMPT_MODE");
    mode = forced && !std::strcmp(forced, "controller") ? KYOEI_INPUT_CONTROLLER :
           forced && !std::strcmp(forced, "keyboard") ? KYOEI_INPUT_KEYBOARD : kyoei_input_current_mode();
    const unsigned variant=mode | (p.kind==1 && !kyoei_prompt_pause_root()?2u:0u);
    changed = state.mode != variant;
    state.mode = variant;
    if (p.kind==1 && !kyoei_prompt_pause_root() && mode==KYOEI_INPUT_KEYBOARD)
        for (const auto &alternative : pairs) if(alternative.kind==3) return &alternative;
    return &p;
}
}
