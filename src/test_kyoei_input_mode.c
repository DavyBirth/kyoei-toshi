#include "kyoei_input_mode.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); exit(1); } } while(0)
int main(void) {
    KyoeiInputMode m={0};
    for (unsigned axis=0;axis<6;++axis) kyoei_input_model_axis(&m,axis,1000);
    CHECK(m.mode==KYOEI_INPUT_KEYBOARD);
    kyoei_input_model_axis(&m,0,-32768); CHECK(m.mode==KYOEI_INPUT_CONTROLLER);
    kyoei_input_model_keyboard(&m);
    kyoei_input_model_axis(&m,0,-30000); CHECK(m.mode==KYOEI_INPUT_KEYBOARD);
    kyoei_input_model_axis(&m,0,9000); CHECK(m.mode==KYOEI_INPUT_KEYBOARD);
    kyoei_input_model_axis(&m,0,100); kyoei_input_model_axis(&m,0,20000);
    CHECK(m.mode==KYOEI_INPUT_CONTROLLER);
    kyoei_input_model_keyboard(&m); kyoei_input_model_axis(&m,5,-32768);
    CHECK(m.mode==KYOEI_INPUT_KEYBOARD);
    kyoei_input_model_axis(&m,5,16000); CHECK(m.mode==KYOEI_INPUT_CONTROLLER);
    kyoei_input_model_keyboard(&m); kyoei_input_model_axis(&m,6,32767);
    CHECK(m.mode==KYOEI_INPUT_KEYBOARD);
    kyoei_input_model_button(&m); CHECK(m.mode==KYOEI_INPUT_CONTROLLER);
    kyoei_input_model_keyboard(&m); kyoei_input_model_release(&m);
    CHECK(m.mode==KYOEI_INPUT_KEYBOARD && !m.axis_engaged[0] && !m.axis_engaged[5]);
    kyoei_input_model_axis(&m,0,20000); CHECK(m.mode==KYOEI_INPUT_CONTROLLER);
    puts("Input mode PASS: activity, drift, hysteresis, held axes and triggers"); return 0;
}
