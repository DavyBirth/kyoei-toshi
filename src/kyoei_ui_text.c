/* Short UI literals outside the script translation catalog (CUSA03562 01.03).
 * Preserve addresses, tokens and trailing string storage; verify each anchor.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
static uint64_t kyoei_text_fnv(const unsigned char *bytes, size_t count) {
    uint64_t hash=UINT64_C(14695981039346656037);
    for (size_t i=0;i<count;++i) hash=(hash^bytes[i])*UINT64_C(1099511628211);
    return hash;
}
/* KYTEXT1 (tools/build_eboot_text.py): fixed-slot display strings of the executable. Each
 * slot is written only when its current bytes match the recorded hash of the original. */
static void kyoei_eboot_text(void *image, uint64_t size) {
    const char *path=getenv("KYOEI_EBOOT_TEXT");
    if (!path || !*path) return;
    FILE *file=fopen(path,"rb");
    if (!file) { gz_log("Kyoei UI text: missing %s",path); return; }
    unsigned char head[12]; uint32_t count=0;
    int ok=fread(head,1,12,file)==12 && !memcmp(head,"KYTEXT1\0",8);
    if (ok) { memcpy(&count,head+8,4); ok=count<=4096; }
    unsigned patched=0, skipped=0;
    for (uint32_t i=0;ok && i<count;++i) {
        uint64_t at,hash; uint32_t slot; uint16_t length; char english[512];
        ok=fread(&at,8,1,file)==1 && fread(&slot,4,1,file)==1 && fread(&hash,8,1,file)==1 &&
           fread(&length,2,1,file)==1 && length<=slot && length<sizeof(english) &&
           fread(english,1,length,file)==length;
        if (!ok) break;
        unsigned char *text=(unsigned char*)image+at;
        if (!image || at>=size || slot>size-at || kyoei_text_fnv(text,slot)!=hash) { ++skipped; continue; }
        DWORD old,ignored;
        if (!VirtualProtect(text,slot,PAGE_READWRITE,&old)) { ++skipped; continue; }
        memset(text,0,slot); memcpy(text,english,length);
        if (!VirtualProtect(text,slot,old,&ignored)) gz_stop(21,"STOP: cannot restore UI text protection");
        ++patched;
    }
    fclose(file);
    gz_log("Kyoei UI text: %u executable strings in English, %u skipped%s",patched,skipped,ok?"":" (pack damaged)");
}
void kyoei_ui_text_configure(void *image, uint64_t size) {
    const char *language=getenv("KYOEI_LANGUAGE");
    if (!language || strcmp(language,"en")) return;
    kyoei_eboot_text(image,size);
    static const struct { size_t at; const char *before,*after; } labels[]={
        {0x10c3b0c,"\xe8\xbf\x94\xe4\xbf\xa1\xe9\x81\xb8\xe6\x8a\x9e","Reply"},
        {0x10c3b33,"\xe8\xbf\x94\xe4\xbf\xa1\xe6\xb1\xba\xe5\xae\x9a","Confirm"},
        {0x10c3aff,"\xe8\xbf\x94\xe4\xbf\xa1\xe3\x81\xaf\xe2\x80\xa6","Reply"},
        {0x10c1708,"\xe5\x8d\x88\xe5\xbe\x8c","PM"},
        {0x10c1701,"\xe5\x8d\x88\xe5\x89\x8d","AM"},
    };
    unsigned patched=0;
    for (unsigned i=0;i<sizeof(labels)/sizeof(*labels);++i) {
        size_t n=strlen(labels[i].before);
        if (!image || labels[i].at>=size || n+1>size-labels[i].at ||
            memcmp((char*)image+labels[i].at,labels[i].before,n+1)) continue;
        DWORD old,ignored;
        char *text=(char*)image+labels[i].at;
        if (!VirtualProtect(text,n+1,PAGE_READWRITE,&old)) continue;
        memset(text,0,n+1);memcpy(text,labels[i].after,strlen(labels[i].after));
        if (!VirtualProtect(text,n+1,old,&ignored)) gz_stop(21,"STOP: cannot restore UI text protection");
        ++patched;
    }
    gz_log("Kyoei UI text: %u verified English mail/time literals",patched);
}
