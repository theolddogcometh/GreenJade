/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch12: strerrorname_np / sig*np, __libc_* malloc
 * aliases, gnu_get_libc_*, fstatfs64, isoc23 aliases, fortify helpers,
 * resolv underscores, _IO_* aliases. Integer-only (no SSE doubles).
 *
 * greppable: CGJ_GRAPH_BATCH12_SOFT_NULL
 * greppable: CGJ_GRAPH_BATCH12_SOFT_ARGS
 * greppable: CGJ_GRAPH_BATCH12_SOFT_EDGE
 *
 * Soft deepen: null/arg guards on user-facing graph nodes; edge
 * hardening only. No multi-def; no API break. Pure C integer/pointer.
 */
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <pthread.h>
#include <resolv.h>
#include <semaphore.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/statvfs.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

void __fortify_fail(const char *szMsg);
void __chk_fail(void);
long __fdelt_chk(long d);

/* ---- GNU error / signal name helpers (glibc 2.32+) ----------------------- */

const char *
strerrorname_np(int nErr)
{
    switch (nErr) {
    case 0:
        return "0";
    case EPERM:
        return "EPERM";
    case ENOENT:
        return "ENOENT";
    case ESRCH:
        return "ESRCH";
    case EINTR:
        return "EINTR";
    case EIO:
        return "EIO";
    case EAGAIN:
        return "EAGAIN";
    case ENOMEM:
        return "ENOMEM";
    case EACCES:
        return "EACCES";
    case EFAULT:
        return "EFAULT";
    case EBUSY:
        return "EBUSY";
    case EEXIST:
        return "EEXIST";
    case ENOTDIR:
        return "ENOTDIR";
    case EISDIR:
        return "EISDIR";
    case EINVAL:
        return "EINVAL";
    case ENOSPC:
        return "ENOSPC";
    case EPIPE:
        return "EPIPE";
    case ERANGE:
        return "ERANGE";
    case ENOSYS:
        return "ENOSYS";
    case ETIMEDOUT:
        return "ETIMEDOUT";
    case ECONNREFUSED:
        return "ECONNREFUSED";
    case EADDRINUSE:
        return "EADDRINUSE";
    default:
        return NULL;
    }
}

const char *
strerrordesc_np(int nErr)
{
    const char *sz;

    if (nErr == 0) {
        return "Success";
    }
    sz = strerror(nErr);
    if (sz == NULL || strcmp(sz, "Unknown error") == 0) {
        return NULL;
    }
    return sz;
}

const char *
sigabbrev_np(int nSig)
{
    switch (nSig) {
    case 1:
        return "HUP";
    case 2:
        return "INT";
    case 3:
        return "QUIT";
    case 4:
        return "ILL";
    case 6:
        return "ABRT";
    case 8:
        return "FPE";
    case 9:
        return "KILL";
    case 11:
        return "SEGV";
    case 13:
        return "PIPE";
    case 14:
        return "ALRM";
    case 15:
        return "TERM";
    case 17:
        return "CHLD";
    case 18:
        return "CONT";
    case 19:
        return "STOP";
    case 20:
        return "TSTP";
    default:
        return NULL;
    }
}

const char *
sigdescr_np(int nSig)
{
    return strsignal(nSig);
}

/* ---- malloc underscored aliases ----------------------------------------- */

void *
__libc_malloc(size_t cb)
{
    return malloc(cb);
}

void *
__libc_calloc(size_t n, size_t cb)
{
    return calloc(n, cb);
}

void *
__libc_realloc(void *p, size_t cb)
{
    return realloc(p, cb);
}

void
__libc_free(void *p)
{
    free(p);
}

char *
__strndup(const char *sz, size_t n)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (sz == NULL) {
        return NULL;
    }

    return strndup(sz, n);
}

/* ---- version / sigrt / sysconf aliases ---------------------------------- */

const char *
gnu_get_libc_version(void)
{
    /* Product identity: libcgj bring-up, glibc-shaped 2.2.5 baseline */
    return "2.2.5-greenjade";
}

