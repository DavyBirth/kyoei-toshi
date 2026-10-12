"""Test the actual submit-and-flip wrapper's ownership of mutable patch data."""
from pathlib import Path
import shutil
import subprocess

ROOT = Path(__file__).resolve().parents[1]
BUILD = ROOT / "build/test-flip-patch"
PRELUDE = r'''
#include <cstdint>
#include <vector>
using u32=uint32_t; using s32=int32_t; using s64=int64_t;
#define PS4_SYSV_ABI
#define LOG_DEBUG(...)
constexpr s32 ORBIS_OK=0;
constexpr u32 HostFlip=0x68750776;
int patch_error=0, submit_calls=0;
std::vector<std::vector<u32>> submitted;
s32 PatchFlipRequest(u32* data,u32 size,u32,u32,u32,s64,void*) {
 data[size-64]=0xc0033700;
 data[size-59]=0xc0391000;
 data[size-58]=HostFlip;
 return patch_error;
}
s32 sceGnmSubmitCommandBuffers(u32 count,const u32** dcb,u32* sizes,const u32**,u32*) {
 ++submit_calls;
 submitted.clear();
 for(u32 i=0;i<count;++i) submitted.emplace_back(dcb[i],dcb[i]+sizes[i]/4);
 return 0;
}
'''
TAIL = r'''
int main() {
 std::vector<u32> first(8,0x1234), last(64,0);
 last[0]=0xc03e1000; last[1]=0x1111;
 const auto original=last;
 u32* pointers[]={first.data(),last.data()}; u32 sizes[]={32,256};
 auto result=sceGnmSubmitAndFlipCommandBuffersForWorkload(1,2,pointers,sizes,nullptr,nullptr,1,0,0,0);
 if(result!=0 || last!=original || submitted.size()!=2 || submitted[0]!=first || submitted[1][6]!=HostFlip) return 1;
 // Guest reuses the NOP header without touching its payload. No host marker remains.
 last[0]=0xc09e1000;
 if(last[6]==HostFlip) return 2;
 const auto reused=last;
 patch_error=-7; submit_calls=0;
 result=sceGnmSubmitAndFlipCommandBuffersForWorkload(1,2,pointers,sizes,nullptr,nullptr,1,0,0,0);
 if(result!=-7 || last!=reused || submit_calls) return 3;
 return 0;
}
'''

def run(name, path, expected):
    text = path.read_text(encoding="utf-8")
    start = text.index("s32 PS4_SYSV_ABI sceGnmSubmitAndFlipCommandBuffersForWorkload(")
    end = text.index("int PS4_SYSV_ABI sceGnmSubmitCommandBuffersForWorkload", start)
    source = BUILD / f"{name}.cpp"
    binary = BUILD / f"{name}.exe"
    source.write_text(PRELUDE + text[start:end] + TAIL, encoding="utf-8")
    subprocess.run([shutil.which("clang++") or "clang++", "-std=c++23", str(source),
                    "-o", str(binary)], check=True)
    code = subprocess.run([str(binary)], check=False).returncode
    if code != expected:
        raise RuntimeError(f"{name}: exit {code}, expected {expected}")
    print(f"{name}: {'guest buffer mutated (reference hazard reproduced)' if expected else 'guest buffers preserved, patched copy submitted, error propagated'}")

if __name__ == "__main__":
    BUILD.mkdir(parents=True, exist_ok=True)
    run("reference", ROOT / "third_party/godzilla-core/third_party/shadPS4/src/core/libraries/gnmdriver/gnmdriver.cpp", 1)
    run("adapted", ROOT / "build/kyoei_gnmdriver.cpp", 0)
    print("PASS: host flip markers stay out of guest command memory")
