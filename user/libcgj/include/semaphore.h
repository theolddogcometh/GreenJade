/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <semaphore.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * sem_init/destroy/wait/trywait/timedwait/post/getvalue for unnamed
 * semaphores; named sem_open path when wired.
 *
 * Design notes
 * ------------
 * Process-private unnamed semaphores are the bring-up priority (futex).
 * SEM_FAILED is the named-open error sentinel.
 *
 * Non-goals
 * ---------
 * Full POSIX named semaphore persistence across reboots.
 */
#pragma once

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    volatile int nValue;
    int          nPshared;
    int          nMagic;
    int          nPad;
} sem_t;

#define SEM_FAILED ((sem_t *)0)

int    sem_init(sem_t *pSem, int nPshared, unsigned uValue);
int    sem_destroy(sem_t *pSem);
int    sem_wait(sem_t *pSem);
int    sem_trywait(sem_t *pSem);
int    sem_timedwait(sem_t *pSem, const struct timespec *pAbs);
int    sem_clockwait(sem_t *pSem, clockid_t clockId, const struct timespec *pAbs);
int    sem_post(sem_t *pSem);
int    sem_getvalue(sem_t *pSem, int *pSval);

/* Named semaphores — bring-up ENOSYS */
sem_t *sem_open(const char *szName, int nFlags, ...);
int    sem_close(sem_t *pSem);
int    sem_unlink(const char *szName);

#ifdef __cplusplus
}
#endif
