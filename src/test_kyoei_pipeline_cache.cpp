#include "kyoei_pipeline_cache.h"
#include <cassert>
int main() {
    using namespace KyoeiPipelineCache;
    Identity id{0x10de,0x1234,{1,2,3}};
    std::vector<uint8_t> bytes(48,0);uint32_t header[]={32,1,id.vendor,id.device};
    std::memcpy(bytes.data(),header,16);std::memcpy(bytes.data()+16,id.uuid.data(),16);
    assert(Matches(bytes,id));assert(!Matches(std::span(bytes).first(31),id));
    auto changed=id;changed.vendor++;assert(!Matches(bytes,changed));
    changed=id;changed.device++;assert(!Matches(bytes,changed));
    changed=id;changed.uuid[15]++;assert(!Matches(bytes,changed));
    bytes[4]=2;assert(!Matches(bytes,id));bytes[4]=1;
    _putenv_s("KYOEI_PIPELINE_CACHE_DIR","pipeline-cache-test");
    assert(Save(id,bytes));assert(Load(id)==bytes);
    bytes.back()=42;assert(Save(id,bytes));assert(Load(id)==bytes);
    auto file=Path(id);{std::ofstream f(file,std::ios::binary|std::ios::trunc);f<<"truncated";}
    assert(Load(id).empty());std::filesystem::remove(file);std::filesystem::remove(file.parent_path());
    _putenv_s("KYOEI_PIPELINE_CACHE_DIR","");assert(Load(id).empty());assert(!Save(id,bytes));
}
