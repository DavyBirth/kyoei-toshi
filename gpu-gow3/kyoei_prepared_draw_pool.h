// Reuse preparation arrays only after their submission's last owner releases them.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <array>
#include <atomic>
#include <bit>
#include <limits>
#include <memory>
#include <mutex>
#include <new>
#include <type_traits>

template <class Draw>
class KyoeiPreparedDrawPool {
    static_assert(std::is_trivially_destructible_v<Draw>);
    struct Entry {
        std::unique_ptr<Draw[]> data;
        size_t capacity = 0;
    };
    struct State {
        std::mutex mutex;
        std::array<Entry, 8> free;
        size_t bytes = 0;
        const size_t budget;
        explicit State(size_t value) : budget(value) {}
    };
    std::shared_ptr<State> state;

public:
    explicit KyoeiPreparedDrawPool(size_t budget = 16u << 20)
        : state(std::make_shared<State>(budget)) {}

private:
    Entry Acquire(size_t count) {
        if (!count) return {};
        constexpr size_t maximum = std::numeric_limits<size_t>::max();
        if (count > maximum / sizeof(Draw)) throw std::bad_array_new_length();
        size_t capacity = count <= maximum / 2 + 1 ? std::bit_ceil(count) : count;
        if (capacity > maximum / sizeof(Draw)) capacity = count;
        const auto owner = state;
        std::unique_ptr<Draw[]> data;
        {
            std::scoped_lock lock(owner->mutex);
            Entry* best = nullptr;
            for (auto& entry : owner->free) {
                if (entry.data && entry.capacity >= count &&
                    (!best || entry.capacity < best->capacity)) best = &entry;
            }
            if (best) {
                capacity = best->capacity;
                owner->bytes -= capacity * sizeof(Draw);
                data = std::move(best->data);
                best->capacity = 0;
            }
        }
        if (!data) data = std::make_unique<Draw[]>(capacity);
        // Only Ready publishes payload. Workers replace all payload used by a ready draw.
        // Old pointers stay inaccessible while Pending; no per-frame bulk zero-fill.
        for (size_t i = 0; i < count; ++i)
            data[i].state.store(Draw::Pending, std::memory_order_relaxed);
        return {std::move(data), capacity};
    }

public:
    // Keep Submission::draws as unique_ptr. Its last release returns the array;
    // no borrower can observe the storage after it becomes reusable.
    template <class Submission>
    std::shared_ptr<Submission> MakeSubmission(std::unique_ptr<Submission> submission) {
        auto storage = Acquire(submission->num_draws);
        submission->draws = std::move(storage.data);
        const auto owner = state;
        const size_t capacity = storage.capacity;
        return std::shared_ptr<Submission>(submission.release(), [owner, capacity](Submission* pointer) {
            auto returned = std::move(pointer->draws);
            delete pointer;
            if (!returned) return;
            const size_t bytes = capacity * sizeof(Draw);
            {
                std::scoped_lock lock(owner->mutex);
                if (bytes <= owner->budget && owner->bytes <= owner->budget - bytes) {
                    for (auto& entry : owner->free) {
                        if (!entry.data) {
                            entry.data = std::move(returned);
                            entry.capacity = capacity;
                            owner->bytes += bytes;
                            break;
                        }
                    }
                }
            }
            // Excess arrays are deleted outside the mutex. The shared state also survives
            // destruction of the pool if an in-flight submission still owns an array.
        });
    }
};
