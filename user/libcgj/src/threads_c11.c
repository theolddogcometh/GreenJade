/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * C11 <threads.h> over clean-room pthread. Integer-only time paths.
 */
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <threads.h>
#include <time.h>
#include <unistd.h>

struct cgj_mtx {
    pthread_mutex_t m;
};

struct cgj_cnd {
    pthread_cond_t c;
};

struct cgj_thrd_arg {
    thrd_start_t pfn;
    void        *pArg;
};

static int
map_pthread_err(int e)
{
    if (e == 0) {
        return thrd_success;
    }
    if (e == ENOMEM) {
        return thrd_nomem;
    }
    if (e == EBUSY) {
        return thrd_busy;
    }
    if (e == ETIMEDOUT) {
        return thrd_timedout;
    }
    return thrd_error;
}

void
call_once(once_flag *pFlag, void (*pfn)(void))
{
    /* once_flag.p holds pthread_once_t-shaped int state */
    static pthread_mutex_t g_onceMu = PTHREAD_MUTEX_INITIALIZER;
    volatile int *pState;

    if (pFlag == NULL || pfn == NULL) {
        return;
    }
    pState = (volatile int *)&pFlag->p;
    if (*pState == 2) {
        return;
    }
    (void)pthread_mutex_lock(&g_onceMu);
    if (*pState == 0) {
        *pState = 1;
        (void)pthread_mutex_unlock(&g_onceMu);
        pfn();
        (void)pthread_mutex_lock(&g_onceMu);
        *pState = 2;
    }
    (void)pthread_mutex_unlock(&g_onceMu);
}

int
mtx_init(mtx_t *pMtx, int nType)
{
    struct cgj_mtx *p;
    pthread_mutexattr_t a;
    int e;

    if (pMtx == NULL) {
        return thrd_error;
    }
    p = (struct cgj_mtx *)malloc(sizeof(*p));
    if (p == NULL) {
        return thrd_nomem;
    }
    (void)pthread_mutexattr_init(&a);
    if (nType & mtx_recursive) {
        (void)pthread_mutexattr_settype(&a, PTHREAD_MUTEX_RECURSIVE);
    }
    e = pthread_mutex_init(&p->m, &a);
    (void)pthread_mutexattr_destroy(&a);
    if (e != 0) {
        free(p);
        return map_pthread_err(e);
    }
    pMtx->p = p;
    return thrd_success;
}

void
mtx_destroy(mtx_t *pMtx)
{
    struct cgj_mtx *p;

    if (pMtx == NULL || pMtx->p == NULL) {
        return;
    }
    p = (struct cgj_mtx *)pMtx->p;
    (void)pthread_mutex_destroy(&p->m);
    free(p);
    pMtx->p = NULL;
}

int
mtx_lock(mtx_t *pMtx)
{
    struct cgj_mtx *p;

    if (pMtx == NULL || pMtx->p == NULL) {
        return thrd_error;
    }
    p = (struct cgj_mtx *)pMtx->p;
    return map_pthread_err(pthread_mutex_lock(&p->m));
}

int
mtx_trylock(mtx_t *pMtx)
{
    struct cgj_mtx *p;

    if (pMtx == NULL || pMtx->p == NULL) {
        return thrd_error;
    }
    p = (struct cgj_mtx *)pMtx->p;
    return map_pthread_err(pthread_mutex_trylock(&p->m));
}

int
mtx_timedlock(mtx_t *pMtx, const struct timespec *pAbs)
{
    struct cgj_mtx *p;

    if (pMtx == NULL || pMtx->p == NULL || pAbs == NULL) {
        return thrd_error;
    }
    p = (struct cgj_mtx *)pMtx->p;
    return map_pthread_err(pthread_mutex_timedlock(&p->m, pAbs));
}

int
mtx_unlock(mtx_t *pMtx)
{
    struct cgj_mtx *p;

    if (pMtx == NULL || pMtx->p == NULL) {
        return thrd_error;
    }
    p = (struct cgj_mtx *)pMtx->p;
    return map_pthread_err(pthread_mutex_unlock(&p->m));
}

int
cnd_init(cnd_t *pCnd)
{
    struct cgj_cnd *p;
    int e;

    if (pCnd == NULL) {
        return thrd_error;
    }
    p = (struct cgj_cnd *)malloc(sizeof(*p));
    if (p == NULL) {
        return thrd_nomem;
    }
    e = pthread_cond_init(&p->c, NULL);
    if (e != 0) {
        free(p);
        return map_pthread_err(e);
    }
    pCnd->p = p;
    return thrd_success;
}

