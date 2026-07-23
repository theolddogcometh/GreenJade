/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * pidfd_open / pidfd_send_signal / pidfd_getfd — Linux desktop graph nodes.
 */
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#define NR_pidfd_send_signal 424
#define NR_pidfd_open        434
#define NR_pidfd_getfd       438

static long
sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
{
    register long r10 __asm__("r10") = a3;
    register long r8 __asm__("r8") = a4;
    register long r9 __asm__("r9") = a5;
    long ret;

    __asm__ volatile("syscall"
                     : "=a"(ret)
                     : "a"(nr), "D"(a0), "S"(a1), "d"(a2), "r"(r10), "r"(r8),
                       "r"(r9)
                     : "rcx", "r11", "memory");
    return ret;
}

static long
sys_ret(long r)
{
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return r;
}

int
pidfd_open(pid_t pid, unsigned int uFlags)
{
    return (int)sys_ret(sys6(NR_pidfd_open, (long)pid, (long)uFlags, 0, 0, 0,
                             0));
}

int
pidfd_send_signal(int nPidfd, int nSig, void *pInfo, unsigned int uFlags)
{
    return (int)sys_ret(sys6(NR_pidfd_send_signal, nPidfd, nSig,
                             (long)(uintptr_t)pInfo, (long)uFlags, 0, 0));
}

int
pidfd_getfd(int nPidfd, int nTargetFd, unsigned int uFlags)
{
    return (int)sys_ret(sys6(NR_pidfd_getfd, nPidfd, nTargetFd, (long)uFlags,
                             0, 0, 0));
}
