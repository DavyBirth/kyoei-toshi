#pragma once
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstdlib>

extern "C" void gz_log(const char*,...);

namespace KyoeiStreamWatches {
inline bool Enabled(){
    static const bool enabled=[]{auto* p=std::getenv("KYOEI_MERGE_STREAM_WATCHES");return !p||p[0]!='0';}();
    static const auto* control=std::getenv("KYOEI_STREAM_WATCH_CONTROL");
    if(!control)return enabled;
    static thread_local unsigned calls=0;
    static thread_local bool active=false;
    if((calls++&4095)==0){
        bool next=false;
        if(auto* f=std::fopen(control,"rb")){next=std::fgetc(f)=='1';std::fclose(f);}
        if(next!=active){active=next;gz_log("Kyoei stream watches: merged=%u",unsigned(active));}
    }
    return active;
}
// cursor is the next free slot. Extend the preceding watch, which already
// owns this tick; looking at cursor instead creates a watch for every copy.
template<class Watches>
bool ExtendLast(Watches& watches,size_t cursor,uint64_t tick,uint64_t upper_bound){
    if(!cursor || watches[cursor-1].tick!=tick)return false;
    watches[cursor-1].upper_bound=upper_bound;
    return true;
}
}
