#include "kyoei_hud_layout.h"
#include <stdio.h>
int main(void) {
    KyoeiHudMetrics metrics;
    for (int i=0;i<95;++i) metrics.equivalent[i]=.6f;
    metrics.equivalent['i'-32]=.2f; metrics.equivalent['W'-32]=1.f;
    KyoeiHudFit narrow,wide,mirror,unchanged={1,2,3};
    if (!kyoei_hud_fit(&metrics,"iiii",4,0,&narrow) || !kyoei_hud_fit(&metrics,"WWWW",4,0,&wide)) return 1;
    if (!(wide.scale_x>narrow.scale_x) || narrow.text_x!=50) return 2;
    if (!kyoei_hud_fit(&metrics,"WWWW",4,1,&mirror) || fabsf(mirror.scale_x-wide.scale_x)>.001f ||
        fabsf(mirror.plate_x+36*mirror.scale_x-24)>.001f || mirror.text_x!=mirror.plate_x+8) return 3;
    const char *long_label="Listen to their conversation";
    if (!kyoei_hud_fit(&metrics,long_label,strlen(long_label),0,&wide) || wide.scale_x*36>768) return 4;
    if (kyoei_hud_fit(&metrics,"[i4]",4,0,&unchanged) || unchanged.plate_x!=2) return 5;
    if (kyoei_hud_fit(&metrics,"bad\nlabel",9,0,&wide) || kyoei_hud_fit(&metrics,"",0,0,&wide)) return 6;
    metrics.equivalent['W'-32]=NAN;
    if (kyoei_hud_fit(&metrics,"WWWW",4,0,&wide)) return 7;
    metrics.equivalent['W'-32]=4;
    char oversized[116]; memset(oversized,'W',115);oversized[115]=0;
    if (kyoei_hud_fit(&metrics,oversized,115,0,&wide)) return 8;
    float measured=0;
    const char *punctuation="\xe2\x80\x98i\xe2\x80\x99\xe2\x80\x9c\xe2\x80\x9d\xe2\x80\xa6\xe2\x99\xaa";
    if (!kyoei_hud_measure(&metrics,punctuation,strlen(punctuation),&measured) || fabsf(measured-6.2f)>.001f) return 9;
    const char *bad[]={"\xe2\x80","\xe2\x80\x80","\xc0\xaf","\xc3\xa9","a\xe2\x80\x99%"};
    for (unsigned i=0;i<sizeof(bad)/sizeof(bad[0]);++i)
        if (kyoei_hud_measure(&metrics,bad[i],strlen(bad[i]),&measured)) return 10;
    puts("HUD LAYOUT PASS: glyph widths, full long label, mirrored anchor, formatting fallback, finite metrics and bounded span");
    return 0;
}
