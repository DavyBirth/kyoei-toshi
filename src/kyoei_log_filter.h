/* SPDX-License-Identifier: GPL-2.0-or-later */
#ifndef KYOEI_LOG_FILTER_H
#define KYOEI_LOG_FILTER_H
#include <stdlib.h>
#include <string.h>
static inline int kyoei_log_visible(const char *format) {
    /* Successful mappings are frequent during streaming. Each log otherwise
       takes a global lock, writes stdout and flushes the file synchronously.
       Errors and memory reports always remain visible. */
    if (strncmp(format,"GPU memory mapped:",18) &&
        strncmp(format,"GPU memory unmapped:",20) &&
        strncmp(format,"Runtime: mapped direct memory ",30) &&
        strncmp(format,"Runtime: mapped flexible memory ",32)) return 1;
    const char *trace=getenv("KYOEI_MEMORY_TRACE");
    return trace && trace[0]=='1';
}
#endif
