#include "kyoei_stutter_trace.h"
#include <fstream>
#include <iterator>
#include <cstdio>
#include <thread>
#include <algorithm>
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"trace failed line %d\n",__LINE__);return 1;}}while(0)
int main(){
    const char* file="stutter-trace-fixture.csv";
    KyoeiStutterTrace::Recorder bounded(2);
    bounded.Add({100,1,100.,25.,0,0,false});
    bounded.Add({3,2,99.,80.,0x1234,256,true});
    bounded.Add({10,2,99.,70.,0,0,false});
    auto result=bounded.Flush(file);
    CHECK(result.written && result.records==2 && result.dropped==1);
    std::ifstream input(file);std::string content(std::istreambuf_iterator<char>(input),{});input.close();
    CHECK(content.find("# dropped=1\n")==0);
    CHECK(content.find("100,1,100.000000,25.000000,0x0,0,0")!=std::string::npos);
    CHECK(content.find("3,2,99.000000,80.000000,0x1234,256,1")!=std::string::npos);
    bounded.Add({100,1,200.,25.,0,0,false});
    CHECK(!bounded.Flush(file).written);
    KyoeiStutterTrace::Recorder concurrent(8000);
    std::vector<std::thread> workers;
    for(unsigned i=0;i<4;++i)workers.emplace_back([&,i]{
        for(unsigned j=0;j<1000;++j)concurrent.Add({100,i,double(j),16.,0,0,false});
    });
    // Closing races with writers safely. Writes after closure are ignored.
    result=concurrent.Flush(file);
    for(auto& worker:workers)worker.join();
    CHECK(result.written && result.records<=4000 && result.dropped==0);
    std::ifstream joined(file);content.assign(std::istreambuf_iterator<char>(joined),{});joined.close();
    CHECK(size_t(std::count(content.begin(),content.end(),'\n'))==result.records+2);
    CHECK(std::remove(file)==0);
    std::puts("stutter trace: bounded storage, exact fields, diagnostic flags and concurrent shutdown PASS");
}
