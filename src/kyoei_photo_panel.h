// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "imgui.h"
#include "kyoei_photo_settings.h"
namespace KyoeiPhoto {
inline void DrawPanel(bool controller,bool player_hidden,float fov=40,float roll=0) {
        const auto size=ImGui::GetIO().DisplaySize;
        ImGui::SetNextWindowPos(ImVec2(16,size.y-16),ImGuiCond_Always,ImVec2(0,1));
        ImGui::SetNextWindowSizeConstraints(ImVec2(0,0),ImVec2(size.x-32,size.y-32));
        ImGui::SetNextWindowBgAlpha(.78f);
        ImGui::Begin("##photo-help",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_AlwaysAutoResize|
            ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing);
        ImGui::Text("MODO FOTO | Ocultar protagonista: %s (experimental)",player_hidden?"ligado":"desligado");
        const auto grade=GetGrade();
        ImGui::Separator();
        ImGui::TextColored(ImVec4(1,.8f,.35f,1),"Ajuste: %s",FieldName(Field(selected.load())));
        ImGui::Text("Lente: %.1f mm* | FOV: %.1f | Rotacao: %.0f graus | Velocidade: %.2fx",
            fov>0?FocalLength(fov):0,fov,roll,camera_speed.load());
        ImGui::Text("Exposicao: %+.1f EV | Contraste: %.2f | Saturacao: %.1f",grade.exposure,grade.contrast,grade.saturation);
        ImGui::Text("Filtro: %s | *Sensor virtual de 24 mm de altura",FilterName(grade.filter));
        ImGui::Text("Granulacao: %.0f%% | Vinheta: %.0f%%",grade.grain*100,grade.vignette*100);
        if(Field(selected.load())==Field::Lens)
            ImGui::TextColored(ImVec4(1,.8f,.35f,1),"Lente: cima = +2 mm (aproxima) | baixo = -2 mm (amplia)");
        ImGui::Separator();
        if(controller) {
            ImGui::TextUnformatted("Analogicos: mover / olhar | LT/RT (L2/R2): descer / subir");
            ImGui::TextUnformatted("LB/RB (L1/R1): rapido | Direcional: selecionar / ajustar | R3: restaurar");
            ImGui::TextUnformatted("A/Cruz: foto | Y/Triangulo: protagonista | X/Quadrado: ajuda");
            ImGui::TextUnformatted("View/Share: sair | A captura nao inclui esta interface");
        } else {
            ImGui::TextUnformatted("WASD: mover | Espaco/Ctrl: altura | Mouse/JKIL: olhar");
            ImGui::TextUnformatted("Roda: zoom | Shift: rapido | F11: protagonista");
            ImGui::TextUnformatted("Setas esq/dir: selecionar | cima/baixo: ajustar | R: restaurar");
            ImGui::TextUnformatted("F12: foto | F9: ajuda | F10: sair");
        }
        ImGui::End();
}
}
