// Host-owned seventh row, composed in the native pause menu's 1920x1080 plane.
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once
#include "kyoei_pause_photo.h"
#include "kyoei_ui_art.h"
#include "kyoei_ui_draw.h"
#include "kyoei_ui_text.h"
#include <SDL3/SDL.h>
namespace KyoeiPausePhotoUi {
// Same fixed menu-plane placement as the six authored native rows. Resolution
// and aspect ratio follow HudSpace, including the swapchain's letterboxing.
inline const ImVec2 row[4]={{1372,927},{1684,962},{1681,1002},{1369,967}};
inline bool Inside(float x,float y){
    for(int i=0;i<4;++i){const auto a=row[i],b=row[(i+1)%4];if((b.x-a.x)*(y-a.y)-(b.y-a.y)*(x-a.x)<0)return false;}
    return true;
}
inline bool HandleEvent(const SDL_Event& event){
    if(event.type!=SDL_EVENT_MOUSE_BUTTON_DOWN || event.button.button!=SDL_BUTTON_LEFT || !kyoei_pause_photo_visible())return false;
    auto *window=SDL_GetWindowFromID(event.button.windowID);int w=0,h=0;if(!window)return false;SDL_GetWindowSize(window,&w,&h);
    float scale=std::min(w/1920.f,h/1080.f);if(scale<=0)return false;
    float x=(event.button.x-(w-1920*scale)/2)/scale,y=(event.button.y-(h-1080*scale)/2)/scale;
    if(!Inside(x,y))return false;kyoei_pause_photo_request();return true;
}
inline void Draw(){
    float offset=0,alpha=0;if(!kyoei_pause_photo_visual(&offset,&alpha))return;
    auto *draw=ImGui::GetBackgroundDrawList();const auto hud=KyoeiUiDraw::Hud();ImVec2 p[4];
    for(int i=0;i<4;++i)p[i]=hud.P(row[i].x+offset,row[i].y);
    bool selected=kyoei_pause_photo_selected()!=0;
    draw->AddConvexPolyFilled(p,4,selected?IM_COL32(5,12,8,int(220*alpha)):IM_COL32(180,180,180,int(180*alpha)));
    const char *name=KyoeiUiText::T("pause_photo_en","pause_photo_pt","pause_photo_ja");
    if(!KyoeiUiArt::DrawQuad(draw,name,p,IM_COL32(255,255,255,int(255*alpha)))){
        KyoeiUiDraw::HudText(draw,hud,1406+offset,962,30,KyoeiUiText::T("Photo Mode","Modo foto","Photo Mode"));
    }
    if(selected){draw->AddPolyline(p,4,IM_COL32(90,210,120,int(255*alpha)),ImDrawFlags_Closed,2*hud.unit);}
}
}
