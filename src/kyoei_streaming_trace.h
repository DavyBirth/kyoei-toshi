#pragma once
#include <array>
#include <chrono>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include "kyoei_stutter_trace.h"
#include "kyoei_diagnostic_activity.h"
extern "C" void gz_log(const char*,...);
namespace KyoeiStreamingTrace {
inline bool Enabled() {
    static const bool value=[] { auto* p=std::getenv("KYOEI_STREAMING_TRACE");return p && p[0]=='1'; }();
    return value;
}
inline double Milliseconds() {
    return std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now().time_since_epoch()).count();
}
// These options are already immutable after first use. Keep the hot guard
// small; clock reads and reporting stay outside the inactive path.
inline bool TimingEnabled() {
    static const bool enabled=Enabled() || KyoeiStutterTrace::Detailed();
    return enabled;
}
class Scope {
    const char* name;
    uint64_t address,bytes;
    unsigned width,height;
    double start=0;
    KyoeiDiagnosticActivity::Stamp diagnostic;
public:
    [[gnu::always_inline]] Scope(const char* label,uint64_t addr=0,uint64_t size=0,unsigned w=0,unsigned h=0) {
        if(TimingEnabled())Begin(label,addr,size,w,h);
    }
    [[gnu::always_inline]] ~Scope() { if(start)Finish(); }
private:
    [[gnu::noinline]] void Begin(const char* label,uint64_t addr,uint64_t size,unsigned w,unsigned h) {
        name=label;address=addr;bytes=size;width=w;height=h;
        if(KyoeiStutterTrace::Detailed())diagnostic=KyoeiDiagnosticActivity::Now();
        start=Milliseconds();
    }
    [[gnu::noinline]] void Finish() {
        const auto ended=std::chrono::steady_clock::now();
        const auto end=std::chrono::duration<double,std::milli>(ended.time_since_epoch()).count();
        if(KyoeiStutterTrace::Detailed() && end-start>=8.){
            unsigned stage=299;
            if(std::strcmp(name,"gfx-packet-inclusive")==0)stage=200;
            else if(std::strcmp(name,"draw")==0)stage=201;
            else if(std::strcmp(name,"find-image")==0)stage=202;
            else if(std::strcmp(name,"refresh-image")==0)stage=203;
            else if(std::strcmp(name,"texture-gc")==0)stage=204;
            else if(std::strcmp(name,"image-buffer")==0)stage=205;
            else if(std::strcmp(name,"command-callbacks")==0)stage=206;
            KyoeiStutterTrace::Record(stage,ended,end-start,
                KyoeiDiagnosticActivity::Overlaps(diagnostic,KyoeiDiagnosticActivity::Now()),address,bytes);
        }
        if(!Enabled())return;
        if(end-start<25)return;
        struct Counter {const char* name=nullptr;uint64_t key=0;unsigned count=0;};
        static thread_local std::array<Counter,32> counters{};
        // Waiting packets in menus must not consume the evidence budget for
        // uploads, draws or other opcodes encountered later in the city.
        const uint64_t key=name[0]=='g' ? address : 0;
        Counter* counter=nullptr;
        for(auto& entry:counters) {
            if(!entry.name){entry.name=name;entry.key=key;counter=&entry;break;}
            if(entry.name==name && entry.key==key){counter=&entry;break;}
        }
        if(!counter || counter->count>=8)return;
        ++counter->count;
        gz_log("Kyoei streaming stall: stage=%s milliseconds=%.2f end_ms=%.2f address=0x%llx bytes=%llu width=%u height=%u",
            name,end-start,end,(unsigned long long)address,(unsigned long long)bytes,width,height);
    }
};
inline void RenderExtent(unsigned width,unsigned height,float xscale,float yscale,bool depth) {
    if(!Enabled())return;
    struct Key {unsigned w,h,vw,vh,depth;bool operator==(const Key&)const=default;};
    // Metadata only, bounded even if a guest changes dimensions every draw.
    const float vw=std::abs(xscale)*2, vh=std::abs(yscale)*2;
    if(!std::isfinite(vw)||!std::isfinite(vh)||vw>65535||vh>65535)return;
    const Key key{width,height,unsigned(vw),unsigned(vh),unsigned(depth)};
    static thread_local std::array<Key,64> seen{};
    static thread_local unsigned count=0;
    for(unsigned i=0;i<count;++i)if(seen[i]==key)return;
    if(count==seen.size())return;
    seen[count++]=key;
    gz_log("Kyoei render extent: width=%u height=%u viewport_width=%u viewport_height=%u depth=%u end_ms=%.2f",
        width,height,key.vw,key.vh,key.depth,Milliseconds());
}
}
