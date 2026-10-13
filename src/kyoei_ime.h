/* PS4 IME parameter layout used by the Kyoei runtime adapter. */
/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef KYOEI_IME_H
#define KYOEI_IME_H
#ifdef __cplusplus
extern "C" {
#endif
#include "gz.h"
typedef int32_t (ABI *KyoeiTextFilter)(uint16_t *,uint32_t *,const uint16_t *,uint32_t);
typedef struct {
    int32_t user; uint32_t type; uint64_t languages;
    uint32_t enter_label,input_method; KyoeiTextFilter filter;
    uint32_t option,max_length; uint16_t *buffer;
    float x,y; uint32_t horizontal,vertical;
    const uint16_t *placeholder,*title; unsigned char reserved[16];
} KyoeiImeParam;
uintptr_t kyoei_ime_resolve(const char *symbol);
#ifdef __cplusplus
}
#endif
#endif
