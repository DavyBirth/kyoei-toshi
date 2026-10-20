// Real presenter/readback test with synthetic pixels; no campaign execution.
// SPDX-License-Identifier: GPL-2.0-or-later
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0A00
#endif
#include <array>
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <filesystem>
#include <memory>
#include <vector>
#include "core/debug_state.h"
#include "core/emulator_settings.h"
#include "core/libraries/videoout/video_out.h"
#include "core/memory.h"
#include "video_core/renderer_vulkan/vk_presenter.h"
extern "C" {
#include "gz.h"
}
extern std::unique_ptr<Vulkan::Presenter> presenter;
namespace Libraries::Kernel {
int PS4_SYSV_ABI sceKernelCreateEqueue(OrbisKernelEqueue*,const char*);
int PS4_SYSV_ABI sceKernelDeleteEqueue(OrbisKernelEqueue);
int PS4_SYSV_ABI sceKernelWaitEqueue(OrbisKernelEqueue,OrbisKernelEvent*,int,int*,OrbisKernelUseconds*);
}
namespace Libraries::VideoOut {
s32 PS4_SYSV_ABI sceVideoOutDeleteFlipEvent(Libraries::Kernel::OrbisKernelEqueue,s32);
}
static void require(bool ok,const char* message){if(!ok)gz_stop(24,"GRAPHICS TEST: FAIL: %s",message);}
extern "C" void kyoei_graphics_self_test(){
    using namespace Libraries::VideoOut;
    using namespace Libraries::Kernel;
    constexpr u32 w=1920,h=1080;
    constexpr u64 size=(u64(w)*h*4+0x3fff)&~u64(0x3fff);
    using Alloc=s32 (ABI*)(s64,s64,u64,u64,s32,s64*);
    using Map=s32 (ABI*)(void**,u64,s32,s32,s64,u64);
    s64 offset{};void* memory{};
    require(reinterpret_cast<Alloc>(gz_memory_resolve("libkernel","sceKernelAllocateDirectMemory"))(0,5056ULL<<20,size,0x4000,0,&offset)==0,"allocation");
    require(reinterpret_cast<Map>(gz_memory_resolve("libkernel","sceKernelMapDirectMemory"))(&memory,size,0x33,0,offset,0x4000)==0,"map");
    auto* source=static_cast<u32*>(memory);
    for(u32 y=0;y<h;++y)for(u32 x=0;x<w;++x){
        u32 color=y<h/2?(x<w/2?0xff0000ff:0xff00ff00):(x<w/2?0xffff0000:0xffffffff);
        if(x>w/3 && x<2*w/3 && y>h/3 && y<2*h/3)color=((x/3+y/3)&1)?0xff101010:0xffeeeeee;
        source[y*w+x]=color;
    }
    const s32 handle=sceVideoOutOpen(1,0,0,nullptr);require(handle>0,"open");
    BufferAttribute attribute{};
    sceVideoOutSetBufferAttribute(&attribute,PixelFormat::A8B8G8R8Srgb,1,0,w,h,w);
    require(sceVideoOutRegisterBuffers(handle,0,&memory,1,&attribute)>=0,"register");
    OrbisKernelEqueue queue{};require(sceKernelCreateEqueue(&queue,"kyoei_graphics_test")==0,"queue");
    require(sceVideoOutAddFlipEvent(queue,handle,nullptr)==0,"flip event");
    require(sceVideoOutSubmitFlip(handle,0,1,1)==0,"submit");
    OrbisKernelEvent event{};int count{};OrbisKernelUseconds timeout=10000000;
    require(sceKernelWaitEqueue(queue,&event,1,&count,&timeout)==0 && count==1,"completed flip");
    std::vector<u8> pixels;u32 width{},height{};
    require(presenter->CaptureLastFrame(pixels,width,height),"GPU output readback");
    const auto requested=[](const char* name,u32 fallback){const char* v=std::getenv(name);return v?u32(std::strtoul(v,nullptr,10)):fallback;};
    require(width==requested("KYOEI_WINDOW_WIDTH",1280) && height==requested("KYOEI_WINDOW_HEIGHT",720),"actual output dimensions");
    require(DebugState.game_resolution==std::array<u32,2>{w,h},"native dimensions preserved");
    const bool expected_fsr=EmulatorSettings.IsFsrEnabled() && (width>w || height>h);
    require(DebugState.is_using_fsr==expected_fsr,"actual FSR activation");
    constexpr u32 colors[][3]={{255,0,0},{0,255,0},{0,0,255},{255,255,255}};
    for(u32 q=0;q<4;++q){
        const u32 x=(q&1)?width*3/4:width/4,y=(q&2)?height*3/4:height/4;
        for(u32 c=0;c<3;++c)require(std::abs(int(pixels[(u64(y)*width+x)*4+c])-int(colors[q][c]))<=4,"quadrant color");
    }
    const char* directory=gz_user_dir();
    const auto path=std::filesystem::path(reinterpret_cast<const char8_t*>(directory))/"graphics-output.ppm";
    FILE* file=_wfopen(path.c_str(),L"wb");require(file!=nullptr,"output open");
    std::fprintf(file,"P6\n%u %u\n255\n",width,height);
    for(size_t i=0;i<pixels.size();i+=4)require(std::fwrite(pixels.data()+i,1,3,file)==3,"output write");
    require(std::fclose(file)==0,"output close");
    require(sceVideoOutDeleteFlipEvent(queue,handle)==0 && sceKernelDeleteEqueue(queue)==0,"release event");
    require(sceVideoOutClose(handle)==0,"close");
    using Unmap=s32 (ABI*)(void*,u64);using Release=s32 (ABI*)(s64,u64);
    require(reinterpret_cast<Unmap>(gz_memory_resolve("libkernel","sceKernelMunmap"))(memory,size)==0,"unmap");
    require(reinterpret_cast<Release>(gz_memory_resolve("libkernel","sceKernelReleaseDirectMemory"))(offset,size)==0,"release");
    gz_stop(0,"GRAPHICS TEST: PASS native=%ux%u output=%ux%u fsr=%u; GPU pixels/colors verified; game not executed",w,h,width,height,unsigned(expected_fsr));
}
