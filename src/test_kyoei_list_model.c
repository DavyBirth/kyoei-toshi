#include "kyoei_list_model.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); exit(1); } } while(0)
int main(void) {
    CHECK(kyoei_list_next(0,0,0x40)==0);
    CHECK(kyoei_list_next(1,0,0x10)==0 && kyoei_list_next(1,0,0x40)==0);
    CHECK(kyoei_list_next(3,0,0x10)==2 && kyoei_list_next(3,2,0x40)==0);
    CHECK(kyoei_list_next(3,1,0x2000)==1 && kyoei_list_next(3,1,0x4000)==1);
    CHECK(kyoei_list_next(3,1,0x10|0x40)==0);
    CHECK(kyoei_list_next(3,99,0x40)==0);
    KyoeiListView view=kyoei_list_view(1025,1000,340);
    CHECK(view.first==995 && view.rows==6);
    view=kyoei_list_view(1025,1024,340); CHECK(view.first==1019 && view.rows==6);
    view=kyoei_list_view(3,1,600); CHECK(!view.first && view.rows==3);
    for (int height=-100;height<190;++height) CHECK(!kyoei_list_view(3,1,height).rows);
    CHECK(!kyoei_list_view(0,0,600).rows && !kyoei_list_view(3,99,600).rows);
    puts("List model PASS: navigation, wrap, empty/invalid lists and viewport bounds"); return 0;
}
