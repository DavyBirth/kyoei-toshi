/* Offline test of the actual bounded provenance walker.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "kyoei_photo_phone_membership.h"
#include <assert.h>
#include <string.h>
#include <stdio.h>
typedef struct {uintptr_t at;size_t size;} Region;
typedef struct {Region regions[512];unsigned count,reads;} Fixture;
typedef struct {uintptr_t table;unsigned char pad[0x50-sizeof(uintptr_t)];uintptr_t children;uint32_t count,reserved;} Node;
static void allow(Fixture *f,void *at,size_t size){assert(f->count<512);f->regions[f->count++]=(Region){(uintptr_t)at,size};}
static int read_fake(void *context,uintptr_t at,void *out,size_t size) {
    Fixture *f=context;++f->reads;
    for(unsigned i=0;i<f->count;++i) {
        Region r=f->regions[i];if(at>=r.at && at-r.at<=r.size && size<=r.size-(at-r.at)) {
            memcpy(out,(void*)at,size);return 1;
        }
    }
    return 0;
}
int main(void) {
    Fixture f={0};uintptr_t table[0xe8/8]={0};
    uintptr_t image=(uintptr_t)table-0x12dd330;
    table[0x98/8]=image+0x2f0090;table[0xe0/8]=image+0x2f01f0;allow(&f,table,sizeof(table));
    Node root={0},nested={0},sprite={0},unrelated={0};
    root.table=nested.table=(uintptr_t)table;
    uintptr_t direct[]={ (uintptr_t)&sprite };
    root.children=(uintptr_t)direct;root.count=1;
    allow(&f,&root,sizeof(root));allow(&f,&nested,sizeof(nested));allow(&f,&sprite,sizeof(sprite));allow(&f,&unrelated,sizeof(unrelated));allow(&f,direct,sizeof(direct));
    assert(photo_phone_contains(&f,read_fake,image,(uintptr_t)&root,(uintptr_t)&sprite));
    assert(!photo_phone_contains(&f,read_fake,image,(uintptr_t)&root,(uintptr_t)&unrelated));
    assert(!photo_phone_contains(&f,read_fake,image,(uintptr_t)&root,(uintptr_t)&root));
    assert(!photo_phone_contains(&f,read_fake,image,(uintptr_t)&unrelated,(uintptr_t)&sprite));
    uintptr_t nested_children[]={(uintptr_t)&sprite};allow(&f,nested_children,sizeof(nested_children));
    nested.children=(uintptr_t)nested_children;nested.count=1;direct[0]=(uintptr_t)&nested;
    assert(photo_phone_contains(&f,read_fake,image,(uintptr_t)&root,(uintptr_t)&sprite));
    /* A non-container leaf with coincidental array/count fields cannot extend
     * provenance: its vtable must resolve the known set draw/traversal pair. */
    nested.table=0;
    assert(!photo_phone_contains(&f,read_fake,image,(uintptr_t)&root,(uintptr_t)&sprite));
    nested.table=(uintptr_t)table;nested_children[0]=(uintptr_t)&root;f.reads=0;
    assert(!photo_phone_contains(&f,read_fake,image,(uintptr_t)&root,(uintptr_t)&sprite));assert(f.reads<50);
    nested_children[0]=0x12345678;
    assert(!photo_phone_contains(&f,read_fake,image,(uintptr_t)&root,(uintptr_t)&sprite));
    root.count=257;f.reads=0;
    assert(!photo_phone_contains(&f,read_fake,image,(uintptr_t)&root,(uintptr_t)&sprite));assert(f.reads<20);
    /* Depth8 may match; depth9 must fail open. */
    Node chain[10]={0};uintptr_t links[9];allow(&f,chain,sizeof(chain));allow(&f,links,sizeof(links));
    for(unsigned i=0;i<10;++i) {
        chain[i].table=(uintptr_t)table;
        if(i<9){links[i]=(uintptr_t)&chain[i+1];chain[i].children=(uintptr_t)&links[i];chain[i].count=1;}
    }
    assert(photo_phone_contains(&f,read_fake,image,(uintptr_t)&chain[0],(uintptr_t)&chain[8]));
    assert(!photo_phone_contains(&f,read_fake,image,(uintptr_t)&chain[0],(uintptr_t)&chain[9]));
    /* Traversal including root is capped at256 enqueued nodes, including
     * malformed/duplicate nodes. No unbounded scan of game resources. */
    uintptr_t many[256];Node leaves[256]={0};allow(&f,many,sizeof(many));allow(&f,leaves,sizeof(leaves));
    for(unsigned i=0;i<256;++i)many[i]=(uintptr_t)&leaves[i];
    root.children=(uintptr_t)many;root.count=256;f.reads=0;
    assert(!photo_phone_contains(&f,read_fake,image,(uintptr_t)&root,(uintptr_t)&leaves[255]));
    assert(f.reads<2000);
    puts("PHONE MEMBERSHIP PASS: provenance, unrelated world sprites, nested types, cycle, bad read, count/depth/work bounds");
}
