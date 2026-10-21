/* Offline-validated provenance for the experimental independent tip-font gate.
 * Always resolve current owners; never retain guest pointers between calls.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include "kyoei_photo_phone_membership.h"

static inline int photo_tips_contains(void *context,PhotoPhoneRead read,
        uintptr_t image,uintptr_t target) {
    if(!image || image>UINTPTR_MAX-0x152cbf8 || !target ||
       target>UINTPTR_MAX-0x60)return 0;
    uintptr_t font_table=0;
    if(!read(context,target,&font_table,sizeof(font_table)) ||
       (font_table!=image+0x12e1ec0 && font_table!=image+0x12e1d78))return 0;
    for(unsigned kind=0;kind<2;++kind) {
        uintptr_t slot=image+(kind?0x152cbf0:0x14976d0);
        uintptr_t owner=0,owner_table=0,root=0,table=0,again=0;
        if(!read(context,slot,&owner,sizeof(owner)) || !owner ||
           owner>UINTPTR_MAX-0x80 ||
           !read(context,owner,&owner_table,sizeof(owner_table)))continue;
        if(kind) {
            if(owner_table!=image+0x12dc718)continue;
            root=owner;
        } else {
            if(owner_table!=image+0x12d7708 ||
               !read(context,owner+0x78,&root,sizeof(root)))continue;
        }
        uintptr_t expected=image+(kind?0x12dc718:0x12d8e58);
        if(!photo_hud_tree_contains(context,read,image,root,expected,target))continue;
        /* A scene/owner replacement during the walk conserves drawing. This
         * recheck is not a substitute for guest lifetime synchronization. */
        if(!read(context,slot,&again,sizeof(again)) || again!=owner ||
           !read(context,owner,&table,sizeof(table)) || table!=owner_table ||
           !read(context,root,&table,sizeof(table)) || table!=expected)continue;
        if(!kind && (!read(context,owner+0x78,&again,sizeof(again)) || again!=root))continue;
        if(!read(context,target,&table,sizeof(table)) || table!=font_table)continue;
        return 1;
    }
    return 0;
}
