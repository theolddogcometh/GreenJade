/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * sched policy/param helpers, __sched_cpucount, process_vm_*,
 * clock_getcpuclockid (glibc-shaped). Not GNU glibc.
 * Soft deepen: process CPU clock ids for other PIDs; popcount; VM null checks.
 */
#include <errno.h>
#include <sched.h>
#include <signal.h>
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

/* Linux private process CPU clock encoding (public kernel ABI). */
#define GJ_CPUCLOCK_SCHED 2
#define GJ_MAKE_PROCESS_CPUCLOCK(pid, clock) \
    ((clockid_t)((~(unsigned long)(unsigned int)(pid) << 3) | (unsigned long)(clock)))

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
    if (pid < 0) {
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
    if (pid < 0) {
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
    if (pid < 0) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_sched_setscheduler, (long)pid, (long)nPolicy,
                             (long)(uintptr_t)pParam, 0, 0, 0));
}

int
sched_getscheduler(pid_t pid)
{
    if (pid < 0) {
        errno = EINVAL;
        return -1;
    }
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
    if (pid < 0) {
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
#if defined(__GNUC__) || defined(__clang__)
        nCount += (int)__builtin_popcountl(u);
#else
        while (u != 0UL) {
            nCount += (int)(u & 1UL);
            u >>= 1;
        }
#endif
    }
    return nCount;
}

ssize_t
process_vm_readv(pid_t pid, const struct iovec *pLocal, unsigned long cLocal,
                 const struct iovec *pRemote, unsigned long cRemote,
                 unsigned long uFlags)
{
    long r;

    if (pid < 0) {
        errno = EINVAL;
        return -1;
    }
    if ((cLocal != 0 && pLocal == NULL) || (cRemote != 0 && pRemote == NULL)) {
        errno = EFAULT;
        return -1;
    }
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

    if (pid < 0) {
        errno = EINVAL;
        return -1;
    }
    if ((cLocal != 0 && pLocal == NULL) || (cRemote != 0 && pRemote == NULL)) {
        errno = EFAULT;
        return -1;
    }
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
    clockid_t clk;
    int nSaved;

    if (pClk == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (pid < 0) {
        errno = EINVAL;
        return -1;
    }
    if (pid == 0) {
        pid = getpid();
    }
    /*
     * Soft: prove the target is reachable (ESRCH) without requiring
     * signal delivery permission for every call — kill(0) ok if EPERM.
     */
    nSaved = errno;
    if (kill(pid, 0) != 0 && errno != EPERM) {
        /* errno already ESRCH/EINVAL from kill */
        return -1;
    }
    errno = nSaved;

    if (pid == getpid()) {
        *pClk = CLOCK_PROCESS_CPUTIME_ID;
        return 0;
    }
    clk = GJ_MAKE_PROCESS_CPUCLOCK(pid, GJ_CPUCLOCK_SCHED);
    /* Soft probe: clock_getres rejects bogus ids on capable kernels. */
    {
        struct timespec ts;

        if (clock_getres(clk, &ts) != 0) {
            /* Fallback closed if host rejects private clockids. */
            return -1;
        }
    }
    *pClk = clk;
    return 0;
}
