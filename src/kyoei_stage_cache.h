#pragma once
#include <array>
#include <atomic>
#include <chrono>
#include <cstring>
#include <cstdlib>
#include <optional>
#include <unordered_map>
#include <vector>
#include "shader_recompiler/specialization.h"

extern "C" void gz_log(const char*,...);
namespace KyoeiStageCache {
inline thread_local uint64_t attempts=0,hits=0;
// Skip specialization construction only when all its inputs equal a previously
// decoded, stable observation. Never cache bound resources. On by default since
// 21/10/2026 (verified 10000 in-game predictions); KYOEI_STAGE_CACHE=0 disables.
inline bool Enabled(){
    static const bool v=[]{auto*p=std::getenv("KYOEI_STAGE_CACHE");return !p||p[0]!='0';}();
    static const bool ab=[]{auto*p=std::getenv("KYOEI_STAGE_CACHE_AB");return p&&p[0]=='1';}();
    if(!ab)return v;
    // Private same-process experiment: alternate equivalent host paths without
    // restarting the game or changing its clocks. Check wall time sparsely.
    static thread_local const auto start=std::chrono::steady_clock::now();
    static thread_local uint64_t calls=0;
    static thread_local int phase=-1;
    if((calls++&511)==0){
        const auto ms=std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now()-start).count();
        const int next=int(ms/20000);
        if(next!=phase){phase=next;gz_log("Kyoei stage cache AB: phase=%d enabled=%u elapsed_ms=%llu previous_lookups=%llu previous_hits=%llu",phase,unsigned(phase&1),(unsigned long long)ms,(unsigned long long)attempts,(unsigned long long)hits);attempts=hits=0;}
    }
    return (phase&1)!=0;
}
inline bool Verify(){static const bool v=[]{auto*p=std::getenv("KYOEI_VERIFY_STAGE_CACHE");return p&&p[0]=='1';}();return v;}
inline std::atomic<uint64_t> epoch{1};
inline void Invalidate(){epoch.fetch_add(1,std::memory_order_relaxed);}
struct Key {
    std::array<unsigned char,4096> bytes;
    size_t size=0;
    template<class T> bool Add(const T& value){
        static_assert(std::is_trivially_copyable_v<T>);
        if(sizeof(T)>bytes.size()-size)return false;
        std::memcpy(bytes.data()+size,&value,sizeof(T));size+=sizeof(T);return true;
    }
    bool operator==(const Key& other)const{return size==other.size&&std::memcmp(bytes.data(),other.bytes.data(),size)==0;}
    void Copy(const Key& other){size=other.size;std::memcpy(bytes.data(),other.bytes.data(),size);}
};
inline bool Resources(const Shader::Info& info,const std::optional<Shader::Gcn::FetchShaderData>& fetch,Key& key){
    key.size=0;
    if(!key.Add(info.stage)||!key.Add(info.l_stage)||!key.Add(info.buffers.size())||
       !key.Add(info.images.size())||!key.Add(info.fmasks.size())||!key.Add(info.samplers.size()))return false;
    // Project exactly the resource decisions read by the frozen constructor.
    // Addresses, filtering and texture extents still reach the live binder.
    if(fetch){
        if(!key.Add(fetch->size)||!key.Add(fetch->vertex_offset_sgpr)||!key.Add(fetch->instance_offset_sgpr))return false;
        for(const auto& attribute:fetch->attributes){
            const auto sharp=attribute.GetSharp(info);
            if(!key.Add(attribute)||!key.Add(bool(sharp)))return false;
            if(sharp&&(!key.Add(sharp.GetNumberFmt())||!key.Add(sharp.DstSelect())))return false;
        }
    }
    for(const auto& desc:info.buffers){
        const auto sharp=desc.GetSharp(info);
        if(!key.Add(bool(sharp)))return false;
        if(!sharp)continue;
        if(!key.Add(sharp.GetStride())||!key.Add(desc.IsStorage(sharp))||!key.Add(bool(desc.is_formatted))||
           !key.Add(u32(sharp.swizzle_enable)))return false;
        if(desc.is_formatted&&(!key.Add(sharp.GetDataFmt())||!key.Add(sharp.GetNumberFmt())||
           !key.Add(sharp.DstSelect())||!key.Add(sharp.GetNumberConversion())))return false;
        if(sharp.swizzle_enable&&(!key.Add(u32(sharp.index_stride))||!key.Add(u32(sharp.element_size))))return false;
    }
    for(const auto& desc:info.images){
        const auto sharp=desc.GetSharp(info);
        if(!key.Add(bool(sharp)))return false;
        if(!sharp)continue;
        if(!key.Add(bool(desc.is_written))||!key.Add(sharp.GetViewType(desc.is_array))||!key.Add(AmdGpu::IsInteger(sharp.GetNumberFmt()))||
           !key.Add(sharp.IsCube())||!key.Add(sharp.GetNumberConversion())||!key.Add(desc.NumBindings(info)))return false;
        if(desc.is_written){if(!key.Add(sharp.DstSelect()))return false;}
        else if(!key.Add(sharp.GetNumberFmt()==AmdGpu::NumberFormat::Srgb))return false;
    }
    for(const auto& desc:info.fmasks){
        const auto sharp=desc.GetSharp(info);
        if(!key.Add(bool(sharp)))return false;
        if(sharp&&(!key.Add(u32(sharp.width))||!key.Add(u32(sharp.height))))return false;
    }
    for(const auto& desc:info.samplers){
        const auto sharp=desc.GetSharp(info);
        if(!key.Add(bool(sharp)))return false;
        if(sharp&&(!key.Add(u32(sharp.force_unnormalized))||!key.Add(u32(sharp.force_degamma))))return false;
    }
    return true;
}
struct Entry {
    const void* owner=nullptr;
    const Shader::Info* info=nullptr;
    uint64_t epoch=0,age=0;
    size_t modules=0,index=0,code_size=0;
    bool valid=false,before_valid=false;
    Shader::RuntimeInfo runtime{};
    Shader::Backend::Bindings start{};
    std::optional<Shader::Gcn::FetchShaderData> fetch;
    std::array<unsigned char,512> code;
    Key key;
};
// One frame binds hundreds of shader programs; 8 slots thrashed. Entries live on
// the heap of the thread that uses them, found by (owner, info) hash lookup.
inline constexpr size_t Capacity=1024;
struct Slot { const void* owner; const Shader::Info* info; bool operator==(const Slot&)const=default; };
struct SlotHash { size_t operator()(const Slot& s)const{return std::hash<const void*>{}(s.owner)*31u^std::hash<const void*>{}(s.info);} };
inline thread_local std::vector<Entry> entries;
inline thread_local std::unordered_map<Slot,size_t,SlotHash> index;
inline thread_local uint64_t age=0;
inline thread_local Entry* pending=nullptr;
inline thread_local Key before,after;
inline bool Supported(const Shader::Info& info){
    return info.l_stage!=Shader::LogicalStage::TessellationControl&&info.l_stage!=Shader::LogicalStage::TessellationEval;
}
inline const void* Code(const Shader::Info& info){return Shader::Gcn::GetFetchShaderCode(info,info.fetch_shader_sgpr_base);}
inline bool FitsPage(const void* address,size_t size){
    return address&&size&&size<=512&&size<=4096-(reinterpret_cast<uintptr_t>(address)&4095);
}
inline bool CodeMatches(const Entry& entry,const Shader::Info& info){
    if(!info.has_fetch_shader)return !entry.fetch&&entry.code_size==0;
    const void* code=Code(info);
    return entry.fetch&&FitsPage(code,entry.code_size)&&std::memcmp(code,entry.code.data(),entry.code_size)==0;
}
inline std::optional<size_t> Probe(const void* owner,const Shader::Info& info,
                                 const Shader::RuntimeInfo& runtime,Shader::Backend::Bindings start,size_t modules){
    pending=nullptr;
    if((!Enabled()&&!Verify())||!Supported(info))return {};
    ++attempts;
    const uint64_t current_epoch=epoch.load(std::memory_order_relaxed);
    if(entries.empty())entries.resize(Capacity);
    Entry* entry=nullptr;
    const Slot slot{owner,&info};
    auto found=index.find(slot);
    if(found!=index.end()&&entries[found->second].epoch==current_epoch)entry=&entries[found->second];
    if(!entry){
        if(found!=index.end())entry=&entries[found->second];
        else{
            entry=&entries[0];
            for(auto& e:entries){if(!e.owner){entry=&e;break;}if(e.age<entry->age)entry=&e;}
            if(entry->owner)index.erase(Slot{entry->owner,entry->info});
            index.emplace(slot,size_t(entry-entries.data()));
        }
        entry->owner=owner;entry->info=&info;entry->epoch=current_epoch;
        entry->valid=false;entry->fetch.reset();entry->code_size=0;
    }
    pending=entry;entry->age=++age;
    entry->before_valid=CodeMatches(*entry,info)&&Resources(info,entry->fetch,before)&&CodeMatches(*entry,info);
    if(entry->valid&&entry->before_valid&&entry->modules==modules&&entry->index<modules&&
       entry->runtime==runtime&&entry->start==start&&entry->key==before){++hits;return entry->index;}
    return {};
}
inline void Record(const Shader::Info& info,const Shader::RuntimeInfo& runtime,
                   Shader::Backend::Bindings start,const Shader::StageSpecialization& spec,
                   size_t index,size_t modules){
    if(!pending||pending->info!=&info)return;
    auto& entry=*pending;
    // Two equal observations enclose the original constructor. A first fetch
    // observation only warms the snapshot; the next slow call must confirm it.
    entry.valid=entry.before_valid&&Resources(info,spec.fetch_shader_data,after)&&before==after&&CodeMatches(entry,info);
    if(entry.valid)entry.key.Copy(after);
    entry.runtime=runtime;entry.start=start;entry.index=index;entry.modules=modules;
    entry.fetch=spec.fetch_shader_data;
    if(!info.has_fetch_shader)entry.code_size=0;
    if(!entry.valid&&info.has_fetch_shader&&entry.fetch){
        entry.code_size=0;
        const void* code=Code(info);const size_t size=entry.fetch->size;
        if(FitsPage(code,size)){entry.code_size=size;std::memcpy(entry.code.data(),code,size);}
        else entry.valid=false;
    }
}
inline void Verified(){
    static thread_local unsigned count=0;
    if(++count==10000)gz_log("Kyoei stage cache: 10000 predictions matched original specialization lookup");
}
}
