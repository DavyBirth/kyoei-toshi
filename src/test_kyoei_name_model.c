/* Synthetic fixtures only; no game, SDL, UI or saves. */
#include "kyoei_name_model.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"line %d: %s\n",__LINE__,#x); exit(1); } } while(0)
int main(void) {
    KyoeiNameModel m;
    const uint16_t initial[]={'D','a','v','y'}, empty[]={0};
    CHECK(kyoei_name_begin(&m,initial,4,8));
    kyoei_name_buttons(&m,0x2000); /* circle enters a, replacing selected default */
    CHECK(m.length==1 && m.text[0]=='a' && m.text[1]==0 && !m.result);
    kyoei_name_buttons(&m,0x1000); kyoei_name_buttons(&m,0x20); kyoei_name_buttons(&m,0x2000);
    CHECK(m.length==2 && m.text[1]=='B');
    kyoei_name_buttons(&m,0x8000); CHECK(m.length==1);
    kyoei_name_buttons(&m,0x4000|0x2000); CHECK(m.result==KYOEI_NAME_CANCELLED);
    kyoei_name_buttons(&m,8); CHECK(m.result==KYOEI_NAME_CANCELLED && m.length==1);
    CHECK(kyoei_name_begin(&m,empty,0,3));
    const uint16_t emoji[]={0xd83d,0xde00}, accent[]={0x00e0}, invalid[]={0xd83d};
    CHECK(kyoei_name_insert(&m,emoji,2)); CHECK(kyoei_name_insert(&m,accent,1));
    CHECK(!kyoei_name_insert(&m,accent,1)); CHECK(m.length==3 && m.text[3]==0);
    kyoei_name_cursor(&m,0); CHECK(m.cursor==2);
    kyoei_name_cursor(&m,0); CHECK(m.cursor==0);
    kyoei_name_cursor(&m,1); CHECK(m.cursor==2);
    kyoei_name_backspace(&m); CHECK(m.length==1 && m.text[0]==0x00e0 && m.cursor==0);
    CHECK(!kyoei_name_insert(&m,invalid,1));
    CHECK(!kyoei_name_begin(&m,invalid,1,8));
    CHECK(kyoei_name_begin(&m,initial,4,4));
    const uint16_t too_long[]={'a','b','c','d','e'};
    CHECK(!kyoei_name_insert(&m,too_long,5)); CHECK(m.select_all && m.length==4);
    kyoei_name_cursor(&m,0); CHECK(m.cursor==0 && !m.select_all);
    kyoei_name_backspace(&m); CHECK(m.length==4);
    CHECK(kyoei_name_begin(&m,empty,0,2048));
    for (unsigned i=0;i<2048;++i) CHECK(kyoei_name_insert(&m,accent,1));
    CHECK(!kyoei_name_insert(&m,accent,1) && m.text[2048]==0);
    kyoei_name_move(&m,-1,-1); CHECK(m.key==39);
    kyoei_name_buttons(&m,0x2000); CHECK(m.result==KYOEI_NAME_ACCEPTED);
    CHECK(!kyoei_name_insert(&m,accent,1));
    puts("Name model PASS: controller, UTF16, cursor, bounds, cancel and completion");
    return 0;
}
