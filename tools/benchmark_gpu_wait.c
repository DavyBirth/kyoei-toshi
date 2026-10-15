/* Compare real-label wait implementations, without running guest code. */
#include "gz.h"
#include "kyoei_gpu_wait.h"
#include <windows.h>
#include <mmsystem.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
void gz_log(const char *format, ...) {(void)format;}
void gz_stop(int code,const char *format,...) {(void)format;ExitProcess(code);}
static HANDLE request,ready;
static volatile LONG64 fence;
static volatile LONG done;
static LARGE_INTEGER frequency;
static unsigned delay_us;
static DWORD WINAPI producer(void *unused) {
    (void)unused;
    for (;;) {
        WaitForSingleObject(request,INFINITE);
        if(InterlockedCompareExchange(&done,0,0))return 0;
        LARGE_INTEGER start,now;QueryPerformanceCounter(&start);
        do {YieldProcessor();QueryPerformanceCounter(&now);}
        while(now.QuadPart-start.QuadPart < frequency.QuadPart*delay_us/1000000);
        InterlockedExchange64(&fence,1);SetEvent(ready);
    }
}
static int old_wait(const volatile uint64_t *p,uint64_t expected,unsigned timeout) {
    ULONGLONG start=GetTickCount64();
    for (;;) {if(__atomic_load_n(p,__ATOMIC_ACQUIRE)==expected)return 1;
        if(GetTickCount64()-start>=timeout)return 0;Sleep(1);}
}
static uint64_t cpu_ticks(void) {
    ULONG64 cycles;
    if(!QueryThreadCycleTime(GetCurrentThread(),&cycles))abort();
    return cycles;
}
int main(int argc,char **argv) {
    int fine=argc==2&&!strcmp(argv[1],"--timer-1ms");
    if(argc>1&&!fine)return 3;
    if(fine&&timeBeginPeriod(1))return 4;
    printf("Windows timer request: %s; fixture latency, not game FPS\n",fine?"1 ms":"default");
    QueryPerformanceFrequency(&frequency);
    request=CreateEvent(NULL,FALSE,FALSE,NULL);ready=CreateEvent(NULL,FALSE,FALSE,NULL);
    HANDLE thread=CreateThread(NULL,0,producer,NULL,0,NULL);
    if(!request||!ready||!thread)return 1;
    for(unsigned delay=50;delay<=800;delay*=4) {
        delay_us=delay;double total[2]={0,0},cpu[2]={0,0};
        for(unsigned i=0;i<400;++i) {
            unsigned mode=i%2;InterlockedExchange64(&fence,0);
            LARGE_INTEGER start,end;uint64_t cpu_start=cpu_ticks();QueryPerformanceCounter(&start);SetEvent(request);
            int ok=(mode?kyoei_wait_real_label:old_wait)((const volatile uint64_t*)&fence,1,1000);
            QueryPerformanceCounter(&end);
            cpu[mode]+=(double)(cpu_ticks()-cpu_start);
            if(!ok||fence!=1)return 2;
            WaitForSingleObject(ready,INFINITE);
            total[mode]+=(end.QuadPart-start.QuadPart)*1000000.0/frequency.QuadPart;
        }
        printf("delay_us=%u old_mean_us=%.2f adaptive_mean_us=%.2f old_cpu_cycles=%.0f adaptive_cpu_cycles=%.0f samples=200 each\n",delay,total[0]/200,total[1]/200,cpu[0]/200,cpu[1]/200);
    }
    InterlockedExchange(&done,1);SetEvent(request);WaitForSingleObject(thread,INFINITE);
    CloseHandle(thread);CloseHandle(request);CloseHandle(ready);
    if(fine)timeEndPeriod(1);
    return 0;
}
