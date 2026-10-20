// Host controls for the opt-in live-rendered photo scene.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "kyoei_mouse.h"
#include "kyoei_photo_hud.h"
#include "kyoei_photo_scene.h"
namespace KyoeiPhotoControls {
inline bool Enabled(){return kyoei_photo_scene_available() && kyoei_photo_hud_hidden();}
template<class Data,class Buttons> inline void Apply(Data& data,Buttons& buttons){
    static Uint64 previous=0;
    const auto now=SDL_GetTicksNS();
    const float dt=previous?std::clamp(float(now-previous)/1e9f,0.f,.1f):0.f;
    previous=now;
    if(!Enabled()){KyoeiMouse::Apply(data.rightStick);return;}
    const auto* keys=SDL_GetKeyboardState(nullptr);
    const float up=float(keys[SDL_SCANCODE_SPACE])-float(keys[SDL_SCANCODE_LCTRL]||keys[SDL_SCANCODE_RCTRL]);
    const float right=std::clamp((int(data.leftStick.x)-128)/127.f,-1.f,1.f);
    const float forward=std::clamp((128-int(data.leftStick.y))/127.f,-1.f,1.f);
    float yaw=(128-int(data.rightStick.x))/127.f*dt*1.5f;
    float pitch=(128-int(data.rightStick.y))/127.f*dt*1.5f;
    if(KyoeiMouse::captured){yaw-=KyoeiMouse::dx*.002f;pitch-=KyoeiMouse::dy*.002f;}
    KyoeiMouse::dx=KyoeiMouse::dy=0;
    kyoei_photo_scene_input(right,up,forward,yaw,pitch,keys[SDL_SCANCODE_LSHIFT]?3.f:1.f);
    buttons=0;data.leftStick=data.rightStick={128,128};data.analogButtons={};
}
}
