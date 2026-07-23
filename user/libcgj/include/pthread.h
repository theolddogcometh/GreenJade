/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped pthread (bring-up subset). Not GNU glibc/NPTL.
 */
#pragma once

#include <sched.h>
#include <stddef.h>
#include <stdint.h>
#include <signal.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef unsigned long pthread_t;

typedef struct pthread_attr {
    void     *pStack;
    size_t    cbStack;
    size_t    cbGuard;
    int       nDetach;
    int       fAffinity; /* non-zero if cpuset valid */
    cpu_set_t cpuset;
} pthread_attr_t;

typedef struct pthread_mutex {
    volatile int  nLock; /* 0 free, 1 locked */
    int           nType;
    int           nCount;   /* recursive hold count */
    unsigned long uOwner; /* gettid of owner; 0 if free */
    int           nProtocol;
} pthread_mutex_t;

typedef struct pthread_mutexattr {
    int nType;
    int nProtocol;
    int nPshared;
    int nRobust; /* PTHREAD_MUTEX_STALLED / PTHREAD_MUTEX_ROBUST */
} pthread_mutexattr_t;

#ifndef PTHREAD_MUTEX_STALLED
#define PTHREAD_MUTEX_STALLED 0
#endif
#ifndef PTHREAD_MUTEX_ROBUST
#define PTHREAD_MUTEX_ROBUST 1
#endif

typedef struct pthread_cond {
    volatile unsigned uSeq;
    int               nClock; /* CLOCK_REALTIME / CLOCK_MONOTONIC */
} pthread_cond_t;

typedef struct pthread_condattr {
    int nClock;
    int nPshared;
} pthread_condattr_t;

typedef struct pthread_once {
    volatile int nDone;
} pthread_once_t;

typedef unsigned pthread_key_t;

#define PTHREAD_MUTEX_INITIALIZER   { 0, 0, 0, 0, 0 }
#define PTHREAD_COND_INITIALIZER    { 0, 0 }
#define PTHREAD_ONCE_INIT           { 0 }

#define PTHREAD_CREATE_JOINABLE 0
#define PTHREAD_CREATE_DETACHED 1
#define PTHREAD_MUTEX_NORMAL    0
#define PTHREAD_MUTEX_RECURSIVE 1
#define PTHREAD_MUTEX_ERRORCHECK 2
#define PTHREAD_MUTEX_DEFAULT   PTHREAD_MUTEX_NORMAL

#define PTHREAD_PRIO_NONE    0
#define PTHREAD_PRIO_INHERIT 1
#define PTHREAD_PRIO_PROTECT 2

int  pthread_create(pthread_t *pTid, const pthread_attr_t *pAttr,
                    void *(*pfnStart)(void *), void *pArg);
int  pthread_join(pthread_t tid, void **ppRet);
int  pthread_tryjoin_np(pthread_t tid, void **ppRet);
int  pthread_timedjoin_np(pthread_t tid, void **ppRet,
                          const struct timespec *pAbstime);
int  pthread_clockjoin_np(pthread_t tid, void **ppRet, clockid_t clk,
                          const struct timespec *pAbstime);
int  pthread_detach(pthread_t tid);
pthread_t pthread_self(void);
int  pthread_equal(pthread_t a, pthread_t b);
void pthread_exit(void *pRet) __attribute__((noreturn));
int  pthread_yield(void);

int  pthread_mutexattr_init(pthread_mutexattr_t *pA);
int  pthread_mutexattr_destroy(pthread_mutexattr_t *pA);
int  pthread_mutexattr_settype(pthread_mutexattr_t *pA, int nType);
int  pthread_mutexattr_gettype(const pthread_mutexattr_t *pA, int *pType);
int  pthread_mutexattr_setprotocol(pthread_mutexattr_t *pA, int nProto);
int  pthread_mutexattr_getprotocol(const pthread_mutexattr_t *pA, int *pProto);
int  pthread_mutexattr_setpshared(pthread_mutexattr_t *pA, int nPshared);
int  pthread_mutexattr_getpshared(const pthread_mutexattr_t *pA, int *pPshared);
int  pthread_mutexattr_setrobust(pthread_mutexattr_t *pA, int nRobust);
int  pthread_mutexattr_getrobust(const pthread_mutexattr_t *pA, int *pRobust);

