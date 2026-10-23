// Kyoei diagnostics: sampling profiler for named host threads, off by default.
//   KYOEI_SAMPLE_THREADS=<name part>[,<name part>...]  threads whose description contains one
//   (* = every named thread)
//   KYOEI_SAMPLE_START=<seconds after GPU start> (default 120), KYOEI_SAMPLE_SECONDS (default 20)
//   KYOEI_SAMPLE_SLOW_MS=<ms>  report only samples taken during flips slower than this
//   (needs KYOEI_FRAME_STATS=1, which reports the slow flips)
//   KYOEI_SAMPLE_FOCUS=<symbol part>  also print the most frequent whole stacks through it
// About every millisecond each matching thread is suspended, its stack unwound (48 frames) and
// resumed. At the end the run log gets, per thread, the functions most often on top of the
// stack (self) and anywhere in it (total), as percentages of that thread's samples.
// SPDX-License-Identifier: GPL-2.0-or-later
#include <windows.h>
#include <dbghelp.h>
#include <tlhelp32.h>
#include <timeapi.h>
#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <cwchar>
#include <map>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

extern "C" void gz_log(const char* format, ...);

namespace {
using Clock = std::chrono::steady_clock;
struct Target {
    DWORD id;
    HANDLE handle;
    std::string name;
    std::vector<std::vector<DWORD64>> stacks;
    std::vector<Clock::time_point> times;
};
std::mutex slow_mutex;
std::vector<std::pair<Clock::time_point, Clock::time_point>> slow_frames;

std::string Narrow(const wchar_t* text) {
    std::string out;
    for (; *text; ++text) out += *text < 128 ? char(*text) : '?';
    return out;
}

std::vector<Target> FindThreads(const std::vector<std::string>& parts) {
    std::vector<Target> found;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) return found;
    THREADENTRY32 entry{.dwSize = sizeof(entry)};
    const DWORD self = GetCurrentProcessId();
    for (BOOL ok = Thread32First(snapshot, &entry); ok; ok = Thread32Next(snapshot, &entry)) {
        if (entry.th32OwnerProcessID != self) continue;
        HANDLE thread = OpenThread(THREAD_SUSPEND_RESUME | THREAD_GET_CONTEXT | THREAD_QUERY_LIMITED_INFORMATION,
                                   FALSE, entry.th32ThreadID);
        if (!thread) continue;
        PWSTR description = nullptr;
        std::string name;
        if (SUCCEEDED(GetThreadDescription(thread, &description)) && description) {
            name = Narrow(description);
            LocalFree(description);
        }
        const bool wanted = !name.empty() && std::any_of(parts.begin(), parts.end(), [&](const std::string& part) {
            return part == "*" || name.find(part) != std::string::npos;
        });
        if (wanted) found.push_back({entry.th32ThreadID, thread, name, {}});
        else CloseHandle(thread);
    }
    CloseHandle(snapshot);
    return found;
}

// Runs while the thread is suspended: no allocation (it may hold the heap lock).
size_t Unwind(CONTEXT context, DWORD64 (&frames)[48]) {
    size_t count = 0;
    for (; count < 48 && context.Rip; ++count) {
        frames[count] = context.Rip;
        DWORD64 image_base = 0;
        PRUNTIME_FUNCTION function = RtlLookupFunctionEntry(context.Rip, &image_base, nullptr);
        if (!function) {
            // Leaf function: the return address is on top of the stack.
            context.Rip = *reinterpret_cast<DWORD64*>(context.Rsp);
            context.Rsp += 8;
            continue;
        }
        void* handler_data = nullptr;
        DWORD64 establisher = 0;
        RtlVirtualUnwind(UNW_FLAG_NHANDLER, image_base, context.Rip, function, &context, &handler_data,
                         &establisher, nullptr);
    }
    return count;
}

std::string Symbol(DWORD64 address, std::unordered_map<DWORD64, std::string>& cache) {
    if (auto it = cache.find(address); it != cache.end()) return it->second;
    alignas(SYMBOL_INFO) unsigned char storage[sizeof(SYMBOL_INFO) + 512]{};
    auto* symbol = reinterpret_cast<SYMBOL_INFO*>(storage);
    symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
    symbol->MaxNameLen = 511;
    DWORD64 displacement = 0;
    std::string name;
    if (SymFromAddr(GetCurrentProcess(), address, &displacement, symbol)) {
        name = symbol->Name;
    } else {
        char text[32];
        std::snprintf(text, sizeof(text), "%llx", (unsigned long long)address);
        name = text;
    }
    return cache[address] = name;
}

