/* Mount2 uses the same backing folders and metadata as the fixed Mount API. */
/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include <stddef.h>
#include <string.h>
typedef struct { int32_t user, pad; const void *directory; uint64_t blocks;
    uint32_t mode; unsigned char reserved[32]; int32_t pad2; } Mount2;
typedef struct { int32_t user, pad; const void *title, *directory, *fingerprint;
    uint64_t blocks; uint32_t mode; unsigned char reserved[32]; } Mount1;
_Static_assert(sizeof(Mount2)==64 && offsetof(Mount2,mode)==24, "Mount2 ABI");
_Static_assert(sizeof(Mount1)==80 && offsetof(Mount1,mode)==40, "Mount ABI");
uintptr_t kyoei_shared_resolve(const char *, const char *, int);
static ABI int32_t mount2(const Mount2 *input, void *result) {
    if (!input) return (int32_t)0x809f0000;
    Mount1 converted = {0};
    converted.user = input->user; converted.directory = input->directory;
    converted.blocks = input->blocks; converted.mode = input->mode;
    typedef int32_t (ABI *Mount)(const Mount1 *, void *);
    Mount original = (Mount)kyoei_shared_resolve("libSceSaveData", "sceSaveDataMount", 0);
    if (!original) gz_stop(21, "STOP: Mount2 backing implementation missing");
    return original(&converted, result);
}
uintptr_t kyoei_savedata_resolve(const char *symbol) {
    return !strcmp(symbol,"sceSaveDataMount2") ? (uintptr_t)mount2 : 0;
}
