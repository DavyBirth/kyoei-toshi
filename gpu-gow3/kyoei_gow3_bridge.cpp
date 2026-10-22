// Kyoei: glue between the Godzilla runtime (gz_* API, loader.c) and the GoW3 GPU core.
// Structure of Godzilla's gpu/bridge.cpp, with the parts of GoW3's shim/gow3gpu.cpp the
// newer core relies on (parallel sparse copies, backing-write notes, kernel service
// thread, screenshot requests, fault recovery for speculative draw preparation).
// SPDX-License-Identifier: GPL-2.0-or-later
#include <boost/asio/io_context.hpp>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <unordered_map>
#include <condition_variable>
#include <cstring>
#include <mutex>
#include <thread>
#include <vector>
#include <windows.h>
#include <SDL3/SDL.h>
#include "common/elf_info.h"
#include "common/polyfill_thread.h"
#include "common/rdtsc.h"
#include "common/thread.h"
#include "core/libraries/libs.h"
#include "core/libraries/kernel/orbis_error.h"
#include "core/memory.h"
#include "core/signals.h"
#include "gow3_copy.h"
#include "gow3_perf_stats.h"
#include "gow3_settings.h"
#include "gow3_toggles.h"
#include "gow3_write_log.h"
#include "sdl_window.h"
#include "video_core/amdgpu/liverpool.h"
#include "video_core/renderdoc.h"
#include "video_core/renderer_vulkan/vk_rasterizer.h"
#include "gpu.h"
extern "C" {
#include "gz.h"
}

Frontend::WindowSDL* g_window{};
extern "C" void* gz_gpu_window_handle() {
    return g_window ? g_window->GetWindowInfo().render_surface : nullptr;
}
extern std::unique_ptr<AmdGpu::Liverpool> liverpool;
namespace Libraries::GnmDriver { void RegisterLib(Core::Loader::SymbolsResolver*); }
namespace Libraries::VideoOut { void RegisterLib(Core::Loader::SymbolsResolver*); }
namespace Libraries::AvPlayer { void RegisterLib(Core::Loader::SymbolsResolver*); }
namespace Libraries::Ajm { void RegisterLib(Core::Loader::SymbolsResolver*); }
namespace Libraries::Pad { void RegisterNative(Core::Loader::SymbolsResolver*); }
namespace Libraries::AudioOut { void RegisterLib(Core::Loader::SymbolsResolver*); }
namespace Libraries::Kernel { void RegisterEventQueue(Core::Loader::SymbolsResolver*); }

namespace {
struct Symbol { std::string library, name; uintptr_t address; };
std::vector<Symbol> symbols;
u32 sdk_version;
}

// GoW3 core toggles (gow3_toggles.h): every optimization on unless GZ_DISABLED_OPTIMIZATIONS
// lists bits to switch off, for A/B comparisons.
extern "C" std::uint64_t runtime_disabled_optimizations = [] {
    const char* value = std::getenv("GZ_DISABLED_OPTIMIZATIONS");
    return value ? std::strtoull(value, nullptr, 0) : 0ull;
}();
// Speculative guest reads (draw preparation) set this around reads that may fault.
extern "C" __thread sigjmp_buf* runtime_fault_recover;
__thread sigjmp_buf* runtime_fault_recover;

