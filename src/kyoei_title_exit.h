/* PC "Return to Desktop" entry below the four native title-menu items. */
/* SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
enum { KYOEI_TITLE_EXIT_HIDDEN, KYOEI_TITLE_EXIT_IDLE, KYOEI_TITLE_EXIT_SELECTED, KYOEI_TITLE_EXIT_CONFIRM };
void kyoei_title_exit_configure(void *image,uint64_t size);
/* Called by the pad bridge each poll; nonzero means the input was consumed. */
int kyoei_title_exit_input(uint32_t buttons,unsigned left_stick_y);
int kyoei_title_exit_state(void);
/* HUD-space (1920x1080) placement and fade of the PC item, mirroring the native items. */
typedef struct { float alpha, x, y, focus; } KyoeiTitleItem;
int kyoei_title_exit_item(KyoeiTitleItem *item);
/* Returns 1 once after the player confirmed; the window thread then quits. */
int kyoei_title_exit_take_quit(void);
#ifdef KYOEI_TITLE_EXIT_TEST
void kyoei_title_exit_test_frame(int cursor,uint64_t now_ms);
void kyoei_title_exit_test_clock(uint64_t now_ms);
int kyoei_title_exit_test_quit_requested(void);
/* Returns and clears the pending sound (1 move, 2 confirm). */
int kyoei_title_exit_test_sound(void);
#endif
#ifdef __cplusplus
}
#endif
