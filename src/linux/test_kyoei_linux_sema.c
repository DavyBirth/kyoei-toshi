/* Include derived code to observe waiter registration deterministically.
 * SPDX-License-Identifier: GPL-2.0-or-later */
#include "linux_sema.c"
#include <assert.h>
#include <sched.h>
#include <stdio.h>

void gz_log(const char *format, ...) { (void)format; }
void gz_stop(int code, const char *format, ...) { (void)format; exit(code); }
typedef struct { uint32_t id; EventFlag *event; int32_t result; } Task;
static void *wait_sema(void *arg) {
    Task *task = arg;
    task->result = sem_wait(task->id, 1, NULL);
    return NULL;
}
static void *wait_event(void *arg) {
    Task *task = arg;
    task->result = evf_wait(task->event, 3, WAIT_AND, NULL, NULL);
    return NULL;
}
static void await_registration(Task *task) {
    uint64_t deadline = host_milliseconds() + 2000;
    for (;;) {
        int ready;
        if (task->event) {
            host_lock(&task->event->lock);
            ready = task->event->waiting == 1;
            host_unlock(&task->event->lock);
        } else {
            host_lock(&sema_lock);
            ready = find(task->id)->active == 1;
            host_unlock(&sema_lock);
        }
        if (ready) return;
        assert(host_milliseconds() < deadline);
        sched_yield();
    }
}
int main(void) {
    uint32_t id;
    assert(sem_create(&id, "invalid", 0, 2, 1, NULL) == GZ_ERR(GZ_EINVAL));
    assert(!sem_create(&id, "tokens", 0, 1, 2, NULL));
    assert(!sem_poll(id, 1));
    assert(sem_poll(id, 1) == GZ_ERR(GZ_EBUSY));
    uint32_t timeout = 1000;
    assert(sem_wait(id, 1, &timeout) == GZ_ERR(GZ_ETIMEDOUT) && !timeout);
    Task task = {.id = id}; pthread_t thread;
    assert(!pthread_create(&thread, NULL, wait_sema, &task)); await_registration(&task);
    assert(!sem_signal(id, 1)); assert(!pthread_join(thread, NULL) && !task.result);
    assert(!pthread_create(&thread, NULL, wait_sema, &task)); await_registration(&task);
    int32_t waiters = 0;
    assert(!sem_cancel(id, 0, &waiters) && waiters == 1);
    assert(!pthread_join(thread, NULL) && task.result == GZ_ERR(85));
    assert(!pthread_create(&thread, NULL, wait_sema, &task)); await_registration(&task);
    assert(!sem_delete(id));
    assert(!pthread_join(thread, NULL) && task.result == GZ_ERR(13));
    assert(sem_poll(id, 1) == GZ_ERR(GZ_ESRCH));
    EventFlag *event;
    assert(!evf_create(&event, "event", EVF_MULTI, 0, NULL));
    uint64_t bits;
    assert(evf_poll(event, 3, WAIT_AND, &bits) == GZ_ERR(GZ_EBUSY));
    assert(!evf_set(event, 7));
    assert(!evf_poll(event, 3, WAIT_AND | CLEAR_PAT, &bits) && bits == 7);
    assert(!evf_clear(event, 0));
    timeout = 1000;
    assert(evf_wait(event, 3, WAIT_OR, NULL, &timeout) == GZ_ERR(GZ_ETIMEDOUT) && !timeout);
    task.event = event;
    assert(!pthread_create(&thread, NULL, wait_event, &task)); await_registration(&task);
    assert(!evf_set(event, 3)); assert(!pthread_join(thread, NULL) && !task.result);
    assert(!evf_clear(event, 0));
    assert(!pthread_create(&thread, NULL, wait_event, &task)); await_registration(&task);
    assert(!evf_cancel(event, 0, &waiters) && waiters == 1);
    assert(!pthread_join(thread, NULL) && task.result == GZ_ERR(85));
    assert(!pthread_create(&thread, NULL, wait_event, &task)); await_registration(&task);
    assert(!evf_delete(event));
    assert(!pthread_join(thread, NULL) && task.result == GZ_ERR(13));
    assert(gz_resolve("libkernel", "sceKernelWaitSema", 0) == (uintptr_t)sem_wait);
    puts("LINUX SEMA PASS: tokens, timeout, signal, cancel/delete with blocked threads; event AND/OR/clear");
    return 0;
}
