#pragma once
#include <windows.h>
#include <stdint.h>

void kyoei_capture_failure(const char *reason, EXCEPTION_POINTERS *exception);
int32_t __attribute__((sysv_abi)) kyoei_render_debug_status(void);
