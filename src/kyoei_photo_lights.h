/* Opt-in native-light research. SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdint.h>
#define KYOEI_PHOTO_LIGHT_COUNT 3
#ifdef __cplusplus
extern "C" {
#endif
void kyoei_photo_lights_configure(void *image,uint64_t size);
int kyoei_photo_lights_available(void);
/* type: 0 point, 1 spot. Orientation and cone angle in degrees. */
void kyoei_photo_lights_input(unsigned slot,int enabled,float red,float green,float blue,float x,float y,float z,int type,float yaw,float pitch,float cone);
#ifdef __cplusplus
}
#endif
