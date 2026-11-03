// Native PNG capture, opt-in photo scene and host controls.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <atomic>
#include <SDL3/SDL.h>
#include "kyoei_photo_hud.h"
#include "kyoei_photo_scene.h"
#include "kyoei_pause_photo.h"
#include "kyoei_overlay.h"
#include "kyoei_title_exit.h"
#include "kyoei_input_mode.h"
#include "kyoei_photo_settings.h"
#include "kyoei_photo_repeat.h"
namespace KyoeiPhoto {
inline std::atomic_bool pending{false};
inline std::atomic_bool hints{true};
inline AdjustmentRepeat adjustment_repeat;
inline unsigned AdjustmentBit(unsigned button){
    return button==SDL_GAMEPAD_BUTTON_DPAD_LEFT?1u:button==SDL_GAMEPAD_BUTTON_DPAD_RIGHT?2u:
        button==SDL_GAMEPAD_BUTTON_DPAD_UP?4u:button==SDL_GAMEPAD_BUTTON_DPAD_DOWN?8u:0u;
}
inline void AdjustSelected(int direction) {
    const auto field=Field(selected.load());
    if(field==Field::Lens) {
        const float fov=kyoei_photo_scene_fov_value();
        if(fov>0){const float mm=std::clamp(FocalLength(fov)+direction*2.f,12.25f,133.f);
            kyoei_photo_scene_fov(FovFromFocal(mm)-fov);}
    } else if(field==Field::Roll)kyoei_photo_scene_roll(direction*2.f);
    else AdjustGrade(field,direction);
}
inline void ResetAdjustments() {
    ResetGrade();kyoei_photo_scene_roll(-kyoei_photo_scene_roll_value());
    const float initial=kyoei_photo_scene_initial_fov();
    if(initial>0)kyoei_photo_scene_fov(initial-kyoei_photo_scene_fov_value());
}
inline bool Blocked(SDL_Window* window) {
    return !(SDL_GetWindowFlags(window)&SDL_WINDOW_INPUT_FOCUS) ||
        (SDL_GetWindowFlags(window)&SDL_WINDOW_MINIMIZED) ||
        kyoei_overlay_kind() || kyoei_title_exit_state();
}
// Call each SDL poll even without events: opening a host modal restores HUD.
inline void Update(SDL_Window* window) {
    if(Blocked(window)){kyoei_pause_photo_cancel();adjustment_repeat.Clear();kyoei_photo_hud_restore();return;}
    if(kyoei_pause_photo_take_entry())kyoei_photo_hud_toggle();
    if(!kyoei_photo_scene_available() || !kyoei_photo_hud_hidden()){adjustment_repeat.Clear();return;}
    const unsigned repeat=adjustment_repeat.Tick(SDL_GetTicksNS());
    if(repeat&1)AdjustSelected(-1);
    if(repeat&2)AdjustSelected(1);
    if(repeat&4)SelectField(-1);
    if(repeat&8)SelectField(1);
}
inline bool HandleEvent(SDL_Window* window,const SDL_Event& event) {
    const bool scene_controls=kyoei_photo_scene_available() && kyoei_photo_hud_hidden();
    if(event.type==SDL_EVENT_GAMEPAD_BUTTON_UP)
        adjustment_repeat.Release(AdjustmentBit(event.gbutton.button),SDL_GetTicksNS());
    if(scene_controls && !Blocked(window) && event.type==SDL_EVENT_MOUSE_WHEEL &&
       event.wheel.windowID==SDL_GetWindowID(window)){
        kyoei_photo_scene_fov(-event.wheel.y*2.f);return true;
    }
    if(event.type==SDL_EVENT_WINDOW_FOCUS_LOST || event.type==SDL_EVENT_WINDOW_MINIMIZED ||
       event.type==SDL_EVENT_QUIT || event.type==SDL_EVENT_WINDOW_CLOSE_REQUESTED ||
       event.type==SDL_EVENT_GAMEPAD_REMOVED) {
        pending.store(false);adjustment_repeat.Clear();kyoei_photo_hud_restore();
    }
    if(scene_controls && (event.type==SDL_EVENT_KEY_DOWN || event.type==SDL_EVENT_KEY_UP) &&
       event.key.windowID==SDL_GetWindowID(window) &&
       (event.key.key==SDLK_TAB || event.key.key==SDLK_HOME || event.key.key==SDLK_END ||
        (event.key.key>=SDLK_1 && event.key.key<=SDLK_4))){
        if(event.type==SDL_EVENT_KEY_DOWN && !event.key.repeat && !Blocked(window)){
            adjustment_repeat.Clear();kyoei_input_keyboard();
            if(event.key.key==SDLK_TAB)SelectGroup(event.key.mod&SDL_KMOD_SHIFT?-1:1);
            else if(event.key.key==SDLK_HOME || event.key.key==SDLK_END){
                const int group=FieldGroup(selected.load());selected=group_begin[group];
                if(event.key.key==SDLK_END)SelectField(-1);else remembered_field[group]=selected.load();
            }
            else SetGroup(int(event.key.key-SDLK_1));
        }
        return true;
    }
    if(event.type==SDL_EVENT_KEY_DOWN &&
       (event.key.key==SDLK_ESCAPE || event.key.key==SDLK_TAB ||
        event.key.key==SDLK_RETURN || (event.key.key==SDLK_SPACE && !scene_controls) ||
        event.key.key==SDLK_BACKSPACE || event.key.key==SDLK_M ||
        event.key.key==SDLK_E || event.key.key==SDLK_F))kyoei_photo_hud_restore();
    if(!scene_controls && event.type==SDL_EVENT_MOUSE_BUTTON_DOWN && event.button.button==SDL_BUTTON_LEFT)
        kyoei_photo_hud_restore();
    if(event.type==SDL_EVENT_GAMEPAD_BUTTON_DOWN) {
        auto button=SDL_GamepadButton(event.gbutton.button);
        // Nintendo layout: A (right) takes the photo and B (bottom) leaves, as confirm/back.
        if(kyoei_input_confirm_on_right(kyoei_input_current_family()))
            button=button==SDL_GAMEPAD_BUTTON_SOUTH?SDL_GAMEPAD_BUTTON_EAST:button==SDL_GAMEPAD_BUTTON_EAST?SDL_GAMEPAD_BUTTON_SOUTH:button;
        // View/Share is unused by the normal guest mapping; preserve Start/map.
        if(button==SDL_GAMEPAD_BUTTON_BACK && kyoei_photo_hud_available()) {
            adjustment_repeat.Clear();
            if(!Blocked(window)){kyoei_input_controller_button();kyoei_photo_hud_toggle();}
            return true;
        }
        if(kyoei_photo_hud_hidden() && !Blocked(window)) {
            kyoei_input_controller_button();
            const unsigned bit=AdjustmentBit(button);
            if(bit)adjustment_repeat.Press(bit,SDL_GetTicksNS());
            if(button==SDL_GAMEPAD_BUTTON_NORTH)kyoei_photo_player_toggle();
            else if(button==SDL_GAMEPAD_BUTTON_EAST){adjustment_repeat.Clear();kyoei_photo_hud_restore();}
            else if(button==SDL_GAMEPAD_BUTTON_SOUTH)pending.store(true);
            else if(button==SDL_GAMEPAD_BUTTON_WEST)hints.store(!hints.load());
            else if(button==SDL_GAMEPAD_BUTTON_LEFT_SHOULDER){adjustment_repeat.Clear();SelectGroup(-1);}
            else if(button==SDL_GAMEPAD_BUTTON_RIGHT_SHOULDER){adjustment_repeat.Clear();SelectGroup(1);}
            else if(button==SDL_GAMEPAD_BUTTON_DPAD_LEFT)AdjustSelected(-1);
            else if(button==SDL_GAMEPAD_BUTTON_DPAD_RIGHT)AdjustSelected(1);
            else if(button==SDL_GAMEPAD_BUTTON_DPAD_UP)SelectField(-1);
            else if(button==SDL_GAMEPAD_BUTTON_DPAD_DOWN)SelectField(1);
            else if(button==SDL_GAMEPAD_BUTTON_RIGHT_STICK)ResetAdjustments();
            return true;
        }
        if(button==SDL_GAMEPAD_BUTTON_START || button==SDL_GAMEPAD_BUTTON_TOUCHPAD ||
           button==SDL_GAMEPAD_BUTTON_SOUTH || button==SDL_GAMEPAD_BUTTON_EAST ||
           button==SDL_GAMEPAD_BUTTON_WEST || button==SDL_GAMEPAD_BUTTON_NORTH)
            kyoei_photo_hud_restore();
    }
    if(event.type!=SDL_EVENT_KEY_DOWN && event.type!=SDL_EVENT_KEY_UP)return false;
    if(event.key.windowID!=SDL_GetWindowID(window))return false;
    if(kyoei_photo_hud_hidden() && (event.key.key==SDLK_LEFT || event.key.key==SDLK_RIGHT ||
       event.key.key==SDLK_UP || event.key.key==SDLK_DOWN || event.key.key==SDLK_R)) {
        if(event.type==SDL_EVENT_KEY_DOWN && !Blocked(window)) {
            kyoei_input_keyboard();
            if(event.key.key==SDLK_LEFT)AdjustSelected(-1);
            else if(event.key.key==SDLK_RIGHT)AdjustSelected(1);
            else if(event.key.key==SDLK_UP)SelectField(-1);
            else if(event.key.key==SDLK_DOWN)SelectField(1);
            else if(!event.key.repeat)ResetAdjustments();
        }
        return true;
    }
    if(event.type==SDL_EVENT_KEY_DOWN && !event.key.repeat && !Blocked(window) &&
       (event.key.key==SDLK_F9 || event.key.key==SDLK_F10 ||
        event.key.key==SDLK_F11 || event.key.key==SDLK_F12))kyoei_input_keyboard();
    if(event.key.key==SDLK_F9 && kyoei_photo_hud_hidden()) {
        if(event.type==SDL_EVENT_KEY_DOWN && !event.key.repeat && !Blocked(window))
            hints.store(!hints.load());
        return true;
    }
    if(event.key.key==SDLK_F11) {
        if(event.type==SDL_EVENT_KEY_DOWN && !event.key.repeat && !Blocked(window) &&
           kyoei_photo_hud_hidden())kyoei_photo_player_toggle();
        return true;
    }
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
