#include "kyoei_title_exit.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Title exit failed: %d\n",__LINE__);return 1;}}while(0)
int kyoei_title_exit_hit(float x,float y){(void)x;(void)y;return 0;}
int main(void){
    CHECK(!kyoei_title_exit_state()&&!kyoei_title_exit_input(0x40,128));
    kyoei_title_exit_test_frame(2,1000);
    CHECK(kyoei_title_exit_state()==KYOEI_TITLE_EXIT_IDLE);
    CHECK(!kyoei_title_exit_input(0x40,128)&&!kyoei_title_exit_input(0,128));
    kyoei_title_exit_test_frame(3,1010);
    CHECK(kyoei_title_exit_input(0,255)&&kyoei_title_exit_state()==KYOEI_TITLE_EXIT_SELECTED);
    CHECK(kyoei_title_exit_input(0,128));
    CHECK(kyoei_title_exit_input(0x10,128)&&kyoei_title_exit_state()==KYOEI_TITLE_EXIT_IDLE);
    CHECK(!kyoei_title_exit_input(0,128));
    CHECK(kyoei_title_exit_input(0x40,128)&&kyoei_title_exit_input(0,128));
    CHECK(kyoei_title_exit_input(0x2000,128)&&kyoei_title_exit_state()==KYOEI_TITLE_EXIT_CONFIRM);
    CHECK(kyoei_title_exit_input(0,128)&&kyoei_title_exit_input(0x4000,128)&&kyoei_title_exit_state()==KYOEI_TITLE_EXIT_SELECTED);
    CHECK(!kyoei_title_exit_test_quit_requested());
    CHECK(kyoei_title_exit_input(0,128)&&kyoei_title_exit_input(0x2000,128)&&kyoei_title_exit_input(0,128));
    CHECK(kyoei_title_exit_input(0x2000,128)&&kyoei_title_exit_test_quit_requested());
    CHECK(kyoei_title_exit_take_quit()&&!kyoei_title_exit_take_quit());
    kyoei_title_exit_test_clock(1500);
    CHECK(!kyoei_title_exit_state()&&!kyoei_title_exit_input(0x2000,128));
    kyoei_title_exit_test_frame(3,2000);
    CHECK(kyoei_title_exit_click()&&kyoei_title_exit_state()==KYOEI_TITLE_EXIT_CONFIRM);
    puts("TITLE EXIT PASS: native cursor handoff, input consumption, confirm/back, timeout and click");return 0;
}
