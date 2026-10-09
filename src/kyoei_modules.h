/* SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdint.h>
void kyoei_modules_configure(const char *out, void *image, uint64_t size);
int kyoei_module_deferred(uint64_t base);
uintptr_t kyoei_modules_resolve(const char *symbol);
