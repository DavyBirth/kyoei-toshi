/* SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <windows.h>
#include <stdint.h>
void kyoei_tls_configure(void *image, uint64_t size, const uint64_t layout[4]);
int kyoei_tls_fault(EXCEPTION_POINTERS *exception);
void *kyoei_tls_address(uint64_t offset);
void *kyoei_tls_tcb(void);
void kyoei_tls_enter(void);
unsigned kyoei_tls_patch_sites(const uint64_t *sites, unsigned count);
void kyoei_tls_optimize(const char *out);
uint64_t kyoei_tls_fault_count(void);
