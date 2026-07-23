/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch29: Solaris thr_* thread aliases over pthread,
 * libcap-shaped stubs, extra TI-RPC clnt_*, text_domain alias, rcmd
 * helpers, fopencookie cookie ops, TLS allocate stubs, obstack fail
 * handler. Integer/pointer only (no SSE doubles). Clean-room public ABI.
 */
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#ifndef bool_t
typedef int bool_t;
#endif

#ifndef off64_t
typedef long long off64_t;
#endif

/* ---- forward decls of existing surface ---------------------------------- */

char *textdomain(const char *szDomain);
int pthread_attr_init(pthread_attr_t *pAttr);
int pthread_attr_destroy(pthread_attr_t *pAttr);
int pthread_attr_setstacksize(pthread_attr_t *pAttr, size_t cb);
int pthread_create(pthread_t *pTh, const pthread_attr_t *pAttr,
                   void *(*pfn)(void *), void *pArg);
int pthread_join(pthread_t th, void **ppRet);
int pthread_kill(pthread_t th, int nSig);
pthread_t pthread_self(void);
void pthread_exit(void *pRet);
int pthread_key_create(pthread_key_t *pKey, void (*pfn)(void *));
int pthread_key_delete(pthread_key_t key);
void *pthread_getspecific(pthread_key_t key);
int pthread_setspecific(pthread_key_t key, const void *pVal);
int pthread_sigmask(int nHow, const sigset_t *pSet, sigset_t *pOld);
int pthread_setname_np(pthread_t th, const char *szName);
int pthread_getname_np(pthread_t th, char *szName, size_t cb);
int pthread_setconcurrency(int nLevel);
int pthread_getconcurrency(void);

/* ---- marker ------------------------------------------------------------- */

const char __libcgj_batch29_marker[] = "libcgj-batch29";

/* ---- text domain alias -------------------------------------------------- */

char *
text_domain(const char *szDomain)
{
    return textdomain(szDomain);
}

/* ---- obstack failure hook ----------------------------------------------- */

void (*obstack_alloc_failed_handler)(void);

/* ---- Solaris thr_* over pthread ----------------------------------------- */

int
thr_create(void *pStack, size_t cbStack, void *(*pfn)(void *), void *pArg,
           long nFlags, pthread_t *pOut)
{
    pthread_attr_t attr;
    int n;

    (void)pStack;
    (void)nFlags;
    if (pOut == NULL || pfn == NULL) {
        return EINVAL;
    }
    if (pthread_attr_init(&attr) != 0) {
        return errno != 0 ? errno : ENOMEM;
    }
    if (cbStack > 0u) {
        (void)pthread_attr_setstacksize(&attr, cbStack);
    }
    n = pthread_create(pOut, &attr, pfn, pArg);
    (void)pthread_attr_destroy(&attr);
    return n;
}

int
thr_join(pthread_t th, pthread_t *pDeparted, void **ppStatus)
{
    int n = pthread_join(th, ppStatus);

    if (pDeparted != NULL && n == 0) {
        *pDeparted = th;
    }
    return n;
}

int
thr_kill(pthread_t th, int nSig)
{
    return pthread_kill(th, nSig);
}

pthread_t
thr_self(void)
{
    return pthread_self();
}

void
thr_exit(void *pStatus)
{
    pthread_exit(pStatus);
}

int
thr_main(void)
{
    return 1;
}

size_t
thr_min_stack(void)
{
    return 16384u;
}

int
thr_stksegment(void *pSs)
{
    (void)pSs;
    errno = ENOSYS;
    return -1;
}

int
thr_getname(pthread_t th, char *szName, size_t cb)
{
    return pthread_getname_np(th, szName, cb);
}

int
thr_setname(pthread_t th, const char *szName)
{
    return pthread_setname_np(th, szName);
}

int
thr_keycreate(pthread_key_t *pKey, void (*pfn)(void *))
{
    return pthread_key_create(pKey, pfn);
}

int
thr_keydelete(pthread_key_t key)
{
    return pthread_key_delete(key);
}

void *
thr_getspecific(pthread_key_t key)
{
    return pthread_getspecific(key);
}

int
thr_setspecific(pthread_key_t key, void *pVal)
{
    return pthread_setspecific(key, pVal);
}

int
thr_continue(pthread_t th)
{
    (void)th;
    return 0;
}

int
thr_suspend(pthread_t th)
{
    (void)th;
    errno = ENOSYS;
    return -1;
}

int
thr_setconcurrency(int nLevel)
{
    return pthread_setconcurrency(nLevel);
}

int
thr_getconcurrency(void)
{
    return pthread_getconcurrency();
}

int
thr_sigsetmask(int nHow, const sigset_t *pSet, sigset_t *pOld)
{
    return pthread_sigmask(nHow, pSet, pOld);
}

/* thr_yield already shipped in batch28 — do not redefine */

