#pragma once
#include <atomic>
#include <cstdlib>
#include <functional>
#include <memory>
#include <utility>
#include <vector>

extern "C" void gz_log(const char*,...);
namespace KyoeiCooperativeReadback {
struct Completion {
    std::atomic<bool> done{false};
    bool Ready() const { return done.load(std::memory_order_acquire); }
    void Wait() const { done.wait(false,std::memory_order_acquire); }
    void Finish() { done.store(true,std::memory_order_release);done.notify_all(); }
};
struct Context { bool recording=false;std::vector<std::function<void()>> publications; };
inline thread_local Context* current=nullptr;
class Activation {
    Context* previous;
public:
    explicit Activation(Context& context):previous(current){current=&context;}
    ~Activation(){current=previous;}
};
class Recording {
    Context& context;
    bool previous;
public:
    explicit Recording(Context& context_):context(context_),previous(context.recording){context.recording=true;}
    ~Recording(){context.recording=previous;}
};
inline bool Enabled() {
    static const bool enabled=[]{const char* value=std::getenv("KYOEI_COOPERATIVE_READBACK");return value && value[0]=='1';}();
    return enabled;
}
inline void Report(unsigned submitted,unsigned polls,bool slept) {
    static thread_local unsigned reports=0;
    if(reports<8){++reports;gz_log("Kyoei cooperative completion: queued_submits=%u resumes=%u slept=%u; publication precedes native fence",submitted,polls,unsigned(slept));}
}
template<class Scheduler,class Download> std::shared_ptr<Completion> Process(Scheduler& scheduler,Download&& download) {
    if(!Enabled()){download();return {};}
    Context context;
    { Activation activation(context);download(); }
    if(context.publications.empty())return {};
    auto completion=std::make_shared<Completion>();
    const auto images=context.publications.size();
    scheduler.DeferPriorityOperation([publications=std::move(context.publications),completion]() mutable {
        for(auto& publish:publications)publish();
        completion->Finish();
    });
    scheduler.Flush();
    static thread_local unsigned reports=0;
    if(reports<8){++reports;gz_log("Kyoei cooperative readback: images=%u payload_bytes=%u; original event waits for publication",unsigned(images),unsigned(images*4));}
    return completion;
}
}
