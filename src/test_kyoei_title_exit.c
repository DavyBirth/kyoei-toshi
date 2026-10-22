#include "kyoei_title_exit.h"
#include <stdio.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Title exit failed: %d\n",__LINE__);return 1;}}while(0)
int main(void){
    CHECK(!kyoei_title_exit_state()&&!kyoei_title_exit_input(0x40,128));
    kyoei_title_exit_test_frame(2,1000);
    CHECK(kyoei_title_exit_state()==KYOEI_TITLE_EXIT_IDLE);
    CHECK(!kyoei_title_exit_input(0x40,128)&&!kyoei_title_exit_input(0,128));          /* game moves 2 -> 3 */
    kyoei_title_exit_test_frame(3,1010);
    CHECK(kyoei_title_exit_input(0,255)&&kyoei_title_exit_state()==KYOEI_TITLE_EXIT_SELECTED); /* stick down */
    CHECK(kyoei_title_exit_input(0,128));                                               /* consumed while ours */
    CHECK(kyoei_title_exit_input(0x10,128)&&kyoei_title_exit_state()==KYOEI_TITLE_EXIT_IDLE);  /* up returns */
    CHECK(kyoei_title_exit_input(0x10,128));                                            /* held: hidden from game */
    CHECK(!kyoei_title_exit_input(0,128));                                              /* released */
    CHECK(kyoei_title_exit_input(0x40,128)&&kyoei_title_exit_input(0,128));
    CHECK(!kyoei_title_exit_test_quit_requested());
    CHECK(kyoei_title_exit_input(0x2000,128)&&kyoei_title_exit_test_quit_requested());  /* native confirm quits */
    CHECK(kyoei_title_exit_take_quit()&&!kyoei_title_exit_take_quit());
    kyoei_title_exit_test_clock(1500);
    CHECK(!kyoei_title_exit_state()&&!kyoei_title_exit_input(0x2000,128));
    kyoei_title_exit_test_frame(3,2000);
    KyoeiTitleItem item; CHECK(kyoei_title_exit_item(&item)&&item.alpha==1);
    puts("TITLE EXIT PASS: native cursor handoff, input consumption, native confirm and timeout");return 0;
}
