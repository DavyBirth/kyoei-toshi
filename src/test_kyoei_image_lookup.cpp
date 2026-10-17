// SPDX-License-Identifier: GPL-2.0-or-later
#include "kyoei_image_lookup.h"
#include <algorithm>
#include <cstdio>
#include <random>
#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <cstdint>
struct Extent {unsigned width,height,depth;bool operator==(const Extent&)const=default;};
struct Info {uint64_t guest_address,guest_size;Extent size;unsigned pixel_format,type;};
struct Image {Info info;bool picked;};
bool Compatible(unsigned a,unsigned b){return a/2==b/2;}
bool ReferenceExact(const Info& a,const Info& b,bool exact) {
    if(a.guest_address!=b.guest_address || a.guest_size!=b.guest_size || a.size!=b.size)return false;
    if(!Compatible(a.pixel_format,b.pixel_format) || (a.type!=b.type && b.size!=Extent{1,1,1}))return false;
    if(exact && a.pixel_format!=b.pixel_format)return false;
    return true;
}
bool Overlaps(const Info& a,const Info& b){return a.guest_address<b.guest_address+b.guest_size && b.guest_address<a.guest_address+a.guest_size;}
int main() {
    constexpr uint64_t Page=1<<20;
    std::mt19937 rng(0x357262);
    unsigned matches=0;
    for(unsigned round=0;round<5000;++round) {
        std::vector<Image> images(201);
        std::unordered_map<uint64_t,std::vector<unsigned>> pages;
        // Registration order is deliberately different from slot/address order.
        std::vector<unsigned> order;
        for(unsigned i=1;i<images.size();++i)order.push_back(i);
        std::shuffle(order.begin(),order.end(),rng);
        for(const auto id:order) {
            auto& image=images[id];
            const auto address=(uint64_t(rng()%64)+1)*Page+(rng()%3)*256;
            image={{address,(rng()%32+1)*Page,{unsigned(rng()%4+1),unsigned(rng()%4+1),1},rng()%6,rng()%3},rng()%11==0};
            // Exact aliases with compatible/different formats and 1x1 types.
            if(id%4==0)image.info=images[order[0]].info;
            image.info.pixel_format=rng()%6;image.info.type=rng()%3;
            if(id%17==0)continue; // deleted image: no stale ID in the index
            for(auto page=image.info.guest_address/Page;page<=(image.info.guest_address+image.info.guest_size-1)/Page;++page)
                pages[page].push_back(id);
        }
        Info request=images[rng()%200+1].info;
        if(round%5==0)request.guest_address+=Page/2; // overlap without exact match
        if(round%7==0)request.guest_size+=256;
        const bool exact=rng()%2;
        std::unordered_set<unsigned> seen;
        unsigned reference=0;
        for(auto page=request.guest_address/Page;page<=(request.guest_address+request.guest_size-1)/Page;++page) {
            for(auto id:pages[page]) {
                const auto& image=images[id];
                if(image.picked || !Overlaps(image.info,request) || !seen.insert(id).second)continue;
                if(ReferenceExact(image.info,request,exact))reference=id;
            }
        }
        auto fast=KyoeiImageLookup::LastExact<unsigned>(pages[request.guest_address/Page],[&](unsigned id){
            const auto& image=images[id];
            return !image.picked && Overlaps(image.info,request) && KyoeiImageLookup::Exact(image.info,request,exact,Compatible);
        });
        if(fast!=reference){std::fprintf(stderr,"image lookup mismatch: round %u fast %u reference %u\n",round,fast,reference);return 1;}
        matches+=fast!=0;
    }
    if(matches<1000)return 2;
    std::printf("IMAGE LOOKUP PASS: 5000 randomized page layouts, %u exact hits; aliases, registration order, deleted IDs, picked flags, compatible formats and overlap fallback\n",matches);
}
