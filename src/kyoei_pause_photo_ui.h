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
inline const ImVec2 row[4]={{1374,922.5f},{1690,974},{1688.5f,1013},{1372.5f,958.5f}};
// The menu's idle sway tilts it in perspective: measured against the native rows,
// the near (left) edge moves 0.65 and the far (right) edge 0.2 of the menu's y.
inline const float sway[4]={0.65f,0.2f,0.2f,0.65f};
// The far edge also swings sideways (about -1 unit per unit of menu y).
inline const float sway_x[4]={0,-1.f,-1.f,0};
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
    float offset=0,offset_y=0,alpha=0;if(!kyoei_pause_photo_visual(&offset,&offset_y,&alpha))return;
    auto *draw=ImGui::GetBackgroundDrawList();const auto hud=KyoeiUiDraw::Hud();ImVec2 p[4];
    for(int i=0;i<4;++i)p[i]=hud.P(row[i].x+offset+offset_y*sway_x[i],row[i].y+offset_y*sway[i]);
    bool selected=kyoei_pause_photo_selected()!=0;
    // Selected: the game's own highlight frame is moved under this row (see update hook).
    if(!selected)draw->AddConvexPolyFilled(p,4,IM_COL32(180,180,180,int(180*alpha)));
    const char *name=KyoeiUiText::T("pause_photo_en","pause_photo_pt","pause_photo_ja");
    if(!KyoeiUiArt::DrawQuad(draw,name,p,IM_COL32(255,255,255,int(255*alpha)))){
        KyoeiUiDraw::HudText(draw,hud,1406+offset,968+offset_y*0.4f,30,KyoeiUiText::T("Photo Mode","Modo foto","Photo Mode"));
    }
}
}
