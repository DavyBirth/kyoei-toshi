// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "imgui.h"
namespace KyoeiPhoto {
inline void DrawPanel(bool controller,bool player_hidden) {
        const auto size=ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(16,size.y-16),ImGuiCond_Always,ImVec2(0,1));
        ImGui::SetNextWindowSizeConstraints(ImVec2(0,0),ImVec2(size.x-32,size.y-32));
        ImGui::SetNextWindowBgAlpha(.78f);
        ImGui::Begin("##photo-help",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_AlwaysAutoResize|
            ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing);
        ImGui::Text("MODO FOTO | Protagonista: %s",player_hidden?"oculto":"visivel");
        if(controller) {
            ImGui::TextUnformatted("Analogicos: mover / olhar | LT/RT (L2/R2): descer / subir");
            ImGui::TextUnformatted("LB/RB (L1/R1): rapido | Direcional cima/baixo: zoom");
            ImGui::TextUnformatted("A/Cruz: foto | Y/Triangulo: protagonista | X/Quadrado: ajuda");
            ImGui::TextUnformatted("View/Share: sair | A captura nao inclui esta interface");
        } else {
            ImGui::TextUnformatted("WASD: mover | Espaco/Ctrl: altura | Mouse/JKIL: olhar");
            ImGui::TextUnformatted("Roda: zoom | Shift: rapido | F11: protagonista");
            ImGui::TextUnformatted("F12: foto | F9: ajuda | F10: sair");
        }
        ImGui::End();
}
}