const char *
gnu_get_libc_release(void)
{
    return "stable";
}

int
__libc_current_sigrtmin(void)
{
    return 34; /* Linux x86_64 SIGRTMIN */
}

int
__libc_current_sigrtmax(void)
{
    return 64; /* Linux x86_64 SIGRTMAX */
}

long
__sysconf(int nName)
{
    return sysconf(nName);
}

/* ---- statfs LFS / underscores ------------------------------------------- */

int
statfs64(const char *szPath, struct statfs *pBuf)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    return statfs(szPath, pBuf);
}

int
fstatfs64(int nFd, struct statfs *pBuf)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    return fstatfs(nFd, pBuf);
}

int
__statfs(const char *szPath, struct statfs *pBuf)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    return statfs(szPath, pBuf);
}

int
__fstatfs(int nFd, struct statfs *pBuf)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    return fstatfs(nFd, pBuf);
}

/* ---- resolv underscores ------------------------------------------------- */

int
__res_init(void)
{
    return res_init();
}

int
__res_ninit(void *pState)
{
    (void)pState;
    return res_init();
}

int
__res_query(const char *szDname, int nClass, int nType, unsigned char *pAns,
            int nAnslen)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (szDname == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pAns == NULL) {
        errno = EFAULT;
        return -1;
    }

    return res_query(szDname, nClass, nType, pAns, nAnslen);
}

/* ---- open64 fortify / aliases ------------------------------------------- */

int open(const char *szPath, int nFlags, ...);
int openat(int nDfd, const char *szPath, int nFlags, ...);
int open64(const char *szPath, int nFlags, ...);

int
__open64(const char *szPath, int nFlags, ...)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    mode_t mode = 0;

    if (nFlags & O_CREAT) {
        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
        return open64(szPath, nFlags, mode);
    }
    return open64(szPath, nFlags);
}

int
__open64_2(const char *szPath, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (nFlags & O_CREAT) {
        __fortify_fail("invalid open64 call: O_CREAT without mode");
    }
    return open64(szPath, nFlags);
}

int
__openat64_2(int nDfd, const char *szPath, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (nDfd < 0) {
        errno = EBADF;
        return -1;
    }
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (nFlags & O_CREAT) {
        __fortify_fail("invalid openat64 call: O_CREAT without mode");
    }
    return openat(nDfd, szPath, nFlags);
}

long
__fdelt_warn(long d)
{
    return __fdelt_chk(d);
}

/* ---- longjmp fortify ---------------------------------------------------- */

void
__longjmp_chk(jmp_buf env, int nVal)
{
    longjmp(env, nVal);
}

/* ---- isoc23 integer/scanf aliases (C23 binary shape; no float) ---------- */

long
__isoc23_strtol(const char *sz, char **ppEnd, int nBase)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    return strtol(sz, ppEnd, nBase);
}

unsigned long
__isoc23_strtoul(const char *sz, char **ppEnd, int nBase)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    return strtoul(sz, ppEnd, nBase);
}

long long
__isoc23_strtoll(const char *sz, char **ppEnd, int nBase)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    return strtoll(sz, ppEnd, nBase);
}

unsigned long long
__isoc23_strtoull(const char *sz, char **ppEnd, int nBase)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    return strtoull(sz, ppEnd, nBase);
}

