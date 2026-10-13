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
    for(unsigned i=0;i<profiles.size()+44;++i) { p.ps=i;Record(p); }
    Presented();Presented();CHECK(size==profiles.size() && dropped==44 && !Capturing());
    CHECK(Request());Record(p);CHECK(Request());Presented();CHECK(Capturing());
    Record(p);Presented();Presented();CHECK(!Capturing());
    VertexProbe vertex;vertex.address=100;vertex.bytes=32;vertex.stride=8;vertex.format=103;
    unsigned copies=0;SampleXY(vertex,[](auto,auto){return true;},[](auto,auto){return false;},[&](uint64_t address,void *out,auto){
        float xy[]={float(address-100),42};std::memcpy(out,xy,8);++copies;return true;});
    CHECK(vertex.status==1 && copies==4 && vertex.xy[6]==24);
    SampleXY(vertex,[](auto,auto){return true;},[](auto,auto){return true;},[&](auto,void *,auto){++copies;return true;});
    CHECK(vertex.status==2 && copies==4);
    vertex.bytes=4;SampleXY(vertex,[](auto,auto){return true;},[](auto,auto){return false;},[&](auto,void *,auto){++copies;return true;});
    CHECK(vertex.status==4 && copies==4);
    vertex.bytes=32;SampleXY(vertex,[](auto,auto){return true;},[](auto,auto){return false;},[](auto,void *,auto){return false;});
    CHECK(vertex.status==7 && vertex.xy[0]==0);
    // Indexed UI subdraw: the selected vertices are nowhere near buffer start.
    DrawSelection selection;selection.valid=1;selection.indexed=1;selection.index_bytes=2;
    selection.address=1000;selection.count=3;selection.base_vertex=-2;
    vertex.address=100;vertex.bytes=800;vertex.stride=8;
    auto selected_copy=[](uint64_t address,void *out,uint64_t bytes) {
        if(address==1000) { uint16_t indices[]={20,22,21};std::memcpy(out,indices,bytes); }
        else { float values[]={float(address-100),1080};std::memcpy(out,values,bytes); }
        return true;
    };
    auto readable=[](auto,auto){return true;};auto clean=[](auto,auto){return false;};
    SampleSelectedXY(vertex,selection,readable,clean,selected_copy);
    CHECK(vertex.status==1 && vertex.sample_count==3 && vertex.selected_indices[0]==18 && vertex.xy[0]==144 && vertex.xy[4]==152);
    selection.index_bytes=4;
    SampleSelectedXY(vertex,selection,readable,clean,[](uint64_t address,void *out,uint64_t bytes){
        if(address==1000) { uint32_t indices[]={20,22,21};std::memcpy(out,indices,bytes); }
        else { float values[]={1,2};std::memcpy(out,values,bytes); }return true;});
    CHECK(vertex.status==1 && vertex.selected_indices[1]==20);
    SampleSelectedXY(vertex,selection,readable,[](auto,auto){return true;},selected_copy);CHECK(vertex.status==2 && vertex.sample_count==0);
    selection.indexed=0;selection.base_vertex=17;selection.count=4;
    SampleSelectedXY(vertex,selection,readable,clean,selected_copy);CHECK(vertex.status==1 && vertex.selected_indices[3]==20);
    selection.base_vertex=1000;SampleSelectedXY(vertex,selection,readable,clean,selected_copy);CHECK(vertex.status==4 && vertex.sample_count==0);
    selection.valid=0;SampleSelectedXY(vertex,selection,readable,clean,selected_copy);CHECK(vertex.status==8);
    std::puts("RASTER TRACE PASS: real registers, independent flags, exact coordinates, deduplication, two frames, bounded profiles and retrigger; no GPU/window");
}
