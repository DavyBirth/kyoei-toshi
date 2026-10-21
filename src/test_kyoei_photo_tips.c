/* Synthetic guest memory; no game files or guest execution.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "kyoei_photo_tips_membership.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const uintptr_t image=0x10000000;
enum { MEMORY_SIZE=0x1600000 };
typedef struct {unsigned char *bytes;unsigned reads;uintptr_t change_at,change_to;} Memory;
static void put64(Memory *m,uintptr_t offset,uintptr_t value) {
    assert(offset<=MEMORY_SIZE-8);memcpy(m->bytes+offset,&value,8);
}
static int read_memory(void *context,uintptr_t address,void *out,size_t size) {
    Memory *m=context;++m->reads;
    if(address<image || address-image>MEMORY_SIZE || size>MEMORY_SIZE-(address-image))return 0;
    memcpy(out,m->bytes+(address-image),size);
    if(address==m->change_at) {
        put64(m,0x14976d0,m->change_to);m->change_at=0;
    }
    return 1;
}
static void set_tree(Memory *m,uintptr_t root,uintptr_t table,uintptr_t child) {
    put64(m,root,image+table);
    put64(m,table+0x98,image+0x2f0090);put64(m,table+0xe0,image+0x2f01f0);
    put64(m,root+0x50,image+root+0x100);
    put64(m,root+0x58,1);put64(m,root+0x100,image+child);
}
int main(void) {
    Memory m={0};m.bytes=calloc(1,MEMORY_SIZE);assert(m.bytes);
    const uintptr_t owner=0x1540000,root=0x1540200,font=0x1540600,world=0x1540800,loading=0x1540a00;
    put64(&m,0x14976d0,image+owner);put64(&m,owner,image+0x12d7708);
    put64(&m,owner+0x78,image+root);set_tree(&m,root,0x12d8e58,font);
    put64(&m,font,image+0x12e1ec0);put64(&m,world,image+0x12e1ec0);
    assert(photo_tips_contains(&m,read_memory,image,image+font));
    assert(!photo_tips_contains(&m,read_memory,image,image+world));
    put64(&m,font,image+0x1256658); /* A sprite is not a font gate target. */
    assert(!photo_tips_contains(&m,read_memory,image,image+font));
    put64(&m,font,image+0x12e1ec0);
    put64(&m,owner,image+0x12dd330);
    assert(!photo_tips_contains(&m,read_memory,image,image+font));
    put64(&m,owner,image+0x12d7708);
    m.change_at=image+font;m.change_to=0;
    assert(!photo_tips_contains(&m,read_memory,image,image+font));
    put64(&m,0x14976d0,image+owner);
    /* Replace the singleton after tree membership succeeds, before recheck. */
    m.change_at=image+root+0x100;m.change_to=image+owner+0x1000;
    assert(!photo_tips_contains(&m,read_memory,image,image+font));
    put64(&m,0x14976d0,0);
    put64(&m,0x152cbf0,image+loading);set_tree(&m,loading,0x12dc718,font);
    put64(&m,font,image+0x12e1d78);
    assert(photo_tips_contains(&m,read_memory,image,image+font));
    put64(&m,0x152cbf0,0);
    assert(!photo_tips_contains(&m,read_memory,image,image+font));
    put64(&m,0x152cbf0,UINTPTR_MAX-7);
    assert(!photo_tips_contains(&m,read_memory,image,image+font));
    assert(!photo_tips_contains(&m,read_memory,UINTPTR_MAX,image+font));
    assert(!photo_tips_contains(&m,read_memory,image,UINTPTR_MAX));
    free(m.bytes);
    puts("TIPS MEMBERSHIP PASS: both owners, world text, wrong types, owner replacement, teardown, bad pointers");
}
