/* Window and native IME threads share activity through this lock. */
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "kyoei_input_mode.h"
static SRWLOCK mode_lock=SRWLOCK_INIT;
static KyoeiInputMode mode_state;
void kyoei_input_keyboard(void) {
    AcquireSRWLockExclusive(&mode_lock); kyoei_input_model_keyboard(&mode_state); ReleaseSRWLockExclusive(&mode_lock);
}
void kyoei_input_controller_button(void) {
    AcquireSRWLockExclusive(&mode_lock); kyoei_input_model_button(&mode_state); ReleaseSRWLockExclusive(&mode_lock);
}
void kyoei_input_controller_axis(unsigned axis, int value) {
    AcquireSRWLockExclusive(&mode_lock); kyoei_input_model_axis(&mode_state,axis,value); ReleaseSRWLockExclusive(&mode_lock);
}
void kyoei_input_release(void) {
    AcquireSRWLockExclusive(&mode_lock); kyoei_input_model_release(&mode_state); ReleaseSRWLockExclusive(&mode_lock);
}
unsigned kyoei_input_current_mode(void) {
    AcquireSRWLockShared(&mode_lock); unsigned result=mode_state.mode; ReleaseSRWLockShared(&mode_lock); return result;
}
static volatile LONG family=KYOEI_PAD_PLAYSTATION;
void kyoei_input_controller_playstation(int playstation) {
    InterlockedExchange(&family,playstation?KYOEI_PAD_PLAYSTATION:KYOEI_PAD_XBOX);
}
unsigned kyoei_input_current_family(void) {
    static int forced=-2;
    if (forced==-2) {
        const char *style=getenv("KYOEI_CONTROLLER_STYLE");
        forced=style && !strcmp(style,"playstation")?KYOEI_PAD_PLAYSTATION:style && !strcmp(style,"xbox")?KYOEI_PAD_XBOX:-1;
    }
    return forced>=0?(unsigned)forced:(unsigned)family;
}
