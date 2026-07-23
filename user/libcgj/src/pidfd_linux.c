/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * pidfd_open / pidfd_send_signal / pidfd_getfd — Linux desktop graph nodes.
 * Soft deepen: arg validation; reserved-flags soft reject.
 */
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <unistd.h>

#define NR_pidfd_send_signal 424
#define NR_pidfd_open        434
#define NR_pidfd_getfd       438

/* Known pidfd_open flag (Linux public ABI). */
#ifndef PIDFD_NONBLOCK
#define PIDFD_NONBLOCK 04000
#endif

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
    /* Soft: reject clearly invalid pid before the SYSCALL. */
    if (pid < 0) {
        errno = EINVAL;
        return -1;
    }
    /* Only known flag bits accepted; unknown → kernel may EINVAL. */
    if ((uFlags & ~(unsigned int)PIDFD_NONBLOCK) != 0) {
        /* Soft pass-through: still invoke; kernel is authority on flags. */
    }
    return (int)sys_ret(sys6(NR_pidfd_open, (long)pid, (long)uFlags, 0, 0, 0,
                             0));
}

int
pidfd_send_signal(int nPidfd, int nSig, void *pInfo, unsigned int uFlags)
{
    if (nPidfd < 0) {
        errno = EBADF;
        return -1;
    }
    if (nSig < 0 || nSig >= 64) {
        errno = EINVAL;
        return -1;
    }
    /* flags currently reserved (must be 0 on Linux). */
    if (uFlags != 0) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_pidfd_send_signal, nPidfd, nSig,
                             (long)(uintptr_t)pInfo, (long)uFlags, 0, 0));
}

int
pidfd_getfd(int nPidfd, int nTargetFd, unsigned int uFlags)
{
    if (nPidfd < 0 || nTargetFd < 0) {
        errno = EBADF;
        return -1;
    }
    /* flags reserved (must be 0). */
    if (uFlags != 0) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_pidfd_getfd, nPidfd, nTargetFd, (long)uFlags,
                             0, 0, 0));
}
