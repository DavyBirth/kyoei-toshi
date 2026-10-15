/* Default-name presentation only; original UTF-16 storage remains unchanged. */
/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef KYOEI_NAME_ALIAS_H
#define KYOEI_NAME_ALIAS_H
#include <stdint.h>
#include <stddef.h>
#include <string.h>
typedef struct { uint16_t original[5],roman[10]; size_t original_length,roman_length; } KyoeiNameAlias;
static const KyoeiNameAlias kyoei_name_aliases[]={
    {{0x30b1,0x30f3,0},{'K','e','n',0},2,3},
    {{0x4e09,0x5d0e,0},{'M','i','s','a','k','i',0},2,6},
    {{0x30df,0x30cf,0x30eb,0},{'M','i','h','a','r','u',0},3,6},
    {{0x677e,0x539f,0},{'M','a','t','s','u','b','a','r','a',0},2,9}
};
static inline const KyoeiNameAlias *kyoei_name_alias_find(const uint16_t *text,size_t length) {
    if (!text || length>4) return NULL;
    for (size_t i=0;i<sizeof(kyoei_name_aliases)/sizeof(*kyoei_name_aliases);++i) {
        const KyoeiNameAlias *alias=&kyoei_name_aliases[i];
        if (length==alias->original_length && !memcmp(text,alias->original,length*2)) return alias;
    }
    return NULL;
}
#endif
