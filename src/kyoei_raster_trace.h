// SPDX-License-Identifier: GPL-2.0-or-later
// Read-only, user-triggered raster snapshots. No textures or guest prose logged.
#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <mutex>
extern "C" void gz_log(const char *,...);
namespace KyoeiRasterTrace {
struct Profile {
    uint64_t target{},vs{},ps{};
    std::array<int,4> screen{},window{},generic{},viewport_scissor{},scissor{};
    std::array<float,4> viewport{};
    std::array<int,2> offset{};
    unsigned viewport_index{},window_disable{},generic_disable{},viewport_enable{},blend{},depth{},primitive{};
    bool operator==(const Profile &) const=default;
};
inline bool Enabled() {
    static const bool enabled=[] { const char *v=std::getenv("KYOEI_RASTER_TRACE");return v && !std::strcmp(v,"1"); }();
    return enabled;
}
inline std::atomic<uint64_t> requested{},completed{};
inline std::mutex mutex;
inline uint64_t recording{};
inline unsigned frames{},draws{},size{},dropped{};
inline std::array<Profile,256> profiles;
inline std::array<unsigned,256> repetitions;
inline bool Request() {
    if (!Enabled()) return false;
    uint64_t id=requested.fetch_add(1)+1;
    gz_log("Kyoei raster capture requested: id=%llu; next two presented frames",(unsigned long long)id);
    return true;
}
inline bool Capturing() { return Enabled() && requested.load()!=completed.load(); }
inline void Record(const Profile &value) {
    if (!Capturing()) return;
    std::scoped_lock lock{mutex};
    uint64_t id=requested.load();
    if (id!=recording) { recording=id;frames=draws=size=dropped=0;repetitions.fill(0); }
    ++draws;
    for (unsigned i=0;i<size;++i) if (profiles[i]==value) { ++repetitions[i];return; }
    if (size<profiles.size()) { profiles[size]=value;repetitions[size++]=1; }
    else ++dropped;
}
inline void Presented() {
    if (!Capturing()) return;
    std::scoped_lock lock{mutex};
    if (!draws || recording!=requested.load() || ++frames<2) return;
    gz_log("Kyoei raster capture begin: id=%llu frames=%u draws=%u profiles=%u dropped=%u",
        (unsigned long long)recording,frames,draws,size,dropped);
    for (unsigned i=0;i<size;++i) {
        const auto &p=profiles[i];
        gz_log("Kyoei raster draw: id=%llu p=%u repeats=%u target=%llx vs=%llx ps=%llx primitive=%u blend=%08x depth=%08x",
            (unsigned long long)recording,i,repetitions[i],(unsigned long long)p.target,
            (unsigned long long)p.vs,(unsigned long long)p.ps,p.primitive,p.blend,p.depth);
        gz_log("Kyoei raster viewport: p=%u index=%u xywh=%.3f,%.3f,%.3f,%.3f scissor_xywh=%d,%d,%d,%d offset=%d,%d disable_window=%u disable_generic=%u viewport_scissor=%u",
            i,p.viewport_index,p.viewport[0],p.viewport[1],p.viewport[2],p.viewport[3],
            p.scissor[0],p.scissor[1],p.scissor[2],p.scissor[3],p.offset[0],p.offset[1],
            p.window_disable,p.generic_disable,p.viewport_enable);
        gz_log("Kyoei raster bounds: p=%u screen=%d,%d,%d,%d window=%d,%d,%d,%d generic=%d,%d,%d,%d viewport=%d,%d,%d,%d",
            i,p.screen[0],p.screen[1],p.screen[2],p.screen[3],p.window[0],p.window[1],p.window[2],p.window[3],
            p.generic[0],p.generic[1],p.generic[2],p.generic[3],p.viewport_scissor[0],p.viewport_scissor[1],p.viewport_scissor[2],p.viewport_scissor[3]);
    }
    gz_log("Kyoei raster capture end: id=%llu",(unsigned long long)recording);
    completed.store(recording);
}
template<class Regs,class Viewport,class Rect>
Profile MakeProfile(const Regs &r,const Viewport &v,const Rect &rect,unsigned i) {
    Profile p;
    p.target=r.color_buffers[0].Address();
    p.vs=r.vs_program.template Address<uint64_t>();p.ps=r.ps_program.template Address<uint64_t>();
    const auto bounds=[](const auto &s) { return std::array<int,4>{s.top_left_x,s.top_left_y,s.bottom_right_x,s.bottom_right_y}; };
    p.screen=bounds(r.screen_scissor);p.window=bounds(r.window_scissor);p.generic=bounds(r.generic_scissor);
    p.viewport_scissor=bounds(r.viewport_scissors[i]);
    p.viewport={v.x,v.y,v.width,v.height};p.scissor={rect.offset.x,rect.offset.y,(int)rect.extent.width,(int)rect.extent.height};
    p.offset={r.window_offset.window_x_offset,r.window_offset.window_y_offset};
    p.window_disable=r.window_scissor.window_offset_disable;p.generic_disable=r.generic_scissor.window_offset_disable;
    p.viewport_enable=r.mode_control.vport_scissor_enable;p.viewport_index=i;p.primitive=(unsigned)r.primitive_type;
    std::memcpy(&p.blend,&r.blend_control[0],4);std::memcpy(&p.depth,&r.depth_control,4);
    return p;
}
}
