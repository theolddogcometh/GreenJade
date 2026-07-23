/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal signal stubs (bring-up). Not GNU glibc.
 */
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <unistd.h>

/* Linux x86_64 */
#define NR_kill 62
#define NR_tkill 200
#define NR_gettid 186
#define NR_rt_sigaction 13

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

sighandler_t
signal(int nSig, sighandler_t pHandler)
{
    /* Bring-up: accept SIG_IGN/SIG_DFL without installing a full sigaction. */
    (void)nSig;
    (void)pHandler;
    return SIG_DFL;
}

int
raise(int nSig)
{
    long tid = sys3(NR_gettid, 0, 0, 0);

    if (tid < 0) {
        tid = (long)getpid();
    }
    return kill((int)tid, nSig);
}

int
kill(int nPid, int nSig)
{
    long r = sys3(NR_kill, nPid, nSig, 0);

    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return 0;
}
