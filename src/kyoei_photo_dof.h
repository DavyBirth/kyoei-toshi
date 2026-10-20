/* Native depth-of-field override, photo preview only.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void kyoei_photo_dof_configure(void *image,uint64_t size);
int kyoei_photo_dof_available(void);
/* 0 keeps original, 1 manual focus/aperture, 2 disables blur in photo only. */
void kyoei_photo_dof_input(int mode,float distance,float aperture,float fov);
#ifdef __cplusplus
}
#endif
