#pragma once
#include <stddef.h>
#include <stdint.h>
void kyoei_hud_trace_configure(void *image, uint64_t size);
size_t kyoei_hud_trace_relay(unsigned char *relay, const void *resume, const void *callback);
