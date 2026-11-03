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
#include <mutex>
#include <vector>
#include <unordered_map>
#include "kyoei_input_mode.h"
#include "kyoei_prompt_state.h"
extern "C" int gz_memory_copy(uintptr_t, void *, uint64_t, int);
extern "C" void gz_log(const char *, ...);
namespace KyoeiPrompts {
struct Pair {
    uint32_t width{}, height{}, levels{}, storage{}, channels{}, kind{}, bytes{};
    std::vector<unsigned char> controller, keyboard, american, xbox, nintendo;
    const std::vector<unsigned char> &Controller() const {
        const unsigned family = kyoei_input_current_family();
        if (!xbox.empty() && family == KYOEI_PAD_XBOX) return xbox;
        // Nintendo confirms with the right button, as the original (circle) art.
        if (family == KYOEI_PAD_NINTENDO) return nintendo.empty() ? controller : nintendo;
        return american.empty() ? controller : american;
    }
};
inline std::vector<Pair> pairs;
inline bool initialized{};
inline std::once_flag loaded;
inline void Load();
// Threads binding textures wait for the whole pack: one that went on while another still read
// it (177 MB with the controls diagrams) cached its images as unmatched for good.
inline void Initialize() { std::call_once(loaded, Load); }
// KYPROMZ1: a KYPROMP4 pack, zlib-compressed (u64 size first): the controls diagrams are mostly
// transparent, 177 MB stored, about 9 MB compressed.
extern "C" int mz_uncompress(unsigned char *, unsigned long *, const unsigned char *, unsigned long);
inline bool ReadAll(const char *path, std::vector<unsigned char> &data) {
    FILE *file = std::fopen(path, "rb");
    if (!file) return false;
    std::fseek(file, 0, SEEK_END); const long size = std::ftell(file); std::fseek(file, 0, SEEK_SET);
    data.resize(size > 0 ? size_t(size) : 0);
    const bool ok = size > 0 && std::fread(data.data(), 1, data.size(), file) == data.size();
    std::fclose(file);
    if (ok && data.size() > 16 && !std::memcmp(data.data(), "KYPROMZ1", 8)) {
        uint64_t raw{}; std::memcpy(&raw, data.data() + 8, 8);
        if (raw > 512u * 1024 * 1024) return false;
        std::vector<unsigned char> plain(raw);
        unsigned long length = (unsigned long)raw;
        if (mz_uncompress(plain.data(), &length, data.data() + 16, (unsigned long)(data.size() - 16)) != 0 ||
            length != raw) return false;
        data.swap(plain);
    }
    return ok;
}
inline void Load() {
    initialized = true;
    const char *path = std::getenv("KYOEI_PROMPT_PACK");
    if (!path || !*path) return;
    std::vector<unsigned char> data;
    if (!ReadAll(path, data)) { gz_log("Kyoei prompts: cannot read %s", path); return; }
    size_t at = 0;
    auto read = [&](void *to, size_t bytes) {
        if (bytes > data.size() - at) return false;
        std::memcpy(to, data.data() + at, bytes); at += bytes; return true;
    };
    char magic[8]; uint32_t count{};
    bool ok = read(magic, 8);
    // KYPROMP3 adds the Xbox artwork after the American one, KYPROMP4 the Nintendo one.
    const bool with_nintendo = ok && !std::memcmp(magic, "KYPROMP4", 8);
    const bool with_xbox = with_nintendo || (ok && !std::memcmp(magic, "KYPROMP3", 8));
    const bool american = with_xbox || (ok && !std::memcmp(magic, "KYPROMP2", 8));
    ok = ok && (american || !std::memcmp(magic, "KYPROMP1", 8)) && read(&count, 4) && count <= 128;
    size_t total{};
    for (uint32_t i = 0; ok && i < count; ++i) {
        uint32_t fields[7];
        if (!read(fields, sizeof(fields))) { ok = false; break; }
        Pair p{fields[0], fields[1], fields[2], fields[3], fields[4], fields[5], fields[6], {}, {}};
        total += size_t(p.bytes) * (with_nintendo ? 5 : with_xbox ? 4 : american ? 3 : 2);
        if (!p.width || !p.height || p.width > 4096 || p.height > 4096 || !p.levels ||
            p.levels > 14 || !p.bytes || total > 256 * 1024 * 1024 ||  // controls diagrams: ~180 MB
            (p.storage != 1 && p.storage != 13)) {
            ok = false; break;
        }
        auto part = [&](std::vector<unsigned char> &to) { to.resize(p.bytes); return read(to.data(), p.bytes); };
        ok = part(p.controller) && part(p.keyboard) && (!american || part(p.american)) &&
             (!with_xbox || part(p.xbox)) && (!with_nintendo || part(p.nintendo));
        if (ok) pairs.push_back(std::move(p));
    }
    ok = ok && at == data.size();
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
    // getenv scans the whole process environment; this runs for every bound texture,
    // so read the switches once (they are fixed for the session).
    static const bool input_test = std::getenv("KYOEI_INPUT_TEST") != nullptr;
    static const char *const forced = std::getenv("KYOEI_PROMPT_MODE");
    static unsigned initial_reports=0;
    static std::unordered_map<uint64_t,bool> initial_shapes;
    const uint64_t shape=uint64_t(info.size.width)<<32|uint64_t(info.size.height)<<8|info.resources.levels;
    if (input_test && initial_reports<160 && info.size.width>1 && info.size.width<512 && info.size.height<600 && initial_shapes.try_emplace(shape,true).second) {
        ++initial_reports;
        gz_log("Kyoei prompt image: %ux%u tiled=%u bits=%u mips=%u layers=%u block=%u bytes=%u",
            info.size.width,info.size.height,unsigned(info.props.is_tiled),info.num_bits,
            info.resources.levels,info.resources.layers,unsigned(info.props.is_block),info.guest_size);
    }
    // Runs for every bound texture: one hash lookup instead of testing every pair.
    // Same predicate as Dimensions() + levels + block storage over all pairs.
    static std::unordered_map<uint64_t, uint32_t> shapes;
    static const Pair *shapes_data = nullptr;
    static size_t shapes_size = 0;
    if (shapes_data != pairs.data() || shapes_size != pairs.size()) {
        shapes.clear(); shapes_data = pairs.data(); shapes_size = pairs.size();
        for (const auto &p : pairs)
            for (const uint32_t w : {p.width, std::bit_ceil(p.width)})
                for (const uint32_t h : {p.height, std::bit_ceil(p.height)}) {
                    auto &levels = shapes[uint64_t(w) << 33 | uint64_t(h) << 1 | uint64_t(p.storage == 13)];
                    levels = std::max<uint32_t>(levels, p.levels);
                }
    }
    const auto candidate = shapes.find(uint64_t(info.size.width) << 33 | uint64_t(info.size.height) << 1 |
                                       uint64_t(bool(info.props.is_block)));
    if (candidate == shapes.end() || info.resources.levels > candidate->second) return nullptr;
    static unsigned reports=0;
    static std::unordered_map<uint64_t,bool> reported;
    if (input_test && reports<24 && reported.try_emplace(image.image_uid,true).second) {
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
    mode = forced && !std::strcmp(forced, "controller") ? KYOEI_INPUT_CONTROLLER :
           forced && !std::strcmp(forced, "keyboard") ? KYOEI_INPUT_KEYBOARD : kyoei_input_current_mode();
    const unsigned variant=mode | (p.kind==1 && !kyoei_prompt_pause_root()?2u:0u) |
        (mode==KYOEI_INPUT_CONTROLLER?kyoei_input_current_family()*4u:0u);
    changed = state.mode != variant;
    state.mode = variant;
    if (p.kind==1 && !kyoei_prompt_pause_root() && mode==KYOEI_INPUT_KEYBOARD)
        for (const auto &alternative : pairs) if(alternative.kind==3) return &alternative;
    return &p;
}
}
