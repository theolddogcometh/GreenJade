/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * signal / raise / kill — soft deepen via sigaction + tkill. Not GNU glibc.
 */
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

/* Linux x86_64 */
#define NR_kill   62
#define NR_tkill  200
#define NR_gettid 186

static long
sys3(long nr, long a0, long a1, long a2)
{
    long ret;

    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(nr), "D"(a0), "S"(a1), "d"(a2)
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

/*
 * ISO C signal(): install handler with SA_RESTART (common glibc shape).
 * Routes through sigaction (signal_more.c) so disposition is real, not a stub.
 */
sighandler_t
signal(int nSig, sighandler_t pHandler)
{
    struct sigaction sa;
    struct sigaction so;

    if (nSig < 1 || nSig == SIGKILL || nSig == SIGSTOP) {
        errno = EINVAL;
        return SIG_ERR;
    }
    if (pHandler == SIG_ERR) {
        errno = EINVAL;
        return SIG_ERR;
    }
    memset(&sa, 0, sizeof(sa));
    memset(&so, 0, sizeof(so));
    sa.sa_handler = pHandler;
    sa.sa_flags = SA_RESTART;
    if (sigemptyset(&sa.sa_mask) != 0) {
        return SIG_ERR;
    }
    if (sigaction(nSig, &sa, &so) != 0) {
        return SIG_ERR;
    }
    return so.sa_handler;
}

int
raise(int nSig)
{
    long tid;
    long r;

    if (nSig < 0) {
        errno = EINVAL;
        return -1;
    }
    /* Thread-directed: tkill(gettid), not kill(tid) as pid. */
    tid = sys3(NR_gettid, 0, 0, 0);
    if (tid < 0) {
        return kill(getpid(), nSig);
    }
    r = sys3(NR_tkill, tid, nSig, 0);
    return sys_ret(r);
}

int
kill(int nPid, int nSig)
{
    long r;

    if (nSig < 0) {
        errno = EINVAL;
        return -1;
    }
    r = sys3(NR_kill, nPid, nSig, 0);
    return sys_ret(r);
}
