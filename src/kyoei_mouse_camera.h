#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void kyoei_mouse_camera_configure(void *image, uint64_t size);
/* Call once per pad sample: nonzero while the mouse drives the camera. */
void kyoei_mouse_camera_boost(int mouse);
#ifdef __cplusplus
}
#endif
