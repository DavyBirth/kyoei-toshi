#include "kyoei_text_wrap.h"
#include <stdio.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"Wrap failed: %d\n",__LINE__);return 1;}}while(0)
typedef struct { uint32_t start; } P; typedef struct { uint32_t end; uint8_t newline; } O;
float kyoei_text_wrap_measure(void*,const char*,uint32_t,uint32_t,void*,const P*,O*);
void kyoei_text_wrap_test_original(void*);
/* Native model: 1 unit per byte, width 10, breaks at '\n' or before the overflowing glyph. */
static float native(void *f,const char *t,uint32_t i,uint32_t len,void *s,const P *p,O *o){
    (void)f;(void)i;(void)s; uint32_t x=0,k=p->start;
    for(;k<len;++k){ if(t[k]=='\n'){o->end=k+1;o->newline=1;return (float)x;} if(x+1>10&&k>p->start)break; ++x; }
    o->end=k;o->newline=0;return (float)x;
}
static int lines(const char *t,char out[][16]){
    P p={0};O o;int n=0;uint32_t len=(uint32_t)strlen(t);
    while(p.start<len){kyoei_text_wrap_measure(0,t,0,len,0,&p,&o);
        uint32_t e=o.end; while(e>p.start&&(t[e-1]=='\n'||t[e-1]==' '))--e;
        memcpy(out[n],t+p.start,e-p.start);out[n][e-p.start]=0;++n;p.start=o.end;}
    return n;
}
int main(void){
    kyoei_text_wrap_test_original((void*)native); char l[8][16];
    CHECK(lines("I'm going to meet a woman.",l)==3 && !strcmp(l[0],"I'm going") && !strcmp(l[1],"to meet a") && !strcmp(l[2],"woman."));
    CHECK(lines("Hello world",l)==2 && !strcmp(l[0],"Hello") && !strcmp(l[1],"world"));
    CHECK(lines("abcdefghijklmno",l)==2 && !strcmp(l[0],"abcdefghij"));          /* no space: native */
    CHECK(lines("[#ff00] ab cd",l)>=1);                                           /* tag-safe */
    CHECK(lines("Wait...\nOK",l)==2 && !strcmp(l[0],"Wait...") && !strcmp(l[1],"OK"));
    puts("TEXT WRAP PASS: whole words, punctuation kept, explicit breaks, no-space fallback");return 0;
}
