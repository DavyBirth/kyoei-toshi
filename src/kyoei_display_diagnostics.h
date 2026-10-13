// Opt-in evidence for blank flips; never suppresses a guest-requested frame.
#pragma once
#include <array>
#include <cstdint>
#include <cstdlib>
namespace KyoeiDisplayDiagnostics {
inline bool Enabled() {
    static const bool enabled=[] { const char* value=std::getenv("KYOEI_DISPLAY_TRACE"); return value && value[0]=='1'; }();
    return enabled;
}
// SubmitFlipInternal owns these counters on the renderer command thread.
inline void Submitted(int index,bool eop,int64_t argument,uint32_t width=0,
                      uint32_t height=0,uint32_t pitch=0,uint32_t format=0,uint32_t tiling=0) {
    if (!Enabled()) return;
    static uint64_t sequence=0,blank_start=0,blank_count=0;
    static std::array<uint32_t,5> previous{};
    ++sequence;
    if (index==-1) {
        if (!blank_count) {
            blank_start=sequence;
            gz_log("Kyoei display trace: event=guest-blank sequence=%llu eop=%u argument=%lld",
                   (unsigned long long)sequence,eop,(long long)argument);
        }
        ++blank_count;
        return;
    }
    if (blank_count) {
        gz_log("Kyoei display trace: event=guest-blank-end sequence=%llu first=%llu count=%llu buffer=%d",
               (unsigned long long)sequence,(unsigned long long)blank_start,(unsigned long long)blank_count,index);
        blank_count=0;
    }
    const std::array<uint32_t,5> attributes{width,height,pitch,format,tiling};
    if (attributes!=previous) {
        previous=attributes;
        gz_log("Kyoei display trace: event=surface sequence=%llu buffer=%d width=%u height=%u pitch=%u format=0x%x tiling=%u",
               (unsigned long long)sequence,index,width,height,pitch,format,tiling);
    }
}
// DrawBlankFrame is called by the presentation thread while the port is closed.
inline void ClosedPort() {
    if (!Enabled()) return;
    static uint64_t count=0;
    if (++count==1 || count%120==0)
        gz_log("Kyoei display trace: event=closed-port-blank count=%llu",(unsigned long long)count);
}
}
