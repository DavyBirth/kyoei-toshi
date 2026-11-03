/* Test-only input: KYOEI_TEST_INPUT=<file> names a text file the test driver appends
 * commands to ("tap <key> [ms]", "down <key>", "up <key>"). Each new line becomes SDL
 * key events in the normal event queue, so tests run without window focus.
 * Unset (every normal launch): nothing happens. SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef KYOEI_TEST_INPUT_H
#define KYOEI_TEST_INPUT_H
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static inline SDL_Scancode kyoei_test_key(const char *name) {
    static const struct { const char *name; SDL_Scancode code; } keys[] = {
        {"enter", SDL_SCANCODE_RETURN}, {"esc", SDL_SCANCODE_ESCAPE}, {"tab", SDL_SCANCODE_TAB},
        {"space", SDL_SCANCODE_SPACE}, {"back", SDL_SCANCODE_BACKSPACE}, {"ctrl", SDL_SCANCODE_LCTRL},
        {"up", SDL_SCANCODE_UP}, {"down", SDL_SCANCODE_DOWN}, {"left", SDL_SCANCODE_LEFT}, {"right", SDL_SCANCODE_RIGHT},
        {"f1", SDL_SCANCODE_F1}, {"f7", SDL_SCANCODE_F7}};
    for (size_t i = 0; i < sizeof keys / sizeof *keys; ++i) if (!strcmp(name, keys[i].name)) return keys[i].code;
    if (strlen(name) == 1 && name[0] >= 'a' && name[0] <= 'z') return (SDL_Scancode)(SDL_SCANCODE_A + name[0] - 'a');
    if (strlen(name) == 1 && name[0] >= '1' && name[0] <= '9') return (SDL_Scancode)(SDL_SCANCODE_1 + name[0] - '1');
    if (strlen(name) == 1 && name[0] == '0') return SDL_SCANCODE_0;
    return SDL_SCANCODE_UNKNOWN;
}

static inline void kyoei_test_push(SDL_Window *window, SDL_Scancode code, bool down) {
    SDL_Event event{};
    event.type = down ? SDL_EVENT_KEY_DOWN : SDL_EVENT_KEY_UP;
    event.key.timestamp = SDL_GetTicksNS();
    event.key.windowID = window ? SDL_GetWindowID(window) : 0;
    event.key.scancode = code;
    event.key.key = SDL_GetKeyFromScancode(code, SDL_KMOD_NONE, false);
    event.key.down = down;
    SDL_PushEvent(&event);
}

/* Called once per event poll; cheap when the variable is unset. */
static inline void kyoei_test_input_pump(SDL_Window *window) {
    static int state = -1; static const char *path; static long offset;
    static SDL_Scancode pending; static Uint64 release_at;
    if (state < 0) { path = getenv("KYOEI_TEST_INPUT"); state = path && *path; }
    if (!state) return;
    Uint64 now = SDL_GetTicks();
    if (pending != SDL_SCANCODE_UNKNOWN) {
        if (now < release_at) return;
        kyoei_test_push(window, pending, false); pending = SDL_SCANCODE_UNKNOWN;
    }
    FILE *file = fopen(path, "rb");
    if (!file) return;
    if (fseek(file, offset, SEEK_SET) == 0) {
        char line[128];
        if (fgets(line, sizeof line, file) && strchr(line, '\n')) {
            offset = ftell(file);
            char verb[16] = {0}, name[32] = {0}; int ms = 120;
            if (sscanf(line, "%15s %31s %d", verb, name, &ms) >= 2) {
                SDL_Scancode code = kyoei_test_key(name);
                if (code != SDL_SCANCODE_UNKNOWN) {
                    if (!strcmp(verb, "down")) kyoei_test_push(window, code, true);
                    else if (!strcmp(verb, "up")) kyoei_test_push(window, code, false);
                    else if (!strcmp(verb, "tap")) {
                        kyoei_test_push(window, code, true);
                        pending = code; release_at = now + (Uint64)(ms > 0 ? ms : 120);
                    }
                }
            }
        }
    }
    fclose(file);
}
#endif
