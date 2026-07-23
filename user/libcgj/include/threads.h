/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room C11 threads.h over pthread. Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <threads.h>. Constants and type sizes
 * follow Linux LP64 (x86_64 / aarch64 product) where ABI numbers matter.
 *
 * Design notes
 * ------------
 * Pure C11 headers only — no inline runtime beyond macros. Implementations
 * live under user/libcgj/src/. Symbol versions are described by libc.map
 * (GLIBC_2.* nodes) for staged libc.so.6.
 *
 * Non-goals
 * ---------
 * Full POSIX/Linux completeness; stubs and soft fills may return ENOSYS
 * until the hybrid ABI path is wired. See docs/GLIBC_COMPAT.md.
 */
#pragma once

#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef __cplusplus
#define thread_local _Thread_local
#endif

#define TSS_DTOR_ITERATIONS 4

enum {
    thrd_success  = 0,
    thrd_busy     = 1,
    thrd_error    = 2,
    thrd_nomem    = 3,
    thrd_timedout = 4
};

enum {
    mtx_plain     = 0,
    mtx_recursive = 1,
    mtx_timed     = 2
};

typedef unsigned long thrd_t;
typedef int (*thrd_start_t)(void *);

typedef struct {
    void *p;
} mtx_t;

typedef struct {
    void *p;
} cnd_t;

typedef unsigned tss_t;
typedef void (*tss_dtor_t)(void *);

typedef struct {
    void *p;
} once_flag;

#define ONCE_FLAG_INIT { 0 }

void call_once(once_flag *pFlag, void (*pfn)(void));

int  mtx_init(mtx_t *pMtx, int nType);
void mtx_destroy(mtx_t *pMtx);
int  mtx_lock(mtx_t *pMtx);
int  mtx_timedlock(mtx_t *pMtx, const struct timespec *pAbs);
int  mtx_trylock(mtx_t *pMtx);
int  mtx_unlock(mtx_t *pMtx);

int  cnd_init(cnd_t *pCnd);
void cnd_destroy(cnd_t *pCnd);
int  cnd_signal(cnd_t *pCnd);
int  cnd_broadcast(cnd_t *pCnd);
int  cnd_wait(cnd_t *pCnd, mtx_t *pMtx);
int  cnd_timedwait(cnd_t *pCnd, mtx_t *pMtx, const struct timespec *pAbs);

int  thrd_create(thrd_t *pThr, thrd_start_t pfn, void *pArg);
int  thrd_equal(thrd_t a, thrd_t b);
thrd_t thrd_current(void);
int  thrd_sleep(const struct timespec *pReq, struct timespec *pRem);
void thrd_yield(void);
void thrd_exit(int nRes);
int  thrd_detach(thrd_t thr);
int  thrd_join(thrd_t thr, int *pRes);

int  tss_create(tss_t *pKey, tss_dtor_t pfnDtor);
void tss_delete(tss_t key);
int  tss_set(tss_t key, void *pVal);
void *tss_get(tss_t key);

#ifdef __cplusplus
}
#endif