int
__isoc23_scanf(const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vscanf(szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc23_fscanf(FILE *pF, const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vfscanf(pF, szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc23_sscanf(const char *szBuf, const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vsscanf(szBuf, szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc23_vscanf(const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vscanf(szFmt, ap);
}

int
__isoc23_vfscanf(FILE *pF, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vfscanf(pF, szFmt, ap);
}

int
__isoc23_vsscanf(const char *szBuf, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vsscanf(szBuf, szFmt, ap);
}

/* ---- stdio internal aliases (desktop graphs) ---------------------------- */

int
_IO_getc(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return fgetc(pF);
}

int
_IO_putc(int ch, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return fputc(ch, pF);
}

int
_IO_feof(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return feof(pF);
}

int
_IO_ferror(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }

    return ferror(pF);
}

/* FILE underflow / overflow bring-up: delegate to fgetc/fputc */
int
__uflow(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return fgetc(pF);
}

int
__overflow(FILE *pF, int ch)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    if (ch == EOF) {
        return fflush(pF);
    }
    return fputc(ch, pF);
}

/* glibc getc/putc names (macros in full glibc; functions here) */
int
getc(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return fgetc(pF);
}

int
putc(int ch, FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return fputc(ch, pF);
}

/* ---- wchar underscored aliases ------------------------------------------ */

size_t
__mbrtowc(wchar_t *pWc, const char *pS, size_t n, mbstate_t *pSt)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (pWc == NULL) {
        return 0;
    }
    if (pS == NULL) {
        return 0;
    }
    if (pSt == NULL) {
        return 0;
    }

    return mbrtowc(pWc, pS, n, pSt);
}

size_t
__mbsnrtowcs(wchar_t *pDst, const char **ppSrc, size_t nmc, size_t len,
             mbstate_t *pSt)
{
    /* greppable: CGJ_GRAPH_BATCH12_SOFT_NULL */
    if (pDst == NULL) {
        return 0;
    }
    if (ppSrc == NULL) {
        return 0;
    }
    if (pSt == NULL) {
        return 0;
    }

    return mbsnrtowcs(pDst, ppSrc, nmc, len, pSt);
}

/* ---- robust mutex bring-up (flag only; no owner-died recovery) ---------- */

#ifndef PTHREAD_MUTEX_STALLED
#define PTHREAD_MUTEX_STALLED 0
#endif
#ifndef PTHREAD_MUTEX_ROBUST
#define PTHREAD_MUTEX_ROBUST 1
#endif

int
pthread_mutexattr_setrobust(pthread_mutexattr_t *pAttr, int nRobust)
{
    if (pAttr == NULL) {
        return EINVAL;
    }
    if (nRobust != PTHREAD_MUTEX_STALLED && nRobust != PTHREAD_MUTEX_ROBUST) {
        return EINVAL;
    }
    pAttr->nRobust = nRobust;
    return 0;
}

int
pthread_mutexattr_getrobust(const pthread_mutexattr_t *pAttr, int *pRobust)
{
    if (pAttr == NULL || pRobust == NULL) {
        return EINVAL;
    }
    *pRobust = pAttr->nRobust;
    return 0;
}

int
pthread_mutex_consistent(pthread_mutex_t *pMtx)
{
    if (pMtx == NULL) {
        return EINVAL;
    }
    /* Bring-up: no owner-died state tracked */
    return 0;
}

/* ---- sem_clockwait (glibc 2.30+) ---------------------------------------- */

int
sem_clockwait(sem_t *pSem, clockid_t clockId, const struct timespec *pAbs)
{
    struct timespec ts;
    struct timespec now;
    struct timespec absRt;

    if (pSem == NULL || pAbs == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (clockId == CLOCK_REALTIME) {
        return sem_timedwait(pSem, pAbs);
    }
    /* Convert non-REALTIME absolute to REALTIME for sem_timedwait */
    if (clock_gettime(clockId, &now) != 0) {
        return -1;
    }
    if (clock_gettime(CLOCK_REALTIME, &ts) != 0) {
        return -1;
    }
    /* absRt = now_realtime + (pAbs - now_clock) */
    absRt.tv_sec = ts.tv_sec + (pAbs->tv_sec - now.tv_sec);
    absRt.tv_nsec = ts.tv_nsec + (pAbs->tv_nsec - now.tv_nsec);
    if (absRt.tv_nsec >= 1000000000L) {
        absRt.tv_sec++;
        absRt.tv_nsec -= 1000000000L;
    } else if (absRt.tv_nsec < 0) {
        absRt.tv_sec--;
        absRt.tv_nsec += 1000000000L;
    }
    return sem_timedwait(pSem, &absRt);
}
