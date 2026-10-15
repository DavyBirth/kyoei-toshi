// Driver pipeline blobs are local, optional and validated before Vulkan sees them.
#pragma once
#include <array>
#include <cstdint>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <filesystem>
#include <fstream>
#include <span>
#include <vector>
#include <windows.h>
namespace KyoeiPipelineCache {
constexpr size_t MaxBytes=64u<<20;
struct Identity { uint32_t vendor,device;std::array<uint8_t,16> uuid; };
template<class Profile> inline bool ProfileMatches(std::span<const uint8_t> bytes,const Profile &profile) {
    return bytes.size()==sizeof(Profile) && std::memcmp(bytes.data(),&profile,sizeof(Profile))==0;
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
}
