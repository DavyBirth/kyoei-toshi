#include <cstdio>
#include "kyoei_mouse.h"
static int modal=0;
extern "C" int kyoei_overlay_kind(void) { return modal; }
extern "C" void kyoei_input_keyboard(void) {}
#define CHECK(x) do { if(!(x)) { std::fprintf(stderr,"Mouse check failed: %d\n",__LINE__);return 1; } } while(0)
int main() {
    using namespace KyoeiMouse;
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
