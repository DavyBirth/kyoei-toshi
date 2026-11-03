#include "kyoei_live_prompts.h"
#include <array>
#include <cassert>
static unsigned current_mode;
static std::vector<unsigned char> guest(64);
static unsigned current_family=KYOEI_PAD_PLAYSTATION;
extern "C" unsigned kyoei_input_current_mode(void) { return current_mode; }
extern "C" unsigned kyoei_input_current_family(void) { return current_family; }
extern "C" int gz_memory_copy(uintptr_t address,void *out,uint64_t size,int write) {
    if(write || address<0x1000 || address-0x1000>guest.size() || size>guest.size()-(address-0x1000))return 0;
    std::memcpy(out,guest.data()+address-0x1000,size);return 1;
}
extern "C" void gz_log(const char *,...) {}
struct Info {
    struct {bool is_tiled{},is_block{};} props;
    struct {unsigned layers{1},levels{2};} resources;
    struct {unsigned width{2},height{2},depth{1};} size;
    struct Mip {unsigned size,pitch,height,offset;};
    std::array<Mip,2> mips_layout{{{32,4,2,0},{16,4,1,32}}};
    uint64_t guest_address{0x1000},guest_size{48};
    unsigned num_samples{1},num_bits{32},pitch{4},array_mode{1},tile_mode{8};
};
struct Image { Info info; uint64_t image_uid{1}; };
int main() {
    using namespace KyoeiPrompts;
    if (std::getenv("KYOEI_PROMPT_PACK")) {
        Initialize();
        assert(!pairs.empty());
        bool swapped=false;
        for (const auto &p:pairs) {
            assert(p.american.size()==p.controller.size());
            swapped|=p.american!=p.controller;
        }
        assert(swapped);
        pairs.clear();
    }
    initialized=true;
    Pair pair{2,2,2,1,0,1,20,std::vector<unsigned char>(20,12),std::vector<unsigned char>(20,34)};
    Image image;
    assert(Rows(pair,image.info,[&](size_t at,size_t from,size_t bytes){
        std::memcpy(guest.data()+at,pair.controller.data()+from,bytes);return true;
    }));
    pairs.push_back(pair);
    pairs.back().american=std::vector<unsigned char>(20,56);
    assert(pairs.back().Controller()[0]==56);
    assert(pair.Controller()[0]==12); // Legacy packs keep their artwork.
    bool changed;unsigned mode;
    assert(Select(image,true,changed,mode) && changed && mode==KYOEI_INPUT_KEYBOARD);
    kyoei_prompt_presented();assert(kyoei_prompt_pause_active());
    assert(Select(image,false,changed,mode) && !changed);
    current_mode=KYOEI_INPUT_CONTROLLER;
    assert(Select(image,false,changed,mode) && changed && mode==KYOEI_INPUT_CONTROLLER);
    current_mode=KYOEI_INPUT_KEYBOARD;
    assert(Select(image,false,changed,mode) && changed && mode==KYOEI_INPUT_KEYBOARD);
    image.info.resources.levels=1;
    assert(Select(image,true,changed,mode)); // A descriptor may bind only mip 0.
    image.info.resources.levels=2;
    guest[0]=99;
    assert(!Select(image,true,changed,mode)); // Memory reuse must discard the match.
    image.info.props.is_tiled=true;assert(!Select(image,false,changed,mode));
    kyoei_prompt_presented();kyoei_prompt_presented();assert(kyoei_prompt_pause_active());
    kyoei_prompt_reset();assert(!kyoei_prompt_pause_active());
    auto bad=pair;bad.bytes=19;
    assert(!Rows(bad,image.info,[](size_t,size_t,size_t){return true;}));
    Image tiled;tiled.info.size={8,8,1};tiled.info.resources.levels=1;
    tiled.info.props.is_tiled=true;tiled.info.tile_mode=13;tiled.info.array_mode=2;
    tiled.info.guest_size=256;tiled.info.mips_layout[0]={256,8,8,0};
    Pair tile_pair{8,8,1,1,0,0,256,{}, {}};
    const size_t first_row[]={0,4,16,20,64,68,80,84};
    assert(Rows(tile_pair,tiled.info,[&](size_t at,size_t from,size_t bytes){
        if(from<32)assert(at==first_row[from/4]);
        assert(bytes==4);return true;
    },true));
    {   // R8 font masks have one byte per pixel, including Display1D's 8-bit order.
        guest.resize(64);Image mask;mask.image_uid=17;mask.info.size={8,8,1};mask.info.num_bits=8;
        mask.info.resources.levels=1;mask.info.guest_size=64;mask.info.mips_layout[0]={64,8,8,0};
        mask.info.props.is_tiled=true;mask.info.array_mode=2;mask.info.tile_mode=9;
        Pair alpha{8,8,1,2,0,0,64,std::vector<unsigned char>(64,12),std::vector<unsigned char>(64,34)};
        assert(Rows(alpha,mask.info,[&](size_t at,size_t from,size_t n){
            assert(n==1);if(from<8)assert(at==from);if(from>=8&&from<16)assert(at==from+8);
            guest[at]=alpha.controller[from];return true;
        },true));
        pairs.push_back(alpha);assert(Select(mask,true,changed,mode));
        current_mode=KYOEI_INPUT_CONTROLLER;assert(Select(mask,false,changed,mode)&&changed);
        current_mode=KYOEI_INPUT_KEYBOARD;assert(Select(mask,false,changed,mode)&&changed);
    }
    {   // Xbox pads get the Xbox artwork; PlayStation pads the American one.
        KyoeiPrompts::Pair pair{};
        pair.controller={1}; pair.american={2}; pair.xbox={3};
        current_family=KYOEI_PAD_XBOX; assert(pair.Controller()[0]==3);
        current_family=KYOEI_PAD_PLAYSTATION; assert(pair.Controller()[0]==2);
        pair.xbox.clear(); current_family=KYOEI_PAD_XBOX; assert(pair.Controller()[0]==2);
        // Nintendo: its own art, else the original (right button confirms, as the circle).
        current_family=KYOEI_PAD_NINTENDO; assert(pair.Controller()[0]==1);
        pair.nintendo={4}; assert(pair.Controller()[0]==4);
    }
    std::puts("LIVE PROMPTS PASS: padded mips, mode changes, memory reuse, layout rejection, pause frame lifetime, Xbox/Nintendo art");
}
