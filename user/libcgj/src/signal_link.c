/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * sigwait/sigpending/sigsuspend + pthread_kill/pthread_sigmask (soft deepen).
 */
#include <errno.h>
#include <pthread.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#define NR_rt_sigpending   127
#define NR_rt_sigsuspend   130
#define NR_rt_sigtimedwait 128
#define NR_tgkill          234
#define NR_gettid          186
#define NR_getpid          39
#define KERNEL_SIGSET_SZ   8

static long
sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
{
    long ret;
    register long r10 __asm__("r10") = a3;
    register long r8 __asm__("r8") = a4;
    register long r9 __asm__("r9") = a5;

    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(nr), "D"(a0), "S"(a1), "d"(a2), "r"(r10), "r"(r8), "r"(r9)
        : "rcx", "r11", "memory");
    return ret;
}

static int
sys_ret(long r)
{
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return (int)r;
}

int
sigpending(sigset_t *pSet)
{
    long r;

    if (pSet == NULL) {
        errno = EINVAL;
        return -1;
    }
    r = sys6(NR_rt_sigpending, (long)(uintptr_t)pSet, KERNEL_SIGSET_SZ, 0, 0, 0,
             0);
    return sys_ret(r);
}

int
sigsuspend(const sigset_t *pSet)
{
    long r;

    if (pSet == NULL) {
        errno = EINVAL;
        return -1;
    }
    r = sys6(NR_rt_sigsuspend, (long)(uintptr_t)pSet, KERNEL_SIGSET_SZ, 0, 0, 0,
             0);
    /* Always returns -1 with EINTR when a signal is delivered. */
    return sys_ret(r);
}

int
sigwait(const sigset_t *pSet, int *pSig)
{
    siginfo_t info;
    long r;

    if (pSet == NULL) {
        return EINVAL;
    }
    memset(&info, 0, sizeof(info));
    for (;;) {
        r = sys6(NR_rt_sigtimedwait, (long)(uintptr_t)pSet,
                 (long)(uintptr_t)&info, 0, KERNEL_SIGSET_SZ, 0, 0);
        if (r < 0 && r > -4096) {
            if ((int)(-r) == EINTR) {
                continue;
            }
            return (int)(-r);
        }
        if (pSig != NULL) {
            *pSig = (int)r;
        }
        return 0;
    }
}

int
sigwaitinfo(const sigset_t *pSet, siginfo_t *pInfo)
{
    long r;

    if (pSet == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (;;) {
        r = sys6(NR_rt_sigtimedwait, (long)(uintptr_t)pSet,
                 (long)(uintptr_t)pInfo, 0, KERNEL_SIGSET_SZ, 0, 0);
        if (r < 0 && r > -4096) {
            if ((int)(-r) == EINTR) {
                /* Soft: restart like many libcs for non-timeout wait. */
                continue;
            }
            errno = (int)(-r);
            return -1;
        }
        return (int)r;
    }
}

int
sigtimedwait(const sigset_t *pSet, siginfo_t *pInfo,
             const struct timespec *pTimeout)
{
    long r;

    if (pSet == NULL) {
        errno = EINVAL;
        return -1;
    }
    /* Do not swallow EINTR — timed wait must surface interrupt. */
    r = sys6(NR_rt_sigtimedwait, (long)(uintptr_t)pSet, (long)(uintptr_t)pInfo,
             (long)(uintptr_t)pTimeout, KERNEL_SIGSET_SZ, 0, 0);
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return (int)r;
}

int
pthread_sigmask(int nHow, const sigset_t *pSet, sigset_t *pOld)
{
    /* Thread mask is process-thread local under Linux rt_sigprocmask. */
    return sigprocmask(nHow, pSet, pOld);
}

int
pthread_kill(pthread_t tid, int nSig)
{
    long pid;
    long r;

    /* nSig == 0 is a valid existence probe. */
    if (nSig < 0) {
        return EINVAL;
    }
    if (tid == 0) {
        return ESRCH;
    }
    pid = sys6(NR_getpid, 0, 0, 0, 0, 0, 0);
    if (pid < 0 && pid > -4096) {
        return (int)(-pid);
    }
    r = sys6(NR_tgkill, pid, (long)tid, nSig, 0, 0, 0);
    if (r < 0 && r > -4096) {
        return (int)(-r);
    }
    return 0;
}

int
tgkill(int nTgid, int nTid, int nSig)
{
    long r;

    if (nSig < 0) {
        errno = EINVAL;
        return -1;
    }
    if (nTgid <= 0 || nTid <= 0) {
        errno = EINVAL;
        return -1;
    }
    r = sys6(NR_tgkill, (long)nTgid, (long)nTid, (long)nSig, 0, 0, 0);
    return sys_ret(r);
}
