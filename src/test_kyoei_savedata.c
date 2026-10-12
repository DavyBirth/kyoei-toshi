#include "kyoei_savedata.c"
#include <stdio.h>
#include <stdlib.h>
static int calls;
static ABI int32_t backing(const Mount1 *input, void *output) {
    ++calls;
    if (input->user!=1 || input->directory!=(void*)0x1234 || input->blocks!=96 ||
        input->mode!=6 || input->title || input->fingerprint || output!=(void*)0x5678) abort();
    return (int32_t)0x809f0007;
}
uintptr_t kyoei_shared_resolve(const char *library,const char *symbol,int data) {
    if (strcmp(library,"libSceSaveData") || strcmp(symbol,"sceSaveDataMount") || data) abort();
    return (uintptr_t)backing;
}
void gz_stop(int code,const char *format,...) { (void)format; exit(code); }
int main(void) {
    Mount2 input={0}; input.user=1; input.directory=(void*)0x1234; input.blocks=96; input.mode=6;
    if (mount2(NULL,NULL)!=(int32_t)0x809f0000 || calls) return 1;
    if (mount2(&input,(void*)0x5678)!=(int32_t)0x809f0007 || calls!=1) return 1;
    puts("MOUNT2 PASS: SysV layout, directory/user/mode/blocks/output forwarded, backing errors preserved");
    return 0;
}
