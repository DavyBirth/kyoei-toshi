/* Exercise the actual draw gate without guest assets or a running game.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "kyoei_photo_save_mark.c"
#include <assert.h>
#include <stdio.h>
static int hud,draws;
int kyoei_photo_hud_hidden(void){return hud;}
void gz_log(const char *format,...){(void)format;}
void gz_stop(int code,const char *format,...){(void)format;exit(code);}
size_t kyoei_hud_trace_relay(unsigned char *relay,const void *resume,const void *callback){
    (void)relay;(void)resume;(void)callback;abort();
}
static ABI void spy(void *object){(void)object;++draws;}
int main(void){
    uintptr_t player=0x12cba00,npc=0x1284630,marker=0x12d2060;
    base=NULL;original_draw=spy;player_supported=1;
    assert(!kyoei_photo_player_toggle());
    draw_model(&player);assert(draws==1); // ordinary gameplay always draws
    hud=1;draw_model(&player);assert(draws==2); // visible by default
    assert(kyoei_photo_player_toggle());
    draw_model(&player);assert(draws==2);
    draw_model(&npc);assert(draws==3); // other actors survive
    draw_model(&marker);assert(draws==3); // existing marker filter survives
    hud=0;draw_model(&player);assert(draws==4); // HUD gate protects gameplay
    kyoei_photo_player_restore();hud=1;
    draw_model(&player);assert(draws==5); // next photo starts visible
    assert(kyoei_photo_player_toggle());assert(kyoei_photo_player_toggle());
    draw_model(&player);assert(draws==6);
    player_supported=0;assert(!kyoei_photo_player_toggle());
    atomic_store(&player_hidden,1);draw_model(&player);assert(draws==7);
    uintptr_t derived=0x12b3140;
    player_supported=1;player2_supported=1;draw_model(&derived);assert(draws==7);
    player2_supported=0;draw_model(&derived);assert(draws==8);
    puts("PHOTO PLAYER PASS");return 0;
}
