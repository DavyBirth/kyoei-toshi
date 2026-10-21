/* Exercise the production gates with synthetic memory and SysV spies.
 * Never loads or executes game code. SPDX-License-Identifier: GPL-2.0-or-later */
#include "kyoei_photo_tips.c"
#include <assert.h>
#include <stdio.h>
static int hud=1,scene=1,hud_reads,restore_during_walk,extended_calls,dummy_calls,logs;
static void *expected_font;
int kyoei_photo_hud_hidden(void) {
    ++hud_reads;return hud && !(restore_during_walk && hud_reads>1);
}
int kyoei_photo_hud_available(void){return 1;}
int kyoei_photo_scene_active(void){return scene;}
void gz_log(const char *format,...){(void)format;++logs;}
void gz_stop(int code,const char *format,...){(void)format;exit(code);}
uint64_t gz_memory_clamp(uintptr_t address,uint64_t size) {
    (void)address;(void)size;return 0; /* No guest allocation registry in this test. */
}
static ABI void dummy_spy(void *font){assert(font==expected_font);++dummy_calls;}
static ABI void extended_spy(void *font) {
    assert(font==expected_font);++extended_calls;
    /* The real extended function calls the base draw, then continues drawing. */
    tips_dummy_draw(font);
}
static void put64(unsigned char *base,size_t at,uintptr_t value){memcpy(base+at,&value,8);}
static void set_tree(unsigned char *base,size_t root,size_t table,size_t child) {
    put64(base,root,(uintptr_t)(base+table));
    put64(base,table+0x98,(uintptr_t)(base+0x2f0090));
    put64(base,table+0xe0,(uintptr_t)(base+0x2f01f0));
    put64(base,root+0x50,(uintptr_t)(base+root+0x100));
    put64(base,root+0x58,1);put64(base,root+0x100,(uintptr_t)(base+child));
}
int main(int argc,char **argv) {
    unsigned char *base=VirtualAlloc(NULL,0x1600000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);assert(base);
    /* Default-off and bad signatures leave both entrypoints and originals intact. */
    assert(!_putenv_s("KYOEI_PHOTO_TIPS",""));
    kyoei_photo_tips_configure(base,0x1600000);assert(!originals[0] && !originals[1]);
    assert(!_putenv_s("KYOEI_PHOTO_TIPS","1"));
    kyoei_photo_tips_configure(NULL,0x1600000);
    kyoei_photo_tips_configure(base,1);
    kyoei_photo_tips_configure(base,0x1600000);
    assert(!originals[0] && !originals[1] && base[0xae6820]==0 && base[0x2e56e0]==0);
    assert(!_putenv_s("KYOEI_PHOTO_TIPS",""));
    tips_image=(uintptr_t)base;originals[0]=extended_spy;originals[1]=dummy_spy;
    const size_t owner=0x1540000,root=0x1540200,font=0x1540600,world=0x1540800,loading=0x1540a00;
    put64(base,0x14976d0,(uintptr_t)(base+owner));put64(base,owner,(uintptr_t)(base+0x12d7708));
    put64(base,owner+0x78,(uintptr_t)(base+root));set_tree(base,root,0x12d8e58,font);
    put64(base,font,(uintptr_t)(base+0x12e1ec0));put64(base,world,(uintptr_t)(base+0x12e1d78));
    expected_font=base+font;
    unsigned char before[0x100];memcpy(before,base+font,sizeof(before));
    tips_dummy_draw(expected_font);assert(dummy_calls==0 && atomic_load(&suppressed)==1);
    assert(!memcmp(before,base+font,sizeof(before)));
    hud=0;tips_dummy_draw(expected_font);assert(dummy_calls==1);
    hud=1;scene=0;tips_dummy_draw(expected_font);assert(dummy_calls==2);
    scene=1;hud_reads=0;restore_during_walk=1;
    tips_dummy_draw(expected_font);assert(dummy_calls==3);restore_during_walk=0;
    expected_font=base+world;
    tips_extended_draw(expected_font);assert(extended_calls==1 && dummy_calls==4);
    /* Destroy the common owner; a former tip is drawn rather than cached. */
    put64(base,0x14976d0,0);expected_font=base+font;
    tips_dummy_draw(expected_font);assert(dummy_calls==5);
    put64(base,0x152cbf0,(uintptr_t)(base+loading));set_tree(base,loading,0x12dc718,font);
    put64(base,font,(uintptr_t)(base+0x12e1d78));
    tips_extended_draw(expected_font);assert(extended_calls==1 && dummy_calls==5);
    /* ReadProcessMemory conserves drawing for inaccessible/freed memory. */
    DWORD previous,ignored;
    assert(VirtualProtect(base+0x1540000,0x1000,PAGE_NOACCESS,&previous));
    tips_extended_draw(expected_font);assert(extended_calls==2 && dummy_calls==6);
    assert(VirtualProtect(base+0x1540000,0x1000,previous,&ignored));
    int previous_logs=logs;
    for(unsigned i=0;i<20;++i)tips_extended_draw(expected_font);
    assert(atomic_load(&suppressed)==8 && logs-previous_logs==6);
    put64(base,0x152cbf0,0);
    tips_extended_draw(expected_font);assert(extended_calls==3 && dummy_calls==7);
    assert(VirtualFree(base,0,MEM_RELEASE));
    assert(!tips_read(NULL,(uintptr_t)base,before,8));
    if(argc==2) {
        /* Optional private installation proof from the owner's prepared ELF.
         * Only copy/hash/patch bytes. Never call the copied game functions.
         * This known prepared ELF maps its code at file offset VA+0x4000;
         * full function hashes reject any incompatible input. */
        FILE *file=fopen(argv[1],"rb");assert(file);
        base=VirtualAlloc(NULL,0x1600000,MEM_RESERVE|MEM_COMMIT,PAGE_READWRITE);assert(base);
        const size_t entries[2]={0xae6820,0x2e56e0},lengths[2]={3681,588};
        for(unsigned i=0;i<2;++i) {
            assert(!fseek(file,(long)(entries[i]+0x4000),SEEK_SET));
            assert(fread(base+entries[i],1,lengths[i],file)==lengths[i]);
        }
        fclose(file);originals[0]=originals[1]=NULL;tips_image=0;
        assert(!_putenv_s("KYOEI_PHOTO_TIPS","1"));
        /* Corrupt only the second function: the first must stay untouched. */
        base[entries[1]+20]^=1;kyoei_photo_tips_configure(base,0x1600000);
        assert(!originals[0] && !originals[1] && base[entries[0]]==0x55);
        base[entries[1]+20]^=1;
        assert(VirtualProtect(base,0x1600000,PAGE_EXECUTE_READ,&previous));
        kyoei_photo_tips_configure(base,0x1600000);
        assert(originals[0] && originals[1] && tips_image==(uintptr_t)base);
        for(unsigned i=0;i<2;++i) {
            assert(base[entries[i]]==0xe9 && base[entries[i]+5]==0x90);
            assert(!memcmp((void*)originals[i],"\x55\x48\x89\xe5\x41\x57",6));
            MEMORY_BASIC_INFORMATION info;
            assert(VirtualQuery(base+entries[i],&info,sizeof(info)) && info.Protect==PAGE_EXECUTE_READ);
            assert(VirtualQuery((void*)originals[i],&info,sizeof(info)) && info.Protect==PAGE_EXECUTE_READ);
        }
        uintptr_t relay=(uintptr_t)originals[0];
        kyoei_photo_tips_configure(base,0x1600000);assert((uintptr_t)originals[0]==relay);
        assert(VirtualFree((void*)relay,0,MEM_RELEASE));assert(VirtualFree(base,0,MEM_RELEASE));
        assert(!_putenv_s("KYOEI_PHOTO_TIPS",""));
        puts("TIPS INSTALL PASS: full signatures, second-function mismatch, both gates, relay prologues, RX protection, repeat configure; no guest execution");
    }
    puts("TIPS GATE PASS: default off, signature rejection, SysV originals, both fonts, world text, restore, teardown, inaccessible memory, bounded logs");
}
