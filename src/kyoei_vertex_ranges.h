#pragma once
#include <algorithm>
#include <cstdlib>

namespace KyoeiVertexRanges {
inline bool LegacySort() {
    static const bool legacy=[] { const char* value=std::getenv("KYOEI_LEGACY_VERTEX_SORT");return value && value[0]=='1'; }();
    return legacy;
}
// A draw commonly uses overlapping views of one vertex allocation. Prove that
// their union is connected in input order before replacing sort/merge. A gap
// falls back without touching the output, even if a later range could bridge it.
template<class Ranges> inline bool TryConnected(const Ranges& ranges,Ranges& merged) {
    if(ranges.empty())return false;
    auto united=ranges.front();
    for(const auto& range:ranges) {
        if(range.base_address>united.end_address || range.end_address<united.base_address)return false;
        united.base_address=std::min(united.base_address,range.base_address);
        united.end_address=std::max(united.end_address,range.end_address);
    }
    merged.emplace_back(united);
    return true;
}
}