void Report(std::vector<Target>& targets) {
    SymSetOptions(SYMOPT_UNDNAME | SYMOPT_DEFERRED_LOADS);
    SymInitialize(GetCurrentProcess(), nullptr, TRUE);
    std::unordered_map<DWORD64, std::string> cache;
    if (std::getenv("KYOEI_SAMPLE_SLOW_MS")) {
        std::lock_guard lock{slow_mutex};
        gz_log("Kyoei sample: keeping samples from %zu slow flips", slow_frames.size());
        for (auto& target : targets) {
            std::vector<std::vector<DWORD64>> kept;
            for (size_t i = 0; i < target.stacks.size(); ++i) {
                const auto at = target.times[i];
                if (std::any_of(slow_frames.begin(), slow_frames.end(),
                                [&](const auto& frame) { return at >= frame.first && at <= frame.second; }))
                    kept.push_back(std::move(target.stacks[i]));
            }
            target.stacks = std::move(kept);
        }
    }
    for (auto& target : targets) {
        if (target.stacks.empty()) continue;
        std::map<std::string, unsigned> self, total;
        for (const auto& stack : target.stacks) {
            std::unordered_set<std::string> seen;
            for (size_t i = 0; i < stack.size(); ++i) {
                const std::string name = Symbol(stack[i], cache);
                if (i == 0) ++self[name];
                if (seen.insert(name).second) ++total[name];
            }
        }
        const auto print = [&](const char* kind, const std::map<std::string, unsigned>& counts) {
            std::vector<std::pair<unsigned, std::string>> top;
            for (const auto& [name, count] : counts) top.emplace_back(count, name);
            std::sort(top.rbegin(), top.rend());
            for (size_t i = 0; i < std::min<size_t>(top.size(), 60); ++i) {
                gz_log("Kyoei sample %s [%s]: %5.1f%% %s", kind, target.name.c_str(),
                       100.0 * top[i].first / target.stacks.size(), top[i].second.substr(0, 160).c_str());
            }
        };
        gz_log("Kyoei sample: thread %s, %zu samples", target.name.c_str(), target.stacks.size());
        print("self", self);
        print("total", total);
        if (const char* focus = std::getenv("KYOEI_SAMPLE_FOCUS")) {
            std::map<std::string, unsigned> paths;
            for (const auto& stack : target.stacks) {
                std::string path;
                bool hit = false;
                for (const DWORD64 address : stack) {
                    const std::string name = Symbol(address, cache);
                    hit = hit || name.find(focus) != std::string::npos;
                    if (name.rfind("Drv", 0) == 0 || name.rfind("vkGetInstanceProcAddr", 0) == 0) { if (path.size() < 4 || path.compare(path.size() - 4, 4, " ...")) path += " ..."; } else path += (path.empty() ? "" : " < ") + name.substr(0, 60);
                }
                if (hit) ++paths[path];
            }
            std::vector<std::pair<unsigned, std::string>> top;
            for (const auto& [path, count] : paths) top.emplace_back(count, path);
            std::sort(top.rbegin(), top.rend());
            for (size_t i = 0; i < std::min<size_t>(top.size(), 8); ++i)
                gz_log("Kyoei sample stack [%s] %u: %s", target.name.c_str(), top[i].first, top[i].second.c_str());
        }
    }
}

void Run(std::vector<std::string> parts, unsigned start, unsigned seconds) {
    std::this_thread::sleep_for(std::chrono::seconds(start));
    auto targets = FindThreads(parts);
    gz_log("Kyoei sample: %zu threads matched, sampling %u s", targets.size(), seconds);
    timeBeginPeriod(1);
    const auto end = std::chrono::steady_clock::now() + std::chrono::seconds(seconds);
    while (std::chrono::steady_clock::now() < end) {
        for (auto& target : targets) {
            if (SuspendThread(target.handle) == DWORD(-1)) continue;
            CONTEXT context{};
            context.ContextFlags = CONTEXT_FULL;
            DWORD64 frames[48];
            size_t count = 0;
            if (GetThreadContext(target.handle, &context)) {
                // Unwinding reads the thread's stack only; nothing it holds is touched.
                count = Unwind(context, frames);
            }
            ResumeThread(target.handle);
            if (count) {
                target.stacks.emplace_back(frames, frames + count);
                target.times.push_back(Clock::now());
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
    timeEndPeriod(1);
    Report(targets);
    for (auto& target : targets) CloseHandle(target.handle);
}
} // namespace

extern "C" void kyoei_thread_sampler_slow_flip(double interval_ms) {
    static const double threshold = [] {
        const char* value = std::getenv("KYOEI_SAMPLE_SLOW_MS");
        return value ? std::atof(value) : 0.0;
    }();
    if (threshold <= 0 || interval_ms < threshold) return;
    const auto now = Clock::now();
    std::lock_guard lock{slow_mutex};
    slow_frames.emplace_back(now - std::chrono::microseconds(static_cast<long long>(interval_ms * 1000)), now);
}

extern "C" void kyoei_thread_sampler_start(void) {
    const char* names = std::getenv("KYOEI_SAMPLE_THREADS");
    if (!names || !*names) return;
    std::vector<std::string> parts;
    for (std::string list = names; !list.empty();) {
        const size_t comma = list.find(',');
        parts.push_back(list.substr(0, comma));
        list = comma == std::string::npos ? "" : list.substr(comma + 1);
    }
    const char* start = std::getenv("KYOEI_SAMPLE_START");
    const char* seconds = std::getenv("KYOEI_SAMPLE_SECONDS");
    std::thread(Run, parts, start ? unsigned(std::atoi(start)) : 120u, seconds ? unsigned(std::atoi(seconds)) : 20u)
        .detach();
}
