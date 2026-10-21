/* Opt-in native-light research. SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void kyoei_photo_lights_configure(void *image,uint64_t size);
int kyoei_photo_lights_available(void);
void kyoei_photo_lights_input(int enabled,float red,float green,float blue,float x,float y,float z);
#ifdef __cplusplus
}
#endif
