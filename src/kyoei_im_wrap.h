#pragma once
#include <stddef.h>
#include <stdint.h>
/* Widths are in native full-width cells (the game's character counts). */
void kyoei_im_set_metrics(const float *equivalent);   /* 95 ASCII advances, as in KHF1 */
/* Break only between words (or after a hyphen); "…" when max_lines is exceeded. 0 = unchanged. */
size_t kyoei_im_rewrap(const char *text, float cells, unsigned max_lines, char *out, size_t cap);
/* Copy that fits in cells, cut at a whole word plus "…" when needed. 0 = leave to native. */
size_t kyoei_im_shorten(const char *text, float cells, char *out, size_t cap);
void kyoei_im_wrap_configure(void *image, uint64_t size);
