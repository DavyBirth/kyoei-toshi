#include "kyoei_primed_detilers.h"
#include <cstdio>
#define CHECK(x) do { if(!(x)){std::fprintf(stderr,"primed detilers failed: %d\n",__LINE__);return 1;} }while(0)
int main() {
    using namespace KyoeiPrimedDetilers;
    int first=0,second=0;
    CHECK(!Claim(&first,0));
    Mark(&first,0);Mark(&first,159);Mark(&second,0);
    CHECK(!Claim(&first,42));
    CHECK(Claim(&first,0));CHECK(!Claim(&first,0));
    // Claiming a slot must not consume another manager's primed pipeline.
    CHECK(Claim(&second,0));CHECK(any.load());
    Forget(&first);CHECK(!Claim(&first,159));CHECK(!any.load());
    Mark(&first,10);Mark(&second,11);Forget(&first);
    CHECK(any.load() && Claim(&second,11) && !any.load());
    Mark(&first,SlotCount);CHECK(!Claim(&first,SlotCount) && !any.load());
    std::puts("PRIMED DETILERS PASS: first use only, owner isolation, unclaimed cleanup and slot bounds");
}
