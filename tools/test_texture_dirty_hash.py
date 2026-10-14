"""Compile the real dirty-mark/refresh decision with a synthetic small image."""
from pathlib import Path
import argparse
import re
import subprocess
import tempfile

root=Path(__file__).resolve().parents[1]
parser=argparse.ArgumentParser(description=__doc__)
parser.add_argument('--source',type=Path,default=root/'build/kyoei_texture_cache.cpp')
args=parser.parse_args()
source=args.source.read_text(encoding='utf-8')
mark=source[source.index('void TextureCache::MarkAsMaybeDirty('):source.index('void TextureCache::InvalidateMemory(')]
refresh=source[source.index('void TextureCache::RefreshImage('):]
refresh=refresh[:refresh.index('    const u32 num_layers =')]
cache=(root/'build/CMakeCache.txt').read_text(encoding='utf-8')
headers=Path(re.search(r'^KYOEI_GPU_HEADERS:PATH=(.*)$',cache,re.M)[1])/'gz_xxhash-src'
fixture=r'''
#include <algorithm>
#include <bit>
#include <cstdint>
#include <cstdio>
#define XXH_INLINE_ALL
#include "xxhash.h"
using u8=uint8_t; using u32=uint32_t; using u64=uint64_t; using ImageId=unsigned;
namespace ImageFlagBits { constexpr unsigned Dirty=3, MaybeCpuDirty=1, CpuDirty=2; }
bool True(unsigned v) {return v!=0;} bool False(unsigned v) {return v==0;}
struct Image {
    struct { uintptr_t guest_address; size_t guest_size; unsigned num_samples=1;
        struct {unsigned width=16,height=16;} size;
        struct {bool is_block=false;} props; unsigned num_bits=32;
    } info;
    unsigned flags=0; uint64_t hash=0;
};
namespace Common {u32 DivCeil(u32 x,u32 y) {return (x+y-1)/y;}}
#define RENDERER_TRACE
#define TRACE_HINT(...)
struct TextureCache {
    unsigned uploads=0;
    void UntrackImage(ImageId) {}
    void MarkAsMaybeDirty(ImageId,Image&);
    void RefreshImage(Image&);
};
'''
fixture+=mark+refresh+'    ++uploads;\n}\n'
fixture+=r'''
#define CHECK(x) do {if(!(x)) {std::fprintf(stderr,"dirty hash failed at %d\n",__LINE__);return 1;}}while(0)
int main() {
    u8 pixels[1024]={}; Image image; image.info.guest_address=(uintptr_t)pixels;
    image.info.guest_size=sizeof(pixels); TextureCache cache;
    cache.MarkAsMaybeDirty(0,image); cache.RefreshImage(image);
    CHECK(cache.uploads==0 && !(image.flags & ImageFlagBits::MaybeCpuDirty));
    cache.MarkAsMaybeDirty(0,image); pixels[1023]=77; cache.RefreshImage(image);
    CHECK(cache.uploads==1);
    image.flags=ImageFlagBits::MaybeCpuDirty; cache.RefreshImage(image);
    CHECK(cache.uploads==1 && !(image.flags & ImageFlagBits::MaybeCpuDirty));
    image.flags=ImageFlagBits::CpuDirty; cache.RefreshImage(image);
    CHECK(cache.uploads==2);
    std::puts("DIRTY HASH PASS: unchanged skips upload, tail write detected, definite dirty retained");
}
'''
with tempfile.TemporaryDirectory(dir=root/'build',prefix='test-dirty-hash-') as temp:
    path=Path(temp); cpp=path/'test.cpp'; exe=path/'test.exe'
    cpp.write_text(fixture,encoding='utf-8')
    subprocess.run(['clang++','-std=c++23','-O2','-I',str(headers),str(cpp),'-o',str(exe)],check=True)
    subprocess.run([str(exe)],check=True)
