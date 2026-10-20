// Host-time repeat; never changes the guest clock or simulation timestep.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <cstdint>
namespace KyoeiPhoto {
struct AdjustmentRepeat {
    unsigned held{};
    uint64_t next{};
    void Clear(){held=0;next=0;}
    void Press(unsigned bit,uint64_t now){held|=bit;next=now+350000000;}
    void Release(unsigned bit,uint64_t now){if(!(held&bit))return;held&=~bit;next=held?now+350000000:0;}
    unsigned Tick(uint64_t now){
        if(!held || now<next)return 0;
        // No catch-up burst after a stalled frame or suspended window.
        next=now+80000000;
        unsigned result=held;
        if((result&3)==3)result&=~3u;
        if((result&12)==12)result&=~12u;
        return result;
    }
};
}
