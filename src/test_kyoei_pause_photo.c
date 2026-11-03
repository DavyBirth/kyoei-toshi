#include "kyoei_pause_photo.h"
#include <assert.h>
#include <stdio.h>
static int input(unsigned value,unsigned stick){uint32_t b=value;return kyoei_pause_photo_input(&b,stick);}
int main(void){
    kyoei_pause_photo_test_frame(4,900);kyoei_pause_photo_cancel();assert(!input(0x2000,128));input(0,128);
    kyoei_pause_photo_test_frame(5,1000);assert(!input(0,128));assert(input(0x40,128));assert(kyoei_pause_photo_selected());
    assert(input(0,128));assert(input(0x10,128));assert(!kyoei_pause_photo_selected());
    input(0,128);assert(input(0x40,128));input(0,128);assert(input(0x2000,128));
    uint32_t b=0;assert(!kyoei_pause_photo_input(&b,128)&&b==8);
    kyoei_pause_photo_test_scene(0,0,1);assert(!kyoei_pause_photo_take_entry());
    kyoei_pause_photo_test_scene(1,1,1);assert(!kyoei_pause_photo_take_entry());
    kyoei_pause_photo_test_scene(1,0,1);assert(kyoei_pause_photo_take_entry());assert(!kyoei_pause_photo_take_entry());
    input(0,128);kyoei_pause_photo_test_frame(5,2000);kyoei_pause_photo_request();
    kyoei_pause_photo_test_scene(1,0,0);assert(!kyoei_pause_photo_take_entry());
    input(0,128);kyoei_pause_photo_test_frame(5,3000);kyoei_pause_photo_request();
    kyoei_pause_photo_test_clock(8001);assert(!kyoei_pause_photo_take_entry());
    assert(!kyoei_pause_photo_visible());
    puts("PAUSE PHOTO PASS: seventh row, return to load, normal close, foreign pause, scene change, timeout");
}
