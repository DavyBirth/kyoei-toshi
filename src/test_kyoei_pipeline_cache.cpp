#include "kyoei_pipeline_cache.h"
#include <cassert>
#include <future>
#include <cstdarg>
#include <string>
static std::vector<std::string> logs;
extern "C" void gz_log(const char *format,...) {
    char line[512];va_list args;va_start(args,format);
    std::vsnprintf(line,sizeof(line),format,args);va_end(args);logs.emplace_back(line);
}
int main() {
    using namespace KyoeiPipelineCache;
    _putenv_s("KYOEI_FRAME_STATS","0");
    struct Profile { uint32_t a,b; } profile{7,11};
    std::vector<uint8_t> profile_data(sizeof(profile));std::memcpy(profile_data.data(),&profile,sizeof(profile));
    assert(ProfileMatches(profile_data,profile));assert(!ProfileMatches(std::span(profile_data).first(1),profile));
    profile_data.back()^=1;assert(!ProfileMatches(profile_data,profile));
    assert(logs.empty());
    _putenv_s("KYOEI_FRAME_STATS","1");
    assert(!ProfileMatches(std::span(profile_data).first(1),profile));
    assert(logs.size()==1 && logs.back().find("cached_bytes=1 current_bytes=8")!=std::string::npos);
    logs.clear();
    struct LargeProfile { uint8_t bytes[32]; } larger{};
    std::vector<uint8_t> incompatible(32,255);
    assert(!ProfileMatches(incompatible,larger));
    assert(logs.size()==9 && logs.back().find("offset=7 cached=255 current=0")!=std::string::npos);
    logs.clear();
    std::fill(incompatible.begin(),incompatible.end(),0);
    assert(ProfileMatches(incompatible,larger) && logs.empty());
    _putenv_s("KYOEI_FRAME_STATS","0");
    Identity id{0x10de,0x1234,{1,2,3}};
    std::vector<uint8_t> bytes(48,0);uint32_t header[]={32,1,id.vendor,id.device};
    std::memcpy(bytes.data(),header,16);std::memcpy(bytes.data()+16,id.uuid.data(),16);
    assert(Matches(bytes,id));assert(!Matches(std::span(bytes).first(31),id));
    auto changed=id;changed.vendor++;assert(!Matches(bytes,changed));
    changed=id;changed.device++;assert(!Matches(bytes,changed));
    changed=id;changed.uuid[15]++;assert(!Matches(bytes,changed));
    bytes[4]=2;assert(!Matches(bytes,id));bytes[4]=1;
    _putenv_s("KYOEI_PIPELINE_CACHE_DIR","pipeline-cache-test");
    assert(Save(id,bytes));assert(Load(id)==bytes);
    bytes.back()=42;assert(Save(id,bytes));assert(Load(id)==bytes);
    assert(Writer().Submit([id,bytes] {return Save(id,bytes);}));
    assert(Writer().Flush(std::chrono::seconds(2)));assert(Load(id)==bytes);
    auto invalid=bytes;invalid[4]=2;
    assert(Writer().Submit([id,invalid] {return Save(id,invalid);}));
    assert(Writer().Flush(std::chrono::seconds(2)));assert(Load(id)==bytes);
    assert(Writer().GetStatus().failed==1 && !Writer().GetStatus().last_success);
    {
        Jobs jobs;std::promise<void> entered,release;auto entered_future=entered.get_future();
        auto release_future=release.get_future().share();int latest=0;
        assert(jobs.Submit([&] {entered.set_value();release_future.wait();latest=1;return true;}));
        assert(entered_future.wait_for(std::chrono::seconds(2))==std::future_status::ready);
        assert(jobs.Submit([&] {latest=2;return true;}));
        assert(jobs.Submit([&] {latest=3;return true;}));
        assert(!jobs.Flush(std::chrono::milliseconds(5)));
        release.set_value();assert(jobs.Flush(std::chrono::seconds(2)));
        assert(latest==3 && jobs.GetStatus().completed==2);
    }
    CheckpointGate gate;gate.Record(1,1000,true);
    assert(!gate.Poll(1));assert(!gate.Due(30999));assert(gate.Due(31000));
    bool polled=false;for(unsigned i=0;i<256;++i)polled|=gate.Poll(2);assert(polled);
    gate.Record(2,31000,false);assert(gate.saved_generation==1 && !gate.Due(60999));
    gate.Record(2,61000,true);assert(gate.saved_generation==2);
    auto file=Path(id);{std::ofstream f(file,std::ios::binary|std::ios::trunc);f<<"truncated";}
    assert(Load(id).empty());std::filesystem::remove(file);std::filesystem::remove(file.parent_path());
    _putenv_s("KYOEI_PIPELINE_CACHE_DIR","");assert(Load(id).empty());assert(!Save(id,bytes));
    std::puts("PIPELINE CACHE PASS: async write/readback, invalid snapshot preserves file, latest pending wins, bounded flush and throttling");
}
