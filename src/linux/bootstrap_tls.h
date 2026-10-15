/* Prepare audited main-TLS sites before final image protection. */
#pragma once
#include "kyoei_linux_tls.h"
static void prepare_main_tls(const char *out,const uint64_t layout[4]) {
    char path[1024];snprintf(path,sizeof(path),"%s/tls-sites.tsv",out);
    FILE *file=fopen(path,"r");if(!file)fail("Linux TLS site metadata missing");
    uint64_t sites[4096];size_t count=0;char line[64];unsigned long long value;
    while(fgets(line,sizeof(line),file)){
        char extra;
        if(count==4096||sscanf(line,"%llx %c",&value,&extra)!=1||!executable(value))fail("invalid Linux TLS site metadata");
        sites[count++]=value;
    }
    if(ferror(file))fail("cannot read Linux TLS site metadata");fclose(file);
    if(count!=580)fail("expected 580 audited Kyoei TLS sites");
    if(kyoei_linux_tls_configure(image,image_size,layout)||kyoei_linux_tls_patch(image,image_size,sites,count)!=(int)count)
        fail("Linux TLS preparation failed");
    if(kyoei_linux_tls_enter(0))fail("Linux TLS thread entry failed");
    uintptr_t tcb;__asm__ volatile("movq %%gs:0,%0":"=r"(tcb));
    if(tcb!=(uintptr_t)kyoei_linux_tls_tcb()||kyoei_linux_tls_leave())fail("Linux TLS GS probe failed");
    gz_log("Linux main TLS: %zu verified FS loads prepared as GS loads; host GS restored; no guest code executed",count);
}
