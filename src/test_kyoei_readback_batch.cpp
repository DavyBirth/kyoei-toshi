#include "kyoei_readback_batch.h"
#include <array>
#include <cstdio>
#include <cstdlib>
#define CHECK(x) do { if (!(x)) { std::fprintf(stderr,"line %d failed\n",__LINE__); std::exit(1); } } while(0)
int main() {
    std::array<unsigned char,16> ring{}; std::array<unsigned char,3> guest{};
    unsigned waits=0;
    {
        KyoeiReadback::Batch batch([&] { ++waits; ring[0]=17; ring[4]=23; });
        batch.Add(0,4,[&] { guest[0]=ring[0]; });
        batch.Add(4,4,[&] { guest[1]=ring[4]; });
        CHECK(!guest[0] && !guest[1] && !waits);
        batch.BeforeReuse(8,4); CHECK(!waits);
        batch.BeforeReuse(0,4); CHECK(waits==1 && guest[0]==17 && guest[1]==23);
        ring[0]=91; batch.Add(0,4,[&] { guest[2]=ring[0]; });
        CHECK(batch.images==3 && batch.bytes==12); batch.Flush();
        CHECK(waits==2 && guest[2]==17 && batch.waits==2);
    }
    CHECK(waits==2);
    {
        KyoeiReadback::Batch batch([&] { ++waits; ring[12]=42; });
        batch.Add(12,4,[&] { guest[2]=ring[12]; });
    }
    CHECK(waits==3 && guest[2]==42);
    {
        KyoeiReadback::Batch batch([&] { ++waits; });
        {
            KyoeiReadback::Activation observe(batch);
            KyoeiReadback::RecordSingle(32);KyoeiReadback::RecordSingle(64);
            CHECK(batch.images==2 && batch.waits==2 && batch.bytes==96);
            KyoeiReadback::Batch nested([&] { ++waits; });
            { KyoeiReadback::Activation inner(nested);KyoeiReadback::RecordSingle(8); }
            CHECK(nested.images==1 && nested.waits==1 && nested.bytes==8);
            CHECK(KyoeiReadback::observing==&batch);
        }
        CHECK(!KyoeiReadback::observing);
        CHECK(waits==3); // Observation adds no real wait/publication.
    }
    puts("READBACK BATCH PASS: one fence for grouped copies, no early publication, ring overlap flushed, scope completion");
}
