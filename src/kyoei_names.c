/* Narrow presentation adapter: only the verified protagonist fields and
 * combined-name call site. Never change guest fields or the saved data. */
/* SPDX-License-Identifier: GPL-2.0-or-later */
#include "gz.h"
#include "kyoei_names.h"
#include "kyoei_name_alias.h"
#include <stdlib.h>
#include <string.h>
typedef int32_t (ABI *Convert)(uint64_t *,char *,uint64_t,const uint16_t *,uint64_t);
static Convert original;
static const uint16_t *first,*last;
static uintptr_t combined_return;

static size_t field_length(const uint16_t *text) {
    size_t n=0;while(n<5 && text[n])++n;return n;
}
static const uint16_t *roman_field(const uint16_t *text) {
    const KyoeiNameAlias *alias=kyoei_name_alias_find(text,field_length(text));
    return alias?alias->roman:text;
}
static const uint16_t *display_source(const uint16_t *source,uintptr_t caller,uint16_t combined[32]) {
    if(source==first || source==last)return roman_field(source);
    if(caller!=combined_return || !source || !first || !last)return source;
    size_t fn=field_length(first),ln=field_length(last);
    if(fn>4 || ln>4 || memcmp(source,last,ln*2) || source[ln]!=' ' ||
        memcmp(source+ln+1,first,(fn+1)*2))return source;
    const uint16_t *a=roman_field(last),*b=roman_field(first);
    size_t n=0;
    while(*a && n<15)combined[n++]=*a++;
    combined[n++]=' ';
    while(*b && n<30)combined[n++]=*b++;
    combined[n]=0;return combined;
}
static ABI int32_t convert_name(uint64_t *written,char *output,uint64_t capacity,const uint16_t *source,uint64_t count) {
    uint16_t combined[32];
    const uint16_t *display=display_source(source,(uintptr_t)__builtin_return_address(0),combined);
    return original(written,output,capacity,display,count);
}
void kyoei_names_configure(void *image,uint64_t bytes,uintptr_t expected_converter) {
    const char *language=getenv("KYOEI_LANGUAGE");
    if(!language || strcmp(language,"en"))return;
    unsigned char *base=image;
    const unsigned char thunk[]={0xff,0x25,0x62,0x97,0x62,0x00};
    const unsigned char field_load[]={0x48,0x8d,0x0d,0xff,0x09,0xdf,0x00};
    uintptr_t converter=0;
    if(!base || bytes<0x13bdaa0 || memcmp(base+0xcce350,thunk,sizeof(thunk)) ||
        memcmp(base+0x5cd086,field_load,sizeof(field_load))) {
        gz_log("Kyoei names: image signature differs; original presentation retained");return;
    }
    memcpy(&converter,base+0x12f7ab8,8);
    if(converter!=expected_converter || converter<(uintptr_t)base || converter>=(uintptr_t)base+bytes) {
        gz_log("Kyoei names: native converter binding differs; original presentation retained");return;
    }
    first=(const uint16_t *)(base+0x13bda8c);last=(const uint16_t *)(base+0x13bda96);
    combined_return=(uintptr_t)base+0xac4e64;
    original=(Convert)converter;
    converter=(uintptr_t)convert_name;memcpy(base+0x12f7ab8,&converter,8);
    gz_log("Kyoei names: romaji presentation for verified protagonist fields; original storage retained");
}
#ifdef KYOEI_NAMES_TEST
void kyoei_names_test_bind(const uint16_t *f,const uint16_t *l,Convert converter) {
    first=f;last=l;original=converter;combined_return=0x1234;
}
int32_t kyoei_names_test_convert(uint64_t *w,char *o,uint64_t c,const uint16_t *s,uint64_t n) {
    return convert_name(w,o,c,s,n);
}
const uint16_t *kyoei_names_test_source(const uint16_t *s,uintptr_t caller,uint16_t *out) {
    return display_source(s,caller,out);
}
#endif
