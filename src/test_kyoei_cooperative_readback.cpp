#include "kyoei_cooperative_readback.h"
#include <array>
#include <cstdio>
#include <cstring>
extern "C" void gz_log(const char*,...) {}
struct Scheduler {
    unsigned flushes=0;
    bool gpu_done=false;
    std::vector<std::function<void()>> pending;
    template<class F> void DeferPriorityOperation(F&& operation){pending.emplace_back(std::forward<F>(operation));}
    void Flush(){++flushes;}
    void Run(){if(!gpu_done)return;auto work=std::move(pending);pending.clear();for(auto& operation:work)operation();}
};
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"cooperative readback failed: %d\n",__LINE__);return 1;}}while(0)
int main(int argc,char**) {
    using namespace KyoeiCooperativeReadback;
    _putenv_s("KYOEI_COOPERATIVE_READBACK",argc>1?"0":"1");
    Scheduler scheduler;
    unsigned downloads=0;
    if(argc>1) {
        auto completion=Process(scheduler,[&]{++downloads;});
        CHECK(downloads==1 && !completion && current==nullptr && scheduler.flushes==0);
        return 0;
    }
    auto empty=Process(scheduler,[]{ });
    CHECK(!empty && scheduler.flushes==0 && current==nullptr);
    std::array<unsigned char,256> destination;destination.fill(0xCA);
    auto owned=std::make_shared<std::array<unsigned char,4>>(std::array<unsigned char,4>{1,2,3,4});
    std::weak_ptr<std::array<unsigned char,4>> lifetime=owned;
    auto completion=Process(scheduler,[&] {
        ++downloads;
        Recording recording(*current);
        current->publications.emplace_back([owned,&destination]{std::memcpy(destination.data(),owned->data(),4);});
    });
    CHECK(downloads==1 && completion && !completion->Ready() && scheduler.flushes==1 && current==nullptr);
    owned.reset();CHECK(!lifetime.expired());
    scheduler.Run();CHECK(!completion->Ready() && destination[0]==0xCA);
    // A second logical queue can progress while the first completion is pending.
    unsigned second_queue=0;
    auto other=Process(scheduler,[&]{current->publications.emplace_back([&]{++second_queue;});});
    CHECK(other && !other->Ready() && scheduler.flushes==2 && current==nullptr);
    scheduler.gpu_done=true;scheduler.Run();
    completion->Wait();other->Wait();
    CHECK(completion->Ready() && other->Ready() && second_queue==1 && lifetime.expired());
    CHECK(destination[0]==1 && destination[1]==2 && destination[2]==3 && destination[3]==4);
    for(size_t i=4;i<destination.size();++i)CHECK(destination[i]==0xCA);
    // Native event signalling remains outside Process, after the completion.
    unsigned fence=0;
    CHECK(fence==0 && completion->Ready());fence=1;CHECK(fence==1);
    std::puts("COOPERATIVE READBACK PASS: GPU gate, owned staging, TLS restoration, multiple queues, padding and event ordering");
}
