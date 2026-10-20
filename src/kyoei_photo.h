// Native PNG capture and opt-in HUD draw gate; simulation remains live.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <atomic>
#include <SDL3/SDL.h>
#include "kyoei_photo_hud.h"
#include "kyoei_photo_scene.h"
#include "kyoei_overlay.h"
#include "kyoei_title_exit.h"
namespace KyoeiPhoto {
inline std::atomic_bool pending{false};
inline bool Blocked(SDL_Window* window) {
    return !(SDL_GetWindowFlags(window)&SDL_WINDOW_INPUT_FOCUS) ||
        (SDL_GetWindowFlags(window)&SDL_WINDOW_MINIMIZED) ||
        kyoei_overlay_kind() || kyoei_title_exit_state();
}
// Call each SDL poll even without events: opening a host modal restores HUD.
inline void Update(SDL_Window* window) {if(Blocked(window))kyoei_photo_hud_restore();}
inline bool HandleEvent(SDL_Window* window,const SDL_Event& event) {
    const bool scene_controls=kyoei_photo_scene_available() && kyoei_photo_hud_hidden();
    if(scene_controls && !Blocked(window) && event.type==SDL_EVENT_MOUSE_WHEEL &&
       event.wheel.windowID==SDL_GetWindowID(window)){
        kyoei_photo_scene_fov(-event.wheel.y*2.f);return true;
    }
    if(event.type==SDL_EVENT_WINDOW_FOCUS_LOST || event.type==SDL_EVENT_WINDOW_MINIMIZED ||
       event.type==SDL_EVENT_QUIT || event.type==SDL_EVENT_WINDOW_CLOSE_REQUESTED) {
        pending.store(false);kyoei_photo_hud_restore();
    }
    if(event.type==SDL_EVENT_KEY_DOWN &&
       (event.key.key==SDLK_ESCAPE || event.key.key==SDLK_TAB ||
        event.key.key==SDLK_RETURN || (event.key.key==SDLK_SPACE && !scene_controls) ||
        event.key.key==SDLK_BACKSPACE || event.key.key==SDLK_M ||
        event.key.key==SDLK_E || event.key.key==SDLK_F))kyoei_photo_hud_restore();
    if(!scene_controls && event.type==SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button==SDL_BUTTON_LEFT)
        kyoei_photo_hud_restore();
    if(event.type==SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
        const auto button=event.gbutton.button;
        if(button==SDL_GAMEPAD_BUTTON_START || button==SDL_GAMEPAD_BUTTON_TOUCHPAD ||
           button==SDL_GAMEPAD_BUTTON_SOUTH || button==SDL_GAMEPAD_BUTTON_EAST ||
           button==SDL_GAMEPAD_BUTTON_WEST || button==SDL_GAMEPAD_BUTTON_NORTH)
            kyoei_photo_hud_restore();
    }
    if(event.type!=SDL_EVENT_KEY_DOWN && event.type!=SDL_EVENT_KEY_UP)return false;
    if(event.key.windowID!=SDL_GetWindowID(window))return false;
    if(event.key.key==SDLK_F10) {
        if(event.type==SDL_EVENT_KEY_DOWN && !event.key.repeat && !Blocked(window))
            kyoei_photo_hud_toggle();
        return true;
    }
    if(event.key.key!=SDLK_F12)return false;
    if(event.type==SDL_EVENT_KEY_DOWN && !event.key.repeat &&
       (SDL_GetWindowFlags(window)&SDL_WINDOW_INPUT_FOCUS))pending.store(true);
    return true;
}
inline bool TakeRequest(){return pending.exchange(false);}
}
