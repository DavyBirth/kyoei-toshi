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
#include <stdio.h>
typedef void (ABI *GuestUpdate)(void *);
static GuestUpdate original_update;
#endif

enum { VISIBLE_MS=200, CONFIRM=0x2000, UP=0x10, DOWN=0x40 };
static _Atomic int cursor=-1, accepting, state=KYOEI_TITLE_EXIT_HIDDEN, hover;
static _Atomic uint64_t last_frame;
static uint32_t previous_buttons;
static int previous_down,previous_up,swallow;
static _Atomic int quit_requested, quit_taken;
/* Geometry published by the guest thread; read by the overlay each frame. */
static volatile float item_alpha, item_x, item_y, item_focus;

#ifdef KYOEI_TITLE_EXIT_TEST
static uint64_t test_now;
static uint64_t now_ms(void) { return test_now; }
static void request_quit(void) { quit_requested=1; }
void kyoei_title_exit_test_frame(int value,uint64_t now) { cursor=value; accepting=1; item_alpha=1; last_frame=now; test_now=now; }
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
static int shown(void) {
    uint64_t frame=last_frame;
    return frame && now_ms()-frame<VISIBLE_MS && item_alpha>0;
}
static int usable(void) { return shown() && accepting && cursor>=0; }
int kyoei_title_exit_state(void) {
    if (!shown()) return KYOEI_TITLE_EXIT_HIDDEN;
    int value=state;
    if (value<=KYOEI_TITLE_EXIT_IDLE) return hover&&usable()?KYOEI_TITLE_EXIT_SELECTED:KYOEI_TITLE_EXIT_IDLE;
    return value;
}
int kyoei_title_exit_item(KyoeiTitleItem *item) {
    if (!item || !shown()) return 0;
    item->alpha=item_alpha; item->x=item_x; item->y=item_y; item->focus=item_focus;
    return 1;
}
void kyoei_title_exit_hover(int inside) { hover=inside!=0; }
int kyoei_title_exit_click(void) {
    if (!usable()) return 0;
    request_quit();
    return 1;
}

static void set_state(int value) {
    if (state==value) return;
    state=value;
#ifndef KYOEI_TITLE_EXIT_TEST
    gz_log("Kyoei title: exit entry state=%d",value);
#endif
}
/* The menu's own confirm (E / Enter / Circle) quits at once: nothing on the
 * title screen can be lost, matching a PC title menu's Quit entry. */
int kyoei_title_exit_input(uint32_t buttons,unsigned stick_y) {
    int down=(buttons&DOWN) || stick_y>=200, up=(buttons&UP) || stick_y<=56;
    uint32_t pressed=buttons&~previous_buttons;
    int down_edge=down&&!previous_down, up_edge=up&&!previous_up;
    previous_buttons=buttons; previous_down=down; previous_up=up;
    if (!usable()) { if (!shown()) set_state(KYOEI_TITLE_EXIT_HIDDEN); return 0; }
    // After leaving our row, hide the still-held direction from the game until
    // it is released; otherwise the game reads it as a fresh press and skips ????.
    if (swallow) { if (buttons || down || up) return 1; swallow=0; }
    if (state<=KYOEI_TITLE_EXIT_IDLE) {
        set_state(KYOEI_TITLE_EXIT_IDLE);
        if (cursor==3 && down_edge) { set_state(KYOEI_TITLE_EXIT_SELECTED); return 1; }
        return 0;
    }
    // Our entry owns the cursor: the game must not see these inputs.
    if (up_edge) { set_state(KYOEI_TITLE_EXIT_IDLE); swallow=1; }
    else if (pressed&CONFIRM) request_quit();
    return 1;
}

#ifndef KYOEI_TITLE_EXIT_TEST
/* UnitHudGameTitle node table at +0x50: items 5/7/9/11 (focus layers 6/8/10/12),
 * reserved fifth slot 13/14, underline 15/16. Node +0x60/+0x64 = x/y in the
 * 1920x1080 HUD, +0x94 = RGBA color (alpha in the high byte). */
typedef struct { unsigned char pad[0x60]; float x, y; unsigned char pad2[0x94-0x68]; uint32_t color; } HudNode;
static HudNode *node(void *self,int index) {
    HudNode **nodes=NULL; memcpy(&nodes,(const char *)self+0x50,8);
    return nodes?nodes[index]:NULL;
}
static ABI void title_update(void *self) {
    original_update(self);
    int value=-1,ready=0;
    memcpy(&value,(const char *)self+0x1b8,sizeof(value));
    // +0x1b0 gates the game's own cursor input: zero during intro/fades.
    memcpy(&ready,(const char *)self+0x1b0,sizeof(ready));
    HudNode *items[4]={node(self,5),node(self,7),node(self,9),node(self,11)};
    HudNode *slot=node(self,13),*line_a=node(self,15),*line_b=node(self,16);
    if (!items[0]||!items[3]||!slot||!line_a||!line_b) return;
    float alpha=(float)(items[3]->color>>24)/255.f;
    float base=items[0]->x, selected_x=items[0]->x;
    for (int i=0;i<4;++i) { if (items[i]->x<base) base=items[i]->x; if (items[i]->x>selected_x) selected_x=items[i]->x; }
    float shift=selected_x>base?selected_x-base:50.f;
    int ours=state>=KYOEI_TITLE_EXIT_SELECTED || (hover && ready);
    if (ours && value==3) {
        // Hand the native highlight to our row: item 4 returns to its resting
        // column and its glow layer hides; the game's underline follows us.
        HudNode *glow=node(self,12);
        float offset=line_a->y-items[3]->y;
        items[3]->x=base; if (glow) { glow->x=base; glow->color&=0x00ffffffu; }
        line_a->y=line_b->y=slot->y+offset;
    }
    float target=ours?base+shift:base;
    item_x=item_x+(target-item_x)*0.35f;
    if (item_x<base-1||item_x>base+shift+1) item_x=target;
    item_focus=item_focus+((ours?1.f:0.f)-item_focus)*0.35f;
    item_y=slot->y; item_alpha=alpha;
    cursor=value>=0&&value<=3?value:-1; accepting=ready;
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
