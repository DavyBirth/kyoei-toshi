#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <thread>
static std::vector<std::string> logs;
extern "C" void gz_log(const char *format,...) {
    char line[2048];va_list args;va_start(args,format);std::vsnprintf(line,sizeof(line),format,args);va_end(args);logs.emplace_back(line);
}
#include "kyoei_frame_stats.h"
extern "C" uint64_t kyoei_tls_fault_count(void) { return 100; }
#define CHECK(x) do { if(!(x)) { std::fprintf(stderr,"Frame stats failed at %d\n",__LINE__);return 1; } } while(0)
int main(int argc,char **) {
    if(argc>1) {
        _putenv_s("KYOEI_FRAME_STATS","0");
        { KyoeiFrameStats::Work work(0); }KyoeiFrameStats::Presented();
        CHECK(logs.empty());return 0;
    }
    _putenv_s("KYOEI_FRAME_STATS","1");
    using namespace KyoeiDiagnosticActivity;
    auto before=Now();Begin();Begin();CHECK(Now().active);End();auto after=Now();
    CHECK(!after.active && Overlaps(before,after));
    for(unsigned i=0;i<120;++i) {
        KyoeiFrameStats::Work work(0);
        if(i==60) { Begin();End(); }
    }
    CHECK(logs.size()==1 && logs[0].find("diagnostic=1")!=std::string::npos);
    for(unsigned i=0;i<120;++i) { KyoeiFrameStats::Work work(0); }
    CHECK(logs.size()==2 && logs[1].find("diagnostic=0")!=std::string::npos);
    // Capture entirely between work calls still contaminates their aggregate.
    for(unsigned i=0;i<120;++i) {
        if(i==30) { Begin();End(); }
        KyoeiFrameStats::Work work(1);
    }
    CHECK(logs.size()==3 && logs[2].find("diagnostic=1")!=std::string::npos);
    auto tick=std::chrono::steady_clock::now();
    KyoeiFrameStats::PresentedAt(tick);
    for(unsigned i=0;i<120;++i) {
        // Twenty long pauses and a hundred 10 ms frames. Test deadlines
        // against individual samples, rather than inferring them from a mean.
        tick+=std::chrono::milliseconds(i<20?110:10);
        KyoeiFrameStats::PresentedAt(tick);
    }
    CHECK(logs.size()==6 && logs[3].find("diagnostic=0")!=std::string::npos);
    CHECK(logs[4].find("over_60hz=20 over_30hz=20 over_50ms=20 over_100ms=20 diagnostic=0")!=std::string::npos);
    CHECK(logs[5].find("delta=0 ")!=std::string::npos);
    for(unsigned i=0;i<120;++i) {
        if(i==60) { Begin();End(); }
        tick+=std::chrono::milliseconds(20);
        KyoeiFrameStats::PresentedAt(tick);
    }
    CHECK(logs.size()==9);
    CHECK(logs[7].find("over_60hz=120 over_30hz=0 over_50ms=0 over_100ms=0 diagnostic=1")!=std::string::npos);
    const char* stages[]={"readback-finish","readback-publish","readback-map"};
    for(unsigned category=3;category<6;++category) {
        const auto previous=logs.size();
        for(unsigned i=0;i<120;++i) { KyoeiFrameStats::Work work(category); }
        CHECK(logs.size()==previous+1);
        CHECK(logs.back().find(stages[category-3])!=std::string::npos);
    }
    const char* compile_stages[]={"shader-compile","graphics-pipeline","compute-pipeline",
        "scheduler-submit","scheduler-wait","queue-lock","command-batch","present-pacing",
        "command-callback","video-label-wait","raster-draw","raster-dispatch","cp-sync"};
    for(unsigned category=6;category<KyoeiFrameStats::CategoryCount;++category) {
        const auto previous=logs.size();
        const unsigned stride=category==16 || category==17 ? 128u : 1u;
        for(unsigned i=0;i<120*stride;++i) { KyoeiFrameStats::Work work(category); }
        CHECK(logs.size()==previous+1);
        CHECK(logs.back().find(compile_stages[category-6])!=std::string::npos);
        CHECK(logs.back().find("sample_stride="+std::to_string(stride))!=std::string::npos);
    }
    // Worker samples cannot complete a partial renderer-thread window.
    const auto previous=logs.size();
    for(unsigned i=0;i<60;++i) { KyoeiFrameStats::Work work(4); }
    std::thread worker([] {
        for(unsigned i=0;i<60;++i) { KyoeiFrameStats::Work work(4); }
    });
    worker.join();
    CHECK(logs.size()==previous);
    for(unsigned i=0;i<60;++i) { KyoeiFrameStats::Work work(4); }
    CHECK(logs.size()==previous+1);
    const auto before_stall=logs.size();
    { KyoeiFrameStats::Work work(6);std::this_thread::sleep_for(std::chrono::milliseconds(20)); }
    CHECK(logs.size()==before_stall+1 && logs.back().find("Kyoei compile stall: stage=shader-compile")!=std::string::npos);
    const auto before_work_stalls=logs.size();
    for(unsigned i=0;i<10;++i) {
        KyoeiFrameStats::Work work(15);
        std::this_thread::sleep_for(std::chrono::milliseconds(55));
    }
    CHECK(logs.size()==before_work_stalls+8);
    CHECK(logs.back().find("Kyoei work stall: stage=video-label-wait")!=std::string::npos);
    CHECK(logs.back().find("end_ms=")!=std::string::npos);
    std::puts("FRAME STATS PASS: capture within/between calls annotated; clean window resets; raw samples retained");
}
