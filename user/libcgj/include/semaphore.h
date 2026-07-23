/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped semaphore.h (subset). Not GNU glibc.
 * Process-private (pshared=0) via atomic+futex; pshared!=0 → ENOSYS.
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
