"""Compile actual texture-cache methods against a simulated GPU/ring buffer.

Checks publication order, noncoherent host visibility, ring wrap and async
completion. Uses synthetic bytes only; never starts the game or Vulkan.
"""
import os
from pathlib import Path
import shutil
import subprocess
ROOT=Path(__file__).resolve().parents[1]
BUILD=ROOT/'build/test-readback-integration'
PRELUDE=r'''
#include <algorithm>
#include <array>
#include <bit>
#include <cstdint>
#include <cstring>
#include <functional>
#include <vector>
#include <cstdio>
#include <cstdlib>
#include "kyoei_readback_batch.h"
using u8=uint8_t; using u32=uint32_t; using u64=uint64_t;
#define ASSERT(x) do { if (!(x)) std::abort(); } while(0)
#define ASSERT_MSG(x,...) ASSERT(x)
#define VK_SUCCESS 0
void gz_log(const char*,...) {}
namespace KyoeiFrameStats { struct Work { Work(int) {} }; bool Enabled() { return false; } }
namespace vk {
enum class ImageAspectFlagBits { eDepth,eColor };
enum class ImageLayout { eTransferSrcOptimal };
enum class AccessFlagBits2 { eTransferRead };
struct BufferImageCopy {
 u64 bufferOffset; u32 bufferRowLength,bufferImageHeight;
 struct { ImageAspectFlagBits aspectMask; u32 mipLevel,baseArrayLayer,layerCount; } imageSubresource;
 struct { int x,y,z; } imageOffset;
 struct { u32 width,height,depth; } imageExtent;
};
}
enum class MemoryUsage { Download };
enum class ImageFlagBits { GpuModified=1 };
int operator&(int value,ImageFlagBits flag) { return value&int(flag); }
bool False(int value) { return !value; }
struct Scheduler;
struct Ring {
 std::array<u8,8> host{},gpu{};
 bool is_coherent=false; u64 cursor=0,mapped=0;
 struct { Ring* allocation; } buffer{this};
 Scheduler* scheduler=nullptr;
 std::pair<u8*,u64> Map(u64 size,u64 alignment=0,bool allow_wait=true);
 void Commit();
 Ring* Handle() { return this; }
};
int vmaInvalidateAllocation(int,Ring* ring,u64 offset,u64 size) {
 std::copy_n(ring->gpu.begin()+offset,size,ring->host.begin()+offset); return 0;
}
struct Instance { int GetAllocator() const { return 1; } } instance;
struct Scheduler {
 struct Copy { Ring* ring; u64 offset; int image; u32 bytes; };
 std::vector<Copy> copies; std::vector<std::function<void()>> deferred;
 unsigned waits=0;
 void Finish() {
   ++waits;
   for(auto& copy:copies) {
     std::fill_n(copy.ring->gpu.begin()+copy.offset,copy.bytes,u8(copy.image));
     if(copy.ring->is_coherent) std::fill_n(copy.ring->host.begin()+copy.offset,copy.bytes,u8(copy.image));
   }
   copies.clear();
   auto callbacks=std::move(deferred); deferred.clear();
   for(auto& callback:callbacks) callback();
 }
 void EndRendering() {}
 struct Command {
   Scheduler* owner;
   void copyImageToBuffer(int image,vk::ImageLayout,Ring* ring,const vk::BufferImageCopy& copy) const {
     owner->copies.push_back({ring,copy.bufferOffset,image,copy.imageExtent.width});
   }
 };
 Command CommandBuffer() { return {this}; }
 void DeferPriorityOperation(std::function<void()> callback) { deferred.push_back(std::move(callback)); }
};
std::pair<u8*,u64> Ring::Map(u64 size,u64,bool allow_wait) {
 if(size>host.size()) return {nullptr,0};
 if(cursor+size>host.size()) cursor=0;
 if(cursor==0 && !scheduler->copies.empty()) {
   if(!allow_wait) return {nullptr,0};
   scheduler->Finish();
 }
 mapped=size; return {host.data()+cursor,cursor};
}
void Ring::Commit() {
 if(!is_coherent) vmaInvalidateAllocation(1,this,cursor,mapped);
 cursor+=mapped;
}
namespace Core {
struct Memory {
 static Memory* Instance() { static Memory memory; return &memory; }
 void TryWriteBacking(u8* destination,const u8* source,u64 bytes) { std::memcpy(destination,source,bytes); }
};
}
using ImageId=size_t;
struct Image {
 int flags=1,marker=0;
 struct {
   u32 pitch=4;
   struct { u32 width=4,height=1,depth=1; } size;
   struct { u32 layers=1; } resources;
   u32 num_bits=8,guest_size=4;
   struct { bool is_depth=false; } props;
   u64 guest_address=0;
 } info;
 int GetImage() { return marker; }
 void Transit(vk::ImageLayout,vk::AccessFlagBits2,int) {}
};
struct Cache { Ring ring; Ring& GetUtilityBuffer(MemoryUsage) { return ring; } };
struct TextureCache {
 const Instance& instance=::instance;
 Scheduler scheduler; Cache buffer_cache;
 std::vector<Image> slot_images; std::vector<ImageId> download_images;
 TextureCache() { buffer_cache.ring.scheduler=&scheduler; }
 void ProcessDownloadImages(); void DownloadImageMemory(ImageId,bool);
};
'''
EPILOGUE=r'''
int main(int argc,char** argv) {
 bool coherent=argc>1 && argv[1][0]=='1'; bool async=argc>2;
 TextureCache cache; cache.buffer_cache.ring.is_coherent=coherent;
 std::array<std::array<u8,4>,3> guest{};
 for(int i=0;i<3;++i) {
   Image image; image.marker=17+i; image.info.guest_address=std::bit_cast<u64>(guest[i].data());
   cache.slot_images.push_back(image); cache.download_images.push_back(i);
 }
 if(async) {
   cache.DownloadImageMemory(0,false);
   if(guest[0][0] || cache.scheduler.waits) return 20;
   cache.scheduler.Finish();
 } else { cache.ProcessDownloadImages(); }
 for(int i=0;i<(async?1:3);++i)
   for(auto byte:guest[i]) if(byte!=17+i) return 21;
 if(!async && !cache.download_images.empty()) return 22;
 if(KyoeiReadback::active) return 23;
 bool batch=KyoeiReadback::Enabled();
 unsigned expected=async?1:(batch?2:3);
 if(cache.scheduler.waits!=expected) return 24;
 std::printf("visible bytes correct; coherent=%d async=%d waits=%u batch=%d\n",coherent,async,cache.scheduler.waits,batch);
 return 0;
}
'''

