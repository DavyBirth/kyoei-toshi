#include "kyoei_vertex_ranges.h"
#include <chrono>
#include <cstdint>
#include <cstdio>
#include <random>
#include <vector>
struct Range {
    uint64_t base_address,end_address;
    bool operator==(const Range&) const=default;
};
using Ranges=std::vector<Range>;
Ranges Reference(Ranges input) {
    Ranges result;
    std::ranges::sort(input,{},&Range::base_address);
    for(const auto& range:input) {
        if(result.empty() || result.back().end_address<range.base_address)result.push_back(range);
        else result.back().end_address=std::max(result.back().end_address,range.end_address);
    }
    return result;
}
Ranges Optimized(Ranges input) {
    Ranges result;
    if(!KyoeiVertexRanges::TryConnected(input,result))return Reference(std::move(input));
    return result;
}
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"vertex ranges failed: %d\n",__LINE__);return 1;}}while(0)
int main(int argc,char**) {
    const std::vector<Ranges> cases={ {},{{1,2}},{{8,24},{0,16},{16,32}},
        {{0,8},{16,24},{4,20}},{{0,8},{8,16}},{{8,9},{0,1}},{{7,9},{7,9}},
        {{UINT64_MAX-16,UINT64_MAX-8},{UINT64_MAX-8,UINT64_MAX}} };
    for(const auto& ranges:cases)CHECK(Optimized(ranges)==Reference(ranges));
    Ranges sentinel={{77,88}};
    CHECK(!KyoeiVertexRanges::TryConnected(Ranges{{0,2},{4,6}},sentinel));
    CHECK((sentinel==Ranges{{77,88}}));
    std::mt19937_64 random(17);
    for(unsigned test=0;test<100000;++test) {
        Ranges ranges;
        const unsigned count=random()%33;
        for(unsigned i=0;i<count;++i) {
            auto begin=random()%256;
            ranges.push_back({begin,begin+1+random()%128});
        }
        CHECK(Optimized(ranges)==Reference(ranges));
    }
    std::puts("VERTEX RANGES PASS: 100000 randomized unions, gaps, bridges, duplicates, adjacency and high addresses");
    if(argc>1) {
        std::vector<Ranges> fixtures;
        for(unsigned i=0;i<64;++i) {
            Ranges ranges;
            for(unsigned j=0;j<8;++j) {
                auto begin=random()%64;
                ranges.push_back({begin,begin+128+random()%64});
            }
            fixtures.push_back(ranges);
        }
        uint64_t reference_sum=0,optimized_sum=0;
        const auto bench=[&](auto function,uint64_t& sum) {
            const auto begin=std::chrono::steady_clock::now();
            for(unsigned i=0;i<200000;++i) {
                const auto output=function(fixtures[i%fixtures.size()]);
                sum+=output.front().base_address+output.front().end_address;
            }
            return std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-begin).count();
        };
        const auto reference_ms=bench(Reference,reference_sum);
        const auto optimized_ms=bench(Optimized,optimized_sum);
        CHECK(reference_sum==optimized_sum);
        std::printf("VERTEX RANGE CPU FIXTURE: overlapping views=8 iterations=200000 reference_ms=%.2f optimized_ms=%.2f checksum=%llu (not gameplay FPS)\n",reference_ms,optimized_ms,(unsigned long long)optimized_sum);
    }
}
