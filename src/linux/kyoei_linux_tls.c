/* Variant-II layout shared with Kyoei's Windows adapter, POSIX ownership.
 * GS is restored on leave/destruction; glibc's FS is never changed.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#define _GNU_SOURCE
#include "kyoei_linux_tls.h"
#include <asm/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

typedef struct {void *block;uint64_t *tcb;uint64_t dtv[4];uintptr_t previous_gs;unsigned depth;} Local;
static pthread_key_t key;
static int configured;
static unsigned char *template;
static size_t memory_bytes,template_bytes,alignment;
static void release(void *value) {
    Local *local=value;
    if(local){
        if(local->depth&&syscall(SYS_arch_prctl,ARCH_SET_GS,local->previous_gs))abort();
        free(local->block);free(local);
    }
}
/* Configure once, before any guest worker starts. */
int kyoei_linux_tls_configure(const void *image,uint64_t image_size,const uint64_t layout[4]) {
    if(configured||layout[0]>image_size||layout[1]>image_size-layout[0]||layout[1]>layout[2]||
       layout[2]>(1u<<20)||!layout[3]||layout[3]>4096||(layout[3]&(layout[3]-1)))return -1;
    unsigned char *copy=malloc(layout[1]?layout[1]:1);
    if(!copy)return -1;
    memcpy(copy,(const unsigned char*)image+layout[0],layout[1]);
    if(pthread_key_create(&key,release)){free(copy);return -1;}
    template=copy;memory_bytes=layout[2];template_bytes=layout[1];
    alignment=layout[3];configured=1;return 0;
}
static Local *current(void) {
    if(!configured)return NULL;
    Local *local=pthread_getspecific(key);
    if(local)return local;
    local=calloc(1,sizeof(*local));if(!local)return NULL;
    size_t offset=(memory_bytes+alignment-1)&~(alignment-1);
    if(posix_memalign(&local->block,alignment<64?64:alignment,offset+256)){free(local);return NULL;}
    memset(local->block,0,offset+256);memcpy(local->block,template,template_bytes);
    local->tcb=(uint64_t*)((unsigned char*)local->block+offset);
    local->tcb[0]=(uintptr_t)local->tcb;local->tcb[1]=(uintptr_t)local->dtv;
    local->dtv[0]=1;local->dtv[2]=(uintptr_t)local->block;
    if(pthread_setspecific(key,local)){release(local);return NULL;}
    return local;
}
void *kyoei_linux_tls_tcb(void){Local *local=current();return local?local->tcb:NULL;}
int kyoei_linux_tls_enter(uintptr_t guest_thread) {
    Local *local=current();if(!local)return -1;
    if(!local->depth){
        if(syscall(SYS_arch_prctl,ARCH_GET_GS,&local->previous_gs))return -1;
        local->tcb[2]=guest_thread;
        if(syscall(SYS_arch_prctl,ARCH_SET_GS,local->tcb))return -1;
    }
    ++local->depth;return 0;
}
int kyoei_linux_tls_leave(void) {
    Local *local=configured?pthread_getspecific(key):NULL;
    if(!local||!local->depth)return -1;
    if(local->depth==1&&syscall(SYS_arch_prctl,ARCH_SET_GS,local->previous_gs))return -1;
    --local->depth;return 0;
}
/* Image must still be writable: called during loading, before mprotect.
 * Validate the whole list first, so a bad site cannot partially patch code. */
int kyoei_linux_tls_patch(unsigned char *image,uint64_t size,const uint64_t *sites,size_t count) {
    const unsigned char expected[9]={0x64,0x48,0x8b,0x04,0x25,0,0,0,0};
    if(count>4096)return -1;
    for(size_t i=0;i<count;++i){
        if(sites[i]>size||size-sites[i]<9||memcmp(image+sites[i],expected,9))return -1;
        for(size_t j=0;j<i;++j)if(sites[j]==sites[i])return -1;
    }
    for(size_t i=0;i<count;++i)image[sites[i]]=0x65;
    return (int)count;
}
