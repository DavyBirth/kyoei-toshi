/* Observes the verified CUSA03562 UnitHudGameTitle update (cursor at +0x1b8,
 * clamped by the game to four items) and adds a fifth PC entry handled
 * outside the guest. The original routine runs unchanged through a relay.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "kyoei_title_exit.h"
#include <stdatomic.h>
#include <string.h>
#ifndef KYOEI_TITLE_EXIT_TEST
#include "gz.h"
#include "kyoei_hud_trace.h"
#include <windows.h>
#include <limits.h>
#include <stdlib.h>
typedef void (ABI *GuestUpdate)(void *);
static GuestUpdate original_update;
#endif

enum { VISIBLE_MS=200, CONFIRM=0x2000, CANCEL=0x4000, UP=0x10, DOWN=0x40 };
static _Atomic int cursor=-1, state=KYOEI_TITLE_EXIT_HIDDEN, hover;
static _Atomic uint64_t last_frame;
static uint32_t previous_buttons;
static int previous_down,previous_up;
static _Atomic int quit_requested, quit_taken;

#ifdef KYOEI_TITLE_EXIT_TEST
static uint64_t test_now;
static uint64_t now_ms(void) { return test_now; }
static void request_quit(void) { quit_requested=1; }
void kyoei_title_exit_test_frame(int value,uint64_t now) { cursor=value; last_frame=now; test_now=now; }
void kyoei_title_exit_test_clock(uint64_t now) { test_now=now; }
int kyoei_title_exit_test_quit_requested(void) { return quit_requested; }
#else
static uint64_t now_ms(void) { return GetTickCount64(); }
static void request_quit(void) {
    if (!quit_requested) gz_log("Kyoei title: Return to Desktop confirmed");
    quit_requested=1;
}
#endif

int kyoei_title_exit_take_quit(void) { return quit_requested && !atomic_exchange(&quit_taken,1); }
static int visible(void) {
    uint64_t frame=last_frame;
    return frame && now_ms()-frame<VISIBLE_MS && cursor>=0;
}
int kyoei_title_exit_state(void) {
    if (!visible()) return KYOEI_TITLE_EXIT_HIDDEN;
    int value=state;
    if (value<=KYOEI_TITLE_EXIT_IDLE) return hover?KYOEI_TITLE_EXIT_SELECTED:KYOEI_TITLE_EXIT_IDLE;
    return value;
}
void kyoei_title_exit_hover(int inside) { hover=inside!=0; }
int kyoei_title_exit_click(void) {
    if (!visible()) return 0;
    if (state==KYOEI_TITLE_EXIT_CONFIRM) request_quit();
    else { state=KYOEI_TITLE_EXIT_CONFIRM; }
    return 1;
}

static void set_state(int value) {
    if (state==value) return;
    state=value;
#ifndef KYOEI_TITLE_EXIT_TEST
    gz_log("Kyoei title: exit entry state=%d",value);
#endif
}
int kyoei_title_exit_input(uint32_t buttons,unsigned stick_y) {
    int down=(buttons&DOWN) || stick_y>=200, up=(buttons&UP) || stick_y<=56;
    uint32_t pressed=buttons&~previous_buttons;
    int down_edge=down&&!previous_down, up_edge=up&&!previous_up;
    previous_buttons=buttons; previous_down=down; previous_up=up;
    if (!visible()) { set_state(KYOEI_TITLE_EXIT_HIDDEN); return 0; }
    int current=state;
    if (current<=KYOEI_TITLE_EXIT_IDLE) {
        set_state(KYOEI_TITLE_EXIT_IDLE);
        if (cursor==3 && down_edge) { set_state(KYOEI_TITLE_EXIT_SELECTED); return 1; }
        return 0;
    }
    // Our entry owns the cursor: the game must not see these inputs.
    if (current==KYOEI_TITLE_EXIT_SELECTED) {
        if (up_edge) set_state(KYOEI_TITLE_EXIT_IDLE);
        else if (pressed&CONFIRM) set_state(KYOEI_TITLE_EXIT_CONFIRM);
    } else if (pressed&CONFIRM) request_quit();
    else if ((pressed&CANCEL) || up_edge || down_edge) set_state(KYOEI_TITLE_EXIT_SELECTED);
    return 1;
}

#ifndef KYOEI_TITLE_EXIT_TEST
static ABI void title_update(void *self) {
    original_update(self);
    int value=-1,accepting=0;
    memcpy(&value,(const char *)self+0x1b8,sizeof(value));
    // +0x1b0 gates the game's own cursor input: zero during intro/fades.
    memcpy(&accepting,(const char *)self+0x1b0,sizeof(accepting));
    if (!accepting) return;
    cursor=value>=0&&value<=3?value:-1;
    last_frame=now_ms();
}
static uint64_t code_hash(const unsigned char *bytes,size_t count) {
    uint64_t hash=UINT64_C(14695981039346656037);
    for (size_t i=0;i<count;++i) hash=(hash^bytes[i])*UINT64_C(1099511628211);
    return hash;
}
void kyoei_title_exit_configure(void *image,uint64_t size) {
    const char *setting=getenv("KYOEI_TITLE_EXIT");
    if (!setting || strcmp(setting,"1")) return;
    const size_t entry=0xaaf9b0,length=0x90;
    if (!image || size<entry+length) { gz_log("Kyoei title: image too small; exit entry skipped"); return; }
    unsigned char *code=(unsigned char *)image+entry;
    if (code_hash(code,length)!=UINT64_C(0xf92e5b62a80c1500)) { gz_log("Kyoei title: routine signature differs; exit entry skipped"); return; }
    unsigned char *relay=NULL;
    for (unsigned i=0;i<16 && !relay;++i)
        relay=VirtualAlloc((unsigned char *)image+0x5100000+i*0x10000,4096,MEM_COMMIT|MEM_RESERVE,PAGE_READWRITE);
    if (!relay) { gz_log("Kyoei title: no relay allocation; exit entry skipped"); return; }
    // Same verified six-byte prologue as the HUD marker routine.
    kyoei_hud_trace_relay(relay,code+6,title_update);
    int64_t distance=(int64_t)(uintptr_t)(relay+32)-(int64_t)(uintptr_t)(code+5);
    DWORD prior;
    if (distance<INT32_MIN || distance>INT32_MAX || !VirtualProtect(relay,4096,PAGE_EXECUTE_READ,&prior) ||
        !VirtualProtect(code,6,PAGE_EXECUTE_READWRITE,&prior)) {
        VirtualFree(relay,0,MEM_RELEASE); gz_log("Kyoei title: relay unavailable; exit entry skipped"); return;
    }
    original_update=(GuestUpdate)relay;
    int32_t relative=(int32_t)distance;
    code[0]=0xe9; memcpy(code+1,&relative,4); code[5]=0x90;
    DWORD ignored;
    if (!VirtualProtect(code,6,prior,&ignored)) gz_stop(21,"STOP: cannot restore title code protection");
    FlushInstructionCache(GetCurrentProcess(),relay,46);
    FlushInstructionCache(GetCurrentProcess(),code,6);
    gz_log("Kyoei title: menu update verified; Return to Desktop entry enabled");
}
#endif