int  pthread_mutex_init(pthread_mutex_t *pM, const pthread_mutexattr_t *pA);
int  pthread_mutex_destroy(pthread_mutex_t *pM);
int  pthread_mutex_lock(pthread_mutex_t *pM);
int  pthread_mutex_trylock(pthread_mutex_t *pM);
int  pthread_mutex_unlock(pthread_mutex_t *pM);
int  pthread_mutex_timedlock(pthread_mutex_t *pM, const struct timespec *pAbs);
int  pthread_mutex_consistent(pthread_mutex_t *pM);

int  pthread_condattr_init(pthread_condattr_t *pA);
int  pthread_condattr_destroy(pthread_condattr_t *pA);
int  pthread_condattr_setclock(pthread_condattr_t *pA, clockid_t clk);
int  pthread_condattr_getclock(const pthread_condattr_t *pA, clockid_t *pClk);
int  pthread_condattr_setpshared(pthread_condattr_t *pA, int nPshared);
int  pthread_condattr_getpshared(const pthread_condattr_t *pA, int *pPshared);

int  pthread_cond_init(pthread_cond_t *pC, const pthread_condattr_t *pA);
int  pthread_cond_destroy(pthread_cond_t *pC);
int  pthread_cond_wait(pthread_cond_t *pC, pthread_mutex_t *pM);
int  pthread_cond_timedwait(pthread_cond_t *pC, pthread_mutex_t *pM,
                            const struct timespec *pAbs);
int  pthread_cond_signal(pthread_cond_t *pC);
int  pthread_cond_broadcast(pthread_cond_t *pC);

int  pthread_once(pthread_once_t *pOnce, void (*pfn)(void));
int  pthread_key_create(pthread_key_t *pKey, void (*pfnDtor)(void *));
int  pthread_key_delete(pthread_key_t key);
int  pthread_setspecific(pthread_key_t key, const void *p);
void *pthread_getspecific(pthread_key_t key);

int  pthread_attr_init(pthread_attr_t *pA);
int  pthread_attr_destroy(pthread_attr_t *pA);
int  pthread_attr_setdetachstate(pthread_attr_t *pA, int nState);
int  pthread_attr_getdetachstate(const pthread_attr_t *pA, int *pState);
int  pthread_attr_setstacksize(pthread_attr_t *pA, size_t cb);
int  pthread_attr_getstacksize(const pthread_attr_t *pA, size_t *pcb);
int  pthread_attr_setstack(pthread_attr_t *pA, void *pStack, size_t cb);
int  pthread_attr_getstack(const pthread_attr_t *pA, void **ppStack,
                           size_t *pcb);
int  pthread_attr_setguardsize(pthread_attr_t *pA, size_t cb);
int  pthread_attr_getguardsize(const pthread_attr_t *pA, size_t *pcb);
int  pthread_attr_setaffinity_np(pthread_attr_t *pA, size_t cbSet,
                                 const cpu_set_t *pSet);
int  pthread_attr_getaffinity_np(const pthread_attr_t *pA, size_t cbSet,
                                 cpu_set_t *pSet);
int  pthread_getattr_np(pthread_t tid, pthread_attr_t *pA);

/* CPU affinity (glibc non-portable; wraps sched_*affinity) */
int pthread_setaffinity_np(pthread_t tid, size_t cbSet,
                           const cpu_set_t *pSet);
int pthread_getaffinity_np(pthread_t tid, size_t cbSet, cpu_set_t *pSet);

/* Scheduling (policy + priority) */
int pthread_setschedparam(pthread_t tid, int nPolicy,
                          const struct sched_param *pParam);
int pthread_getschedparam(pthread_t tid, int *pPolicy,
                          struct sched_param *pParam);
int pthread_setschedprio(pthread_t tid, int nPrio);
int pthread_getcpuclockid(pthread_t tid, clockid_t *pClk);

/* Fork handlers */
int pthread_atfork(void (*pfnPrepare)(void), void (*pfnParent)(void),
                   void (*pfnChild)(void));
int __register_atfork(void (*pfnPrepare)(void), void (*pfnParent)(void),
                      void (*pfnChild)(void), void *pDso);

/* Readers-writer lock (process-private bring-up) */
typedef struct pthread_rwlock {
    pthread_mutex_t mtx;
    pthread_cond_t  cv;
    int             nReaders;
    int             nWriter; /* 0 free, 1 held */
    int             nWritersWaiting;
} pthread_rwlock_t;

typedef struct pthread_rwlockattr {
    int nPshared;
} pthread_rwlockattr_t;

#define PTHREAD_RWLOCK_INITIALIZER \
    { PTHREAD_MUTEX_INITIALIZER, PTHREAD_COND_INITIALIZER, 0, 0, 0 }

