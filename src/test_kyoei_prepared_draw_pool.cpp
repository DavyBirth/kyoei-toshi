// Exercise the production pool's ownership, reset, bounds and cross-thread return.
// SPDX-License-Identifier: GPL-2.0-or-later
#include "kyoei_prepared_draw_pool.h"
#include <atomic>
#include <cstdio>
#include <cstdlib>
#include <thread>
#include <unordered_set>
#include <vector>

#define CHECK(value) do { if (!(value)) { std::fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #value); return 1; } } while (0)
struct Draw {
    enum { Pending = 0, Ready = 1 };
    std::atomic<unsigned> state{Pending};
    size_t payload[8]{};
    static inline std::atomic<int> arrays = 0;
    static void* operator new[](size_t bytes) {
        void* result = std::malloc(bytes);
        if (!result) throw std::bad_alloc();
        ++arrays;
        return result;
    }
    static void operator delete[](void* pointer) noexcept { --arrays; std::free(pointer); }
};
struct Submission {
    size_t num_draws = 0;
    std::unique_ptr<Draw[]> draws;
};
static auto acquire(KyoeiPreparedDrawPool<Draw>& pool, size_t count) {
    auto owner = std::make_unique<Submission>();
    owner->num_draws = count;
    return pool.MakeSubmission(std::move(owner));
}

int main() {
    CHECK(Draw::arrays == 0);
    {
        KyoeiPreparedDrawPool<Draw> pool;
        CHECK(!acquire(pool, 0)->draws);
        auto first = acquire(pool, 16);
        auto held = first;
        Draw* original = first->draws.get();
        first->draws[0].payload[0] = 9817;
        first->draws[0].state.store(Draw::Ready);
        first.reset();
        auto second = acquire(pool, 16);
        CHECK(second->draws.get() != original);
        CHECK(held->draws[0].payload[0] == 9817);
        held.reset();
        auto reused = acquire(pool, 16);
        CHECK(reused->draws.get() == original);
        for (size_t i = 0; i < 16; ++i) CHECK(reused->draws[i].state == Draw::Pending);
        bool overflow = false;
        try { acquire(pool, std::numeric_limits<size_t>::max() / sizeof(Draw) + 1); }
        catch (const std::bad_array_new_length&) { overflow = true; }
        CHECK(overflow);
    }
    CHECK(Draw::arrays == 0);
    {
        KyoeiPreparedDrawPool<Draw> pool(sizeof(Draw) * 16);
        auto small = acquire(pool, 16);
        auto excess = acquire(pool, 32);
        small.reset(); excess.reset();
        CHECK(Draw::arrays == 1);
    }
    CHECK(Draw::arrays == 0);
    std::shared_ptr<Submission> survivor;
    { KyoeiPreparedDrawPool<Draw> local; survivor = acquire(local, 7); }
    survivor->draws[6].payload[3] = 772;
    CHECK(survivor->draws[6].payload[3] == 772);
    survivor.reset();
    CHECK(Draw::arrays == 0);
    {
        KyoeiPreparedDrawPool<Draw> pool;
        std::mutex mutex;
        std::unordered_set<const Draw*> active;
        std::atomic<int> failures = 0;
        std::vector<std::thread> threads;
        for (size_t thread = 0; thread < 4; ++thread) threads.emplace_back([&, thread] {
            for (size_t iteration = 0; iteration < 2000; ++iteration) {
                const size_t count = 1 + (iteration * 17 + thread) % 64;
                auto array = acquire(pool, count);
                const auto pointer = array->draws.get();
                { std::scoped_lock lock(mutex); if (!active.insert(pointer).second) { ++failures; return; } }
                const size_t stamp = thread * 2000 + iteration + 1;
                for (size_t i = 0; i < count; ++i) {
                    if (array->draws[i].state != Draw::Pending) ++failures;
                    array->draws[i].payload[0] = stamp;
                    array->draws[i].state.store(Draw::Ready, std::memory_order_release);
                }
                auto held = array;
                array.reset();
                std::this_thread::yield();
                for (size_t i = 0; i < count; ++i)
                    if (held->draws[i].state.load(std::memory_order_acquire) != Draw::Ready ||
                        held->draws[i].payload[0] != stamp) ++failures;
                { std::scoped_lock lock(mutex); active.erase(pointer); }
                held.reset();
            }
        });
        for (auto& thread : threads) thread.join();
        CHECK(failures == 0 && active.empty());
        CHECK(Draw::arrays <= 8);
    }
    CHECK(Draw::arrays == 0);
    std::puts("PASS: pending reset, last-owner lifetime, memory bound and 8000 concurrent handoffs");
}
