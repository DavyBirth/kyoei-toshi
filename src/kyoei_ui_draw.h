// Drawing helpers for the PC screens in the game's style: 1920x1080 HUD space letterboxed into
// the window, text with the native soft shadow, prompts with the pad's icons.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "imgui.h"
#include "kyoei_ui_art.h"
#include <algorithm>
#include <cfloat>
#include <initializer_list>
namespace KyoeiUiDraw {
struct HudSpace {
    float unit, left, top;
    ImVec2 P(float x, float y) const { return ImVec2(left + x * unit, top + y * unit); }
};
inline HudSpace Hud() {
    const auto size = ImGui::GetIO().DisplaySize;
    const float unit = std::min(size.x / 1920.f, size.y / 1080.f);
    return {unit, (size.x - 1920 * unit) / 2, (size.y - 1080 * unit) / 2};
}
/// Text with the native soft shadow; `baseline` in HUD pixels, `align` 0 left .. 1 right.
inline void HudText(ImDrawList* draw, const HudSpace& hud, float x, float baseline, float px, const char* text,
                    ImU32 color = IM_COL32(232, 232, 228, 255), float align = 0, float wrap = 0) {
    ImFont* font = KyoeiUiArt::Font();
    const float size = px * hud.unit;
    const ImVec2 extent = font->CalcTextSizeA(size, FLT_MAX, wrap * hud.unit, text);
    ImVec2 pos = hud.P(x, baseline - px * .82f);
    pos.x -= extent.x * align;
    draw->AddText(font, size, ImVec2(pos.x + 2 * hud.unit, pos.y + 2 * hud.unit), IM_COL32(0, 0, 0, 170), text,
                  nullptr, wrap * hud.unit);
    draw->AddText(font, size, pos, color, text, nullptr, wrap * hud.unit);
}
inline float HudTextWidth(const HudSpace& hud, float px, const char* text) {
    return KyoeiUiArt::Font()->CalcTextSizeA(px * hud.unit, FLT_MAX, 0, text).x / hud.unit;
}
/// One prompt: icons then label; returns the x after it.
inline float HudPrompt(ImDrawList* draw, const HudSpace& hud, float x, float baseline,
                       std::initializer_list<const char*> pieces, const char* label) {
    for (const char* piece : pieces) {
        const ImVec2 size = KyoeiUiArt::Size(piece);
        if (size.y <= 0) continue;
        const float h = 42, w = size.x * h / size.y;
        KyoeiUiArt::Draw(draw, piece, hud.P(x, baseline - 33), hud.P(x + w, baseline + 9));
        x += w + 4;
    }
    HudText(draw, hud, x + 6, baseline, 29, label);
    return x + 6 + HudTextWidth(hud, 29, label) + 40;
}
}
