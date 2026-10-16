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
#include "kyoei_command_batch.h"
extern "C" uint64_t kyoei_tls_fault_count(void) { return 100; }
#define CHECK(x) do { if(!(x)) { std::fprintf(stderr,"Frame stats failed at %d\n",__LINE__);return 1; } } while(0)
int main(int argc,char **) {
    if(argc>1) {
        _putenv_s("KYOEI_FRAME_STATS","0");
        { KyoeiFrameStats::Work work(0); }KyoeiFrameStats::Presented();
        for(unsigned category=19;category<22;++category) { KyoeiFrameStats::Work work(category); }
        unsigned calls=0;
        struct Task { unsigned* calls; void resume() { ++*calls; } bool done() { return true; } };
        { KyoeiFrameStats::CommandBatch batch; batch.Resume(Task{&calls}); batch.Empty(); batch.Finish(90.,1000.); }
        CHECK(calls==1 && logs.empty());return 0;
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
        "command-callback","video-label-wait","raster-draw","raster-dispatch","cp-sync",
        "graphics-construct","graphics-register","graphics-driver"};
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
    CHECK(logs.back().find("end_ms=")!=std::string::npos);
    for(unsigned category=19;category<22;++category) {
        const auto before_stage=logs.size();
        for(unsigned i=0;i<10;++i) {
            KyoeiFrameStats::Work work(category);
            if(i==0) { Begin(); End(); }
            std::this_thread::sleep_for(std::chrono::milliseconds(20));
        }
        // Each pipeline stage retains its own eight sparse compile records.
        unsigned reports=0;
        for(size_t i=before_stage;i<logs.size();++i)
            reports+=logs[i].find("Kyoei compile stall:")!=std::string::npos;
        CHECK(reports==8);
        CHECK(logs[before_stage].find("diagnostic=1 end_ms=")!=std::string::npos);
        CHECK(logs.back().find("diagnostic=0 end_ms=")!=std::string::npos);
    }
    const auto after_pipeline_stages=logs.size();
    for(unsigned i=0;i<10;++i) {
        KyoeiFrameStats::Work work(15);
        std::this_thread::sleep_for(std::chrono::milliseconds(55));
    }
    CHECK(logs.size()==after_pipeline_stages+8);
    CHECK(logs.back().find("Kyoei work stall: stage=video-label-wait")!=std::string::npos);
    CHECK(logs.back().find("end_ms=")!=std::string::npos);
    const auto before_readback=logs.size();
    { KyoeiFrameStats::Work work(3,256,8,8);std::this_thread::sleep_for(std::chrono::milliseconds(55)); }
    CHECK(logs.size()==before_readback+2);
    CHECK(logs.back().find("Kyoei readback stall: bytes=256 width=8 height=8")!=std::string::npos);
    const auto before_gpu=logs.size();
    KyoeiFrameStats::GpuWait(20,21,18,20,false,49.,Now());
    CHECK(logs.size()==before_gpu);
    for(unsigned i=0;i<10;++i) KyoeiFrameStats::GpuWait(20,21,18,20,false,240.,Now());
    CHECK(logs.size()==before_gpu+8);
    CHECK(logs.back().find("requested=20 next=21 known_before=18 known_after=20 forced_flush=0")!=std::string::npos);
    const auto before_images=logs.size();
    KyoeiFrameStats::SmallImage(0x1234,2,2,64,37,2);
    CHECK(logs.size()==before_images);
    for(unsigned i=0;i<1000;++i) KyoeiFrameStats::SmallImage(0x1234,1,1,64,37,2);
    CHECK(logs.size()==before_images+1);
    KyoeiFrameStats::SmallImage(0x1234,1,1,64,37,3);
    CHECK(logs.size()==before_images+2);
    for(unsigned i=0;i<100;++i) KyoeiFrameStats::SmallImage(0x2000+i,1,1,64,37,2);
    CHECK(logs.size()==before_images+16);
    const auto before_batches=logs.size();
    { KyoeiFrameStats::CommandBatch batch; batch.Finish(49.,1000.); }
    CHECK(logs.size()==before_batches);
    unsigned active_calls=0;
    struct ActiveTask {
        unsigned* calls;
        void resume() { ++*calls; }
        bool done() { return *calls==2; }
    };
    {
        KyoeiFrameStats::CommandBatch batch;
        batch.Resume(ActiveTask{&active_calls}); batch.Resume(ActiveTask{&active_calls});
        batch.Finish(90.,1000.);
    }
    CHECK(active_calls==2 && logs.size()==before_batches+1);
    CHECK(logs.back().find("resumes=2 completed=1 yielded=1")!=std::string::npos);
    for(unsigned i=0;i<10;++i) {
        KyoeiFrameStats::CommandBatch batch;
        batch.Record(12.,false); batch.Record(3.,true); batch.Empty();
        if(i==0) { Begin();End(); }
        batch.Finish(90.,1000.);
    }
    CHECK(logs.size()==before_batches+8);
    CHECK(logs[before_batches+1].find("diagnostic=1")!=std::string::npos);
    CHECK(logs.back().find("resume_ms=15.00 max_resume_ms=12.00 resumes=2 completed=1 yielded=1 empty=1")!=std::string::npos);
    CHECK(logs.back().find("diagnostic=0")!=std::string::npos);
    // Report exhaustion must never suppress execution of a task.
    unsigned calls=0;
    struct Task { unsigned* calls; void resume() { ++*calls; } bool done() { return true; } };
    { KyoeiFrameStats::CommandBatch batch; batch.Resume(Task{&calls}); }
    CHECK(calls==1 && logs.size()==before_batches+8);
    std::puts("FRAME STATS PASS: diagnostics, deadlines, batch accounting/cap and disabled execution");
}