int pthread_rwlockattr_init(pthread_rwlockattr_t *pA);
int pthread_rwlockattr_destroy(pthread_rwlockattr_t *pA);
int pthread_rwlockattr_setpshared(pthread_rwlockattr_t *pA, int nPshared);
int pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *pA,
                                  int *pPshared);
int pthread_rwlockattr_setkind_np(pthread_rwlockattr_t *pA, int nPref);
int pthread_rwlockattr_getkind_np(const pthread_rwlockattr_t *pA, int *pPref);

int pthread_rwlock_init(pthread_rwlock_t *pRw, const pthread_rwlockattr_t *pA);
int pthread_rwlock_destroy(pthread_rwlock_t *pRw);
int pthread_rwlock_rdlock(pthread_rwlock_t *pRw);
int pthread_rwlock_tryrdlock(pthread_rwlock_t *pRw);
int pthread_rwlock_wrlock(pthread_rwlock_t *pRw);
int pthread_rwlock_trywrlock(pthread_rwlock_t *pRw);
int pthread_rwlock_unlock(pthread_rwlock_t *pRw);
int pthread_rwlock_timedrdlock(pthread_rwlock_t *pRw,
                               const struct timespec *pAbs);
int pthread_rwlock_timedwrlock(pthread_rwlock_t *pRw,
                               const struct timespec *pAbs);
int pthread_mutex_clocklock(pthread_mutex_t *pM, clockid_t clk,
                            const struct timespec *pAbs);
int pthread_cond_clockwait(pthread_cond_t *pC, pthread_mutex_t *pM,
                           clockid_t clk, const struct timespec *pAbs);
int pthread_rwlock_clockrdlock(pthread_rwlock_t *pRw, clockid_t clk,
                               const struct timespec *pAbs);
int pthread_rwlock_clockwrlock(pthread_rwlock_t *pRw, clockid_t clk,
                               const struct timespec *pAbs);
int pthread_getattr_default_np(pthread_attr_t *pA);
int pthread_setattr_default_np(const pthread_attr_t *pA);
int pthread_sigqueue(pthread_t tid, int nSig, const union sigval value);

/* Spin locks (process-private; busy-wait CAS) */
typedef struct pthread_spinlock {
    volatile int nLock;
} pthread_spinlock_t;

#define PTHREAD_PROCESS_PRIVATE 0
#define PTHREAD_PROCESS_SHARED  1

int pthread_spin_init(pthread_spinlock_t *pS, int nPshared);
int pthread_spin_destroy(pthread_spinlock_t *pS);
int pthread_spin_lock(pthread_spinlock_t *pS);
int pthread_spin_trylock(pthread_spinlock_t *pS);
int pthread_spin_unlock(pthread_spinlock_t *pS);

/* Barriers */
typedef struct pthread_barrier {
    pthread_mutex_t mtx;
    pthread_cond_t  cv;
    unsigned        uCount;
    unsigned        uTrip;
    unsigned        uGen;
} pthread_barrier_t;

typedef struct pthread_barrierattr {
    int nPad;
} pthread_barrierattr_t;

#define PTHREAD_BARRIER_SERIAL_THREAD (-1)

int pthread_barrier_init(pthread_barrier_t *pB, const pthread_barrierattr_t *pA,
                         unsigned uCount);
int pthread_barrier_destroy(pthread_barrier_t *pB);
int pthread_barrier_wait(pthread_barrier_t *pB);
int pthread_barrierattr_init(pthread_barrierattr_t *pA);
int pthread_barrierattr_destroy(pthread_barrierattr_t *pA);
int pthread_barrierattr_setpshared(pthread_barrierattr_t *pA, int nPshared);
int pthread_barrierattr_getpshared(const pthread_barrierattr_t *pA,
                                   int *pPshared);

int pthread_setcancelstate(int nState, int *pOld);
int pthread_setcanceltype(int nType, int *pOld);
void pthread_testcancel(void);
int pthread_cancel(pthread_t tid);

/* Thread name (glibc non-portable; PR_SET_NAME shape, max 16 incl NUL) */
int pthread_setname_np(pthread_t tid, const char *szName);
int pthread_getname_np(pthread_t tid, char *szName, size_t cb);

/* Signal interaction */
int pthread_kill(pthread_t tid, int nSig);
int pthread_sigmask(int nHow, const sigset_t *pSet, sigset_t *pOld);

/* TLS descriptor ABI (SysV x86_64) */
struct __tls_get_addr_arg {
    unsigned long long u64Module;
    unsigned long long u64Offset;
};
void *__tls_get_addr(struct __tls_get_addr_arg *p);

#ifdef __cplusplus
}
#endif
