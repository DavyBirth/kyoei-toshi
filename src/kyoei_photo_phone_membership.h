/* Bounded structural provenance, no shader/depth/position heuristics.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdint.h>
#include <stddef.h>
typedef int (*PhotoPhoneRead)(void *context,uintptr_t address,void *out,size_t size);
static inline int photo_hud_tree_contains(void *context,PhotoPhoneRead read,
        uintptr_t image,uintptr_t root,uintptr_t root_table,uintptr_t target) {
    uintptr_t table;
    if(!root || !target || root==target || !read(context,root,&table,sizeof(table)) ||
       table!=root_table)return 0;
    struct Item {uintptr_t node;unsigned depth;} pending[256];
    uintptr_t visited[256];unsigned queued=1,seen=0,enqueued=1;
    pending[0]=(struct Item){root,0};
    while(queued && seen<256) {
        struct Item current=pending[--queued];
        int duplicate=0;for(unsigned i=0;i<seen;++i)if(visited[i]==current.node){duplicate=1;break;}
        if(duplicate)continue;
        visited[seen++]=current.node;
        if(current.node==target)return 1;
        if(current.depth>=8 || current.node>UINTPTR_MAX-0x60)continue;
        uintptr_t draw,walk;
        if(!read(context,current.node,&table,sizeof(table)) || table>UINTPTR_MAX-0xe8 ||
           !read(context,table+0x98,&draw,sizeof(draw)) || draw!=image+0x2f0090 ||
           !read(context,table+0xe0,&walk,sizeof(walk)) || walk!=image+0x2f01f0)continue;
        uintptr_t children;uint32_t count;
        if(!read(context,current.node+0x50,&children,sizeof(children)) ||
           !read(context,current.node+0x58,&count,sizeof(count)) || !children || count>256)continue;
        for(uint32_t i=0;i<count && queued<256 && enqueued<256;++i) {
            uintptr_t child;
            if(children>UINTPTR_MAX-(uintptr_t)i*sizeof(child) ||
               !read(context,children+(uintptr_t)i*sizeof(child),&child,sizeof(child)))continue;
            if(child){pending[queued++]=(struct Item){child,current.depth+1};++enqueued;}
        }
    }
    return 0; /* Failure/incomplete tree is always fail-open for drawing. */
}
static inline int photo_phone_contains(void *context,PhotoPhoneRead read,
        uintptr_t image,uintptr_t root,uintptr_t target) {
    return photo_hud_tree_contains(context,read,image,root,image+0x12dd330,target);
}
