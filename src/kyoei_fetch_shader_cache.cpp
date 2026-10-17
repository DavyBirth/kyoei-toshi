#include "kyoei_fetch_cache.h"
#include "common/assert.h"
#include "shader_recompiler/frontend/fetch_shader.h"
#include <chrono>
#include <cstdlib>
#include <cstdio>

extern "C" void gz_log(const char*,...);
namespace Shader::Gcn {
std::optional<FetchShaderData> ParseFetchShaderUncached(const Shader::Info& info);
namespace {
thread_local uint64_t lookups=0,hits=0;
bool Enabled() {
    static const bool enabled=[] { const auto* p=std::getenv("KYOEI_FETCH_CACHE");return !p||p[0]=='1'; }();
    static const bool ab=[] { const auto* p=std::getenv("KYOEI_FETCH_CACHE_AB");return p&&p[0]=='1'; }();
    static const auto* control=std::getenv("KYOEI_FETCH_CACHE_CONTROL");
    if(control) {
        static thread_local uint64_t control_calls=0;
        static thread_local bool active=false;
        if((control_calls++&511)==0) {
            if(auto* file=std::fopen(control,"rb")) { active=std::fgetc(file)=='1';std::fclose(file); }
            else active=false;
        }
        return active;
    }
    if(!ab)return enabled;
    static thread_local const auto start=std::chrono::steady_clock::now();
    static thread_local uint64_t calls=0;
    static thread_local int phase=-1;
    if((calls++&511)==0) {
        const auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count();
        const int next=int(ms/20000);
        if(next!=phase) {
            phase=next;
            gz_log("Kyoei fetch cache AB: phase=%d enabled=%u elapsed_ms=%llu previous_lookups=%llu previous_hits=%llu",phase,unsigned(phase&1),(unsigned long long)ms,(unsigned long long)lookups,(unsigned long long)hits);
            lookups=hits=0;
        }
    }
    return (phase&1)!=0;
}
bool Verify() {
    static const bool enabled=[] { const auto* p=std::getenv("KYOEI_VERIFY_FETCH_CACHE");return p&&p[0]=='1'; }();
    return enabled;
}
bool Equal(const FetchShaderData& a,const FetchShaderData& b) {
    if(a.size!=b.size||a.vertex_offset_sgpr!=b.vertex_offset_sgpr||
       a.instance_offset_sgpr!=b.instance_offset_sgpr||a.attributes.size()!=b.attributes.size())return false;
    for(size_t i=0;i<a.attributes.size();++i) {
        const auto& x=a.attributes[i];const auto& y=b.attributes[i];
        if(x.semantic!=y.semantic||x.dest_vgpr!=y.dest_vgpr||x.num_elements!=y.num_elements||
           x.sgpr_base!=y.sgpr_base||x.dword_offset!=y.dword_offset||x.instance_data!=y.instance_data||
           x.inst_offset!=y.inst_offset||x.data_format!=y.data_format||x.num_format!=y.num_format)return false;
    }
    return true;
}
}
std::optional<FetchShaderData> ParseFetchShader(const Shader::Info& info) {
    if(!info.has_fetch_shader)return std::nullopt;
    if(!Enabled()&&!Verify())return ParseFetchShaderUncached(info);
    static thread_local KyoeiFetchCache::Cache<FetchShaderData> cache;
    const auto* code=GetFetchShaderCode(info,info.fetch_shader_sgpr_base);
    ++lookups;
    if(const auto* cached=cache.Probe(code)) {
        ++hits;
        if(Verify()) {
            auto original=ParseFetchShaderUncached(info);
            ASSERT_MSG(original&&Equal(*cached,*original),"Kyoei fetch cache prediction mismatch");
            static thread_local unsigned checked=0;
            if(++checked==10000)gz_log("Kyoei fetch cache: 10000 complete decodings matched original");
            return original;
        }
        return *cached;
    }
    auto original=ParseFetchShaderUncached(info);
    if(original)cache.Record(code,original->size,*original);
    return original;
}
}
