#include "kyoei_fetch_cache.h"
#include <cstdio>
#include <random>
#define CHECK(x) do { if (!(x)) { std::fprintf(stderr,"failed line %d\n",__LINE__); return 1; } } while(0)
int main() {
    KyoeiFetchCache::Cache<std::array<unsigned char,16>> cache;
    alignas(4096) std::array<unsigned char,4096> page{};
    const auto* code=page.data();
    auto decode=[&] { std::array<unsigned char,16> value; std::memcpy(value.data(),code,value.size()); return value; };
    CHECK(!cache.Probe(code));cache.Record(code,16,decode());
    CHECK(!cache.Probe(code));cache.Record(code,16,decode());CHECK(*cache.Probe(code)==decode());
    std::mt19937 rng(0x60f17);
    for(unsigned i=0;i<10000;++i) {
        if(i%3==0) page[rng()%16]^=1;
        const auto expected=decode();
        if(const auto* hit=cache.Probe(code)) { CHECK(*hit==expected); }
        else cache.Record(code,16,expected);
        CHECK(*cache.Probe(code)==decode());
    }
    // A mutation during parsing cannot associate the old result with new code.
    page[0]^=1;CHECK(!cache.Probe(code));auto old=decode();page[0]^=2;cache.Record(code,16,old);
    CHECK(!cache.Probe(code));cache.Record(code,16,decode());CHECK(*cache.Probe(code)==decode());
    // First observation also cannot authorize reuse after a concurrent change.
    KyoeiFetchCache::Cache<std::array<unsigned char,16>> cold;
    CHECK(!cold.Probe(code));old=decode();page[0]^=4;cold.Record(code,16,old);
    CHECK(!cold.Probe(code));cold.Record(code,16,decode());CHECK(*cold.Probe(code)==decode());
    CHECK(!cache.Fits(code,513));CHECK(!cache.Fits(code,0));CHECK(!cache.Fits(nullptr,16));
    CHECK(cache.Fits(code+4092,4));CHECK(!cache.Fits(code+4092,8));
    CHECK(!cache.Probe(code));cache.Record(code,513,decode());CHECK(!cache.Probe(code));
    // Changing consumed length needs another enclosed parse.
    cache.Record(code,12,decode());CHECK(!cache.Probe(code));cache.Record(code,12,decode());
    CHECK(*cache.Probe(code)==decode());
    auto* other=page.data()+512;
    CHECK(cache.SlotFor(code)==cache.SlotFor(other));
    CHECK(!cache.Probe(other));cache.Record(other,16,{});CHECK(!cache.Probe(code));
    std::puts("fetch cache: 10000 mutations, enclosing snapshots, length/page limits and collisions PASS");
}
