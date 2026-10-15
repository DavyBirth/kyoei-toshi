#include <cstdio>
#include "kyoei_mouse.h"
#include "kyoei_pc_controls.h"
static int modal=0;
extern "C" int kyoei_overlay_kind(void) { return modal; }
extern "C" int kyoei_title_exit_state(void) { return 0; }
extern "C" void kyoei_input_keyboard(void) {}
#define CHECK(x) do { if(!(x)) { std::fprintf(stderr,"Mouse check failed: %d\n",__LINE__);return 1; } } while(0)
int main() {
    using namespace KyoeiMouse;
    CHECK(KyoeiPcControls::KeyMask(SDL_SCANCODE_E)==0x2000);
    CHECK(KyoeiPcControls::KeyMask(SDL_SCANCODE_LSHIFT)==0x800);
    CHECK(KyoeiPcControls::KeyMask(SDL_SCANCODE_LCTRL)==0x8000);
    CHECK(KyoeiPcControls::KeyMask(SDL_SCANCODE_ESCAPE)==8);
    kyoei_prompt_pause_seen(); kyoei_prompt_status_seen(); kyoei_prompt_presented();
    CHECK(KyoeiPcControls::KeyMask(SDL_SCANCODE_LEFT)==0x400);
    CHECK(KyoeiPcControls::KeyMask(SDL_SCANCODE_D)==0x800);
    CHECK(KyoeiPcControls::KeyMask(SDL_SCANCODE_Q)==0);
    CHECK(KyoeiPcControls::KeyMask(SDL_SCANCODE_LSHIFT)==0);
    kyoei_prompt_reset();kyoei_prompt_pause_seen();kyoei_prompt_presented();
    CHECK(KyoeiPcControls::KeyMask(SDL_SCANCODE_LEFT)==0x80); // inventory grid
    CHECK(KyoeiPcControls::KeyMask(SDL_SCANCODE_A)==0x400);
    kyoei_prompt_reset();kyoei_prompt_presented();
    CHECK(KyoeiPcControls::KeyMask(SDL_SCANCODE_LSHIFT)==0x800);
    CHECK(KyoeiPcControls::KeyMask(SDL_SCANCODE_LEFT)==0x80);
    _putenv_s("KYOEI_MOUSE_SENSITIVITY","1.0");float low=Axis(1,.008,128);
    _putenv_s("KYOEI_MOUSE_SENSITIVITY","2.0");CHECK(Axis(1,.008,128)>low);
    _putenv_s("KYOEI_MOUSE_SENSITIVITY","nan");CHECK(Sensitivity()==1.8f);
    _putenv_s("KYOEI_MOUSE_SENSITIVITY","");
    CHECK(Axis(0,.008,203)==203);
    CHECK(Axis(2,.008,128)==Axis(4,.016,128));
    CHECK(Axis(-100,.001,128)==1 && Axis(100,.001,128)==255);
    CHECK(Axis(1,0,128)>128);
    struct Stick { unsigned char x,y; } stick{128,128};
    captured=true;previous=SDL_GetTicksNS();dx=4;dy=-4;Apply(stick);
    CHECK(stick.x>128 && stick.y<128 && dx==0 && dy==0);
    stick={128,128};Apply(stick);CHECK(stick.x==128 && stick.y==128);
    modal=1;dx=50;dy=50;Apply(stick);CHECK(stick.x==128 && stick.y==128 && dx==0);
    modal=0;captured=false;dx=50;Apply(stick);CHECK(stick.x==128 && dx==0);
    // Real SDL focus/cursor transitions, using an invisible dummy window.
    SDL_SetHint(SDL_HINT_VIDEO_DRIVER,"dummy");CHECK(SDL_Init(SDL_INIT_VIDEO));
    auto* window=SDL_CreateWindow("mouse-test",64,64,SDL_WINDOW_HIDDEN);CHECK(window);
    captured=false;Update(window);CHECK(!captured);
    SDL_Event event{};event.type=SDL_EVENT_KEY_DOWN;event.key.key=SDLK_F7;
    released=false;Event(window,event);CHECK(released);
    event.key.repeat=true;Event(window,event);CHECK(released);
    event.key.repeat=false;Event(window,event);CHECK(!released);
    _putenv_s("KYOEI_MOUSE_CAMERA","0");Update(window);CHECK(!captured);
    SDL_DestroyWindow(window);SDL_Quit();
    std::puts("MOUSE PASS: speed normalization, bounds, consumption, modal, unfocused window, toggle");
}
