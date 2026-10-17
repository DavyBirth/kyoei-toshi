// Driver pipeline blobs are local, optional and validated before Vulkan sees them.
#pragma once
#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <span>
#include <vector>
#include <windows.h>
#include "kyoei_cache_jobs.h"
extern "C" void gz_log(const char *format,...);
namespace KyoeiPipelineCache {
constexpr size_t MaxBytes=64u<<20;
struct Identity { uint32_t vendor,device;std::array<uint8_t,16> uuid; };
inline std::mutex file_mutex;
template<class Profile> inline bool ProfileMatches(std::span<const uint8_t> bytes,const Profile &profile,
                                                   size_t compared_bytes=sizeof(Profile)) {
    if(compared_bytes==0 || compared_bytes>sizeof(Profile))return false;
    const bool matches=bytes.size()==sizeof(Profile) && std::memcmp(bytes.data(),&profile,compared_bytes)==0;
    const char *stats=std::getenv("KYOEI_FRAME_STATS");
    if(!matches && stats && stats[0]=='1') {
        gz_log("Kyoei shader profile mismatch: cached_bytes=%llu current_bytes=%llu",
            (unsigned long long)bytes.size(),(unsigned long long)sizeof(Profile));
        const auto *current=reinterpret_cast<const uint8_t*>(&profile);
        unsigned reported=0;
        for(size_t i=0;i<bytes.size() && i<compared_bytes && reported<8;++i) {
            if(bytes[i]==current[i])continue;
            ++reported;
            gz_log("Kyoei shader profile difference: offset=%llu cached=%u current=%u",
                (unsigned long long)i,unsigned(bytes[i]),unsigned(current[i]));
        }
    }
    return matches;
}
// ABI of the frozen Shader::Profile: 28 scalar bytes, 38 consecutive bools,
// then six trailing alignment bytes. Keep the stored format and compare every
// field byte, excluding only padding. Review these guards if the snapshot changes.
template<class Profile> inline bool ShaderProfileMatches(std::span<const uint8_t> bytes,const Profile &profile) {
    static_assert(sizeof(Profile)==72);
    static_assert(offsetof(Profile,support_int8)==28);
    static_assert(offsetof(Profile,supports_shader_stencil_export)==65);
    static_assert(sizeof(profile.supports_shader_stencil_export)==1);
    return ProfileMatches(bytes,profile,66);
}
inline bool Matches(std::span<const uint8_t> bytes,const Identity &id) {
    if(bytes.size()<32||bytes.size()>MaxBytes)return false;
    uint32_t words[4];std::memcpy(words,bytes.data(),16);
    return words[0]>=32&&words[0]<=bytes.size()&&words[1]==1&&words[2]==id.vendor&&words[3]==id.device&&
        std::memcmp(bytes.data()+16,id.uuid.data(),16)==0;
}
inline std::filesystem::path Path(const Identity &id) {
    const char *directory=std::getenv("KYOEI_PIPELINE_CACHE_DIR");if(!directory||!*directory)return {};
    char key[80];std::snprintf(key,sizeof(key),"%08x-%08x-",id.vendor,id.device);
    size_t n=std::strlen(key);for(uint8_t b:id.uuid){std::snprintf(key+n,3,"%02x",b);n+=2;}
    return std::filesystem::u8path(directory)/(std::string(key)+".vkcache");
}
inline std::vector<uint8_t> Load(const Identity &id) {
    auto path=Path(id);if(path.empty())return {};
    std::ifstream file(path,std::ios::binary|std::ios::ate);if(!file)return {};
    const auto size=file.tellg();if(size<32||size>MaxBytes)return {};
    std::vector<uint8_t> bytes(static_cast<size_t>(size));file.seekg(0);
    if(!file.read(reinterpret_cast<char*>(bytes.data()),bytes.size())||!Matches(bytes,id))return {};
    return bytes;
}
inline bool Save(const Identity &id,std::span<const uint8_t> bytes) {
    std::lock_guard lock{file_mutex};
    auto path=Path(id);if(path.empty()||!Matches(bytes,id))return false;
    std::error_code error;std::filesystem::create_directories(path.parent_path(),error);if(error)return false;
    auto temp=path;temp+=".tmp";
    { std::ofstream file(temp,std::ios::binary|std::ios::trunc);
      if(!file||!file.write(reinterpret_cast<const char*>(bytes.data()),bytes.size()))return false; }
    // Same-directory atomic replacement; failure retains the previous valid cache.
    return MoveFileExW(temp.c_str(),path.c_str(),MOVEFILE_REPLACE_EXISTING|MOVEFILE_WRITE_THROUGH)!=0;
}
template<class Instance,class Cache> inline bool Persist(const Instance &instance,const Cache &cache) {
    if(!std::getenv("KYOEI_PIPELINE_CACHE_DIR"))return false;
    const auto properties=instance.GetPhysicalDevice().getProperties();
    Identity id{properties.vendorID,properties.deviceID,{}};
    std::memcpy(id.uuid.data(),properties.pipelineCacheUUID.data(),16);
    auto result=instance.GetDevice().getPipelineCacheData(*cache);
    return static_cast<int>(result.result)==0 && Save(id,result.value);
}
template<class Instance,class Cache> inline bool Checkpoint(const Instance &instance,const Cache &cache,
                                                           uint64_t generation,bool force=false) {
    static const bool enabled=[] {const char *p=std::getenv("KYOEI_PIPELINE_CACHE_DIR");return p && *p;}();
    if(!enabled)return false;
    static thread_local CheckpointGate gate;
    if(!force && !gate.Poll(generation))return false;
    const auto start=std::chrono::steady_clock::now();
    const uint64_t now_ms=std::chrono::duration_cast<std::chrono::milliseconds>(start.time_since_epoch()).count();
    if(!force && !gate.Due(now_ms))return false;
    bool queued=false;
    try {
        const auto properties=instance.GetPhysicalDevice().getProperties();
        Identity id{properties.vendorID,properties.deviceID,{}};
        std::memcpy(id.uuid.data(),properties.pipelineCacheUUID.data(),16);
        // Called after warmup, or between synchronous pipeline lookups on the
        // GPU command thread. Only an owned byte vector is passed to the writer.
        auto result=instance.GetDevice().getPipelineCacheData(*cache);
        if(static_cast<int>(result.result)==0 && Matches(result.value,id)) {
            const auto bytes=result.value.size();
            queued=Writer().Submit([id,data=std::move(result.value)] {return Save(id,data);});
            const double ms=std::chrono::duration<double,std::milli>(std::chrono::steady_clock::now()-start).count();
            gz_log("Kyoei driver cache: snapshot queued=%u bytes=%llu capture_ms=%.2f",unsigned(queued),(unsigned long long)bytes,ms);
        } else gz_log("Kyoei driver cache: invalid driver snapshot; previous cache retained");
    } catch(...) {gz_log("Kyoei driver cache: snapshot failed; previous cache retained");}
    gate.Record(generation,now_ms,queued);return queued;
}
}