void
cnd_destroy(cnd_t *pCnd)
{
    struct cgj_cnd *p;

    if (pCnd == NULL || pCnd->p == NULL) {
        return;
    }
    p = (struct cgj_cnd *)pCnd->p;
    (void)pthread_cond_destroy(&p->c);
    free(p);
    pCnd->p = NULL;
}

int
cnd_signal(cnd_t *pCnd)
{
    struct cgj_cnd *p;

    if (pCnd == NULL || pCnd->p == NULL) {
        return thrd_error;
    }
    p = (struct cgj_cnd *)pCnd->p;
    return map_pthread_err(pthread_cond_signal(&p->c));
}

int
cnd_broadcast(cnd_t *pCnd)
{
    struct cgj_cnd *p;

    if (pCnd == NULL || pCnd->p == NULL) {
        return thrd_error;
    }
    p = (struct cgj_cnd *)pCnd->p;
    return map_pthread_err(pthread_cond_broadcast(&p->c));
}

int
cnd_wait(cnd_t *pCnd, mtx_t *pMtx)
{
    struct cgj_cnd *pc;
    struct cgj_mtx *pm;

    if (pCnd == NULL || pCnd->p == NULL || pMtx == NULL || pMtx->p == NULL) {
        return thrd_error;
    }
    pc = (struct cgj_cnd *)pCnd->p;
    pm = (struct cgj_mtx *)pMtx->p;
    return map_pthread_err(pthread_cond_wait(&pc->c, &pm->m));
}

int
cnd_timedwait(cnd_t *pCnd, mtx_t *pMtx, const struct timespec *pAbs)
{
    struct cgj_cnd *pc;
    struct cgj_mtx *pm;

    if (pCnd == NULL || pCnd->p == NULL || pMtx == NULL || pMtx->p == NULL ||
        pAbs == NULL) {
        return thrd_error;
    }
    pc = (struct cgj_cnd *)pCnd->p;
    pm = (struct cgj_mtx *)pMtx->p;
    return map_pthread_err(pthread_cond_timedwait(&pc->c, &pm->m, pAbs));
}

static void *
thrd_tramp(void *pV)
{
    struct cgj_thrd_arg *pA = (struct cgj_thrd_arg *)pV;
    thrd_start_t pfn = pA->pfn;
    void *pArg = pA->pArg;
    int n;

    free(pA);
    n = pfn(pArg);
    return (void *)(intptr_t)n;
}

int
thrd_create(thrd_t *pThr, thrd_start_t pfn, void *pArg)
{
    pthread_t t;
    struct cgj_thrd_arg *pA;
    int e;

    if (pThr == NULL || pfn == NULL) {
        return thrd_error;
    }
    pA = (struct cgj_thrd_arg *)malloc(sizeof(*pA));
    if (pA == NULL) {
        return thrd_nomem;
    }
    pA->pfn = pfn;
    pA->pArg = pArg;
    e = pthread_create(&t, NULL, thrd_tramp, pA);
    if (e != 0) {
        free(pA);
        return map_pthread_err(e);
    }
    *pThr = (thrd_t)t;
    return thrd_success;
}

int
thrd_equal(thrd_t a, thrd_t b)
{
    return a == b;
}

thrd_t
thrd_current(void)
{
    return (thrd_t)pthread_self();
}

int
thrd_sleep(const struct timespec *pReq, struct timespec *pRem)
{
    return nanosleep(pReq, pRem);
}

void
thrd_yield(void)
{
    (void)sched_yield();
}

void
thrd_exit(int nRes)
{
    pthread_exit((void *)(intptr_t)nRes);
}

int
thrd_detach(thrd_t thr)
{
    return map_pthread_err(pthread_detach((pthread_t)thr));
}

int
thrd_join(thrd_t thr, int *pRes)
{
    void *pRet = NULL;
    int e;

    e = pthread_join((pthread_t)thr, &pRet);
    if (e != 0) {
        return map_pthread_err(e);
    }
    if (pRes != NULL) {
        *pRes = (int)(intptr_t)pRet;
    }
    return thrd_success;
}

int
tss_create(tss_t *pKey, tss_dtor_t pfnDtor)
{
    pthread_key_t k;
    int e;

    if (pKey == NULL) {
        return thrd_error;
    }
    e = pthread_key_create(&k, pfnDtor);
    if (e != 0) {
        return map_pthread_err(e);
    }
    *pKey = (tss_t)k;
    return thrd_success;
}

void
tss_delete(tss_t key)
{
    (void)pthread_key_delete((pthread_key_t)key);
}

int
tss_set(tss_t key, void *pVal)
{
    return map_pthread_err(pthread_setspecific((pthread_key_t)key, pVal));
}

void *
tss_get(tss_t key)
{
    return pthread_getspecific((pthread_key_t)key);
}
