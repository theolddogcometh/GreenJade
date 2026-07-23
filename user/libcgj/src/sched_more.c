/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * sched policy/param helpers, __sched_cpucount, process_vm_*,
 * clock_getcpuclockid (glibc-shaped bring-up). Not GNU glibc.
 */
#include <errno.h>
#include <sched.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

#define NR_sched_setparam         142
#define NR_sched_getparam         143
#define NR_sched_setscheduler     144
#define NR_sched_getscheduler     145
#define NR_sched_get_priority_max 146
#define NR_sched_get_priority_min 147
#define NR_sched_rr_get_interval  148
#define NR_process_vm_readv       310
#define NR_process_vm_writev      311

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
sched_setparam(pid_t pid, const struct sched_param *pParam)
{
    if (pParam == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_sched_setparam, (long)pid,
                             (long)(uintptr_t)pParam, 0, 0, 0, 0));
}

int
sched_getparam(pid_t pid, struct sched_param *pParam)
{
    if (pParam == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_sched_getparam, (long)pid,
                             (long)(uintptr_t)pParam, 0, 0, 0, 0));
}

int
sched_setscheduler(pid_t pid, int nPolicy, const struct sched_param *pParam)
{
    if (pParam == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_sched_setscheduler, (long)pid, (long)nPolicy,
                             (long)(uintptr_t)pParam, 0, 0, 0));
}

int
sched_getscheduler(pid_t pid)
{
    return (int)sys_ret(sys6(NR_sched_getscheduler, (long)pid, 0, 0, 0, 0, 0));
}

int
sched_get_priority_max(int nPolicy)
{
    return (int)sys_ret(
        sys6(NR_sched_get_priority_max, (long)nPolicy, 0, 0, 0, 0, 0));
}

int
sched_get_priority_min(int nPolicy)
{
    return (int)sys_ret(
        sys6(NR_sched_get_priority_min, (long)nPolicy, 0, 0, 0, 0, 0));
}

int
sched_rr_get_interval(pid_t pid, struct timespec *pTs)
{
    if (pTs == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_sched_rr_get_interval, (long)pid,
                             (long)(uintptr_t)pTs, 0, 0, 0, 0));
}

int
__sched_cpucount(size_t cbSet, const cpu_set_t *pSet)
{
    size_t cWords;
    size_t i;
    int nCount = 0;
    unsigned long u;

    if (pSet == NULL || cbSet == 0) {
        return 0;
    }
    cWords = cbSet / sizeof(unsigned long);
    if (cWords > (size_t)(__CPU_SETSIZE / __NCPUBITS)) {
        cWords = (size_t)(__CPU_SETSIZE / __NCPUBITS);
    }
    for (i = 0; i < cWords; i++) {
        u = pSet->__bits[i];
        while (u != 0UL) {
            nCount += (int)(u & 1UL);
            u >>= 1;
        }
    }
    return nCount;
}

ssize_t
process_vm_readv(pid_t pid, const struct iovec *pLocal, unsigned long cLocal,
                 const struct iovec *pRemote, unsigned long cRemote,
                 unsigned long uFlags)
{
    long r;

    r = sys6(NR_process_vm_readv, (long)pid, (long)(uintptr_t)pLocal,
             (long)cLocal, (long)(uintptr_t)pRemote, (long)cRemote,
             (long)uFlags);
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return (ssize_t)r;
}

ssize_t
process_vm_writev(pid_t pid, const struct iovec *pLocal, unsigned long cLocal,
                  const struct iovec *pRemote, unsigned long cRemote,
                  unsigned long uFlags)
{
    long r;

    r = sys6(NR_process_vm_writev, (long)pid, (long)(uintptr_t)pLocal,
             (long)cLocal, (long)(uintptr_t)pRemote, (long)cRemote,
             (long)uFlags);
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return (ssize_t)r;
}

int
clock_getcpuclockid(pid_t pid, clockid_t *pClk)
{
    if (pClk == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (pid != 0 && pid != getpid()) {
        /* Bring-up: only self process CPU clock */
        errno = EPERM;
        return -1;
    }
    *pClk = CLOCK_PROCESS_CPUTIME_ID;
    return 0;
}
