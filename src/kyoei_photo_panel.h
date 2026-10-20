// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "imgui.h"
#include "kyoei_photo_settings.h"
#include "kyoei_photo_notice.h"
namespace KyoeiPhoto {
inline void DrawCaptureNotice(){
    const auto notice=CurrentCaptureNotice();if(notice.message.empty())return;
    const auto size=ImGui::GetIO().DisplaySize;
    ImGui::SetNextWindowPos(ImVec2(size.x*.5f,16),ImGuiCond_Always,ImVec2(.5f,0));
    ImGui::SetNextWindowSize(ImVec2(std::min(700.f,size.x-32),0));
    ImGui::SetNextWindowBgAlpha(.88f);
    ImGui::Begin("##photo-result",nullptr,ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_AlwaysAutoResize|
        ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing);
    ImGui::PushStyleColor(ImGuiCol_Text,notice.error?ImVec4(1,.5f,.45f,1):ImVec4(.7f,1,.7f,1));
    ImGui::TextWrapped("%s",notice.message.c_str());ImGui::PopStyleColor();ImGui::End();
}
inline void DrawPanel(bool controller,bool player_hidden,float fov=40,float roll=0,bool dof_available=false) {
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
        ImGui::Text("Filtro: %s | *Lente equivalente a convencao nativa de 35 mm",FilterName(grade.filter));
        ImGui::Text("Granulacao: %.0f%% | Vinheta: %.0f%%",grade.grain*100,grade.vignette*100);
        constexpr const char* modes[]={"Original","Manual","Desligado"};
        if(experimental_dof.load())ImGui::Text("Desfoque experimental: %s%s | Foco: %.2f m | Abertura: f/%.1f",modes[dof_mode.load()],dof_available?"":" (indisponivel)",focus_distance.load(),aperture.load());
        if(Field(selected.load())==Field::FocusDistance || Field(selected.load())==Field::Aperture)
            ImGui::TextUnformatted("Foco: distancia nitida | Abertura menor (f/): mais desfoque");
        if(Field(selected.load())==Field::Lens)
            ImGui::TextColored(ImVec4(1,.8f,.35f,1),"Lente: cima = +2 mm (aproxima) | baixo = -2 mm (amplia)");
        ImGui::Separator();
        if(controller) {
            ImGui::TextUnformatted("Analogicos: mover / olhar | LT/RT (L2/R2): descer / subir");
            ImGui::TextUnformatted("LB/RB (L1/R1): rapido | Direcional: selecionar / ajustar | R3: restaurar");
            ImGui::TextUnformatted("A/Cruz: foto | Y/Triangulo: protagonista | X/Quadrado: ajuda");
            ImGui::TextUnformatted("B/Bola ou View/Share: sair | A foto nao inclui esta interface");
            ImGui::TextUnformatted("Segure o direcional para repetir o ajuste");
        } else {
            ImGui::TextUnformatted("WASD: mover | Espaco/Ctrl: altura | Mouse/JKIL: olhar");
            ImGui::TextUnformatted("Roda: zoom | Shift: rapido | F11: protagonista");
            ImGui::TextUnformatted("Setas esq/dir: selecionar | cima/baixo: ajustar | R: restaurar");
            ImGui::TextUnformatted("F12: foto | F9: ajuda | F10: sair");
        }
        ImGui::End();
}
}
