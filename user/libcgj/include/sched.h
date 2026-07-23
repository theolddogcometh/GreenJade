/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <sched.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * sched_yield, affinity (cpu_set_t / CPU_* macros), sched_param, and
 * policy/priority helpers used by pthread and desktop runtimes.
 *
 * Design notes
 * ------------
 * cpu_set_t size targets glibc CPU_SETSIZE bring-up; __sched_cpucount is
 * exported for CPU_COUNT. Affinity may soft-fill to single-CPU masks early.
 *
 * Non-goals
 * ---------
 * Full deadline/FIFO realtime guarantees on the microkernel scheduler.
 */
#pragma once

#include <stddef.h>
#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* cpu_set_t: 1024 CPUs (glibc default shape) */
#define __CPU_SETSIZE 1024
#define __NCPUBITS    (8 * (int)sizeof(unsigned long))

typedef struct {
    unsigned long __bits[__CPU_SETSIZE / __NCPUBITS];
} cpu_set_t;

#define CPU_SETSIZE __CPU_SETSIZE

#define CPU_ZERO(cpusetp) \
    do { \
        size_t __i; \
        for (__i = 0; \
             __i < (sizeof(cpu_set_t) / sizeof(unsigned long)); \
             __i++) { \
            (cpusetp)->__bits[__i] = 0UL; \
        } \
    } while (0)

#define CPU_SET(cpu, cpusetp) \
    ((void)((cpusetp)->__bits[(size_t)(cpu) / (size_t)__NCPUBITS] |= \
            (1UL << ((size_t)(cpu) % (size_t)__NCPUBITS))))

#define CPU_CLR(cpu, cpusetp) \
    ((void)((cpusetp)->__bits[(size_t)(cpu) / (size_t)__NCPUBITS] &= \
            ~(1UL << ((size_t)(cpu) % (size_t)__NCPUBITS))))

#define CPU_ISSET(cpu, cpusetp) \
    (((cpusetp)->__bits[(size_t)(cpu) / (size_t)__NCPUBITS] & \
      (1UL << ((size_t)(cpu) % (size_t)__NCPUBITS))) != 0UL)

struct sched_param {
    int sched_priority;
};

/* Scheduling policies (Linux) */
#define SCHED_OTHER    0
#define SCHED_FIFO     1
#define SCHED_RR       2
#define SCHED_BATCH    3
#define SCHED_IDLE     5
#define SCHED_DEADLINE 6

/* clone(2) flags (Linux; subset used by desktop graphs) */
#define CSIGNAL              0x000000ff
#define CLONE_VM             0x00000100
#define CLONE_FS             0x00000200
#define CLONE_FILES          0x00000400
#define CLONE_SIGHAND        0x00000800
#define CLONE_PIDFD          0x00001000
#define CLONE_PTRACE         0x00002000
#define CLONE_VFORK          0x00004000
#define CLONE_PARENT         0x00008000
#define CLONE_THREAD         0x00010000
#define CLONE_NEWNS          0x00020000
#define CLONE_SYSVSEM        0x00040000
#define CLONE_SETTLS         0x00080000
#define CLONE_PARENT_SETTID  0x00100000
#define CLONE_CHILD_CLEARTID 0x00200000
#define CLONE_DETACHED       0x00400000
#define CLONE_UNTRACED       0x00800000
#define CLONE_CHILD_SETTID   0x01000000
#define CLONE_NEWCGROUP      0x02000000
#define CLONE_NEWUTS         0x04000000
#define CLONE_NEWIPC         0x08000000
#define CLONE_NEWUSER        0x10000000
#define CLONE_NEWPID         0x20000000
#define CLONE_NEWNET         0x40000000
#define CLONE_IO             0x80000000

int clone(int (*fn)(void *), void *pStack, int nFlags, void *pArg, ...);

int sched_yield(void);
int __sched_yield(void);
int sched_getaffinity(pid_t pid, size_t cbSet, cpu_set_t *pSet);
int sched_setaffinity(pid_t pid, size_t cbSet, const cpu_set_t *pSet);
int sched_getcpu(void);
int getcpu(unsigned *pCpu, unsigned *pNode, void *pTcache);

int sched_setparam(pid_t pid, const struct sched_param *pParam);
int sched_getparam(pid_t pid, struct sched_param *pParam);
int sched_setscheduler(pid_t pid, int nPolicy, const struct sched_param *pParam);
int sched_getscheduler(pid_t pid);
int sched_get_priority_max(int nPolicy);
int sched_get_priority_min(int nPolicy);
int sched_rr_get_interval(pid_t pid, struct timespec *pTs);

/* Linux sched_setattr(2) shape (size field first) */
struct sched_attr {
    unsigned int size;
    unsigned int sched_policy;
    unsigned long long sched_flags;
    int          sched_nice;
    unsigned int sched_priority;
    unsigned long long sched_runtime;
    unsigned long long sched_deadline;
    unsigned long long sched_period;
    unsigned int sched_util_min;
    unsigned int sched_util_max;
};

int sched_setattr(pid_t pid, struct sched_attr *pAttr, unsigned int uFlags);
int sched_getattr(pid_t pid, struct sched_attr *pAttr, unsigned int uSize,
                  unsigned int uFlags);

int __sched_cpucount(size_t cbSet, const cpu_set_t *pSet);

#define CPU_COUNT(cpusetp) __sched_cpucount(sizeof(cpu_set_t), (cpusetp))
#define CPU_COUNT_S(setsize, cpusetp) __sched_cpucount((setsize), (cpusetp))

#ifdef __cplusplus
}
#endif
