#include "kyoei_stage_cache.h"
#include <cstdio>
#include <random>

extern "C" void gz_log(const char*,...){}
extern "C" [[noreturn]] void gz_stop(int,const char*,...){std::abort();}
namespace Shader::Gcn {
// This fixture tests cache inputs, not instruction decoding. Real decoding is
// exercised by the in-game differential verification mode.
std::optional<FetchShaderData> ParseFetchShader(const Shader::Info& info){
    if(!info.has_fetch_shader)return {};
    FetchShaderData data;data.size=4;
    const u32* code;std::memcpy(&code,&info.user_data[info.fetch_shader_sgpr_base],sizeof(code));
    data.vertex_offset_sgpr=static_cast<s8>(*code&0x7f);return data;
}
const u32* GetFetchShaderCode(const Info& info,u32 base){
    const u32* code;std::memcpy(&code,&info.user_data[base],sizeof(code));return code;
}
}
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"failed line %d\n",__LINE__);return 1;}}while(0)
int main(){
    _putenv_s("KYOEI_STAGE_CACHE","1");
    Shader::Info info{};info.stage=Shader::Stage::Fragment;info.l_stage=Shader::LogicalStage::Fragment;
    Shader::RuntimeInfo runtime{};runtime.stage=Shader::Stage::Fragment;
    Shader::Profile profile{};
    Shader::Backend::Bindings start{};
    auto& desc=info.buffers.emplace_back();desc={};
    desc.inline_cbuf=AmdGpu::Buffer::Placeholder(128);desc.inline_cbuf.stride=16;
    desc.buffer_type=Shader::BufferType::Guest;
    int owner=0,other=0;
    auto probe=[&](size_t modules=1){return KyoeiStageCache::Probe(&owner,info,runtime,start,modules);};
    auto original=[&]{return Shader::StageSpecialization(info,runtime,profile,start);};
    auto record=[&](const auto& spec){KyoeiStageCache::Record(info,runtime,start,spec,0,1);};
    CHECK(!probe());auto spec=original();record(spec);CHECK(probe()==0);
    std::mt19937_64 rng(0x603ff0);
    for(unsigned i=0;i<5000;++i){
        desc.inline_cbuf.base_address=(rng()&0xfffffff)|0x1000;
        desc.inline_cbuf.num_records=1+unsigned(rng()%100000);
        CHECK(probe()==0);CHECK(original()==spec); // addresses never specialize
    }
    ++desc.inline_cbuf.stride;CHECK(!probe());CHECK(!(original()==spec));
    spec=original();record(spec);CHECK(probe()==0);
    runtime.fs_info.dual_source_blending=true;CHECK(!probe());runtime.fs_info.dual_source_blending=false;CHECK(probe()==0);
    ++start.buffer;CHECK(!probe());--start.buffer;CHECK(probe()==0);
    CHECK(!probe(2));CHECK(probe()==0);
    CHECK(!KyoeiStageCache::Probe(&other,info,runtime,start,1));
    KyoeiStageCache::Invalidate();CHECK(!probe());
    record(original());CHECK(probe()==0);
    // A changed guest descriptor between the two observations cannot be saved.
    ++desc.inline_cbuf.stride;CHECK(!probe());spec=original();++desc.inline_cbuf.stride;
    record(spec);CHECK(!probe());record(original());CHECK(probe()==0);
    // Cover image/fmask/sampler inputs as well as buffers. Addresses may
    // change; image geometry, mip counts and sampler modes may not be lost.
    AmdGpu::Image image=AmdGpu::Image::Null(false);
    image.base_address=0x1000;
    info.flattened_ud_buf.resize(16);
    std::memcpy(info.flattened_ud_buf.data(),&image,sizeof(image));
    std::memcpy(info.flattened_ud_buf.data()+8,&image,sizeof(image));
    auto& image_desc=info.images.emplace_back();image_desc={};image_desc.sharp_idx=0;
    image_desc.mip_fallback_mode=Shader::MipStorageFallbackMode::DynamicIndex;
    auto& fmask=info.fmasks.emplace_back();fmask.sharp_idx=8;
    auto& sampler=info.samplers.emplace_back();sampler={};sampler.is_inline_sampler=1;
    KyoeiStageCache::Invalidate();CHECK(!probe());spec=original();record(spec);CHECK(probe()==0);
    for(unsigned i=0;i<1000;++i){
        image.base_address=(rng()&0xfffffff)|0x1000;
        std::memcpy(info.flattened_ud_buf.data(),&image,sizeof(image));
        std::memcpy(info.flattened_ud_buf.data()+8,&image,sizeof(image));
        CHECK(probe()==0);CHECK(original()==spec);
    }
    image.base_address=0;std::memcpy(info.flattened_ud_buf.data(),&image,sizeof(image));CHECK(!probe());
    image.base_address=0x1000;std::memcpy(info.flattened_ud_buf.data(),&image,sizeof(image));CHECK(probe()==0);
    sampler.inline_sampler.force_unnormalized.Assign(1);CHECK(!probe());
    sampler.inline_sampler.force_unnormalized.Assign(0);CHECK(probe()==0);
    ++image.last_level;std::memcpy(info.flattened_ud_buf.data(),&image,sizeof(image));CHECK(!probe());
    --image.last_level;std::memcpy(info.flattened_ud_buf.data(),&image,sizeof(image));CHECK(probe()==0);
    ++image.width;std::memcpy(info.flattened_ud_buf.data()+8,&image,sizeof(image));CHECK(!probe());
    --image.width;std::memcpy(info.flattened_ud_buf.data()+8,&image,sizeof(image));CHECK(probe()==0);
    info.l_stage=Shader::LogicalStage::TessellationEval;CHECK(!probe());
    info.l_stage=Shader::LogicalStage::Vertex;info.stage=Shader::Stage::Vertex;
    info.has_fetch_shader=true;info.fetch_shader_sgpr_base=0;
    alignas(4096) std::array<u32,1024> code{};code[0]=0xbeef;
    std::array<u32,16> ud{};const auto* pointer=code.data();std::memcpy(ud.data(),&pointer,sizeof(pointer));info.user_data=ud;
    KyoeiStageCache::Invalidate();CHECK(!probe());record(original());
    CHECK(!probe());record(original());CHECK(probe()==0);
    code[0]^=1;CHECK(!probe());record(original());CHECK(!probe());record(original());CHECK(probe()==0);
    CHECK(probe()==0);auto mixed=original();code[0]^=2;record(mixed);
    CHECK(!probe());record(original());CHECK(!probe());record(original());CHECK(probe()==0);
    CHECK(!KyoeiStageCache::FitsPage(code.data()+1023,8));
    CHECK(KyoeiStageCache::FitsPage(code.data()+1023,4));
    CHECK(!KyoeiStageCache::FitsPage(code.data(),513));
    KyoeiStageCache::Key key;std::array<unsigned char,4097> too_large{};
    CHECK(!key.Add(too_large));
    // Mix fields that do and do not specialize. Every predicted hit must
    // still match the complete constructor, including null descriptors.
    info.has_fetch_shader=false;info.stage=Shader::Stage::Fragment;
    info.l_stage=Shader::LogicalStage::Fragment;
    sampler.inline_sampler.max_lod.Assign(1);
    KyoeiStageCache::Invalidate();CHECK(!probe());
    auto resident=original();record(resident);
    for(unsigned i=0;i<1000;++i){
        image.width=unsigned(rng()%2048);image.height=unsigned(rng()%2048);
        std::memcpy(info.flattened_ud_buf.data(),&image,sizeof(image));
        sampler.inline_sampler.max_lod.Assign(1+rng()%4095);
        sampler.inline_sampler.lod_bias.Assign(rng()%16384);
        CHECK(probe()==0);CHECK(original()==resident);
    }
    for(unsigned i=0;i<5000;++i){
        desc.inline_cbuf.stride=unsigned(rng()%4)*16;
        desc.inline_cbuf.num_records=unsigned(rng()%1000);
        desc.inline_cbuf.swizzle_enable=unsigned(rng()%2);
        desc.inline_cbuf.index_stride=unsigned(rng()%4);
        desc.inline_cbuf.element_size=unsigned(rng()%4);
        image.width=unsigned(rng()%2048);image.height=unsigned(rng()%2048);
        image.base_address=(rng()%8)?0x1000:0;
        image.last_level=unsigned(rng()%4);
        std::memcpy(info.flattened_ud_buf.data(),&image,sizeof(image));
        sampler.inline_sampler.max_lod.Assign(rng()%4096);
        sampler.inline_sampler.lod_bias.Assign(rng()%16384);
        sampler.inline_sampler.force_degamma.Assign(rng()%2);
        const auto expected=original();
        if(probe()){CHECK(expected==resident);}
        else{resident=expected;record(resident);}
        CHECK(probe()==0);CHECK(original()==resident);
    }
    // Fill beyond the bound: eviction may lose a hit, never return another
    // owner's result. Touching a live entry must protect it from the next miss.
    Shader::Info empty{};
    empty.stage=Shader::Stage::Fragment;empty.l_stage=Shader::LogicalStage::Fragment;
    std::array<int,KyoeiStageCache::Capacity+1> owners{};
    KyoeiStageCache::Invalidate();
    for(auto& cache_owner:owners){
        CHECK(!KyoeiStageCache::Probe(&cache_owner,empty,runtime,start,1));
        const Shader::StageSpecialization expected(empty,runtime,profile,start);
        KyoeiStageCache::Record(empty,runtime,start,expected,0,1);
        CHECK(KyoeiStageCache::Probe(&cache_owner,empty,runtime,start,1)==0);
    }
    CHECK(KyoeiStageCache::Probe(&owners[1],empty,runtime,start,1)==0);
    CHECK(!KyoeiStageCache::Probe(&owners[0],empty,runtime,start,1));
    CHECK(KyoeiStageCache::Probe(&owners[1],empty,runtime,start,1)==0);
    std::puts("stage cache: 12000 differential resource changes, mutation/runtime/binding/module/owner/epoch/page/eviction fixtures PASS");
}
