// PC bindings verified against the owner's operation_normal.tex diagram.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <SDL3/SDL.h>
#include "kyoei_prompt_state.h"
namespace KyoeiPcControls {
inline unsigned KeyMask(SDL_Scancode key) {
    if (kyoei_prompt_pause_active()) {
        if (key==SDL_SCANCODE_A || (key==SDL_SCANCODE_LEFT && kyoei_prompt_pause_root())) return 0x400;
        if (key==SDL_SCANCODE_D || (key==SDL_SCANCODE_RIGHT && kyoei_prompt_pause_root())) return 0x800;
        if (key==SDL_SCANCODE_Q || key==SDL_SCANCODE_LSHIFT || key==SDL_SCANCODE_RSHIFT) return 0;
    }
    switch(key) {
    case SDL_SCANCODE_E: case SDL_SCANCODE_RETURN: return 0x2000;
    case SDL_SCANCODE_SPACE: case SDL_SCANCODE_BACKSPACE: return 0x4000;
    case SDL_SCANCODE_LCTRL: case SDL_SCANCODE_RCTRL: return 0x8000;
    case SDL_SCANCODE_F: return 0x1000;
    case SDL_SCANCODE_LSHIFT: case SDL_SCANCODE_RSHIFT: return 0x800;
    case SDL_SCANCODE_Q: return 0x400;
    case SDL_SCANCODE_V: return 0x100;
    case SDL_SCANCODE_G: return 0x200;
    case SDL_SCANCODE_ESCAPE: case SDL_SCANCODE_TAB: return 8;
    case SDL_SCANCODE_M: return 0x100000;
    case SDL_SCANCODE_H: return 2;
    case SDL_SCANCODE_R: return 4;
    case SDL_SCANCODE_UP: return 0x10;case SDL_SCANCODE_RIGHT: return 0x20;
    case SDL_SCANCODE_DOWN: return 0x40;case SDL_SCANCODE_LEFT: return 0x80;
    default:return 0;
    }
}
}