// gow3_jmp_buf: rbx rbp rdi rsi r12-r15 (0-56), rsp after return (64), return address (72),
// xmm6-15 (80-239), mxcsr (240), x87 control word (244). From GoW3's src/compat_win.c.
extern "C" void gow3_longjmp(unsigned long long* buffer, int value);
__asm__(".text\n"
        ".globl gow3_setjmp\n"
        ".def gow3_setjmp; .scl 2; .type 32; .endef\n"
        "gow3_setjmp:\n"
        "    mov %rbx,0(%rcx)\n    mov %rbp,8(%rcx)\n    mov %rdi,16(%rcx)\n    mov %rsi,24(%rcx)\n"
        "    mov %r12,32(%rcx)\n    mov %r13,40(%rcx)\n    mov %r14,48(%rcx)\n    mov %r15,56(%rcx)\n"
        "    lea 8(%rsp),%rdx\n    mov %rdx,64(%rcx)\n    mov (%rsp),%rdx\n    mov %rdx,72(%rcx)\n"
        "    movups %xmm6,80(%rcx)\n    movups %xmm7,96(%rcx)\n    movups %xmm8,112(%rcx)\n"
        "    movups %xmm9,128(%rcx)\n    movups %xmm10,144(%rcx)\n    movups %xmm11,160(%rcx)\n"
        "    movups %xmm12,176(%rcx)\n    movups %xmm13,192(%rcx)\n    movups %xmm14,208(%rcx)\n"
        "    movups %xmm15,224(%rcx)\n    stmxcsr 240(%rcx)\n    fnstcw 244(%rcx)\n"
        "    xor %eax,%eax\n    ret\n"
        ".globl gow3_longjmp\n"
        ".def gow3_longjmp; .scl 2; .type 32; .endef\n"
        "gow3_longjmp:\n"
        "    mov %edx,%eax\n    test %eax,%eax\n    jnz 1f\n    mov $1,%eax\n"
        "1:  mov 0(%rcx),%rbx\n    mov 8(%rcx),%rbp\n    mov 16(%rcx),%rdi\n    mov 24(%rcx),%rsi\n"
        "    mov 32(%rcx),%r12\n    mov 40(%rcx),%r13\n    mov 48(%rcx),%r14\n    mov 56(%rcx),%r15\n"
        "    movups 80(%rcx),%xmm6\n    movups 96(%rcx),%xmm7\n    movups 112(%rcx),%xmm8\n"
        "    movups 128(%rcx),%xmm9\n    movups 144(%rcx),%xmm10\n    movups 160(%rcx),%xmm11\n"
        "    movups 176(%rcx),%xmm12\n    movups 192(%rcx),%xmm13\n    movups 208(%rcx),%xmm14\n"
        "    movups 224(%rcx),%xmm15\n    ldmxcsr 240(%rcx)\n    fldcw 244(%rcx)\n"
        "    mov 64(%rcx),%rsp\n    jmp *72(%rcx)\n");

namespace Core::Loader {
void SymbolsResolver::AddSymbol(const char*, const char* library, const char*, SymbolType,
                                u64 address, const char* name) {
    symbols.push_back({library, name, address});
}
}
namespace Core {
class Emulator {
public:
    static void FillElfInfo(u32 sdk) {
        auto& info = Common::ElfInfo::Instance();
        info.initialized = true;
        info.game_serial = "CUSA03562";
        info.title = "Kyoei Toshi";
        info.app_ver = "01.03";
        info.sdk_ver = sdk;
        info.game_folder = std::filesystem::u8path(gz_app0());
    }
};
void MemoryManager::SetRasterizer(Vulkan::Rasterizer* value) { rasterizer = value; }
void MemoryManager::InvalidateMemory(VAddr address, u64 size) { gz_gpu_invalidate(address, size); }
u64 MemoryManager::ClampRangeSize(VAddr address, u64 size) { return gz_memory_clamp(address, size); }
static void CopySparseSerial(VAddr source, u8* dest, u64 size) {
    if (!gz_memory_copy(source, dest, size, 0)) std::memset(dest, 0, size);
}
void MemoryManager::CopySparseMemory(VAddr source, u8* dest, u64 size) {
    // Large uploads (streaming) are split across the copy threads, as in the GoW3 glue.
    constexpr u64 Chunk = 512 * 1024;
    if (size < 4 * Chunk || !Gow3Copy::Enabled()) return CopySparseSerial(source, dest, size);
    Gow3Copy::ParallelFor((size + Chunk - 1) / Chunk, [&](std::size_t i) {
        const u64 offset = i * Chunk;
        CopySparseSerial(source + offset, dest + offset, std::min(Chunk, size - offset));
    });
}
bool MemoryManager::TryWriteBacking(void* dest, const void* source, u64 size) {
    Gow3WriteLog::Note(reinterpret_cast<uintptr_t>(dest), source, size, Gow3WriteLog::Backing);
    const bool written = gz_memory_copy(reinterpret_cast<uintptr_t>(dest), const_cast<void*>(source), size, 1) != 0;
    // The write skipped page tracking: textures over it must reload. Fence and label
    // writes (<= 8 bytes) never hold texels.
    if (written && rasterizer && size > 8)
        rasterizer->GetTextureCache().NoteBackingWrite(reinterpret_cast<VAddr>(dest), size);
    return written;
}
void AddressSpace::Protect(VAddr address, u64 size, MemoryPermission perms) {
    gz_memory_gpu_protect(address, size, True(perms & MemoryPermission::Read), True(perms & MemoryPermission::Write));
}
boost::icl::interval_set<VAddr> AddressSpace::GetUsableRegions() {
    boost::icl::interval_set<VAddr> result;
    result += boost::icl::interval<VAddr>::right_open(0x10'0000'0000ULL, 0xfc'0000'0000ULL);
    return result;
}
}

