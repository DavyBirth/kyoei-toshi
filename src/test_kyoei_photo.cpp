#include "kyoei_photo.h"
#include "kyoei_photo_controls.h"
#include <cstdio>
#include <initializer_list>
static int hidden=0,modal=0,exit_state=0,toggles=0,scene=0,lights_available=0;
static int player_toggles=0;
extern "C" void kyoei_input_controller_button(void){}
extern "C" void kyoei_input_keyboard(void){}
extern "C" int kyoei_photo_player_toggle(void){++player_toggles;return 1;}
static float move_right=0,move_up=0,move_forward=0,look_yaw=0,fov_delta=0,speed=0;
extern "C" int kyoei_photo_scene_available(void){return scene;}
extern "C" int kyoei_photo_dof_available(void){return 0;}
extern "C" int kyoei_photo_lights_available(void){return lights_available;}
extern "C" void kyoei_photo_lights_input(unsigned,int,float,float,float,float,float,float,int,float,float,float){}
extern "C" int kyoei_photo_scene_light_move(float right,float up,float forward,float xyz[3]){xyz[0]=right;xyz[1]=up;xyz[2]=forward;return 1;}
extern "C" void kyoei_photo_dof_input(int,float,float,float){}
extern "C" void kyoei_photo_scene_fov(float value){fov_delta+=value;}
extern "C" float kyoei_photo_scene_fov_value(void){return 40;}
extern "C" float kyoei_photo_scene_initial_fov(void){return 40;}
extern "C" float kyoei_photo_scene_roll_value(void){return 0;}
extern "C" void kyoei_photo_scene_roll(float){}
extern "C" void kyoei_photo_scene_input(float right,float up,float forward,float yaw,float,float rate){
    move_right=right;move_up=up;move_forward=forward;look_yaw=yaw;speed=rate;
}
extern "C" int kyoei_overlay_kind(void){return modal;}
extern "C" int kyoei_title_exit_state(void){return exit_state;}
extern "C" void kyoei_photo_hud_restore(void){hidden=0;}
extern "C" int kyoei_photo_hud_hidden(void){return hidden;}
extern "C" int kyoei_photo_hud_available(void){return 1;}
extern "C" int kyoei_photo_hud_toggle(void){++toggles;hidden=!hidden;return 1;}
#define CHECK(x) do{if(!(x)){std::fprintf(stderr,"Photo check failed: %d\n",__LINE__);return 1;}}while(0)
int main(){
    KyoeiPhoto::AdjustmentRepeat repeat;
    repeat.Press(4,100);
    CHECK(!repeat.Tick(350000099));CHECK(repeat.Tick(350000100)==4);
    CHECK(!repeat.Tick(430000099));CHECK(repeat.Tick(430000100)==4);
    CHECK(repeat.Tick(10000000000)==4);CHECK(!repeat.Tick(10000000001));
    repeat.Press(8,10000000001);CHECK(!repeat.Tick(10350000001)); // opposite directions cancel
    repeat.Release(8,10400000000);CHECK(repeat.Tick(10750000000)==4);
    repeat.Clear();CHECK(!repeat.Tick(20000000000));
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER,"dummy");CHECK(SDL_Init(SDL_INIT_VIDEO));
    auto* window=SDL_CreateWindow("photo-test",64,64,SDL_WINDOW_HIDDEN);CHECK(window);
    SDL_Event event{};event.type=SDL_EVENT_KEY_DOWN;event.key.windowID=SDL_GetWindowID(window);
    event.key.key=SDLK_F10;
    CHECK(KyoeiPhoto::HandleEvent(window,event));CHECK(toggles==0); // no focus
    event.key.key=SDLK_F11;hidden=1;
    CHECK(KyoeiPhoto::HandleEvent(window,event));CHECK(player_toggles==0);
    event.key.windowID+=1;
    CHECK(!KyoeiPhoto::HandleEvent(window,event));CHECK(player_toggles==0);
    event.key.windowID=SDL_GetWindowID(window);event.key.key=SDLK_F10;hidden=0;
    event.key.repeat=true;KyoeiPhoto::HandleEvent(window,event);CHECK(toggles==0);
    event.key.key=SDLK_F12;event.key.repeat=false;
    CHECK(KyoeiPhoto::HandleEvent(window,event));CHECK(!KyoeiPhoto::TakeRequest());
    KyoeiPhoto::pending.store(true);
    event.key.windowID=SDL_GetWindowID(window)+1;
    CHECK(!KyoeiPhoto::HandleEvent(window,event));CHECK(KyoeiPhoto::TakeRequest());
    CHECK(!KyoeiPhoto::TakeRequest());
    for(auto type:{SDL_EVENT_WINDOW_FOCUS_LOST,SDL_EVENT_WINDOW_MINIMIZED,
                   SDL_EVENT_QUIT,SDL_EVENT_WINDOW_CLOSE_REQUESTED,SDL_EVENT_GAMEPAD_REMOVED}){
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
    KyoeiPhoto::adjustment_repeat.Press(4,0);hidden=1;KyoeiPhoto::Update(window);
    CHECK(!KyoeiPhoto::adjustment_repeat.held && !hidden);
    hidden=1;modal=1;KyoeiPhoto::Update(window);CHECK(!hidden);modal=0;
    hidden=1;exit_state=1;KyoeiPhoto::Update(window);CHECK(!hidden);
    scene=1;hidden=1;exit_state=0;
    event={};event.type=SDL_EVENT_KEY_DOWN;event.key.windowID=SDL_GetWindowID(window);event.key.key=SDLK_SPACE;
    KyoeiPhoto::HandleEvent(window,event);CHECK(hidden); // vertical photo control
    event.key.key=SDLK_TAB;const int before_category=KyoeiPhoto::selected.load();
    KyoeiPhoto::HandleEvent(window,event);CHECK(hidden && KyoeiPhoto::selected==before_category); // blocked category never exits photo
    struct Stick{unsigned char x=128,y=128;};
    struct Data{Stick leftStick,rightStick;struct{unsigned char l2=0,r2=0;}analogButtons;} data;
    unsigned buttons=0xffff;data.leftStick={255,0};data.analogButtons={255,255};
    KyoeiMouse::captured=true;KyoeiMouse::dx=20;
    KyoeiPhotoControls::Apply(data,buttons);
    CHECK(move_right==1 && move_forward==1 && look_yaw<0);
    CHECK(!buttons && data.leftStick.x==128 && data.leftStick.y==128 &&
          data.rightStick.x==128 && !data.analogButtons.l2 && !data.analogButtons.r2 && !KyoeiMouse::dx);
    KyoeiMouse::captured=false;SDL_Delay(2);
    for(unsigned char neutral:{127,128,129,120,136}){
        data.leftStick=data.rightStick={neutral,neutral};
        KyoeiPhotoControls::Apply(data,buttons);
        CHECK(move_right==0 && move_forward==0 && look_yaw==0);
    }
    data.analogButtons={0,255};buttons=0x2;KyoeiPhotoControls::Apply(data,buttons);
    CHECK(move_up==1 && speed==3 && !buttons && !data.analogButtons.r2);
    data.analogButtons={255,0};buttons=0;KyoeiPhotoControls::Apply(data,buttons);
    CHECK(move_up==-1 && speed==1);
    data.analogButtons={8,8};KyoeiPhotoControls::Apply(data,buttons);CHECK(move_up==0);
    SDL_Delay(2);buttons=0x10;KyoeiPhotoControls::Apply(data,buttons);CHECK(fov_delta==0);
    // Settings are adjusted once by SDL events, not again by held guest masks.
    KyoeiPhoto::selected=int(KyoeiPhoto::Field::Lens);KyoeiPhoto::AdjustSelected(1);CHECK(fov_delta<0);
    const float zoomed=fov_delta;
    event={};event.type=SDL_EVENT_MOUSE_WHEEL;event.wheel.windowID=SDL_GetWindowID(window);event.wheel.y=1;
    KyoeiPhoto::HandleEvent(window,event);CHECK(fov_delta==zoomed); // inactive window cannot zoom
    lights_available=1;KyoeiPhoto::light_control=true;
    data.leftStick={255,128};data.rightStick={255,128};SDL_Delay(10);KyoeiPhotoControls::Apply(data,buttons);
    CHECK(move_right==0 && move_forward==0 && look_yaw==0 && KyoeiPhoto::CurrentLight().x>0 && KyoeiPhoto::CurrentLight().yaw>0);
    KyoeiPhoto::ResetGrade();CHECK(!KyoeiPhoto::light_control && KyoeiPhoto::CurrentLight().x==0);
    lights_available=0;
    scene=0;hidden=0;buttons=0x4000;data.leftStick={255,128};
    KyoeiPhotoControls::Apply(data,buttons);CHECK(!buttons && data.leftStick.x==128);
    buttons=0x4000;KyoeiPhotoControls::Apply(data,buttons);CHECK(!buttons); // held exit cannot leak
    buttons=0;data.leftStick=data.rightStick={128,128};data.analogButtons={};
    KyoeiPhotoControls::Apply(data,buttons);CHECK(!buttons); // release opens the gate
    buttons=0x4000;KyoeiPhotoControls::Apply(data,buttons);CHECK(buttons==0x4000);
    SDL_DestroyWindow(window);SDL_Quit();
    std::puts("PHOTO INPUT PASS: focus, repeat, request, restore, guest input suppression, photo axis deadzone");
}
