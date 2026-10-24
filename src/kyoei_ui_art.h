// UI art for the PC screens in the game's own style (tools/make_ui_art.py, KYOEI_UI_ART).
// Pieces live in the overlay's font atlas, like the title art. SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "imgui.h"
namespace KyoeiUiArt {
/// Draws the named piece into `draw` (stretched to `min`..`max`). False when it is missing.
bool Draw(ImDrawList* draw, const char* name, ImVec2 min, ImVec2 max, ImU32 tint = IM_COL32_WHITE);
/// Native size of the named piece (0,0 when missing).
ImVec2 Size(const char* name);
/// Button icon for the pad in use: "A","B","X","Y","LB","RB","DPAD",... (PlayStation or Xbox art).
const char* Pad(const char* button);
/// Font for the PC screens (heavier than the dialogs' font).
ImFont* Font();
}
