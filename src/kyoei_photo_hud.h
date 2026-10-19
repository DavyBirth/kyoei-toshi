/* SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void kyoei_photo_hud_configure(void *image,uint64_t size);
int kyoei_photo_hud_available(void);
int kyoei_photo_hud_hidden(void);
int kyoei_photo_hud_toggle(void);
void kyoei_photo_hud_restore(void);
uint64_t kyoei_photo_hud_observed_calls(void);
#ifdef __cplusplus
}
#endif
