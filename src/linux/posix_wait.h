/* Host synchronization for the derived kernel semaphore/event runtime.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#pragma once
#include <pthread.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <stdint.h>
#define POSIX_INFINITE UINT64_MAX

static void host_check(int error) { if (error) abort(); }
static void host_lock(pthread_mutex_t *lock) { host_check(pthread_mutex_lock(lock)); }
static void host_unlock(pthread_mutex_t *lock) { host_check(pthread_mutex_unlock(lock)); }
static void host_mutex_init(pthread_mutex_t *lock) { host_check(pthread_mutex_init(lock, NULL)); }
static void host_cond_init(pthread_cond_t *cond) {
    pthread_condattr_t attr;
    host_check(pthread_condattr_init(&attr));
    host_check(pthread_condattr_setclock(&attr, CLOCK_MONOTONIC));
    host_check(pthread_cond_init(cond, &attr));
    host_check(pthread_condattr_destroy(&attr));
}
static uint64_t host_milliseconds(void) {
    struct timespec now;
    if (clock_gettime(CLOCK_MONOTONIC, &now)) abort();
    return (uint64_t)now.tv_sec * 1000 + (uint64_t)now.tv_nsec / 1000000;
}
static void host_broadcast(pthread_cond_t *cond) { host_check(pthread_cond_broadcast(cond)); }
static void host_wait(pthread_cond_t *cond, pthread_mutex_t *lock, uint64_t ms, int flags) {
    (void)flags;
    int error;
    if (ms == POSIX_INFINITE) error = pthread_cond_wait(cond, lock);
    else {
        struct timespec deadline;
        if (clock_gettime(CLOCK_MONOTONIC, &deadline)) abort();
        uint64_t ns = (uint64_t)deadline.tv_nsec + ms % 1000 * 1000000;
        deadline.tv_sec += (time_t)(ms / 1000 + ns / 1000000000);
        deadline.tv_nsec = (long)(ns % 1000000000);
        error = pthread_cond_timedwait(cond, lock, &deadline);
    }
    if (error != ETIMEDOUT) host_check(error);
}
