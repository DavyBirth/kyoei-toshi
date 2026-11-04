#pragma once
#include <stdint.h>
/* Turn limit multiplier while the mouse is used; the mouse gain is divided by
 * the same factor over the original x6 so slow motion keeps its precision. */
#define KYOEI_MOUSE_CAMERA_SCALE 24.f
#ifdef __cplusplus
extern "C" {
#endif
void kyoei_mouse_camera_configure(void *image, uint64_t size);
/* Call once per pad sample: nonzero while the mouse drives the camera. */
void kyoei_mouse_camera_boost(int mouse);
#ifdef __cplusplus
}
#endif
