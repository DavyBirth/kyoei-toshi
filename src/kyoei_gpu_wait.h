#pragma once
#include <stddef.h>
#include <stdint.h>

void kyoei_gpu_wait_configure(void *image, uint64_t size);
int kyoei_wait_real_label(const volatile uint64_t *label, uint64_t expected, unsigned timeout_ms);
size_t kyoei_build_wait_relay(unsigned char *relay, const void *callback, const unsigned char *original);
