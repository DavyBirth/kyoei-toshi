/* Host helpers for portable adaptations of Kyoei's module manager. */
#pragma once
#include "runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
static inline void gz_log(const char *format,...) {
    va_list ap;va_start(ap,format);vprintf(format,ap);va_end(ap);putchar('\n');
}
__attribute__((noreturn)) static inline void gz_stop(int code,const char *format,...) {
    va_list ap;va_start(ap,format);vfprintf(stderr,format,ap);va_end(ap);fputc('\n',stderr);fflush(NULL);_Exit(code);
}
