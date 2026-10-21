#pragma once
#include <chrono>
#include "kyoei_stutter_trace.h"

// Stutter-trace only: wall time a scope holds (or spends inside) a shared lock.
// Stages 300+ are listed in tools/analyze_stutter_trace.py.
struct KyoeiLockHold {
    unsigned stage;
    std::chrono::steady_clock::time_point start=std::chrono::steady_clock::now();
    explicit KyoeiLockHold(unsigned s):stage(s){}
    ~KyoeiLockHold(){
        if(!KyoeiStutterTrace::Path())return;
        const auto end=std::chrono::steady_clock::now();
        const double ms=std::chrono::duration<double,std::milli>(end-start).count();
        if(ms>=4.)KyoeiStutterTrace::Record(stage,end,ms,false);
    }
};
