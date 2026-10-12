"""Compile the actual Flip method against an immediate event consumer.

The consumer reuses the released surface as soon as completion is signaled.
The reference must lose that new label; the adapted method must preserve it.
No game files are used, and neither driver implementation is copied here.
"""
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build" / "test-videoout-order"
PRELUDE = r'''
#include <array>
#include <cstdint>
#include <functional>
#include <mutex>
#include <vector>
using u64 = uint64_t;
namespace KyoeiFlipTrace { template<class... T> void Action(T...) {} }
namespace KyoeiFrameStats { void Presented() {} struct Work { Work(int) {} }; }
struct Presenter { void SetHDR(bool) {} void Present(void*) {} } instance;
auto* presenter = &instance;
namespace Libraries::Kernel {
uint64_t sceKernelGetProcessTime() { return 1; }
uint64_t sceKernelReadTsc() { return 2; }
}
namespace Libraries::VideoOut {
struct FlipStatus {
 uint64_t count=0, process_time=0, tsc=0;
 int64_t flip_arg=0;
 int current_buffer=-1, gc_queue_num=1, flip_pending_num=1;
};
struct Port {
 bool is_hdr=false;
 std::mutex port_mutex;
 std::array<u64,2> buffer_labels{1,1};
 FlipStatus flip_status;
 std::vector<int> flip_events{1};
 int prev_index=0;
 void SignalVoLabel() {}
};
struct Request { Port* port; void* frame=nullptr; int index=1; int64_t flip_arg=0; bool eop=true; };
enum class OrbisVideoOutInternalEventId { Flip=1 };
class VideoOutDriver { public: void Flip(const Request&); };
namespace Kernel {
struct OrbisKernelEvent { enum class Filter { VideoOut }; };
struct Queue {
 std::function<void()> consume;
 void TriggerEvent(u64, OrbisKernelEvent::Filter, void*) { consume(); }
} queue;
Queue* GetEqueue(int) { return &queue; }
}
'''
EPILOGUE = r'''
}
int main() {
 using namespace Libraries::VideoOut;
 Port port;
 bool completion=false;
 Kernel::queue.consume = [&] {
   // Guest sees completion and submits its next frame using surface zero.
   completion = port.flip_status.count==1 && port.flip_status.current_buffer==1 &&
                port.flip_status.flip_pending_num==0 && port.flip_status.gc_queue_num==0;
   port.buffer_labels[0] = 1;
 };
 VideoOutDriver driver;
 driver.Flip(Request{&port});
 return completion && port.buffer_labels[0]==1 && port.prev_index==1 ? 0 : 1;
}
'''


def method(path):
    source = path.read_text(encoding="utf-8")
    start = source.index("void VideoOutDriver::Flip(const Request& req) {")
    end = source.index("void VideoOutDriver::DrawBlankFrame()", start)
    return source[start:end]


def run_case(name, path, expected):
    source = BUILD / f"{name}.cpp"
    binary = BUILD / f"{name}.exe"
    source.write_text(PRELUDE + method(path) + EPILOGUE, encoding="utf-8")
    subprocess.run([shutil.which("clang++") or "clang++", "-std=c++23", str(source),
                    "-o", str(binary)], check=True)
    result = subprocess.run([str(binary)], check=False)
    if result.returncode != expected:
        raise RuntimeError(f"{name}: exit {result.returncode}, expected {expected}")
    print(f"{name}: {'lost new label (race reproduced)' if expected else 'new label preserved'}")


def run_gate(name, path, expected):
    source_text = path.read_text(encoding="utf-8")
    start = source_text.index("                u64* address = write_data->Address<u64*>();")
    end = source_text.index("                if (!write_data->wr_one_addr.Value())", start)
    gate = source_text[start:end]
    harness = r'''
#include <coroutine>
#include <cstdint>
#include <cstring>
#include <exception>
using u64=uint64_t;
namespace KyoeiFlipTrace {
 template<class... T> void Action(T...) {}
 int LabelIndex(const void*) { return -1; }
}
u64 label=1;
struct Port { bool IsVoLabel(const u64* p) { return p==&label; } } port;
auto* vo_port=&port;
struct Write { uint32_t data[1]{1}; template<class T> T Address() { return reinterpret_cast<T>(&label); } } command;
auto* write_data=&command;
constexpr uint32_t data_size=4;
struct Task {
 struct promise_type {
  Task get_return_object() { return {std::coroutine_handle<promise_type>::from_promise(*this)}; }
  std::suspend_always initial_suspend() noexcept { return {}; }
  std::suspend_always final_suspend() noexcept { return {}; }
  void return_void() {}
  void unhandled_exception() { std::terminate(); }
 };
 std::coroutine_handle<promise_type> handle;
 ~Task() { handle.destroy(); }
};
#define YIELD_GFX() co_await std::suspend_always{}
Task write_next_lock() {
''' + gate + r'''
 std::memcpy(address, write_data->data, data_size);
 co_return;
}
int main() {
 auto task=write_next_lock();
 task.handle.resume();
 bool waited=!task.handle.done();
 label=0; // Presentation completes and releases the previous frame's surface.
 if (waited) task.handle.resume();
 return waited && task.handle.done() && label==1 ? 0 : 1;
}
'''
    source = BUILD / f"{name}.cpp"
    binary = BUILD / f"{name}.exe"
    source.write_text(harness, encoding="utf-8")
    subprocess.run([shutil.which("clang++") or "clang++", "-std=c++23", str(source),
                    "-o", str(binary)], check=True)
    result = subprocess.run([str(binary)], check=False)
    if result.returncode != expected:
        raise RuntimeError(f"{name}: exit {result.returncode}, expected {expected}")
    print(f"{name}: {'early write lost (race reproduced)' if expected else 'waited for release, new lock preserved'}")


if __name__ == "__main__":
    BUILD.mkdir(parents=True, exist_ok=True)
    run_case("reference", ROOT / "third_party/godzilla-core/third_party/shadPS4/src/core/libraries/videoout/driver.cpp", 1)
    run_case("adapted", ROOT / "build/kyoei_videoout_driver.cpp", 0)
    run_gate("reference-gate", ROOT / "third_party/godzilla-core/third_party/shadPS4/src/video_core/amdgpu/liverpool.cpp", 1)
    run_gate("adapted-gate", ROOT / "build/kyoei_liverpool.cpp", 0)
    print("PASS: both reference races reproduced; adapted event order and surface wait pass")
