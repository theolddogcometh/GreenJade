/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch33: y2038 time64 aliases (redirect to existing
 * APIs), thr priority/scheduler/keycreate_once, getrandom_r/getentropy_r,
 * sprofil stub. Integer/pointer only (no SSE doubles). Clean-room ABI.
 */
#include <errno.h>
#include <poll.h>
#include <pthread.h>
#include <sched.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <sys/sendfile.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

/* Existing surface not always in freestanding headers */
int utimensat(int nDfd, const char *szPath, const struct timespec aTs[2],
              int nFlags);
int timer_settime(timer_t timerid, int nFlags, const struct itimerspec *pNew,
                  struct itimerspec *pOld);
int timer_gettime(timer_t timerid, struct itimerspec *pVal);
ssize_t copy_file_range(int nIn, off_t *pOffIn, int nOut, off_t *pOffOut,
                        size_t cb, unsigned uFlags);
int fallocate(int nFd, int nMode, off_t off, off_t cb);
int posix_fallocate(int nFd, off_t off, off_t cb);
int getentropy(void *pBuf, size_t cb);

const char __libcgj_batch33_marker[] = "libcgj-batch33";

/* ---- thr priority / scheduler / once ------------------------------------ */

int
thr_setprio(pthread_t th, int nPrio)
{
    struct sched_param sp;
    int nPolicy = 0;

    memset(&sp, 0, sizeof(sp));
    if (pthread_getschedparam(th, &nPolicy, &sp) != 0) {
        return errno != 0 ? errno : EINVAL;
    }
    sp.sched_priority = nPrio;
    return pthread_setschedparam(th, nPolicy, &sp);
}

int
thr_getprio(pthread_t th, int *pPrio)
{
    struct sched_param sp;
    int nPolicy = 0;

    if (pPrio == NULL) {
        return EINVAL;
    }
    memset(&sp, 0, sizeof(sp));
    if (pthread_getschedparam(th, &nPolicy, &sp) != 0) {
        return errno != 0 ? errno : EINVAL;
    }
    *pPrio = sp.sched_priority;
    return 0;
}

int
thr_setscheduler(pthread_t th, int nPolicy, int nPrio)
{
    struct sched_param sp;

    memset(&sp, 0, sizeof(sp));
    sp.sched_priority = nPrio;
    return pthread_setschedparam(th, nPolicy, &sp);
}

int
thr_getscheduler(pthread_t th, int *pPolicy, int *pPrio)
{
    struct sched_param sp;
    int nPolicy = 0;

    if (pPolicy == NULL || pPrio == NULL) {
        return EINVAL;
    }
    memset(&sp, 0, sizeof(sp));
    if (pthread_getschedparam(th, &nPolicy, &sp) != 0) {
        return errno != 0 ? errno : EINVAL;
    }
    *pPolicy = nPolicy;
    *pPrio = sp.sched_priority;
    return 0;
}

int
thr_keycreate_once(pthread_key_t *pKey, void (*pfn)(void *))
{
    static pthread_mutex_t s_lock = PTHREAD_MUTEX_INITIALIZER;
    int n;

    if (pKey == NULL) {
        return EINVAL;
    }
    (void)pthread_mutex_lock(&s_lock);
    if (*pKey != (pthread_key_t)0) {
        (void)pthread_mutex_unlock(&s_lock);
        return 0;
    }
    n = pthread_key_create(pKey, pfn);
    (void)pthread_mutex_unlock(&s_lock);
    return n;
}

/* ---- getrandom_r / getentropy_r ----------------------------------------- */

int
getrandom_r(void *pBuf, size_t cb, unsigned uFlags, size_t *pcbOut)
{
    ssize_t n;

    if (pBuf == NULL || pcbOut == NULL) {
        errno = EINVAL;
        return -1;
    }
    n = getrandom(pBuf, cb, uFlags);
    if (n < 0) {
        return -1;
    }
    *pcbOut = (size_t)n;
    return 0;
}

int
getentropy_r(void *pBuf, size_t cb)
{
    return getentropy(pBuf, cb);
}

/* ---- sprofil stub ------------------------------------------------------- */

int
sprofil(void *pSprof, int nProfCnt, struct timeval *pTv, unsigned uFlags)
{
    (void)pSprof;
    (void)nProfCnt;
    (void)pTv;
    (void)uFlags;
    errno = ENOSYS;
    return -1;
}

/* ---- time64 aliases (LP64: same layout as base APIs) -------------------- */

int
__clock_gettime64(clockid_t clk, struct timespec *pTs)
{
    return clock_gettime(clk, pTs);
}

int
__clock_settime64(clockid_t clk, const struct timespec *pTs)
{
    return clock_settime(clk, pTs);
}

int
__clock_getres_time64(clockid_t clk, struct timespec *pTs)
{
    return clock_getres(clk, pTs);
}

int
__clock_nanosleep_time64(clockid_t clk, int nFlags,
                         const struct timespec *pReq, struct timespec *pRem)
{
    return clock_nanosleep(clk, nFlags, pReq, pRem);
}

int
__nanosleep_time64(const struct timespec *pReq, struct timespec *pRem)
{
    return nanosleep(pReq, pRem);
}

int
__nanosleep64(const struct timespec *pReq, struct timespec *pRem)
{
    return nanosleep(pReq, pRem);
}

int
__gettimeofday_time64(struct timeval *pTv, void *pTz)
{
    return gettimeofday(pTv, pTz);
}

