// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <atomic>
#include <SDL3/SDL.h>
namespace KyoeiPhoto {
// The SDL thread requests; the draw thread consumes. Repeated requests coalesce.
inline std::atomic_bool pending{false};
inline bool HandleEvent(SDL_Window* window,const SDL_Event& event) {
    if (event.type==SDL_EVENT_WINDOW_FOCUS_LOST) pending.store(false);
    if ((event.type!=SDL_EVENT_KEY_DOWN && event.type!=SDL_EVENT_KEY_UP) || event.key.key!=SDLK_F12) return false;
    if (event.type==SDL_EVENT_KEY_DOWN && !event.key.repeat &&
        (SDL_GetWindowFlags(window)&SDL_WINDOW_INPUT_FOCUS)) pending.store(true);
    return true;
}
inline bool TakeRequest() { return pending.exchange(false); }
}
