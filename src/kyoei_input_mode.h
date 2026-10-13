/* Input activity, independent of controller connection. SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef KYOEI_INPUT_MODE_H
#define KYOEI_INPUT_MODE_H
#include <stdint.h>
#include <string.h>
enum { KYOEI_INPUT_KEYBOARD, KYOEI_INPUT_CONTROLLER };
typedef struct { unsigned mode; unsigned char axis_engaged[6]; } KyoeiInputMode;
static inline void kyoei_input_model_keyboard(KyoeiInputMode *m) { m->mode=KYOEI_INPUT_KEYBOARD; }
static inline void kyoei_input_model_button(KyoeiInputMode *m) { m->mode=KYOEI_INPUT_CONTROLLER; }
static inline void kyoei_input_model_axis(KyoeiInputMode *m, unsigned axis, int value) {
    if (axis>=6) return;
    /* Sticks are signed, triggers only activate on positive travel. */
    int magnitude=axis<4?(value<0?-value:value):(value>0?value:0);
    if (magnitude<=4096) m->axis_engaged[axis]=0;
    else if (magnitude>=12000 && !m->axis_engaged[axis]) {
        m->axis_engaged[axis]=1; m->mode=KYOEI_INPUT_CONTROLLER;
    }
}
static inline void kyoei_input_model_release(KyoeiInputMode *m) {
    memset(m->axis_engaged,0,sizeof(m->axis_engaged));
}
#ifdef __cplusplus
extern "C" {
#endif
void kyoei_input_keyboard(void);
void kyoei_input_controller_button(void);
void kyoei_input_controller_axis(unsigned axis, int value);
void kyoei_input_release(void);
unsigned kyoei_input_current_mode(void);
#ifdef __cplusplus
}
#endif
#endif
