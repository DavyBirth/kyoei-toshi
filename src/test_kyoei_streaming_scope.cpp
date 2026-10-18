#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <string>
#include <thread>
#include <vector>
static std::vector<std::string> logs;
extern "C" void gz_log(const char* format,...) {
    char text[1024];va_list args;va_start(args,format);std::vsnprintf(text,sizeof(text),format,args);va_end(args);
    logs.emplace_back(text);
}
#ifndef KYOEI_STREAMING_TEST_HEADER
#define KYOEI_STREAMING_TEST_HEADER "kyoei_streaming_trace.h"
#endif
#include KYOEI_STREAMING_TEST_HEADER
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"Streaming scope failed line %d\n",__LINE__);return 1;}}while(0)
int main(int argc,char** argv) {
    const std::string mode=argc>1?argv[1]:"disabled";
    const bool streaming=mode=="streaming" || mode=="both";
    const bool detailed=mode=="detailed" || mode=="both";
    CHECK(mode=="disabled" || streaming || detailed);
    _putenv_s("KYOEI_STREAMING_TRACE",streaming?"1":"0");
    _putenv_s("KYOEI_STUTTER_PACKETS",detailed?"1":"0");
    const char* path="streaming-scope-fixture.csv";
    _putenv_s("KYOEI_STUTTER_TRACE",detailed?path:"");
    if(mode=="disabled") {
        for(unsigned i=0;i<100000;++i){KyoeiStreamingTrace::Scope scope("draw",0x1234,256,4,8);}
        CHECK(logs.empty() && !KyoeiStutterTrace::Flush().written);
    } else if(mode=="detailed") {
        const char* labels[]={"gfx-packet-inclusive","draw","find-image","refresh-image",
                              "texture-gc","image-buffer","command-callbacks","unknown"};
        for(unsigned i=0;i<8;++i){
            if(i==7)KyoeiDiagnosticActivity::Begin();
            KyoeiStreamingTrace::Scope scope(labels[i],0x40+i,128);
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
            if(i==7)KyoeiDiagnosticActivity::End();
        }
        const auto result=KyoeiStutterTrace::Flush();CHECK(result.written && result.records==8 && result.dropped==0);
        std::ifstream file(path);std::string line;
        CHECK(std::getline(file,line) && line=="# dropped=0");CHECK(bool(std::getline(file,line)));
        for(unsigned i=0;i<8;++i){
            CHECK(bool(std::getline(file,line)));
            CHECK(line.starts_with(std::to_string(i<7?200+i:299)+","));
            char suffix[64];std::snprintf(suffix,sizeof(suffix),",0x%x,128,%u",0x40+i,i==7?1u:0u);
            CHECK(line.ends_with(suffix));
        }
        CHECK(!std::getline(file,line));file.close();CHECK(std::remove(path)==0);CHECK(logs.empty());
    } else {
        for(unsigned i=0;i<10;++i){
            KyoeiStreamingTrace::Scope scope("draw",0x1234,256,4,8);
            std::this_thread::sleep_for(std::chrono::milliseconds(27));
        }
        CHECK(logs.size()==8);
        for(const auto& line:logs){
            CHECK(line.find("stage=draw milliseconds=")!=std::string::npos);
            CHECK(line.ends_with("address=0x1234 bytes=256 width=4 height=8"));
        }
        const auto result=KyoeiStutterTrace::Flush();
        if(detailed){CHECK(result.written && result.records==10 && result.dropped==0);CHECK(std::remove(path)==0);}
        else CHECK(!result.written);
    }
    std::printf("STREAMING SCOPE PASS: %s\n",mode.c_str());
}
