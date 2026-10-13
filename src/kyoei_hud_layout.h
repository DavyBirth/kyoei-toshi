#pragma once
#include <stddef.h>
#include <math.h>
#include <string.h>

typedef struct {
    float equivalent[95]; /* conservative native advance / native full-width advance */
} KyoeiHudMetrics;
typedef struct { float scale_x, plate_x, text_x; } KyoeiHudFit;

/* The verified marker geometry uses a 36-unit base, 26-unit text inset,
 * and mirrored positioning when its existing direction flag is set.
 * Leave unsupported formatting/long labels to the original implementation. */
static inline int kyoei_hud_fit(const KyoeiHudMetrics *metrics,const char *text,
                                size_t bytes,int mirrored,KyoeiHudFit *fit) {
    if (!metrics || !text || !fit || !bytes || bytes>115 || text[bytes]) return 0;
    float equivalent=0;
    for (size_t i=0;i<bytes;++i) {
        unsigned char ch=(unsigned char)text[i];
        if (ch<32 || ch>126 || strchr("[]<>{}%\\",ch)) return 0;
        float advance=metrics->equivalent[ch-32];
        if (!isfinite(advance) || advance<=0 || advance>4) return 0;
        equivalent+=advance;
    }
    float scale=equivalent*(29.f/36.f)+(32.f/36.f);
    if (!isfinite(scale) || scale*36>768) return 0;
    fit->scale_x=scale;
    fit->plate_x=mirrored?24-36*scale:24;
    fit->text_x=mirrored?fit->plate_x+8:50;
    return 1;
}
