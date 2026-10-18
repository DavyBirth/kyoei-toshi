#include "kyoei_stream_watches.h"
#include <cstdio>
#include <random>
#include <vector>
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"failed line %d\n",__LINE__);return 1;}}while(0)
struct Watch{uint64_t tick{},upper_bound{};};
int main(){
    std::vector<Watch> watches(4);
    watches[0]={7,128};watches[1]={99,999};
    CHECK(!KyoeiStreamWatches::ExtendLast(watches,0,7,256));
    CHECK(KyoeiStreamWatches::ExtendLast(watches,1,7,256));
    CHECK(watches[0].upper_bound==256 && watches[1].upper_bound==999);
    CHECK(!KyoeiStreamWatches::ExtendLast(watches,1,8,512));
    CHECK(watches[0].upper_bound==256);
    // Compare the tick protecting each requested range against a reference
    // that retains a separate fence watch for every individual allocation.
    // Includes alignment gaps, new ticks, ring wraps and storage growth.
    std::mt19937 rng(0x57a7c);
    std::vector<Watch> full;
    size_t cursor=0;uint64_t offset=0,tick=1,full_count=0,compact_count=0,wraps=0;
    const auto required=[](const auto& records,size_t count,uint64_t end){
        for(size_t i=0;i<count;++i)if(records[i].upper_bound>=end)return records[i].tick;
        return uint64_t{0};
    };
    for(unsigned i=0;i<100000;++i){
        const uint64_t size=1+rng()%512;
        offset=(offset+63)/64*64;
        if(offset+size>65536){
            for(uint64_t end=1;end<=full.back().upper_bound;end+=67)
                CHECK(required(watches,cursor,end)==required(full,full.size(),end));
            compact_count+=cursor;full_count+=full.size();
            cursor=0;full.clear();offset=0;++tick;++wraps;
        }
        if(rng()%19==0)++tick;
        offset+=size;full.push_back({tick,offset});
        if(!KyoeiStreamWatches::ExtendLast(watches,cursor,tick,offset)){
            if(cursor==watches.size())watches.resize(watches.size()*2);
            watches[cursor++]={tick,offset};
        }
        // Different ticks may share a ring; check the end of this allocation
        // and a random earlier point with the same request in both models.
        CHECK(required(watches,cursor,offset)==tick);
        const auto point=1+rng()%offset;
        CHECK(required(watches,cursor,point)==required(full,full.size(),point));
    }
    compact_count+=cursor;full_count+=full.size();
    CHECK(compact_count<full_count/4 && wraps>100);
    std::printf("stream watches: 100000 allocations, %llu wraps, identical protecting ticks; records %llu -> %llu PASS\n",
        (unsigned long long)wraps,(unsigned long long)full_count,(unsigned long long)compact_count);
}
