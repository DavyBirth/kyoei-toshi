// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <SDL3/SDL.h>
#include <algorithm>
#include <cmath>
#include <cstdlib>
#include "kyoei_overlay.h"
#include "kyoei_input_mode.h"

// All access is on the SDL window/pad polling thread. Guest pad reads use
// the existing bridge mutex and only see the completed pad sample.
namespace KyoeiMouse {
inline bool released=false, captured=false;
inline float dx=0,dy=0;
inline Uint64 previous=0;
inline void Clear() { dx=dy=0;previous=SDL_GetTicksNS(); }
inline void Update(SDL_Window* window) {
    const char* option=std::getenv("KYOEI_MOUSE_CAMERA");
    bool want=(!option || option[0]!='0') && !released && !kyoei_overlay_kind() &&
        (SDL_GetWindowFlags(window)&SDL_WINDOW_INPUT_FOCUS) &&
        !(SDL_GetWindowFlags(window)&SDL_WINDOW_MINIMIZED);
    if (want==captured) return;
    if (SDL_SetWindowRelativeMouseMode(window,want)) captured=want;
    Clear();
}
inline void Event(SDL_Window* window,const SDL_Event& event) {
    if (event.type==SDL_EVENT_KEY_DOWN && event.key.key==SDLK_F7 && !event.key.repeat)
        released=!released;
    if (event.type==SDL_EVENT_WINDOW_FOCUS_LOST) Clear();
    Update(window);
    if (captured && event.type==SDL_EVENT_MOUSE_MOTION &&
        event.motion.windowID==SDL_GetWindowID(window)) {
        dx=std::clamp(dx+event.motion.xrel,-4096.f,4096.f);
        dy=std::clamp(dy+event.motion.yrel,-4096.f,4096.f);
        if (event.motion.xrel || event.motion.yrel) kyoei_input_keyboard();
    }
}
inline unsigned char Axis(float distance,double seconds,unsigned char fallback) {
    if (distance==0) return fallback;
    // Mouse speed, rather than distance per poll, keeps sensitivity independent
    // of the SDL poll rate. This emulates the stick; the game's turn cap remains.
    float value=float(distance/std::max(seconds,0.001)*0.09);
    float magnitude=std::min(127.f,24.f+std::abs(value));
    return static_cast<unsigned char>(std::clamp(128+int(std::copysign(magnitude,value)),0,255));
}
template<class Stick> inline void Apply(Stick& stick) {
    const auto now=SDL_GetTicksNS();
    double elapsed=previous?double(now-previous)/1e9:0.008;
    previous=now;
    if(captured && !kyoei_overlay_kind() && elapsed<0.1) {
        stick.x=Axis(dx,elapsed,stick.x);stick.y=Axis(dy,elapsed,stick.y);
    }
    dx=dy=0;
}
}
