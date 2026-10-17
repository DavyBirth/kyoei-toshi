#pragma once

#include <array>
#include <condition_variable>
#include <cstddef>
#include <deque>
#include <memory>
#include <mutex>
#include <new>
#include <span>
#include <cstring>
#include <thread>
#include <type_traits>
#include <utility>
#ifdef _WIN32
#include <windows.h>
#endif

namespace KyoeiVulkanRecording {

// One producer per queue, as with the original scheduler. Only the worker
// touches submitted chunks. Bounded backlog; storage is reused after replay.
class Queue {
    struct Chunk {
        static constexpr size_t Capacity = 64 * 1024;
        struct Node {
            void (*execute)(Node*);
            Node* next{};
        };
        template<class F> struct Command : Node {
            F fn;
            explicit Command(F&& f) : fn(std::move(f)) {
                this->execute = [](Node* node) {
                    auto* command = static_cast<Command*>(node);
                    command->fn();
                    command->~Command();
                };
            }
        };
        alignas(std::max_align_t) std::array<std::byte, Capacity> bytes;
        size_t used{};
        size_t count{};
        Node* first{};
        Node* last{};

        void* Allocate(size_t size, size_t alignment) {
            const auto offset=(used+alignment-1)&~(alignment-1);
            if(offset+size>Capacity) return nullptr;
            used=offset+size;
            return bytes.data()+offset;
        }

        template<class F> bool Push(F&& fn) {
            using C = Command<std::decay_t<F>>;
            static_assert(alignof(C) <= alignof(std::max_align_t));
            static_assert(sizeof(C) <= Capacity);
            const auto offset = (used + alignof(C) - 1) & ~(alignof(C) - 1);
            if (offset + sizeof(C) > Capacity) return false;
            auto* node = new(bytes.data() + offset) C(std::forward<F>(fn));
            if (last) last->next = node; else first = node;
            last = node;
            used = offset + sizeof(C);
            ++count;
            return true;
        }
        void Execute() {
            while (first) {
                auto* node = first;
                first = first->next;
                node->execute(node);
            }
            last = nullptr;
            count = used = 0;
        }
    };

public:
    Queue() : current(std::make_unique<Chunk>()), worker([this] { Run(); }) {}
    Queue(const Queue&) = delete;
    ~Queue() {
        Drain();
        { std::lock_guard lock(mutex); stopping = true; }
        ready.notify_one();
        worker.join();
    }

    template<class F> void Record(F&& fn) {
        static_assert(sizeof(typename Chunk::template Command<std::decay_t<F>>) <= 2048,
                      "increase the payload reservation margin before enlarging closures");
        if (!current->Push(std::forward<F>(fn))) {
            Publish();
            current->Push(std::forward<F>(fn));
        }
        // Early enough to overlap driver recording with translation of the
        // next draws, without waking a worker for every individual vkCmd.
        if (current->used >= 32 * 1024 || current->count >= 96) Publish();
    }

    // Reserve all variable payloads of one command together. The 2 KiB
    // margin covers its closure and alignment; payloads and closure always
    // belong to the same chunk and are recycled only after driver recording.
    bool Reserve(size_t bytes) {
        if(bytes>Chunk::Capacity-2048) return false;
        if(current->used+bytes+2048>Chunk::Capacity) Publish();
        return true;
    }
    template<class T> std::span<T> Copy(const T* values,size_t count) {
        static_assert(std::is_trivially_copyable_v<T>);
        if(!values || !count) return {};
        auto* data=static_cast<T*>(current->Allocate(count*sizeof(T),alignof(T)));
        // Every caller reserves its complete packet before storing payloads.
        if(!data) std::terminate();
        std::memcpy(data,values,count*sizeof(T));
        return {data,count};
    }

    void Drain() {
        Publish();
        std::unique_lock lock(mutex);
        completed.wait(lock, [this] { return pending.empty() && !busy; });
    }

private:
    void Publish() {
        if (!current->count) return;
        std::unique_lock lock(mutex);
        completed.wait(lock, [this] { return pending.size() < 8; });
        pending.push_back(std::move(current));
        if (free.empty()) current = std::make_unique<Chunk>();
        else { current = std::move(free.back()); free.pop_back(); }
        lock.unlock();
        ready.notify_one();
    }
    void Run() {
#ifdef _WIN32
        SetThreadDescription(GetCurrentThread(),L"Kyoei:VulkanRecording");
#endif
        for (;;) {
            std::unique_lock lock(mutex);
            ready.wait(lock, [this] { return stopping || !pending.empty(); });
            if (stopping) return;
            auto chunk = std::move(pending.front());
            pending.pop_front();
            busy = true;
            lock.unlock();
            chunk->Execute();
            lock.lock();
            free.push_back(std::move(chunk));
            busy = false;
            lock.unlock();
            completed.notify_all();
        }
    }
    std::unique_ptr<Chunk> current;
    std::mutex mutex;
    std::condition_variable ready, completed;
    std::deque<std::unique_ptr<Chunk>> pending;
    std::deque<std::unique_ptr<Chunk>> free;
    bool stopping{}, busy{};
    std::thread worker;
};
}
