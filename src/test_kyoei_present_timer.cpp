#include <string>
#include <windows.h>
#include "common/thread.h"
#include <cstdio>
#include <cstdlib>
#include <thread>

namespace {
int starts=0,ends=0,closed=0,waits=0,fallbacks=0;
bool supported=true,armed=true;
LONGLONG requested=0;
HANDLE CreateTimer(LPSECURITY_ATTRIBUTES,LPCWSTR,DWORD flags,DWORD) {
    if(flags!=CREATE_WAITABLE_TIMER_HIGH_RESOLUTION)std::abort();
    return supported?reinterpret_cast<HANDLE>(1):nullptr;
}
BOOL ArmTimer(HANDLE,const LARGE_INTEGER* due,LONG,PTIMERAPCROUTINE,LPVOID,BOOL) {
    requested=due->QuadPart;return armed;
}
DWORD WaitTimer(HANDLE,DWORD){++waits;return WAIT_OBJECT_0;}
BOOL CloseTimer(HANDLE){++closed;return TRUE;}
}
#define CreateWaitableTimerExW CreateTimer
#define SetWaitableTimer ArmTimer
#define WaitForSingleObject WaitTimer
#define CloseHandle CloseTimer
#include "kyoei_present_timer.h"
#undef CreateWaitableTimerExW
#undef SetWaitableTimer
#undef WaitForSingleObject
#undef CloseHandle

// The legacy implementation is a sentinel: the high-resolution path must
// preserve fallback without accidentally changing the guest-facing clock.
namespace Common {
AccurateTimer::AccurateTimer(std::chrono::nanoseconds value):target_interval(value){}
void AccurateTimer::Start(){++starts;}
void AccurateTimer::End(){++ends;}
bool AccurateSleep(std::chrono::nanoseconds,std::chrono::nanoseconds*,bool){++fallbacks;return true;}
}
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"failed line %d\n",__LINE__);return 1;}}while(0)
int main(){
    using namespace std::chrono_literals;
    _putenv_s("KYOEI_HIGH_RES_PRESENT_TIMER","0");
    {KyoeiPresentTimer::Timer t(16ms);t.Start();t.End();CHECK(starts==1&&ends==1);}
    _putenv_s("KYOEI_HIGH_RES_PRESENT_TIMER","1");
    supported=false;
    {KyoeiPresentTimer::Timer t(16ms);t.Start();t.End();CHECK(starts==2&&ends==2);}
    supported=true;
    {
        KyoeiPresentTimer::Timer t(100ms);
        t.Start();CHECK(waits==0);t.End();
        CHECK(t.GetTotalWait()>0ns);
        t.Start();CHECK(waits==1&&requested<0&&requested>=-1000000);
        armed=false;t.End();t.Start();CHECK(fallbacks==1);
        CHECK(starts==2&&ends==2);
    }
    CHECK(closed==1);
    armed=true;
    {
        KyoeiPresentTimer::Timer t(1ms);
        t.Start();std::this_thread::sleep_for(5ms);t.End();
        CHECK(t.GetTotalWait()<0ns);
        t.Start();CHECK(waits==1); // no extra delay when the frame is late
    }
    CHECK(closed==2);
    std::puts("present timer PASS");
}
