// SPDX-License-Identifier: GPL-2.0-or-later
// Exact images start on the requested address's first page. Searching later
// pages cannot add another exact candidate. Keep the last match in page order,
// as the original full-range walk does. No persistent cache or stale handles.
#pragma once
namespace KyoeiImageLookup {
template<class Info,class Compatible>
bool Exact(const Info& cached,const Info& requested,bool exact_format,Compatible compatible) {
    if(cached.guest_address!=requested.guest_address || cached.guest_size!=requested.guest_size ||
       cached.size!=requested.size || !compatible(cached.pixel_format,requested.pixel_format))return false;
    const bool one_pixel=requested.size.width==1 && requested.size.height==1 && requested.size.depth==1;
    if(cached.type!=requested.type && !one_pixel)return false;
    return !exact_format || cached.pixel_format==requested.pixel_format;
}
template<class Id,class Range,class Match>
Id LastExact(const Range& candidates,Match match) {
    Id result{};
    for(const auto id:candidates)if(match(id))result=id;
    return result;
}
}
