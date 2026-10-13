#include "kyoei_raster_trace.h"
#include "video_core/amdgpu/regs.h"
#include <cstdarg>
#include <cstdio>
#include <string>
#include <vector>
static std::vector<std::string> logs;
extern "C" void gz_log(const char *format,...) {
    char buffer[2048];va_list args;va_start(args,format);std::vsnprintf(buffer,sizeof(buffer),format,args);va_end(args);
    logs.emplace_back(buffer);
}
#define CHECK(condition) do { if (!(condition)) { std::fprintf(stderr,"Raster trace failure at line %d\n",__LINE__);return 1; } } while (0)
int main(int argc,char **) {
    _putenv_s("KYOEI_RASTER_TRACE",argc>1?"0":"1");
    using namespace KyoeiRasterTrace;
    if (argc>1) { CHECK(!Request() && !Capturing());Profile p;Record(p);Presented();CHECK(logs.empty());return 0; }
    AmdGpu::Regs regs{};
    regs.window_offset.window_x_offset=153;regs.window_scissor.window_offset_disable=0;
    regs.generic_scissor.window_offset_disable=1;
    regs.screen_scissor.bottom_right_x=1920;regs.screen_scissor.bottom_right_y=1080;
    struct View { float x=153,y=0,width=1767,height=1080; } view;
    struct Rect { struct { int x=153,y=0; } offset;struct { unsigned width=1767,height=1080; } extent; } rect;
    Profile p=MakeProfile(regs,view,rect,0);
    CHECK(p.offset[0]==153 && p.generic_disable==1 && p.window_disable==0 && p.scissor[0]==153);
    CHECK(Request());Record(p);Record(p);Presented();CHECK(Capturing());
    Record(p);Presented();CHECK(!Capturing());
    CHECK(size==1 && repetitions[0]==3 && draws==3);
    bool end=false;for(const auto &line:logs) if(line.find("capture end")!=std::string::npos)end=true;CHECK(end);
    CHECK(Request());
    for(unsigned i=0;i<300;++i) { p.ps=i;Record(p); }
    Presented();Presented();CHECK(size==256 && dropped==44 && !Capturing());
    CHECK(Request());Record(p);CHECK(Request());Presented();CHECK(Capturing());
    Record(p);Presented();Presented();CHECK(!Capturing());
    std::puts("RASTER TRACE PASS: real registers, independent flags, exact coordinates, deduplication, two frames, bounded profiles and retrigger; no GPU/window");
}
