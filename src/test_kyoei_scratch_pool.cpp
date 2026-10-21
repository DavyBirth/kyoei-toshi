#include <cassert>
#include <cstdio>
#include "kyoei_scratch_pool.h"

int main() {
    using namespace KyoeiScratchPool;
    assert(Class(1)==MinClass && Class(MinClass+1)==2*MinClass && Class(3u<<20)==4u<<20);
    Pool<int,int> pool;
    assert(!pool.Take(1000));
    pool.Give({Class(1000),1,10});
    assert(!pool.Take(MinClass+1));          // other class is not reused
    auto e=pool.Take(4096);                  // same class is reused
    assert(e && e->buffer==1 && e->allocation==10 && !pool.Take(4096));
    // Budget keeps the newest entries and hands the oldest back for destruction.
    const uint64_t big=BudgetBytes/2;
    assert(pool.Give({big,2,0}).empty() && pool.Give({big,3,0}).empty());
    auto evicted=pool.Give({MinClass,4,0});
    assert(evicted.size()==1 && evicted[0].buffer==2);
    assert(pool.Give({2*BudgetBytes,5,0}).size()==1);   // never pool over budget
    assert(pool.Drain().size()==2 && pool.free_bytes==0);
    std::puts("scratch pool: class/reuse/budget/drain PASS");
}
