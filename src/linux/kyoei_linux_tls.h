/* Linux host TLS adapter. Guest FS loads become GS loads; libc retains FS.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <stdint.h>
#include <stddef.h>
int kyoei_linux_tls_configure(const void *image,uint64_t image_size,const uint64_t layout[4]);
int kyoei_linux_tls_enter(uintptr_t guest_thread);
int kyoei_linux_tls_leave(void);
void *kyoei_linux_tls_tcb(void);
int kyoei_linux_tls_patch(unsigned char *image,uint64_t image_size,const uint64_t *sites,size_t count);