namespace Libraries::Kernel {
// Kernel service thread: runs boost::asio timers for equeue timer events.
boost::asio::io_context io_context;
static std::mutex m_asio_req;
static std::condition_variable_any cv_asio_req;
static std::atomic<u32> asio_requests;
static std::jthread service_thread;
void KernelSignalRequest() {
    std::unique_lock lock{m_asio_req};
    ++asio_requests;
    cv_asio_req.notify_one();
}
static void KernelServiceThread(std::stop_token stoken) {
    Common::SetCurrentThreadName("kyoei:kernel_service");
    while (!stoken.stop_requested()) {
        {
            std::unique_lock lock{m_asio_req};
            cv_asio_req.wait(lock, stoken, [] { return asio_requests != 0; });
        }
        if (stoken.stop_requested()) break;
        io_context.run();
        io_context.restart();
        asio_requests = 0;
    }
}
int* PS4_SYSV_ABI __Error() { return gz_errno(); }
u64 PS4_SYSV_ABI sceKernelGetTscFrequency() {
    using Fn = u64 (PS4_SYSV_ABI*)();
    return reinterpret_cast<Fn>(gz_kernel_resolve("libkernel", "sceKernelGetTscFrequency"))();
}
u64 PS4_SYSV_ABI sceKernelReadTsc() { return Common::FencedRDTSC(); }
u64 PS4_SYSV_ABI sceKernelGetProcessTime() {
    using Fn = u64 (PS4_SYSV_ABI*)();
    return reinterpret_cast<Fn>(gz_kernel_resolve("libkernel", "sceKernelGetProcessTime"))();
}
u64 PS4_SYSV_ABI sceKernelGetProcessTimeCounter() {
    using Fn = u64 (PS4_SYSV_ABI*)();
    return reinterpret_cast<Fn>(gz_kernel_resolve("libkernel", "sceKernelGetProcessTimeCounter"))();
}
u64 PS4_SYSV_ABI sceKernelGetProcessTimeCounterFrequency() { return 1'000'000'000; }
s32 PS4_SYSV_ABI sceKernelIsNeoMode() { return 0; }
s32 PS4_SYSV_ABI sceKernelGetCompiledSdkVersion(s32* out) {
    if (!out) return ORBIS_KERNEL_ERROR_EINVAL;
    *out = static_cast<s32>(sdk_version);
    return 0;
}
s32 PS4_SYSV_ABI sceKernelUsleep(u32 usec) {
    using Fn = s32 (PS4_SYSV_ABI*)(u32);
    return reinterpret_cast<Fn>(gz_thread_resolve("libkernel", "sceKernelUsleep"))(usec);
}
}

// RenderDoc is not used; screenshot requests are counted for the presenter.
namespace VideoCore {
namespace {
std::atomic<u32> screenshot_requests{0};
}
void LoadRenderDoc() {}
void StartCapture() {}
void EndCapture() {}
void TriggerCapture() {}
void SetOutputDir(const std::filesystem::path&, const std::string&) {}
bool IsRenderDocLoaded() { return false; }
void RequestScreenshot(ScreenshotRequest request) {
    if (request != ScreenshotRequest::None) screenshot_requests.fetch_add(1, std::memory_order_relaxed);
}
u32 ConsumeGameOnlyScreenshotRequests() { return screenshot_requests.exchange(0, std::memory_order_acq_rel); }
u32 ConsumeWithOverlaysScreenshotRequests() { return 0; }
ScreenshotRequests ConsumeScreenshotRequests() {
    return {.game_only_count = ConsumeGameOnlyScreenshotRequests(), .with_overlays_count = 0};
}
}

u32 Gow3DisplayRefreshHz() {
    static const u32 hz = [] {
        const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
        const u32 rate = mode && mode->refresh_rate > 0 ? u32(mode->refresh_rate + 0.5f) : 60;
        return std::max<u32>(rate, 60);
    }();
    return hz;
}

extern "C" void gz_gpu_start(uint32_t sdk, int headless) {
    sdk_version = sdk;
    Gow3Settings::Load();
    Core::Emulator::FillElfInfo(sdk);
    if (!headless) Libraries::Kernel::service_thread = std::jthread{Libraries::Kernel::KernelServiceThread};
    Core::Loader::SymbolsResolver resolver;
    Libraries::Kernel::RegisterEventQueue(&resolver);
    if (!headless) {
        struct WindowReady { std::mutex mutex; std::condition_variable cv; bool ready = false; };
        auto state = std::make_shared<WindowReady>();
        std::thread([state] {
            auto* window = new Frontend::WindowSDL(1280, 720, "Kyoei Toshi");
            {
                std::lock_guard lock{state->mutex};
                g_window = window;
                state->ready = true;
            }
            state->cv.notify_one();
            while (window->PollEvents()) std::this_thread::sleep_for(std::chrono::milliseconds(8));
            gz_stop(0, "GPU: window closed by user");
        }).detach();
        std::unique_lock lock{state->mutex};
        state->cv.wait(lock, [&] { return state->ready; });
    }
    Libraries::GnmDriver::RegisterLib(&resolver);
    Libraries::VideoOut::RegisterLib(&resolver);
    Libraries::AvPlayer::RegisterLib(&resolver);
    Libraries::Ajm::RegisterLib(&resolver);
    Libraries::Pad::RegisterNative(&resolver);
    Libraries::AudioOut::RegisterLib(&resolver);
    gz_log("GPU (GoW3 core): %s, %zu registered symbols, SDK 0x%x",
           headless ? "symbols registered without window" : "Vulkan backend ready", symbols.size(), sdk);
    gz_memory_replay_gpu();
}
extern "C" uintptr_t gz_gpu_resolve(const char* library, const char* symbol) {
    for (const auto& entry : symbols)
        if (entry.library == library && entry.name == symbol) return entry.address;
    return 0;
}
// KYOEI_FAULT_REPORT=1: guest writes to GPU-tracked pages, by instruction, every 5 s, with the
// copy registers (rsi/rdi) of the last fault: which game code writes GPU memory, from where.
static void ReportWriteFault(void* exception, void* address) {
    static const bool enabled = [] { const char* v = std::getenv("KYOEI_FAULT_REPORT"); return v && v[0] == '1'; }();
    if (!enabled) return;
    const auto* e = static_cast<EXCEPTION_POINTERS*>(exception);
    if (e->ExceptionRecord->ExceptionInformation[0] != 1) return;
    struct Stat { u64 count, rsi, rdi, low, high; };
    static std::mutex mutex;
    static std::unordered_map<u64, Stat> stats;
    static auto window = std::chrono::steady_clock::now();
    std::scoped_lock lock{mutex};
    const u64 at = reinterpret_cast<u64>(address);
    auto [it, fresh] = stats.try_emplace(e->ContextRecord->Rip, Stat{0, 0, 0, at, at});
    auto& s = it->second;
    ++s.count;
    s.rsi = e->ContextRecord->Rsi;
    s.rdi = e->ContextRecord->Rdi;
    s.low = std::min(s.low, at);
    s.high = std::max(s.high, at);
    if (std::chrono::steady_clock::now() - window < std::chrono::seconds(5)) return;
    std::vector<std::pair<u64, Stat>> top(stats.begin(), stats.end());
    std::sort(top.begin(), top.end(), [](auto& a, auto& b) { return a.second.count > b.second.count; });
    std::printf("Kyoei write faults (5 s): %zu instructions\n", top.size());
    for (size_t i = 0; i < std::min<size_t>(top.size(), 6); ++i) {
        const auto& [rip, st] = top[i];
        std::printf("  rip %#llx: %llu faults, %#llx..%#llx, rsi %#llx rdi %#llx\n", (unsigned long long)rip,
                    (unsigned long long)st.count, (unsigned long long)st.low, (unsigned long long)st.high,
                    (unsigned long long)st.rsi, (unsigned long long)st.rdi);
    }
    stats.clear();
    window = std::chrono::steady_clock::now();
}
// KYOEI_PROTECT_WATCH=<hex low>-<hex high> (diagnostic): guest memory there is write-protected
// when mapped; each page's first guest write is logged with the copy registers, then allowed.
// Finds the game code that fills memory before the GPU core tracks it.
static std::pair<u64, u64> ProtectWatch() {
    static const std::pair<u64, u64> range = [] {
        const char* v = std::getenv("KYOEI_PROTECT_WATCH");
        if (!v) return std::pair<u64, u64>{0, 0};
        char* end{};
        const u64 low = std::strtoull(v, &end, 16);
        return std::pair<u64, u64>{low, *end == '-' ? std::strtoull(end + 1, nullptr, 16) : 0};
    }();
    return range;
}
static std::mutex protect_watch_mutex;
static std::unordered_map<u64, bool> protect_watch_pages;
static void ProtectWatchMap(uintptr_t address, uint64_t size) {
    const auto [low, high] = ProtectWatch();
    const u64 start = std::max<u64>(low, address) & ~u64(4095), end = std::min<u64>(high, address + size);
    if (!high || start >= end) return;
    const auto arm = [start, end] {
        std::scoped_lock lock{protect_watch_mutex};
        for (u64 page = start; page < end; page += 4096) protect_watch_pages[page] = true;
        gz_memory_gpu_protect(start, end - start, 1, 0);
    };
    // KYOEI_PROTECT_DELAY=<seconds>: arm later (after start-up clears the memory).
    const char* delay = std::getenv("KYOEI_PROTECT_DELAY");
    if (delay && std::atoi(delay) > 0) {
        std::thread([arm, seconds = std::atoi(delay)] {
            std::this_thread::sleep_for(std::chrono::seconds(seconds));
            arm();
            std::printf("Kyoei protect watch armed\n");
        }).detach();
        return;
    }
    arm();
    std::printf("Kyoei protect watch: %#llx..%#llx write-protected\n", (unsigned long long)start, (unsigned long long)end);
}
static bool ProtectWatchFault(void* exception, void* address) {
    if (!ProtectWatch().second) return false;
    const auto* e = static_cast<EXCEPTION_POINTERS*>(exception);
    if (e->ExceptionRecord->ExceptionInformation[0] != 1) return false;
    const u64 page = reinterpret_cast<u64>(address) & ~u64(4095);
    std::scoped_lock lock{protect_watch_mutex};
    const auto it = protect_watch_pages.find(page);
    if (it == protect_watch_pages.end()) return false;
    protect_watch_pages.erase(it);
    static std::unordered_map<u64, unsigned> per_instruction;
    if (per_instruction[e->ContextRecord->Rip]++ < 3) {
        std::printf("Kyoei protected write: page %#llx rip %#llx rsi %#llx rdi %#llx rcx %#llx\n",
                    (unsigned long long)page, (unsigned long long)e->ContextRecord->Rip,
                    (unsigned long long)e->ContextRecord->Rsi, (unsigned long long)e->ContextRecord->Rdi,
                    (unsigned long long)e->ContextRecord->Rcx);
    }
    gz_memory_gpu_protect(page, 4096, 1, 1);
    return true;
}
extern "C" int gz_gpu_handle_fault(void* exception, void* address) {
    if (ProtectWatchFault(exception, address)) return 1;
    ReportWriteFault(exception, address);
    // Page tracking handlers registered by the core (PageManager, SRT walker) come first.
    if (Core::Signals::Instance()->DispatchAccessViolation(exception, address)) return 1;
    if (runtime_fault_recover) {
        // A speculative guest read (draw preparation) faulted: resume in gow3_longjmp,
        // outside the exception dispatcher, at the thread's recovery point.
        auto* buffer = reinterpret_cast<unsigned long long*>(*runtime_fault_recover);
        runtime_fault_recover = nullptr;
        CONTEXT* c = static_cast<EXCEPTION_POINTERS*>(exception)->ContextRecord;
        c->Rsp = ((c->Rsp - 128) & ~DWORD64(15)) - 40;
        c->Rcx = DWORD64(uintptr_t(buffer));
        c->Rdx = 1;
        c->Rip = DWORD64(uintptr_t(&gow3_longjmp));
        return 1;
    }
    return 0;
}
extern "C" void gz_gpu_map(uintptr_t address, uint64_t size) {
    if (auto* r = Core::Memory::Instance()->GetRasterizer()) {
        r->RegisterMemory(address, size);
        r->MapMemory(address, size);
        gz_log("GPU memory mapped: 0x%llx size=0x%llx", (unsigned long long)address, (unsigned long long)size);
        ProtectWatchMap(address, size);
    }
}
extern "C" void gz_gpu_unmap(uintptr_t address, uint64_t size) {
    if (auto* r = Core::Memory::Instance()->GetRasterizer()) {
        gz_log("GPU memory unmapped: 0x%llx size=0x%llx", (unsigned long long)address, (unsigned long long)size);
        r->UnmapMemory(address, size);
    }
}
extern "C" void gz_gpu_invalidate(uintptr_t address, uint64_t size) {
    if (auto* r = Core::Memory::Instance()->GetRasterizer()) r->InvalidateMemory(address, size);
}
extern "C" void gz_gpu_prepare_write(void* address, uint64_t size) {
    if (auto* r = Core::Memory::Instance()->GetRasterizer()) {
        const auto start = reinterpret_cast<uintptr_t>(address);
        // Invalidate each page before ReadFile writes in kernel mode (no user VEH fault).
        for (u64 offset = 0; offset < size;) {
            r->InvalidateMemory(start + offset, 1);
            offset += std::min<u64>(size - offset, 4096 - ((start + offset) & 4095));
        }
    }
}
