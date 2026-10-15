/* Short UI literals outside the script translation catalog (CUSA03562 01.03).
 * Preserve addresses, tokens and trailing string storage; verify each anchor.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include <windows.h>
#include <stdlib.h>
#include <string.h>
void kyoei_ui_text_configure(void *image, uint64_t size) {
    const char *language=getenv("KYOEI_LANGUAGE");
    if (!language || strcmp(language,"en")) return;
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
