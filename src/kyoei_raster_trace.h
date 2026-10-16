// SPDX-License-Identifier: GPL-2.0-or-later
// Read-only, user-triggered raster snapshots. No textures or guest prose logged.
#pragma once
#include <array>
#include <atomic>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <mutex>
#include <cmath>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include "kyoei_diagnostic_activity.h"
extern "C" void gz_log(const char *,...);
extern "C" int gz_memory_copy(uintptr_t,void *,uint64_t,int);
namespace KyoeiRasterTrace {
struct VertexProbe {
    uint64_t address{},bytes{};
    unsigned location{},stride{},format{},status{};
    std::array<float,8> xy{};
    unsigned sample_count{},selection_status{};
    std::array<uint32_t,4> selected_indices{};
    bool operator==(const VertexProbe &) const=default;
};
template<class Readable,class Dirty,class Copy>
void SampleXY(VertexProbe &probe,Readable readable,Dirty dirty,Copy copy) {
    probe.xy.fill(0);
    if (probe.format!=103 && probe.format!=106 && probe.format!=109) { probe.status=3;return; }
    uint64_t need=uint64_t(probe.stride)*3+8;
    if (probe.stride<8 || probe.stride>256 || probe.bytes<need ||
        probe.address>UINT64_MAX-need || !readable(probe.address,need)) { probe.status=4;return; }
    if (dirty(probe.address,need)) { probe.status=2;return; }
    for (unsigned i=0;i<4;++i) if (!copy(probe.address+uint64_t(i)*probe.stride,&probe.xy[i*2],8)) {
        probe.xy.fill(0);probe.status=7;return;
    }
    for (float value:probe.xy) if (!std::isfinite(value)) { probe.xy.fill(0);probe.status=5;return; }
    probe.status=1;
}
inline thread_local std::array<VertexProbe,4> current_vertices{};
inline thread_local unsigned current_vertex_count{};
struct DrawSelection {
    uint64_t address{};
    unsigned count{},index_bytes{},indexed{},valid{};
    int64_t base_vertex{};
};
inline thread_local DrawSelection current_selection{};
struct UniformProbe {
    uint64_t address{},bytes{};
    unsigned stage{},binding{},status{},word_count{};
    std::array<uint32_t,16> words{};
    bool operator==(const UniformProbe &) const=default;
};
inline thread_local std::array<UniformProbe,8> current_uniforms{};
inline thread_local unsigned current_uniform_count{};
template<class Resource> bool CanSampleResource(const Resource &resource) {
    // Vulkan storage classification also includes read-only constant buffers.
    return !resource.IsSpecial() && !resource.is_written;
}
template<class Readable,class Dirty,class Copy>
void SampleWords(UniformProbe &probe,Readable readable,Dirty dirty,Copy copy) {
    probe.words.fill(0);probe.word_count=0;
    const uint64_t bytes=std::min(probe.bytes,uint64_t(64))&~uint64_t(3);
    if(!bytes || probe.address>UINT64_MAX-bytes || !readable(probe.address,bytes)) { probe.status=4;return; }
    if(dirty(probe.address,bytes)) { probe.status=2;return; }
    if(!copy(probe.address,probe.words.data(),bytes)) { probe.words.fill(0);probe.status=7;return; }
    probe.word_count=static_cast<unsigned>(bytes/4);probe.status=1;
}
template<class Readable,class Dirty,class Copy>
void SampleSelectedXY(VertexProbe &probe,const DrawSelection &draw,Readable readable,Dirty dirty,Copy copy) {
    probe.xy.fill(0);probe.sample_count=0;probe.selected_indices.fill(0);probe.selection_status=draw.valid;
    if (!draw.valid) { probe.status=8;return; }
    if (probe.format!=103 && probe.format!=106 && probe.format!=109) { probe.status=3;return; }
    if (probe.stride<8 || probe.stride>256 || !draw.count) { probe.status=4;return; }
    unsigned count=std::min(draw.count,4u);
    if (draw.indexed) {
        const uint64_t size=uint64_t(count)*draw.index_bytes;
        if ((draw.index_bytes!=2 && draw.index_bytes!=4) || draw.address>UINT64_MAX-size || !readable(draw.address,size)) { probe.status=4;return; }
        if (dirty(draw.address,size)) { probe.status=2;return; }
        std::array<uint32_t,4> raw{};
        if (!copy(draw.address,raw.data(),size)) { probe.status=7;return; }
        for(unsigned i=0;i<count;++i) {
            uint32_t index{};
            if(draw.index_bytes==2) { uint16_t small{};std::memcpy(&small,reinterpret_cast<const char *>(raw.data())+i*2,2);index=small; }
            else index=raw[i];
            const int64_t selected=int64_t(index)+draw.base_vertex;
            if(selected<0 || selected>UINT32_MAX) { probe.status=4;return; }
            probe.selected_indices[i]=static_cast<uint32_t>(selected);
        }
    } else {
        if(draw.base_vertex<0 || uint64_t(draw.base_vertex)+count-1>UINT32_MAX) { probe.status=4;return; }
        for(unsigned i=0;i<count;++i) probe.selected_indices[i]=static_cast<uint32_t>(draw.base_vertex+i);
    }
    // Validate every range before copying any coordinates; never synchronize GPU data.
    for(unsigned i=0;i<count;++i) {
        const uint64_t offset=uint64_t(probe.selected_indices[i])*probe.stride;
        if(offset>probe.bytes || probe.bytes-offset<8 || probe.address>UINT64_MAX-offset-8 || !readable(probe.address+offset,8)) { probe.status=4;return; }
        if(dirty(probe.address+offset,8)) { probe.status=2;return; }
    }
    for(unsigned i=0;i<count;++i) {
        if(!copy(probe.address+uint64_t(probe.selected_indices[i])*probe.stride,&probe.xy[i*2],8)) { probe.xy.fill(0);probe.status=7;return; }
        if(!std::isfinite(probe.xy[i*2]) || !std::isfinite(probe.xy[i*2+1])) { probe.xy.fill(0);probe.status=5;return; }
    }
    probe.sample_count=count;probe.status=1;
}
struct Profile {
    uint64_t target{},vs{},ps{};
    std::array<int,4> screen{},window{},generic{},viewport_scissor{},scissor{};
    std::array<float,4> viewport{};
    std::array<int,2> offset{};
    unsigned viewport_index{},window_disable{},generic_disable{},viewport_enable{},blend{},depth{},primitive{};
    unsigned vertex_count{};
    std::array<VertexProbe,4> vertices{};
    unsigned parameter_count{};
    std::array<uint32_t,16> vs_parameters{},ps_parameters{};
    unsigned uniform_count{};
    std::array<UniformProbe,8> uniforms{};
    bool operator==(const Profile &) const=default;
};
inline bool Enabled() {
    static const bool enabled=[] { const char *v=std::getenv("KYOEI_RASTER_TRACE");return v && !std::strcmp(v,"1"); }();
    return enabled;
}
inline void BeginDraw() {
    // Requests may arrive between draw setup and resource binding. Keep the
    // reset when tracing is enabled, even before a capture is requested.
    // With tracing disabled these thread-local buffers are never consumed.
    if (!Enabled()) return;
    current_selection={};
    current_uniforms={};
    current_uniform_count=0;
}
inline std::atomic<uint64_t> requested{},completed{};
inline std::mutex mutex;
inline uint64_t recording{};
inline unsigned frames{},draws{},size{},dropped{};
inline std::array<Profile,2048> profiles;
inline std::array<unsigned,2048> repetitions;
inline std::unordered_map<uint64_t,std::vector<unsigned>> profile_buckets;
inline unsigned last_comparisons{};
inline uint64_t ProfileBucket(const Profile &p) {
    uint64_t hash=UINT64_C(14695981039346656037);
    auto add=[&](uint64_t value) { hash=(hash^value)*UINT64_C(1099511628211); };
    add(p.target);add(p.vs);add(p.ps);add(p.primitive);add(p.blend);add(p.depth);
    add(p.vertex_count);add(p.parameter_count);add(p.uniform_count);
    for(unsigned i=0;i<p.vertex_count;++i) {
        const auto &v=p.vertices[i];add(v.address);add(v.location);add(v.status);
        for(auto index:v.selected_indices) add(index);
    }
    for(unsigned i=0;i<p.parameter_count;++i) { add(p.vs_parameters[i]);add(p.ps_parameters[i]); }
    for(unsigned i=0;i<p.uniform_count;++i) {
        const auto &u=p.uniforms[i];add(u.address);add(u.stage);add(u.binding);add(u.status);
        for(auto word:u.words) add(word);
    }
    // Omitted fields still participate in exact equality below. A collision
    // cannot merge unequal profiles; +0/-0 floats share this integer key.
    return hash;
}
inline void RecordProfile(const Profile &value,uint64_t bucket) {
    ++draws;last_comparisons=0;
    auto found=profile_buckets.find(bucket);
    if(found!=profile_buckets.end()) for(unsigned i:found->second) {
        ++last_comparisons;
        if(profiles[i]==value) { ++repetitions[i];return; }
    }
    if(size<profiles.size()) {
        profiles[size]=value;repetitions[size]=1;profile_buckets[bucket].push_back(size++);
    } else ++dropped;
}
inline bool Request() {
    if (!Enabled()) return false;
    uint64_t id=requested.fetch_add(1)+1;
    KyoeiDiagnosticActivity::Begin();
    gz_log("Kyoei raster capture requested: id=%llu; next two presented frames",(unsigned long long)id);
    return true;
}
inline bool Capturing() { return Enabled() && requested.load()!=completed.load(); }
inline void Record(const Profile &value) {
    if (!Capturing()) return;
    std::scoped_lock lock{mutex};
    uint64_t id=requested.load();
    if (id!=recording) {
        recording=id;frames=draws=size=dropped=0;repetitions.fill(0);
        profile_buckets.clear();profile_buckets.reserve(profiles.size());
    }
    RecordProfile(value,ProfileBucket(value));
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
        for(unsigned slot=0;slot<p.vertex_count;++slot) {
            const auto &v=p.vertices[slot];
            gz_log("Kyoei raster vertex: p=%u slot=%u location=%u address=%llx bytes=%llu stride=%u format=%u status=%u samples=%u selection=%u indices=%u,%u,%u,%u selected_xy=%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f,%.3f",
                i,slot,v.location,(unsigned long long)v.address,(unsigned long long)v.bytes,v.stride,v.format,v.status,
                v.sample_count,v.selection_status,v.selected_indices[0],v.selected_indices[1],v.selected_indices[2],v.selected_indices[3],
                v.xy[0],v.xy[1],v.xy[2],v.xy[3],v.xy[4],v.xy[5],v.xy[6],v.xy[7]);
        }
        for(unsigned j=0;j<p.parameter_count;j+=8) {
            gz_log("Kyoei raster parameters: p=%u stage=vs start=%u words=%08x,%08x,%08x,%08x,%08x,%08x,%08x,%08x",
                i,j,p.vs_parameters[j],p.vs_parameters[j+1],p.vs_parameters[j+2],p.vs_parameters[j+3],p.vs_parameters[j+4],p.vs_parameters[j+5],p.vs_parameters[j+6],p.vs_parameters[j+7]);
            gz_log("Kyoei raster parameters: p=%u stage=ps start=%u words=%08x,%08x,%08x,%08x,%08x,%08x,%08x,%08x",
                i,j,p.ps_parameters[j],p.ps_parameters[j+1],p.ps_parameters[j+2],p.ps_parameters[j+3],p.ps_parameters[j+4],p.ps_parameters[j+5],p.ps_parameters[j+6],p.ps_parameters[j+7]);
        }
        for(unsigned slot=0;slot<p.uniform_count;++slot) {
            const auto &u=p.uniforms[slot];
            for(unsigned j=0;j<16;j+=8)
                gz_log("Kyoei raster uniform: p=%u slot=%u stage=%u binding=%u address=%llx bytes=%llu status=%u count=%u start=%u words=%08x,%08x,%08x,%08x,%08x,%08x,%08x,%08x",
                    i,slot,u.stage,u.binding,(unsigned long long)u.address,(unsigned long long)u.bytes,u.status,u.word_count,j,
                    u.words[j],u.words[j+1],u.words[j+2],u.words[j+3],u.words[j+4],u.words[j+5],u.words[j+6],u.words[j+7]);
        }
    }
    gz_log("Kyoei raster capture end: id=%llu",(unsigned long long)recording);
    completed.store(recording);
    KyoeiDiagnosticActivity::End();
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
    p.vertex_count=current_vertex_count;p.vertices=current_vertices;
    p.uniform_count=current_uniform_count;p.uniforms=current_uniforms;
    if ((p.blend || p.primitive==17 || (p.primitive==6 && p.depth==0)) && std::fabs(v.width)>=128) {
        p.parameter_count=16;std::copy_n(r.vs_program.user_data.begin(),16,p.vs_parameters.begin());
        std::copy_n(r.ps_program.user_data.begin(),16,p.ps_parameters.begin());
    }
    return p;
}
}
