#define _GNU_SOURCE
#include "kyoei_linux_tls.h"
#include <asm/prctl.h>
#include <sys/syscall.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"TLS check failed: %s line %d\n",#x,__LINE__);abort();}}while(0)
static pthread_barrier_t barrier;
static void *tcbs[4];
static void *worker(void *value) {
    uintptr_t id=(uintptr_t)value;uintptr_t previous,after,guest;
    CHECK(!syscall(SYS_arch_prctl,ARCH_GET_GS,&previous));
    errno=(int)id+70;
    CHECK(!kyoei_linux_tls_enter(id));
    CHECK(!kyoei_linux_tls_enter(id));
    __asm__ volatile("movq %%gs:0,%0":"=r"(guest));
    tcbs[id]=(void*)guest;CHECK(tcbs[id]==kyoei_linux_tls_tcb());
    uint64_t *tcb=(void*)guest,*dtv=(void*)(uintptr_t)tcb[1];
    unsigned char *data=(void*)(uintptr_t)dtv[2];
    CHECK(dtv[0]==1&&tcb[2]==id&&data[0]==42&&data[1]==43&&data[2]==0);
    CHECK((unsigned char*)tcb-data==48); // guest alignment, independent of host allocation alignment
    data[2]=(unsigned char)(id+1);pthread_barrier_wait(&barrier);
    for(unsigned i=0;i<4;++i)if(i!=id)CHECK(tcbs[i]!=tcbs[id]);
    CHECK(data[2]==id+1&&errno==(int)id+70);
    CHECK(!kyoei_linux_tls_leave());
    __asm__ volatile("movq %%gs:0,%0":"=r"(guest));CHECK(guest==(uintptr_t)tcb);
    CHECK(!kyoei_linux_tls_leave());CHECK(kyoei_linux_tls_leave()==-1);
    CHECK(!syscall(SYS_arch_prctl,ARCH_GET_GS,&after)&&after==previous);
    return NULL;
}
int main(void) {
    unsigned char image[64]={42,43};uint64_t bad[4]={0,2,1,16},layout[4]={0,2,33,16};
    CHECK(kyoei_linux_tls_configure(image,sizeof(image),bad)==-1);
    CHECK(!kyoei_linux_tls_configure(image,sizeof(image),layout));
    CHECK(kyoei_linux_tls_configure(image,sizeof(image),layout)==-1);
    pthread_t threads[4];CHECK(!pthread_barrier_init(&barrier,NULL,4));
    for(uintptr_t i=0;i<4;++i)CHECK(!pthread_create(&threads[i],NULL,worker,(void*)i));
    for(unsigned i=0;i<4;++i)CHECK(!pthread_join(threads[i],NULL));
    unsigned char code[20]={0x64,0x48,0x8b,0x04,0x25,0,0,0,0};
    uint64_t sites[2]={0,19};CHECK(kyoei_linux_tls_patch(code,sizeof(code),sites,2)==-1&&code[0]==0x64);
    sites[1]=0;CHECK(kyoei_linux_tls_patch(code,sizeof(code),sites,2)==-1&&code[0]==0x64);
    CHECK(kyoei_linux_tls_patch(code,sizeof(code),sites,1)==1&&code[0]==0x65);
    puts("LINUX TLS PASS: four isolated threads, template/zero fill, real GS loads, libc FS/errno preserved, nested enter/leave, GS restored, atomic site validation");
    return 0;
}