int
__settimeofday_time64(const struct timeval *pTv, void *pTz)
{
    /* Prefer existing settimeofday if linked; else ENOSYS. */
    extern int settimeofday(const struct timeval *, void *);
    return settimeofday(pTv, pTz);
}

int
__adjtimex_time64(struct timex *pBuf)
{
    return adjtimex(pBuf);
}

int
__utimensat_time64(int nDfd, const char *szPath, const struct timespec aTs[2],
                   int nFlags)
{
    return utimensat(nDfd, szPath, aTs, nFlags);
}

int
__utimensat64(int nDfd, const char *szPath, const struct timespec aTs[2],
              int nFlags)
{
    return utimensat(nDfd, szPath, aTs, nFlags);
}

int
__ppoll_time64(struct pollfd *pFds, nfds_t nFds, const struct timespec *pTimeout,
               const sigset_t *pSigmask)
{
    return ppoll(pFds, nFds, pTimeout, pSigmask);
}

int
ppoll_time64(struct pollfd *pFds, nfds_t nFds, const struct timespec *pTimeout,
             const sigset_t *pSigmask)
{
    return ppoll(pFds, nFds, pTimeout, pSigmask);
}

int
__pselect6_time64(int nNfds, fd_set *pR, fd_set *pW, fd_set *pE,
                  const struct timespec *pTimeout, const sigset_t *pSigmask)
{
    return pselect(nNfds, pR, pW, pE, pTimeout, pSigmask);
}

int
pselect_time64(int nNfds, fd_set *pR, fd_set *pW, fd_set *pE,
               const struct timespec *pTimeout, const sigset_t *pSigmask)
{
    return pselect(nNfds, pR, pW, pE, pTimeout, pSigmask);
}

int
__timer_settime64(timer_t timerid, int nFlags, const struct itimerspec *pNew,
                  struct itimerspec *pOld)
{
    return timer_settime(timerid, nFlags, pNew, pOld);
}

int
__timer_gettime64(timer_t timerid, struct itimerspec *pVal)
{
    return timer_gettime(timerid, pVal);
}

int
__sched_rr_get_interval_time64(pid_t pid, struct timespec *pTs)
{
    return sched_rr_get_interval(pid, pTs);
}

ssize_t
__copy_file_range64(int nIn, off_t *pOffIn, int nOut, off_t *pOffOut, size_t cb,
                    unsigned uFlags)
{
    return copy_file_range(nIn, pOffIn, nOut, pOffOut, cb, uFlags);
}

ssize_t
__sendfile64(int nOut, int nIn, off_t *pOff, size_t cb)
{
    return sendfile(nOut, nIn, pOff, cb);
}

void *
__mmap64(void *pAddr, size_t cb, int nProt, int nFlags, int nFd, off_t off)
{
    return mmap(pAddr, cb, nProt, nFlags, nFd, off);
}

int
__fallocate64(int nFd, int nMode, off_t off, off_t cb)
{
    return fallocate(nFd, nMode, off, cb);
}

int
__posix_fallocate64(int nFd, off_t off, off_t cb)
{
    return posix_fallocate(nFd, off, cb);
}

/* ---- underscored aliases ------------------------------------------------ */

int
__thr_setprio(pthread_t th, int nPrio)
{
    return thr_setprio(th, nPrio);
}

int
__thr_getprio(pthread_t th, int *pPrio)
{
    return thr_getprio(th, pPrio);
}

int
__thr_setscheduler(pthread_t th, int nPolicy, int nPrio)
{
    return thr_setscheduler(th, nPolicy, nPrio);
}

int
__thr_getscheduler(pthread_t th, int *pPolicy, int *pPrio)
{
    return thr_getscheduler(th, pPolicy, pPrio);
}

int
__thr_keycreate_once(pthread_key_t *pKey, void (*pfn)(void *))
{
    return thr_keycreate_once(pKey, pfn);
}

int
__getrandom_r(void *pBuf, size_t cb, unsigned uFlags, size_t *pcbOut)
{
    return getrandom_r(pBuf, cb, uFlags, pcbOut);
}

int
__getentropy_r(void *pBuf, size_t cb)
{
    return getentropy_r(pBuf, cb);
}

int
__sprofil(void *pSprof, int nProfCnt, struct timeval *pTv, unsigned uFlags)
{
    return sprofil(pSprof, nProfCnt, pTv, uFlags);
}

/* ---- soft deepen: extra time64 / thr underscored names ------------------ */

int
__clock_getres64(clockid_t clk, struct timespec *pTs)
{
    return clock_getres(clk, pTs);
}

int
__pselect_time64(int nNfds, fd_set *pR, fd_set *pW, fd_set *pE,
                 const struct timespec *pTimeout, const sigset_t *pSigmask)
{
    return pselect_time64(nNfds, pR, pW, pE, pTimeout, pSigmask);
}

/* __ppoll_time64 already defined above — do not redefine. */

/* thr_setprio family already has __thr_* above; add get/set name variants. */
int
thr_setprio_np(pthread_t th, int nPrio)
{
    return thr_setprio(th, nPrio);
}

int
thr_getprio_np(pthread_t th, int *pPrio)
{
    return thr_getprio(th, pPrio);
}

int __thr_setprio_np(pthread_t th, int nPrio)
    __attribute__((alias("thr_setprio_np")));
int __thr_getprio_np(pthread_t th, int *pPrio)
    __attribute__((alias("thr_getprio_np")));

