"""Exercise the actual submission-owner coroutine with reused command memory."""
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build/test-gpu-snapshot"

HARNESS = r'''
#include <coroutine>
#include <cstdint>
#include <exception>
#include <span>
#include <vector>
using u32 = uint32_t;
int live_frames=0;
struct Task {
 struct promise_type {
  promise_type() { ++live_frames; }
  ~promise_type() { --live_frames; }
  Task get_return_object() { return {std::coroutine_handle<promise_type>::from_promise(*this)}; }
  std::suspend_always initial_suspend() noexcept { return {}; }
  std::suspend_always final_suspend() noexcept { return {}; }
  std::suspend_always yield_value(std::nullptr_t) noexcept { return {}; }
  void return_void() {}
  void unhandled_exception() { std::terminate(); }
 };
 std::coroutine_handle<promise_type> handle;
};
struct Liverpool {
 std::vector<std::vector<u32>> results;
 Task ProcessGraphics(std::span<const u32> dcb, std::span<const u32> ccb) {
  co_yield {};
  std::vector<u32> result(dcb.begin(), dcb.end());
  result.insert(result.end(), ccb.begin(), ccb.end());
  results.push_back(result);
 }
};
int main() {
'''

TAIL = r'''
 Liverpool owner;
 std::vector<u32> dcb{11,12}, ccb{21};
 auto first = make_owned_task(&owner, {dcb.begin(),dcb.end()}, {ccb.begin(),ccb.end()});
 dcb={31,32}; ccb={41};
 auto second = make_owned_task(&owner, {dcb.begin(),dcb.end()}, {ccb.begin(),ccb.end()});
 dcb={91,92}; ccb={99}; // Game reuses both source command buffers.
 first.handle.resume();
 second.handle.resume();
 if (first.handle.done() || second.handle.done() || live_frames!=4) return 1;
 first.handle.resume();
 second.handle.resume();
 if (!first.handle.done() || !second.handle.done() || live_frames!=2) return 2;
 if (owner.results!=std::vector<std::vector<u32>>{{11,12,21},{31,32,41}}) return 3;
 first.handle.destroy(); second.handle.destroy();
 return live_frames==0 ? 0 : 4;
}
'''

if __name__ == "__main__":
    text = (ROOT / "build/kyoei_liverpool.cpp").read_text(encoding="utf-8")
    start = text.index("        const auto make_owned_task =")
    end = text.index("        task = make_owned_task", start)
    BUILD.mkdir(parents=True, exist_ok=True)
    source = BUILD / "snapshot.cpp"
    binary = BUILD / "snapshot.exe"
    source.write_text(HARNESS + text[start:end] + TAIL, encoding="utf-8")
    subprocess.run([shutil.which("clang++") or "clang++", "-std=c++23", str(source),
                    "-o", str(binary)], check=True)
    subprocess.run([str(binary)], check=True)
    print("PASS: reused DCB/CCB remain independent across suspended submissions; all coroutine frames released")
