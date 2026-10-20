/* Opt-in photo scene preview. SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void kyoei_photo_scene_configure(void *image,uint64_t size);
int kyoei_photo_scene_available(void);
int kyoei_photo_scene_active(void);
/* SDL host input only; axes in[-1,1], look in radians, speed in[.1,10].
 * Moves are latched; look/fov impulses are accumulated until guest consumes.
 * Focus loss must restore the HUD request even without a new key event. */
void kyoei_photo_scene_input(float right,float up,float forward,float yaw,float pitch,float speed);
void kyoei_photo_scene_fov(float delta_degrees);
#ifdef __cplusplus
}
#endif
