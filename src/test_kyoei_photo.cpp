#include "kyoei_photo.h"
#include "kyoei_photo_controls.h"
#include <cstdio>
#include <initializer_list>
static int hidden=0,modal=0,exit_state=0,toggles=0,scene=0;
static float move_right=0,move_forward=0,look_yaw=0,fov_delta=0;
extern "C" int kyoei_photo_scene_available(void){return scene;}
extern "C" void kyoei_photo_scene_fov(float value){fov_delta+=value;}
extern "C" void kyoei_photo_scene_input(float right,float,float forward,float yaw,float,float){
    move_right=right;move_forward=forward;look_yaw=yaw;
}
extern "C" int kyoei_overlay_kind(void){return modal;}
extern "C" int kyoei_title_exit_state(void){return exit_state;}
extern "C" void kyoei_photo_hud_restore(void){hidden=0;}
extern "C" int kyoei_photo_hud_hidden(void){return hidden;}
extern "C" int kyoei_photo_hud_toggle(void){++toggles;hidden=!hidden;return 1;}
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"Photo check failed: %d\n",__LINE__);return 1;}}while(0)
int main(){
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER,"dummy");CHECK(SDL_Init(SDL_INIT_VIDEO));
    auto* window=SDL_CreateWindow("photo-test",64,64,SDL_WINDOW_HIDDEN);CHECK(window);
    SDL_Event event{};event.type=SDL_EVENT_KEY_DOWN;event.key.windowID=SDL_GetWindowID(window);
    event.key.key=SDLK_F10;
    CHECK(KyoeiPhoto::HandleEvent(window,event));CHECK(toggles==0); // no focus
    event.key.repeat=true;KyoeiPhoto::HandleEvent(window,event);CHECK(toggles==0);
    event.key.key=SDLK_F12;event.key.repeat=false;
    CHECK(KyoeiPhoto::HandleEvent(window,event));CHECK(!KyoeiPhoto::TakeRequest());
    KyoeiPhoto::pending.store(true);
    event.key.windowID=SDL_GetWindowID(window)+1;
    CHECK(!KyoeiPhoto::HandleEvent(window,event));CHECK(KyoeiPhoto::TakeRequest());
    CHECK(!KyoeiPhoto::TakeRequest());
    for(auto type:{SDL_EVENT_WINDOW_FOCUS_LOST,SDL_EVENT_WINDOW_MINIMIZED,
                   SDL_EVENT_QUIT,SDL_EVENT_WINDOW_CLOSE_REQUESTED}){
        hidden=1;KyoeiPhoto::pending.store(true);event={};event.type=type;
        KyoeiPhoto::HandleEvent(window,event);CHECK(!hidden && !KyoeiPhoto::TakeRequest());
    }
    for(auto key:{SDLK_ESCAPE,SDLK_TAB,SDLK_RETURN,SDLK_SPACE,SDLK_BACKSPACE,SDLK_M,SDLK_E,SDLK_F}){
        hidden=1;event={};event.type=SDL_EVENT_KEY_DOWN;event.key.key=key;
        event.key.windowID=SDL_GetWindowID(window);
        KyoeiPhoto::HandleEvent(window,event);CHECK(!hidden);
    }
    hidden=1;event={};event.type=SDL_EVENT_MOUSE_BUTTON_DOWN;event.button.button=SDL_BUTTON_LEFT;
    KyoeiPhoto::HandleEvent(window,event);CHECK(!hidden);
    for(auto button:{SDL_GAMEPAD_BUTTON_START,SDL_GAMEPAD_BUTTON_TOUCHPAD,
                     SDL_GAMEPAD_BUTTON_SOUTH,SDL_GAMEPAD_BUTTON_EAST,
                     SDL_GAMEPAD_BUTTON_WEST,SDL_GAMEPAD_BUTTON_NORTH}){
        hidden=1;event={};event.type=SDL_EVENT_GAMEPAD_BUTTON_DOWN;event.gbutton.button=button;
        KyoeiPhoto::HandleEvent(window,event);CHECK(!hidden);
    }
    hidden=1;KyoeiPhoto::Update(window);CHECK(!hidden); // focus loss without event
    hidden=1;modal=1;KyoeiPhoto::Update(window);CHECK(!hidden);modal=0;
    hidden=1;exit_state=1;KyoeiPhoto::Update(window);CHECK(!hidden);
    scene=1;hidden=1;exit_state=0;
    event={};event.type=SDL_EVENT_KEY_DOWN;event.key.windowID=SDL_GetWindowID(window);event.key.key=SDLK_SPACE;
    KyoeiPhoto::HandleEvent(window,event);CHECK(hidden); // vertical photo control
    struct Stick{unsigned char x=128,y=128;};
    struct Data{Stick leftStick,rightStick;struct{unsigned char l2=0,r2=0;}analogButtons;} data;
    unsigned buttons=0xffff;data.leftStick={255,0};data.analogButtons={255,255};
    KyoeiMouse::captured=true;KyoeiMouse::dx=20;
    KyoeiPhotoControls::Apply(data,buttons);
    CHECK(move_right==1 && move_forward==1 && look_yaw<0);
    CHECK(!buttons && data.leftStick.x==128 && data.leftStick.y==128 &&
          data.rightStick.x==128 && !data.analogButtons.l2 && !data.analogButtons.r2 && !KyoeiMouse::dx);
    event={};event.type=SDL_EVENT_MOUSE_WHEEL;event.wheel.windowID=SDL_GetWindowID(window);event.wheel.y=1;
    KyoeiPhoto::HandleEvent(window,event);CHECK(!fov_delta); // inactive window cannot zoom
    SDL_DestroyWindow(window);SDL_Quit();
    std::puts("PHOTO INPUT PASS: inactive focus, repeat, foreign window, request consumption, close/modal/dialog/controller restore");
}
