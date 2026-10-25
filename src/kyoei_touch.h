// Touch pad contacts for the game (scePadRead touchData), which the Godzilla pad left empty: the
// game's "slide touch pad" actions (minimap controls, the safe's arm) never received a finger.
// Real fingers come from a PlayStation pad's touch pad. KYOEI_BENCH_SWIPE_FILE (tests): while
// that file exists, one finger slides left to right across the pad and starts again.
// Pads and keyboards without a touch pad: hold the touch pad button (View, Minus, M) and move
// the right stick or the mouse; the finger follows and the camera stays still. A short press
// with no movement still reaches the game as the button (map on/off), on release.
// KYOEI_BENCH_DRAG_FILE (tests): the same, by holding the button with the stick pushed.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include <SDL3/SDL.h>
#include <windows.h>
#include <cstdlib>

namespace KyoeiTouch {
constexpr int Width = 1920, Height = 950;  // the range scePadGetControllerInformation reports

struct Contact { bool down; float x, y; };

constexpr unsigned ButtonMask = 0x100000;  // the touch pad button

template <class PadData>  // OrbisPadData; included before the pad header
inline void Fill(SDL_Gamepad* device, PadData& data, unsigned& buttons) {
    static unsigned char next_id = 0;
    static bool was_down[2]{};
    static unsigned char ids[2]{};
    Contact contacts[2]{};
    if (device && SDL_GetNumGamepadTouchpads(device) > 0) {
        for (int f = 0; f < 2; ++f) {
            float pressure = 0;
            SDL_GetGamepadTouchpadFinger(device, 0, f, &contacts[f].down, &contacts[f].x, &contacts[f].y, &pressure);
        }
    }
    static const char* swipe_file = std::getenv("KYOEI_BENCH_SWIPE_FILE");
    if (swipe_file && GetFileAttributesA(swipe_file) != INVALID_FILE_ATTRIBUTES) {
        const Uint64 ms = SDL_GetTicks() % 1500;  // 1.2 s across, then lifted for 0.3 s
        contacts[0] = {ms < 1200, .1f + .8f * float(ms) / 1200.f, .5f};
    }
    // KYOEI_BENCH_DRAG_FILE (tests): while it exists, the touch pad button is held and the right
    // stick pushed right, as a player dragging the minimap.
    static const char* drag_file = std::getenv("KYOEI_BENCH_DRAG_FILE");
    if (drag_file && GetFileAttributesA(drag_file) != INVALID_FILE_ATTRIBUTES) {
        buttons |= ButtonMask; data.rightStick.x = 255;
    }
    // The slide alternative.
    static bool holding = false, panned = false;
    static float fx = .5f, fy = .5f;
    static Uint64 last = 0, pulse_until = 0;
    const Uint64 now = SDL_GetTicks();
    const float seconds = last ? SDL_min(float(now - last) / 1000.f, .1f) : 0.f;
    last = now;
    if (buttons & ButtonMask) {
        buttons &= ~ButtonMask;
        if (!holding) { holding = true; panned = false; fx = fy = .5f; }
        const float sx = (int(data.rightStick.x) - 128) / 127.f, sy = (int(data.rightStick.y) - 128) / 127.f;
        if (sx * sx + sy * sy > .06f) panned = true;
        if (panned && !contacts[0].down) {
            fx = SDL_clamp(fx + sx * .8f * seconds, 0.f, 1.f);  // a full stick crosses the pad in 1.25 s
            fy = SDL_clamp(fy + sy * .8f * seconds, 0.f, 1.f);
            contacts[0] = {true, fx, fy};
            data.rightStick = {128, 128};
        }
    } else if (holding) {
        holding = false;
        if (!panned) pulse_until = now + 120;  // long enough for the game's next pad reads
    }
    if (now < pulse_until) buttons |= ButtonMask;
    unsigned char count = 0;
    for (int f = 0; f < 2; ++f) {
        if (contacts[f].down && !was_down[f]) ids[f] = next_id = (unsigned char)(next_id % 127 + 1);
        was_down[f] = contacts[f].down;
        if (!contacts[f].down) continue;
        auto& touch = data.touchData.touch[count++];
        touch.x = (unsigned short)SDL_clamp(int(contacts[f].x * (Width - 1)), 0, Width - 1);
        touch.y = (unsigned short)SDL_clamp(int(contacts[f].y * (Height - 1)), 0, Height - 1);
        touch.id = ids[f];
    }
    data.touchData.touchNum = count;
}
}