def methods(path):
    source=path.read_text(encoding='utf8')
    start=source.index('void TextureCache::ProcessDownloadImages() {')
    end=source.index('void TextureCache::MarkAsMaybeDirty',start)
    return source[start:end]

def main():
    BUILD.mkdir(parents=True,exist_ok=True)
    reference=ROOT/'third_party/godzilla-core/third_party/shadPS4/src/video_core/texture_cache/texture_cache.cpp'
    for name,path in [('reference',reference),('adapted',ROOT/'build/kyoei_texture_cache.cpp')]:
        source=BUILD/(name+'.cpp'); binary=BUILD/(name+'.exe')
        source.write_text(PRELUDE+methods(path)+EPILOGUE,encoding='utf8')
        subprocess.run([shutil.which('clang++') or 'clang++','-std=c++23','-O2','-D_CRT_SECURE_NO_WARNINGS',
                        '-I'+str(ROOT/'src'),str(source),'-o',str(binary)],check=True)
        for batch in (False,True):
            if name=='reference' and batch: continue
            for coherent,async_ in ((True,False),(False,False),(True,True),(False,True)):
                env=dict(os.environ,KYOEI_BATCH_READBACK='1' if batch else '0')
                result=subprocess.run([str(binary),'1' if coherent else '0']+(['async'] if async_ else []),env=env)
                expected=21 if name=='reference' and not coherent else 0
                if result.returncode!=expected:
                    raise RuntimeError(f'{name} batch={batch} coherent={coherent} async={async_}: {result.returncode}, expected {expected}')
                if expected: print('Reference noncoherent stale bytes reproduced',flush=True)
    print('READBACK INTEGRATION PASS: real methods, grouped fences, ring wrap, cache visibility, async publication')

if __name__=='__main__': main()
