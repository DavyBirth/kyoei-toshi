#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
void kyoei_pause_photo_configure(void *image,uint64_t size);
int kyoei_pause_photo_input(uint32_t *buttons,unsigned stick_y);
int kyoei_pause_photo_visible(void);
int kyoei_pause_photo_selected(void);
void kyoei_pause_photo_request(void);
void kyoei_pause_photo_cancel(void);
int kyoei_pause_photo_take_entry(void);
#ifdef KYOEI_PAUSE_PHOTO_TEST
void kyoei_pause_photo_test_frame(int cursor,uint64_t now);
void kyoei_pause_photo_test_scene(int closed,int paused,int same);
void kyoei_pause_photo_test_clock(uint64_t now);
#endif
#ifdef __cplusplus
}
#endif
