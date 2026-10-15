/* Audited Kyoei FS loads -> GS. The POSIX runtime owns per-thread TCBs.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include "kyoei_modules.h"
static void kyoei_prepare_modules(const char *boot,uint64_t size) {
    char directory[4096];
    if(strlen(boot)>=sizeof(directory)) fail("module metadata path too long");
    strcpy(directory,boot);char *slash=strrchr(directory,'/');
    if(slash)*slash=0;else strcpy(directory,".");
    kyoei_modules_configure(directory,image,size);
}
static void kyoei_patch_tls(const char *boot,uint64_t size,const Segment *segments,uint64_t ns) {
    char path[4096];
    if (strlen(boot)>=sizeof(path)) fail("TLS metadata path too long");
    strcpy(path,boot);
    char *slash=strrchr(path,'/');
    if (!slash) strcpy(path,"tls-sites.tsv");
    else strcpy(slash+1,"tls-sites.tsv");
    FILE *f=fopen(path,"r");
    if (!f) fail("cannot open audited TLS sites");
    uint64_t sites[580]; size_t count=0; unsigned long long at; char line[64],extra;
    const unsigned char expected[]={0x64,0x48,0x8b,0x04,0x25,0,0,0,0};
    while (fgets(line,sizeof(line),f)) {
        if (count==580 || sscanf(line,"%llx %c",&at,&extra)!=1 || at>size || size-at<sizeof(expected) ||
            !mapped((Segment*)segments,ns,at,sizeof(expected)) || memcmp(image+at,expected,sizeof(expected)))
            fail("TLS instruction signature mismatch");
        int executable=0;
        for(uint64_t s=0;s<ns;++s) if((segments[s].flags&1) && at>=segments[s].address && at-segments[s].address<segments[s].size) executable=1;
        if(!executable) fail("TLS site outside executable segment");
        for(size_t i=0;i<count;++i) if(sites[i]==at) fail("duplicate TLS site");
        sites[count++]=at;
    }
    if(ferror(f) || count!=580) fail("incomplete Kyoei TLS audit");
    fclose(f);
    for(size_t i=0;i<count;++i) image[sites[i]]=0x65;
    printf("Kyoei Linux: %zu verified guest TLS loads redirected to GS\n",count);
}
