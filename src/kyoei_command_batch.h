#pragma once
#include "kyoei_frame_stats.h"

namespace KyoeiFrameStats {
// Worker-stack scope only. Never keep a timer inside a suspended coroutine.
class CommandBatch {
    using Clock = std::chrono::steady_clock;
    inline static thread_local unsigned reports = 0;
    bool active = Enabled() && reports < 8;
    Clock::time_point start{};
    KyoeiDiagnosticActivity::Stamp diagnostic{};
    uint64_t resumes = 0, completed = 0, empty = 0;
    double resume_ms = 0, max_resume_ms = 0;
public:
    CommandBatch() {
        if (active) { start=Clock::now(); diagnostic=KyoeiDiagnosticActivity::Now(); }
    }
    void Empty() { if (active) ++empty; }
    template<class Task> void Resume(Task task) {
        if (!active) { task.resume(); return; }
        const auto begin=Clock::now();
        task.resume();
        Record(std::chrono::duration<double,std::milli>(Clock::now()-begin).count(),task.done());
    }
    void Record(double milliseconds, bool done) {
        if (!active) return;
        ++resumes; completed+=done;
        resume_ms+=milliseconds; max_resume_ms=std::max(max_resume_ms,milliseconds);
    }
    void Finish(double milliseconds, double end_ms) {
        if (!active) return;
        active=false;
        if (milliseconds<50.) return;
        ++reports;
        gz_log("Kyoei command batch: milliseconds=%.2f resume_ms=%.2f max_resume_ms=%.2f resumes=%llu completed=%llu yielded=%llu empty=%llu end_ms=%.2f diagnostic=%u",
            milliseconds,resume_ms,max_resume_ms,(unsigned long long)resumes,
            (unsigned long long)completed,(unsigned long long)(resumes-completed),
            (unsigned long long)empty,end_ms,
            unsigned(KyoeiDiagnosticActivity::Overlaps(diagnostic,KyoeiDiagnosticActivity::Now())));
    }
    ~CommandBatch() {
        if (!active) return;
        const auto end=Clock::now();
        Finish(std::chrono::duration<double,std::milli>(end-start).count(),
               std::chrono::duration<double,std::milli>(end.time_since_epoch()).count());
    }
};
}