/* ---- libcap-shaped stubs (not full libcap) ------------------------------ */

typedef void *cap_t;

cap_t
cap_init(void)
{
    void *p = malloc(16u);

    if (p != NULL) {
        memset(p, 0, 16u);
    }
    return (cap_t)p;
}

int
cap_free(cap_t pCap)
{
    free(pCap);
    return 0;
}

cap_t
cap_dup(cap_t pCap)
{
    cap_t pNew;

    if (pCap == NULL) {
        errno = EINVAL;
        return NULL;
    }
    pNew = cap_init();
    if (pNew != NULL) {
        memcpy(pNew, pCap, 16u);
    }
    return pNew;
}

cap_t
cap_get_proc(void)
{
    return cap_init();
}

int
cap_set_proc(cap_t pCap)
{
    (void)pCap;
    errno = EPERM;
    return -1;
}

cap_t
cap_from_text(const char *szText)
{
    if (szText == NULL) {
        errno = EINVAL;
        return NULL;
    }
    return cap_init();
}

char *
cap_to_text(cap_t pCap, ssize_t *pcb)
{
    char *sz;

    (void)pCap;
    sz = (char *)malloc(2u);
    if (sz == NULL) {
        return NULL;
    }
    sz[0] = '=';
    sz[1] = '\0';
    if (pcb != NULL) {
        *pcb = 1;
    }
    return sz;
}

/* ---- TI-RPC clnt extras (stubs) ----------------------------------------- */

void *
clnt_call(void *pCl, unsigned long nProc, void *pfnIn, void *pIn, void *pfnOut,
          void *pOut, struct timeval tv)
{
    (void)pCl;
    (void)nProc;
    (void)pfnIn;
    (void)pIn;
    (void)pfnOut;
    (void)pOut;
    (void)tv;
    errno = ENOSYS;
    return (void *)(uintptr_t)1;
}

int
clnt_control(void *pCl, unsigned nReq, char *pInfo)
{
    (void)pCl;
    (void)nReq;
    (void)pInfo;
    errno = ENOSYS;
    return 0;
}

void
clnt_destroy(void *pCl)
{
    free(pCl);
}

bool_t
clnt_freeres(void *pCl, void *pfnOut, char *pOut)
{
    (void)pCl;
    (void)pfnOut;
    (void)pOut;
    return 1;
}

void
clnt_geterr(void *pCl, void *pErr)
{
    (void)pCl;
    if (pErr != NULL) {
        memset(pErr, 0, 16u);
    }
}

/* ---- rcmd helpers ------------------------------------------------------- */

const char *__rcmd_errstr = NULL;
int __check_rhosts_file = 1;

/* ---- fopencookie cookie ops --------------------------------------------- */

ssize_t
cookie_read(void *pCookie, char *pBuf, size_t cb)
{
    (void)pCookie;
    (void)pBuf;
    (void)cb;
    return 0;
}

ssize_t
cookie_write(void *pCookie, const char *pBuf, size_t cb)
{
    (void)pCookie;
    (void)pBuf;
    return (ssize_t)cb;
}

int
cookie_seek(void *pCookie, off64_t *pOff, int nWhence)
{
    (void)pCookie;
    (void)pOff;
    (void)nWhence;
    return -1;
}

int
cookie_close(void *pCookie)
{
    (void)pCookie;
    return 0;
}

/* ---- TLS allocate stubs (loader helpers) -------------------------------- */

void *
_dl_allocate_tls(void *pMem)
{
    (void)pMem;
    return NULL;
}

void
_dl_deallocate_tls(void *pMem, int nDealloc)
{
    (void)pMem;
    (void)nDealloc;
}

/* ---- underscored aliases ------------------------------------------------ */

char *
__text_domain(const char *szDomain)
{
    return text_domain(szDomain);
}

int
__thr_create(void *pStack, size_t cbStack, void *(*pfn)(void *), void *pArg,
             long nFlags, pthread_t *pOut)
{
    return thr_create(pStack, cbStack, pfn, pArg, nFlags, pOut);
}

int
__thr_join(pthread_t th, pthread_t *pDeparted, void **ppStatus)
{
    return thr_join(th, pDeparted, ppStatus);
}

pthread_t
__thr_self(void)
{
    return thr_self();
}

void *
__cap_init(void)
{
    return cap_init();
}

int
__cap_free(void *pCap)
{
    return cap_free((cap_t)pCap);
}

void *
__clnt_call(void *pCl, unsigned long nProc, void *pfnIn, void *pIn,
            void *pfnOut, void *pOut, struct timeval tv)
{
    return clnt_call(pCl, nProc, pfnIn, pIn, pfnOut, pOut, tv);
}

void *
__dl_allocate_tls(void *pMem)
{
    return _dl_allocate_tls(pMem);
}

void
__dl_deallocate_tls(void *pMem, int nDealloc)
{
    _dl_deallocate_tls(pMem, nDealloc);
}
