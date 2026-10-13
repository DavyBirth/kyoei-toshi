#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
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
    KyoeiFrameStats::Presented();
    for(unsigned i=0;i<1000 && logs.size()<5;++i) KyoeiFrameStats::Presented();
    CHECK(logs.size()==5 && logs[3].find("diagnostic=0")!=std::string::npos);
    CHECK(logs[4].find("delta=0 ")!=std::string::npos);
    std::puts("FRAME STATS PASS: capture within/between calls annotated; clean window resets; raw samples retained");
}
