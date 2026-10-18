#pragma once
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

namespace KyoeiStutterTrace {
inline constexpr unsigned Frame=100;
struct Event {
    unsigned stage;
    uint64_t thread;
    double end_ms,duration_ms;
    uint64_t address,bytes;
    bool diagnostic;
};
struct Result { bool written{};size_t records{},dropped{}; };
class Recorder {
    std::mutex mutex;
    std::vector<Event> events;
    size_t limit,dropped{};
    bool closed{};
public:
    explicit Recorder(size_t capacity=131072):limit(capacity){events.reserve(capacity);}
    void Add(Event event){
        std::lock_guard lock(mutex);
        if(closed)return;
        if(events.size()==limit){++dropped;return;}
        events.push_back(event);
    }
    Result Flush(const char* path){
        std::vector<Event> snapshot;
        size_t lost=0;
        {
            std::lock_guard lock(mutex);
            if(closed)return {};
            closed=true;lost=dropped;snapshot.swap(events);
        }
        auto* file=std::fopen(path,"wb");
        if(!file)return {false,snapshot.size(),lost};
        std::vector<char> buffer(64*1024);
        std::setvbuf(file,buffer.data(),_IOFBF,buffer.size());
        std::fprintf(file,"# dropped=%llu\nstage,thread,end_ms,duration_ms,address,bytes,diagnostic\n",(unsigned long long)lost);
        for(const auto& event:snapshot)
            std::fprintf(file,"%u,%llu,%.6f,%.6f,0x%llx,%llu,%u\n",event.stage,
                (unsigned long long)event.thread,event.end_ms,event.duration_ms,
                (unsigned long long)event.address,(unsigned long long)event.bytes,unsigned(event.diagnostic));
        const bool ok=std::ferror(file)==0;
        const int close=std::fclose(file);
        return {ok && close==0,snapshot.size(),lost};
    }
};
inline const char* Path(){
    static const std::string path=[]{auto* p=std::getenv("KYOEI_STUTTER_TRACE");return std::string(p?p:"");}();
    return path.empty()?nullptr:path.c_str();
}
inline Recorder& State(){
    // GPU/present threads may still exist during ExitProcess. Keep the
    // closed recorder alive until process teardown instead of racing a dtor.
    static auto* state=new Recorder;
    return *state;
}
inline void Record(unsigned stage,std::chrono::steady_clock::time_point end,
                   double duration_ms,bool diagnostic,uint64_t address=0,uint64_t bytes=0){
    if(!Path())return;
    static thread_local const auto thread=std::hash<std::thread::id>{}(std::this_thread::get_id());
    const auto end_ms=std::chrono::duration<double,std::milli>(end.time_since_epoch()).count();
    State().Add({stage,thread,end_ms,duration_ms,address,bytes,diagnostic});
}
inline Result Flush(){return Path()?State().Flush(Path()):Result{};}
}
