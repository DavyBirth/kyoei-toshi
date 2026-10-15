#include "gz.h"
#include "kyoei_name_model.h"
#include <stdio.h>
#include <stdarg.h>
typedef int32_t (ABI *Convert)(uint64_t *,char *,uint64_t,const uint16_t *,uint64_t);
void kyoei_names_test_bind(const uint16_t *,const uint16_t *,Convert);
int32_t kyoei_names_test_convert(uint64_t *,char *,uint64_t,const uint16_t *,uint64_t);
const uint16_t *kyoei_names_test_source(const uint16_t *,uintptr_t,uint16_t *);
void gz_log(const char *format,...) {(void)format;}
static const uint16_t *seen;
static ABI int32_t converter(uint64_t *written,char *output,uint64_t capacity,const uint16_t *source,uint64_t count) {
    if(!written || !output || capacity!=20 || count!=19)return -1;
    seen=source;*written=7;output[0]='!';return 23;
}
#define CHECK(x) do { if(!(x)) {fprintf(stderr,"Names failed at %d\n",__LINE__);return 1;} } while(0)
int main(void) {
    for(size_t i=0;i<4;++i) {
        const KyoeiNameAlias *alias=&kyoei_name_aliases[i];KyoeiNameModel m;
        CHECK(kyoei_name_begin(&m,alias->original,alias->original_length,4));
        kyoei_name_show_roman(&m);
        CHECK(m.length==alias->roman_length && !memcmp(m.text,alias->roman,(m.length+1)*2));
        CHECK(kyoei_name_can_accept(&m));
        uint16_t result[7]={0,0,0,0,0,0xaaaa,0xbbbb};
        CHECK(kyoei_name_export(&m,result,5));
        CHECK(!memcmp(result,alias->original,(alias->original_length+1)*2));
        CHECK(result[5]==0xaaaa && result[6]==0xbbbb);
        const uint16_t custom[]={'D','a','v','y'};
        CHECK(kyoei_name_insert(&m,custom,4));CHECK(kyoei_name_can_accept(&m));
        CHECK(kyoei_name_export(&m,result,5) && result[0]=='D' && result[3]=='y' && !result[4]);
    }
    KyoeiNameModel m;const KyoeiNameAlias *female=&kyoei_name_aliases[2];
    CHECK(kyoei_name_begin(&m,female->original,3,4));kyoei_name_show_roman(&m);
    uint16_t five[]={'J','a','m','e','s'};CHECK(kyoei_name_insert(&m,five,5));
    CHECK(!kyoei_name_can_accept(&m));kyoei_name_buttons(&m,8);CHECK(!m.result);
    uint16_t out[32]={0};CHECK(!kyoei_name_export(&m,out,32));
    const uint16_t *first=kyoei_name_aliases[2].original,*last=kyoei_name_aliases[3].original;
    kyoei_names_test_bind(first,last,converter);
    uint64_t written=0;char bytes[22]={0};bytes[20]='A';bytes[21]='B';
    CHECK(kyoei_names_test_convert(&written,bytes,20,first,19)==23 && written==7 && bytes[0]=='!');
    CHECK(!memcmp(seen,kyoei_name_aliases[2].roman,14));
    CHECK(bytes[20]=='A' && bytes[21]=='B');
    CHECK(kyoei_names_test_convert(&written,bytes,20,five,19)==23 && seen==five);
    uint16_t combined[10]={last[0],last[1],' ',first[0],first[1],first[2],0};
    CHECK(kyoei_names_test_source(combined,0x4321,out)==combined);
    CHECK(kyoei_names_test_source(combined,0x1234,out)==out);
    const uint16_t expected[]={'M','a','t','s','u','b','a','r','a',' ','M','i','h','a','r','u',0};
    CHECK(!memcmp(out,expected,sizeof(expected)));
    combined[2]='-';CHECK(kyoei_names_test_source(combined,0x1234,out)==combined);
    puts("NAMES PASS: complete romaji, unchanged native storage, custom names, bounds, scoped conversion and ABI delegation");return 0;
}
