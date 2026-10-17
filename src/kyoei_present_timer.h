#pragma once
#include <chrono>
#include <cstdlib>
#include <windows.h>
#include "common/thread.h"

namespace KyoeiPresentTimer {
// Same interval/debt accounting as Common::AccurateTimer. Only the host wait
// is replaced; guest time, vblank frequency and flip-rate selection stay intact.
class Timer {
    using Clock=std::chrono::high_resolution_clock;
    Common::AccurateTimer legacy;
    std::chrono::nanoseconds interval,wait{};
    Clock::time_point start;
    HANDLE handle=nullptr;
public:
    explicit Timer(std::chrono::nanoseconds period):legacy(period),interval(period) {
        const auto* p=std::getenv("KYOEI_HIGH_RES_PRESENT_TIMER");
        if(!p || p[0]=='1')handle=CreateWaitableTimerExW(nullptr,nullptr,CREATE_WAITABLE_TIMER_HIGH_RESOLUTION,TIMER_ALL_ACCESS);
    }
    Timer(const Timer&)=delete;
    Timer& operator=(const Timer&)=delete;
    ~Timer(){if(handle)CloseHandle(handle);}
    void Start() {
        if(!handle){legacy.Start();return;}
        const auto before=Clock::now();
        if(wait.count()>0) {
            LARGE_INTEGER due{.QuadPart=-static_cast<LONGLONG>((wait.count()+99)/100)};
            if(SetWaitableTimer(handle,&due,0,nullptr,nullptr,FALSE))WaitForSingleObject(handle,INFINITE);
            else Common::AccurateSleep(wait,nullptr,false);
        }
        start=Clock::now();
        wait-=std::chrono::duration_cast<std::chrono::nanoseconds>(start-before);
    }
    void End() {
        if(!handle){legacy.End();return;}
        wait+=interval-std::chrono::duration_cast<std::chrono::nanoseconds>(Clock::now()-start);
    }
    std::chrono::nanoseconds GetTotalWait()const{return handle?wait:legacy.GetTotalWait();}
};
}
