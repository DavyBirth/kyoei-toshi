#include "kyoei_prompt_state.h"
#include <stdatomic.h>
#include <stdint.h>
#ifdef _WIN32
#include <windows.h>
static uint64_t now_ms(void) { return GetTickCount64()+1; }
#else
#include <time.h>
static uint64_t now_ms(void) { struct timespec t;clock_gettime(CLOCK_MONOTONIC,&t);return (uint64_t)t.tv_sec*1000+(uint64_t)t.tv_nsec/1000000+1; }
#endif
static _Atomic uint64_t pause_seen, status_seen;
void kyoei_prompt_pause_seen(void) { atomic_store(&pause_seen,now_ms()); }
void kyoei_prompt_status_seen(void) { atomic_store(&status_seen,now_ms()); }
void kyoei_prompt_reset(void) { atomic_store(&pause_seen,0);atomic_store(&status_seen,0); }
void kyoei_prompt_presented(void) {} /* Duplicate presents must not clear a live guest frame. */
static int recent(_Atomic uint64_t *stamp) { uint64_t t=atomic_load(stamp);return t && now_ms()-t<200; }
int kyoei_prompt_pause_active(void) { return recent(&pause_seen); }
int kyoei_prompt_pause_root(void) { return recent(&pause_seen) && recent(&status_seen); }
