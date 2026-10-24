// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "imgui.h"
#include <cstdio>
#include <string>
#include "kyoei_photo_settings.h"
#include "kyoei_photo_notice.h"
#include "kyoei_ui_art.h"
#include "kyoei_ui_text.h"
#include <algorithm>
#include <cfloat>
#include <initializer_list>
namespace KyoeiPhoto {
inline void DrawCompositionGuide(){
    const int mode=guide.load();if(!mode)return;
    const auto size=ImGui::GetIO().DisplaySize;
    const float scale=std::min(size.x/1920.f,size.y/1080.f);
    const ImVec2 image(1920*scale,1080*scale),origin((size.x-image.x)*.5f,(size.y-image.y)*.5f);
    const auto r=VisibleFrame(1920,1080,frame.load());
    const ImVec2 a(origin.x+r.x*image.x,origin.y+r.y*image.y),b(a.x+r.w*image.x,a.y+r.h*image.y);
    auto* draw=ImGui::GetBackgroundDrawList();
    const auto line=[&](ImVec2 from,ImVec2 to){
        draw->AddLine(from,to,IM_COL32(0,0,0,110),3);
        draw->AddLine(from,to,IM_COL32(220,245,238,145),1);
    };
    if(mode==3){const ImVec2 center((a.x+b.x)*.5f,(a.y+b.y)*.5f);
        line(ImVec2(center.x-12,center.y),ImVec2(center.x+12,center.y));
        line(ImVec2(center.x,center.y-12),ImVec2(center.x,center.y+12));
    }else{
        const float fraction=mode==1?1.f/3.f:.381966011f;
        for(float t:{fraction,1-fraction}){
            line(ImVec2(a.x+(b.x-a.x)*t,a.y),ImVec2(a.x+(b.x-a.x)*t,b.y));
            line(ImVec2(a.x,a.y+(b.y-a.y)*t),ImVec2(b.x,a.y+(b.y-a.y)*t));
        }
    }
}
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
inline std::string FieldValue(Field field,float fov,float roll) {
    char text[96]={};const auto grade=GetGrade();const auto& light=CurrentLight();
    switch(field){
    case Field::Lens:std::snprintf(text,sizeof(text),"%.1f mm",fov>0?FocalLength(fov):0);break;
    case Field::Roll:std::snprintf(text,sizeof(text),"%.0f°",roll);break;
    case Field::Speed:std::snprintf(text,sizeof(text),"%.2fx",camera_speed.load());break;
    case Field::Guide:return GuideName(guide.load());
    case Field::Frame:return FrameName(frame.load());
    case Field::Exposure:std::snprintf(text,sizeof(text),"%+.1f EV",grade.exposure);break;
    case Field::Contrast:std::snprintf(text,sizeof(text),"%.2f",grade.contrast);break;
    case Field::Saturation:std::snprintf(text,sizeof(text),"%.1f",grade.saturation);break;
    case Field::Filter:return FilterName(grade.filter);
    case Field::Grain:std::snprintf(text,sizeof(text),"%.0f%%",grade.grain*100);break;
    case Field::Vignette:std::snprintf(text,sizeof(text),"%.0f%%",grade.vignette*100);break;
    case Field::DofMode:{const int m=std::clamp(dof_mode.load(),0,2);constexpr const char* en[]={"Original","Manual","Off"};constexpr const char* pt[]={"Original","Manual","Desligado"};constexpr const char* ja[]={"オリジナル","手動","なし"};return KyoeiUiText::T(en[m],pt[m],ja[m]);}
    case Field::FocusDistance:std::snprintf(text,sizeof(text),"%.2f m",focus_distance.load());break;
    case Field::Aperture:std::snprintf(text,sizeof(text),"f/%.1f",aperture.load());break;
    case Field::LightSlot:std::snprintf(text,sizeof(text),"%d / %d",light_slot.load()+1,KYOEI_PHOTO_LIGHT_COUNT);break;
    case Field::LightControl:return light_control.load()?KyoeiUiText::T("Selected Light","Luz selecionada","選択中のライト"):KyoeiUiText::T("Camera","Câmera","カメラ");
    case Field::LightType:return light.type.load()?KyoeiUiText::T("Spot","Direcionada","スポット"):KyoeiUiText::T("General","Geral","全体");
    case Field::LightMode:return light.mode.load()?KyoeiUiText::T("On","Sim","オン"):KyoeiUiText::T("Off","Não","オフ");
    case Field::LightColor:return LightColorName(light.color.load());
    case Field::LightPower:std::snprintf(text,sizeof(text),"%.2f",light.power.load());break;
    case Field::LightX:std::snprintf(text,sizeof(text),"%+.2f",light.x.load());break;
    case Field::LightY:std::snprintf(text,sizeof(text),"%+.2f",light.y.load());break;
    case Field::LightZ:std::snprintf(text,sizeof(text),"%+.2f",light.z.load());break;
    case Field::LightYaw:std::snprintf(text,sizeof(text),"%.0f°",light.yaw.load());break;
    case Field::LightPitch:std::snprintf(text,sizeof(text),"%.0f°",light.pitch.load());break;
    case Field::LightCone:std::snprintf(text,sizeof(text),"%.0f°",light.cone.load());break;
    default:break;
    }
    return text;
}
// The panel in the game's own style (Option menu pieces from tools/make_ui_art.py), laid out on
// the 1920x1080 HUD like the native screens; the prompt bar shows the pad in use.
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
inline const char* GroupNote(int group, bool editing_light, bool dof_available) {
    using KyoeiUiText::T;
    switch (group) {
    case 0: return T("Guides stay out of the PNG. Framing adds bars to the photo without stretching it.",
                     "Guias ficam fora do PNG. O enquadramento adiciona barras na foto, sem esticar a imagem.",
                     "ガイドは保存画像に写りません。フレームは画像を伸ばさずに帯を加えます。");
    case 1: return T("Adjustments show in the scene and in the PNG. The interface stays out of the photo.",
                     "Os ajustes aparecem na cena e no PNG. A interface fica fora da foto.",
                     "調整は画面と保存画像に反映されます。メニューは写りません。");
    case 2: return dof_available ? T("Focus distance: what is sharp. Lower f-number: more blur.",
                                     "Distância de foco: onde fica nítido. Número f menor: mais desfoque.",
                                     "ピント距離でくっきり写る位置を決めます。F値が小さいほどぼけます。")
                                 : T("Depth of field is unavailable in this session.", "Desfoque indisponível nesta execução.",
                                     "このセッションでは被写界深度を使用できません。");
    default: return editing_light ? T("Move and aim control this light. Set Control back to Camera to reframe.",
                                      "Mover e apontar controlam esta luz. Volte Controle para Câmera ao reenquadrar.",
                                      "移動と向きでこのライトを動かします。構図を変えるときは操作対象をカメラに戻してください。")
                                  : T("Pick one of three lights. General spreads light; Spot makes a beam.",
                                      "Escolha uma das três luzes. Geral espalha luz; Direcionada cria um facho.",
                                      "3つのライトから選びます。全体は広く照らし、スポットは光の筋を作ります。");
    }
}
inline void DrawPanel(bool controller, bool player_hidden, float fov = 40, float roll = 0, bool dof_available = false) {
    using KyoeiUiText::T;
    (void)player_hidden;
    const HudSpace hud = Hud();
    auto* draw = ImGui::GetBackgroundDrawList();
    const int group = FieldGroup(selected.load());
    const bool editing_light = experimental_lights.load() && light_control.load();
    const ImU32 text = IM_COL32(232, 232, 228, 255), dim = IM_COL32(150, 158, 158, 255), off = IM_COL32(90, 98, 98, 255);

    // Title bar, as the Option screen's.
    const ImVec2 title = KyoeiUiArt::Size("option_title");
    KyoeiUiArt::Draw(draw, "option_title", hud.P(0, 54), hud.P(title.x, 54 + title.y));
    HudText(draw, hud, 236, 90, 34, T("Photo Mode", "Modo Foto", "フォトモード"));

    // Panel on the right over a soft dark backing.
    const float x0 = 1190, x1 = 1860, top = 150;
    draw->AddRectFilled(hud.P(x0 - 24, top - 24), hud.P(x1 + 24, 880), IM_COL32(6, 10, 12, 140), 6 * hud.unit);
    float x = x0;
    for (int tab = 0; tab < 4; ++tab) {
        const char* name = GroupName(tab);
        const bool enabled = FieldEnabled(group_begin[tab]);
        HudText(draw, hud, x, top + 30, 33, name, tab == group ? text : enabled ? dim : off);
        const float w = HudTextWidth(hud, 33, name);
        if (tab == group) KyoeiUiArt::Draw(draw, "option_bar", hud.P(x, top + 40), hud.P(x + w, top + 44));
        x += w + 42;
    }
    float y = top + 100;
    if (group == int(Group::Lights)) {
        unsigned count = 0;
        for (const auto& light : lights) count += light.mode.load() != 0;
        char line[96];
        std::snprintf(line, sizeof(line), T("Light %d of %d  ·  %u on", "Luz %d de %d  ·  %u ligadas", "ライト %d / %d  ・  点灯 %u"),
                      light_slot.load() + 1, KYOEI_PHOTO_LIGHT_COUNT, count);
        HudText(draw, hud, x0 + 8, y - 20, 24, line, dim);
        y += 20;
    }
    for (int field = group_begin[group]; field < group_end[group]; ++field) {
        if (!FieldEnabled(field)) continue;
        const bool chosen = field == selected.load();
        if (chosen) KyoeiUiArt::Draw(draw, "option_point", hud.P(x0 - 10, y - 30), hud.P(x0 + 300, y + 2));
        HudText(draw, hud, x0 + 8, y, 31, FieldName(Field(field)));
        const auto value = FieldValue(Field(field), fov, roll);
        HudText(draw, hud, x1 - 130, y, 31, value.c_str(), text, .5f);
        HudText(draw, hud, x1 - 250, y, 30, "«", chosen ? text : dim, .5f);
        HudText(draw, hud, x1 - 10, y, 30, "»", chosen ? text : dim, .5f);
        KyoeiUiArt::Draw(draw, "option_line", hud.P(x0, y + 8), hud.P(x1, y + 18));
        y += 54;
    }
    // Description in the native frame.
    KyoeiUiArt::Draw(draw, "option_frame", hud.P(x0 - 20, y + 4), hud.P(x1 + 20, y + 118));
    HudText(draw, hud, x0 + 6, y + 48, 26, GroupNote(group, editing_light, dof_available), text, 0, x1 - x0 - 12);

    // Prompt bar at the bottom, with the icons of the pad in use (or the keys).
    draw->AddRectFilled(hud.P(0, 962), hud.P(1920, 1080), IM_COL32(6, 10, 12, 165));
    KyoeiUiArt::Draw(draw, "pause_line", hud.P(0, 960), hud.P(1920, 964));
    float px = 60;
    if (controller) {
        const std::string a = KyoeiUiArt::Pad("A"), b = KyoeiUiArt::Pad("B"), lb = KyoeiUiArt::Pad("LB"),
                          rb = KyoeiUiArt::Pad("RB"), dpad = KyoeiUiArt::Pad("DPAD"), xb = KyoeiUiArt::Pad("X"),
                          yb = KyoeiUiArt::Pad("Y"), rs = KyoeiUiArt::Pad("RS"), ls = KyoeiUiArt::Pad("LSTICK"),
                          rstick = KyoeiUiArt::Pad("RSTICK"), lt = KyoeiUiArt::Pad("LT"), rt = KyoeiUiArt::Pad("RT"),
                          l3 = KyoeiUiArt::Pad("LS");
        px = HudPrompt(draw, hud, px, 1004, {a.c_str()}, T("Take Photo", "Fotografar", "撮影"));
        px = HudPrompt(draw, hud, px, 1004, {b.c_str()}, T("Exit", "Sair", "終了"));
        px = HudPrompt(draw, hud, px, 1004, {lb.c_str(), rb.c_str()}, T("Tab", "Aba", "タブ"));
        px = HudPrompt(draw, hud, px, 1004, {dpad.c_str()}, T("Select / Adjust", "Selecionar / Ajustar", "選択 / 調整"));
        px = HudPrompt(draw, hud, px, 1004, {xb.c_str()}, T("Hide Panel", "Ocultar painel", "パネル非表示"));
        px = HudPrompt(draw, hud, px, 1004, {yb.c_str()}, T("Hide Protagonist", "Ocultar protagonista", "主人公非表示"));
        HudPrompt(draw, hud, px, 1004, {rs.c_str()}, T("Reset", "Restaurar", "リセット"));
        px = 60;
        px = HudPrompt(draw, hud, px, 1058, {ls.c_str()}, editing_light ? T("Move Light", "Mover luz", "ライト移動") : T("Move", "Mover", "移動"));
        px = HudPrompt(draw, hud, px, 1058, {rstick.c_str()}, editing_light ? T("Aim Light", "Apontar luz", "ライトの向き") : T("Look", "Olhar", "視点"));
        px = HudPrompt(draw, hud, px, 1058, {lt.c_str(), rt.c_str()}, T("Down / Up", "Descer / Subir", "下降 / 上昇"));
        HudPrompt(draw, hud, px, 1058, {l3.c_str()}, T("Faster", "Mais rápido", "高速"));
    } else {
        px = HudPrompt(draw, hud, px, 1004, {"key_F12"}, T("Take Photo", "Fotografar", "撮影"));
        px = HudPrompt(draw, hud, px, 1004, {"key_F10"}, T("Exit", "Sair", "終了"));
        px = HudPrompt(draw, hud, px, 1004, {"key_Tab"}, T("Tab", "Aba", "タブ"));
        px = HudPrompt(draw, hud, px, 1004, {"key_arrows"}, T("Select / Adjust", "Selecionar / Ajustar", "選択 / 調整"));
        px = HudPrompt(draw, hud, px, 1004, {"key_F9"}, T("Hide Panel", "Ocultar painel", "パネル非表示"));
        px = HudPrompt(draw, hud, px, 1004, {"key_F11"}, T("Hide Protagonist", "Ocultar protagonista", "主人公非表示"));
        HudPrompt(draw, hud, px, 1004, {"key_R"}, T("Reset", "Restaurar", "リセット"));
        px = 60;
        px = HudPrompt(draw, hud, px, 1058, {"key_WASD"}, editing_light ? T("Move Light", "Mover luz", "ライト移動") : T("Move", "Mover", "移動"));
        px = HudPrompt(draw, hud, px, 1058, {"key_mouse"}, editing_light ? T("Aim Light", "Apontar luz", "ライトの向き") : T("Look", "Olhar", "視点"));
        px = HudPrompt(draw, hud, px, 1058, {"key_Ctrl", "key_Space"}, T("Down / Up", "Descer / Subir", "下降 / 上昇"));
        HudPrompt(draw, hud, px, 1058, {"key_Shift"}, T("Faster", "Mais rápido", "高速"));
    }
}
}
