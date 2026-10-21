// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "imgui.h"
#include <cstdio>
#include <string>
#include "kyoei_photo_settings.h"
#include "kyoei_photo_notice.h"
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
    case Field::Roll:std::snprintf(text,sizeof(text),"%.0f graus",roll);break;
    case Field::Speed:std::snprintf(text,sizeof(text),"%.2fx",camera_speed.load());break;
    case Field::Guide:return GuideName(guide.load());
    case Field::Frame:return FrameName(frame.load());
    case Field::Exposure:std::snprintf(text,sizeof(text),"%+.1f EV",grade.exposure);break;
    case Field::Contrast:std::snprintf(text,sizeof(text),"%.2f",grade.contrast);break;
    case Field::Saturation:std::snprintf(text,sizeof(text),"%.1f",grade.saturation);break;
    case Field::Filter:return FilterName(grade.filter);
    case Field::Grain:std::snprintf(text,sizeof(text),"%.0f%%",grade.grain*100);break;
    case Field::Vignette:std::snprintf(text,sizeof(text),"%.0f%%",grade.vignette*100);break;
    case Field::DofMode:{constexpr const char* modes[]={"Original","Manual","Desligado"};return modes[dof_mode.load()];}
    case Field::FocusDistance:std::snprintf(text,sizeof(text),"%.2f m",focus_distance.load());break;
    case Field::Aperture:std::snprintf(text,sizeof(text),"f/%.1f",aperture.load());break;
    case Field::LightSlot:std::snprintf(text,sizeof(text),"%d / %d",light_slot.load()+1,KYOEI_PHOTO_LIGHT_COUNT);break;
    case Field::LightControl:return light_control.load()?"Luz selecionada":"Camera";
    case Field::LightType:return light.type.load()?"Direcionada":"Geral";
    case Field::LightMode:return light.mode.load()?"Sim":"Nao";
    case Field::LightColor:return LightColorName(light.color.load());
    case Field::LightPower:std::snprintf(text,sizeof(text),"%.2f",light.power.load());break;
    case Field::LightX:std::snprintf(text,sizeof(text),"%+.2f",light.x.load());break;
    case Field::LightY:std::snprintf(text,sizeof(text),"%+.2f",light.y.load());break;
    case Field::LightZ:std::snprintf(text,sizeof(text),"%+.2f",light.z.load());break;
    case Field::LightYaw:std::snprintf(text,sizeof(text),"%.0f graus",light.yaw.load());break;
    case Field::LightPitch:std::snprintf(text,sizeof(text),"%.0f graus",light.pitch.load());break;
    case Field::LightCone:std::snprintf(text,sizeof(text),"%.0f graus",light.cone.load());break;
    default:break;
    }
    return text;
}
inline void DrawPanel(bool controller,bool player_hidden,float fov=40,float roll=0,bool dof_available=false) {
    const auto size=ImGui::GetIO().DisplaySize;
    const int group=FieldGroup(selected.load());
    const bool editing_light=experimental_lights.load() && light_control.load();
    const ImVec4 accent(.34f,.88f,.76f,1),muted(.65f,.74f,.73f,1);
    constexpr auto flags=ImGuiWindowFlags_NoDecoration|ImGuiWindowFlags_AlwaysAutoResize|
        ImGuiWindowFlags_NoInputs|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoFocusOnAppearing;
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding,ImVec2(16,14));
    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding,3);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize,1);
    ImGui::PushStyleColor(ImGuiCol_WindowBg,ImVec4(.025f,.055f,.055f,.91f));
    ImGui::PushStyleColor(ImGuiCol_Border,ImVec4(.22f,.52f,.46f,.7f));
    const float width=std::min(440.f,size.x-40);
    ImGui::SetNextWindowPos(ImVec2(size.x-20,size.y*.47f),ImGuiCond_Always,ImVec2(1,.5f));
    ImGui::SetNextWindowSize(ImVec2(width,0));
    ImGui::SetNextWindowSizeConstraints(ImVec2(width,0),ImVec2(width,size.y-120));
    ImGui::Begin("##photo-help",nullptr,flags);
    ImGui::TextColored(accent,"MODO FOTO");
    ImGui::SameLine();ImGui::TextColored(muted," / %s",editing_light?"EDITAR LUZ":"CAMERA LIVRE");
    ImGui::Separator();
    for(int tab=0;tab<4;++tab){
        if(tab)ImGui::SameLine(0,14);
        const bool enabled=FieldEnabled(group_begin[tab]);
        ImGui::TextColored(tab==group?accent:enabled?muted:ImVec4(.32f,.38f,.38f,1),"%s",GroupName(tab));
        if(tab==group){const auto first=ImGui::GetItemRectMin(),last=ImGui::GetItemRectMax();
            ImGui::GetWindowDrawList()->AddLine(ImVec2(first.x,last.y+2),ImVec2(last.x,last.y+2),ImGui::GetColorU32(accent),2);}
    }
    ImGui::Spacing();
    if(group==int(Group::Lights)){
        unsigned count=0;for(const auto& light:lights)count+=light.mode.load()!=0;
        ImGui::TextColored(muted,"Luz %d de %d    |    %u ligadas",light_slot.load()+1,KYOEI_PHOTO_LIGHT_COUNT,count);
    }
    if(ImGui::BeginTable("##photo-fields",2,ImGuiTableFlags_SizingStretchProp)){
        ImGui::TableSetupColumn("Ajuste",ImGuiTableColumnFlags_WidthStretch,1.35f);
        ImGui::TableSetupColumn("Valor",ImGuiTableColumnFlags_WidthStretch,1.f);
        for(int field=group_begin[group];field<group_end[group];++field){
            if(!FieldEnabled(field))continue;
            const bool chosen=field==selected.load();
            ImGui::TableNextRow(0,27);
            if(chosen)ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0,ImGui::GetColorU32(ImVec4(.12f,.43f,.37f,.72f)));
            ImGui::TableSetColumnIndex(0);ImGui::TextColored(chosen?ImVec4(1,1,1,1):muted,"%s",FieldName(Field(field)));
            ImGui::TableSetColumnIndex(1);const auto value=FieldValue(Field(field),fov,roll);
            ImGui::TextColored(chosen?accent:ImVec4(.88f,.93f,.92f,1),"%s%s%s",chosen?"< ":"",value.c_str(),chosen?" >":"");
        }
        ImGui::EndTable();
    }
    ImGui::Separator();
    ImGui::PushStyleColor(ImGuiCol_Text,muted);
    if(group==int(Group::Camera)){
        ImGui::TextWrapped("Guias ficam fora do PNG. Enquadramento adiciona barras na foto, sem esticar a imagem.");
        ImGui::TextWrapped("Protagonista: %s | %s",player_hidden?"oculto":"visivel",controller?"Y/Triangulo alterna":"F11 alterna");
    }
    else if(group==int(Group::Image))ImGui::TextWrapped("Ajustes aparecem na cena e no PNG. A interface fica fora da foto.");
    else if(group==int(Group::Focus)){
        ImGui::TextWrapped("Distancia de foco: onde fica nitido. Menor numero f/: mais desfoque.");
        if(!dof_available)ImGui::TextWrapped("Desfoque indisponivel nesta execucao.");
    }else{
        ImGui::TextWrapped(editing_light?"Mover e apontar controla esta luz. Volte Controle para Camera ao reenquadrar.":"Escolha uma das tres luzes. Geral espalha luz; Direcionada cria um facho.");
        ImGui::TextWrapped("Ligar posiciona perto da camera. Desligue e ligue para reposicionar.");
    }
    ImGui::PopStyleColor();ImGui::End();
    ImGui::SetNextWindowPos(ImVec2(size.x*.5f,size.y-12),ImGuiCond_Always,ImVec2(.5f,1));
    ImGui::SetNextWindowSize(ImVec2(size.x-40,0));
    ImGui::Begin("##photo-controls",nullptr,flags);
    if(controller){
        ImGui::TextUnformatted("L1/R1: categoria    |    Cima/baixo: selecionar    |    Esquerda/direita: ajustar");
        ImGui::Text("Analogicos: %s    |    L2/R2: altura    |    L3: rapido",editing_light?"mover/apontar luz":"mover/olhar camera");
        ImGui::TextUnformatted("A/Cruz: foto    |    R3: restaurar tudo    |    X/Quadrado: ocultar painel    |    B/Bola: sair");
    }else{
        ImGui::TextUnformatted("Tab ou 1-4: categoria    |    Cima/baixo: selecionar    |    Esquerda/direita: ajustar");
        ImGui::Text("WASD: mover %s    |    Mouse/JKIL: %s    |    Espaco/Ctrl: altura    |    Shift: rapido",editing_light?"luz":"camera",editing_light?"apontar luz":"olhar");
        ImGui::TextUnformatted("F12: foto    |    R: restaurar tudo    |    F9: ocultar painel    |    F10: sair");
    }
    ImGui::End();ImGui::PopStyleColor(2);ImGui::PopStyleVar(3);
}
}
