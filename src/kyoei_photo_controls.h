// Host controls for the opt-in live-rendered photo scene.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "kyoei_mouse.h"
#include "kyoei_photo_hud.h"
#include "kyoei_photo_scene.h"
#include "kyoei_photo_dof.h"
#include "kyoei_photo_lights.h"
#include "kyoei_photo_settings.h"
namespace KyoeiPhotoControls {
inline bool Enabled(){return kyoei_photo_scene_available() && kyoei_photo_hud_hidden();}
inline float StickAxis(unsigned char value){
    // Center quantization and small stick noise must not move a paused camera.
    const int delta=int(value)-128;
    constexpr int deadzone=16;
    if(std::abs(delta)<=deadzone)return 0.f;
    const float magnitude=std::min(1.f,float(std::abs(delta)-deadzone)/(127-deadzone));
    return delta<0?-magnitude:magnitude;
}
template<class Data,class Buttons> inline void Apply(Data& data,Buttons& buttons){
    static bool release_gate=false;
    static Uint64 previous=0;
    const auto now=SDL_GetTicksNS();
    const float dt=previous?std::clamp(float(now-previous)/1e9f,0.f,.1f):0.f;
    previous=now;
    if(!Enabled()){
        if(release_gate){
            const bool neutral=!buttons && !StickAxis(data.leftStick.x) && !StickAxis(data.leftStick.y) &&
                !StickAxis(data.rightStick.x) && !StickAxis(data.rightStick.y) &&
                data.analogButtons.l2<=16 && data.analogButtons.r2<=16;
            release_gate=!neutral;
            buttons=0;data.leftStick=data.rightStick={128,128};data.analogButtons={};
            KyoeiMouse::dx=KyoeiMouse::dy=0;return;
        }
        KyoeiMouse::Apply(data.rightStick);return;
    }
    release_gate=true;
    KyoeiPhoto::experimental_dof=kyoei_photo_dof_available()!=0;
    KyoeiPhoto::experimental_lights=kyoei_photo_lights_available()!=0;
    if(!KyoeiPhoto::FieldEnabled(KyoeiPhoto::selected.load()))KyoeiPhoto::SelectField(1);
    for(unsigned slot=0;slot<KYOEI_PHOTO_LIGHT_COUNT;++slot){
        float rgb[3];KyoeiPhoto::LightRgb(rgb,int(slot));const auto& light=KyoeiPhoto::lights[slot];
        kyoei_photo_lights_input(slot,light.mode.load(),rgb[0],rgb[1],rgb[2],light.x.load(),light.y.load(),light.z.load(),light.type.load(),light.yaw.load(),light.pitch.load(),light.cone.load());
    }
    kyoei_photo_dof_input(KyoeiPhoto::dof_mode.load(),KyoeiPhoto::focus_distance.load(),KyoeiPhoto::aperture.load(),kyoei_photo_scene_fov_value());
    const auto* keys=SDL_GetKeyboardState(nullptr);
    const auto trigger=[](unsigned char value){return value<=16?0.f:float(value-16)/239.f;};
    const float up=std::clamp(float(keys[SDL_SCANCODE_SPACE])-float(keys[SDL_SCANCODE_LCTRL]||keys[SDL_SCANCODE_RCTRL])+
        trigger(data.analogButtons.r2)-trigger(data.analogButtons.l2),-1.f,1.f);
    const float right=StickAxis(data.leftStick.x);
    const float forward=-StickAxis(data.leftStick.y);
    float yaw=-StickAxis(data.rightStick.x)*dt*1.5f;
    float pitch=-StickAxis(data.rightStick.y)*dt*1.5f;
    if(KyoeiMouse::captured){yaw-=KyoeiMouse::dx*.002f;pitch-=KyoeiMouse::dy*.002f;}
    KyoeiMouse::dx=KyoeiMouse::dy=0;
    // D-pad adjustment edges are handled by the host event loop.
    if(KyoeiPhoto::experimental_lights.load() && KyoeiPhoto::light_control.load()){
        auto& light=KyoeiPhoto::CurrentLight();float delta[3];
        const float rate=dt*2.f*KyoeiPhoto::camera_speed.load()*(keys[SDL_SCANCODE_LSHIFT] || (buttons&0x2)?3.f:1.f);
        if(kyoei_photo_scene_light_move(right*rate,up*rate,forward*rate,delta)){
            light.x=std::clamp(light.x.load()+delta[0],-50.f,50.f);
            light.y=std::clamp(light.y.load()+delta[1],-50.f,50.f);
            light.z=std::clamp(light.z.load()+delta[2],-50.f,50.f);
        }
        light.yaw=std::remainder(light.yaw.load()-yaw*57.29578f,360.f);
        light.pitch=std::clamp(light.pitch.load()+pitch*57.29578f,-89.f,89.f);
        kyoei_photo_scene_input(0,0,0,0,0,1);
    }else kyoei_photo_scene_input(right,up,forward,yaw,pitch,
        KyoeiPhoto::camera_speed.load()*(keys[SDL_SCANCODE_LSHIFT] || (buttons&0x2)?3.f:1.f));
    buttons=0;data.leftStick=data.rightStick={128,128};data.analogButtons={};
}
}
