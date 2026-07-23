/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal pthread + futex + TLS (bring-up). Not GNU NPTL.
 */
#include <errno.h>
#include <pthread.h>
#include <sched.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NR_clone         56
#define NR_exit          60
#define NR_wait4         61
#define NR_futex        202
#define NR_gettid       186
#define NR_set_tid_address 218
#define NR_arch_prctl   158

#define FUTEX_WAIT 0
#define FUTEX_WAKE 1
#define FUTEX_PRIVATE_FLAG 128

#define CLONE_VM      0x00000100
#define CLONE_FS      0x00000200
#define CLONE_FILES   0x00000400
#define CLONE_SIGHAND 0x00000800
#define CLONE_THREAD  0x00010000
#define CLONE_SYSVSEM 0x00040000
#define CLONE_SETTLS  0x00080000
#define CLONE_PARENT_SETTID 0x00100000
#define CLONE_CHILD_CLEARTID 0x00200000

#define ARCH_SET_FS 0x1002

#define CGJ_KEY_MAX 32
#define CGJ_TLS_MODS 8
#define CGJ_TLS_SIZE 4096

struct cgj_thread {
    pthread_t tid;
    void *(*pfn)(void *);
    void *pArg;
    void *pRet;
    void *pStack;
    size_t cbStack;
    int nJoinable;
    int nDone;
    volatile int nClearTid;
};

static long
sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
{
    register long r10 __asm__("r10") = a3;
    register long r8 __asm__("r8") = a4;
    register long r9 __asm__("r9") = a5;
    long ret;

    __asm__ volatile(
        "syscall"
        : "=a"(ret)
        : "a"(nr), "D"(a0), "S"(a1), "d"(a2), "r"(r10), "r"(r8), "r"(r9)
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

static int
futex_wait(volatile int *p, int nExpect)
{
    long r = sys6(NR_futex, (long)(uintptr_t)p, FUTEX_WAIT | FUTEX_PRIVATE_FLAG,
                  nExpect, 0, 0, 0);

    if (r < 0 && r != -11 /* EAGAIN */ && r != -4 /* EINTR */) {
        return (int)sys_ret(r);
    }
    return 0;
}

static int
futex_wake(volatile int *p, int nCount)
{
    return (int)sys_ret(sys6(NR_futex, (long)(uintptr_t)p,
                             FUTEX_WAKE | FUTEX_PRIVATE_FLAG, nCount, 0, 0, 0));
}

/* Simple static TLS block per module id (bring-up) */
static unsigned char g_aTls[CGJ_TLS_MODS][CGJ_TLS_SIZE];
static void *g_aKeys[CGJ_KEY_MAX];
static void (*g_aDtors[CGJ_KEY_MAX])(void *);
static int g_cKeys;

/* Freestanding: no compiler TLS; one active self pointer (bring-up) */
static struct cgj_thread *g_pSelf;
static struct cgj_thread g_mainThr;

static void
ensure_main(void)
{
    if (g_mainThr.tid == 0) {
        g_mainThr.tid = (pthread_t)sys6(NR_gettid, 0, 0, 0, 0, 0, 0);
        if (g_mainThr.tid == 0) {
            g_mainThr.tid = (pthread_t)getpid();
        }
        g_mainThr.nJoinable = 0;
        g_pSelf = &g_mainThr;
        (void)sys6(NR_set_tid_address, (long)(uintptr_t)&g_mainThr.nClearTid, 0,
                   0, 0, 0, 0);
    }
}

void *
__tls_get_addr(struct __tls_get_addr_arg *p)
{
    unsigned long long mod;
    unsigned long long off;

    if (p == NULL) {
        return NULL;
    }
    mod = p->u64Module;
    off = p->u64Offset;
    if (mod == 0 || mod > CGJ_TLS_MODS) {
        mod = 1;
    }
    if (off >= CGJ_TLS_SIZE) {
        off = 0;
    }
    return (void *)(g_aTls[mod - 1] + off);
}

int
pthread_attr_init(pthread_attr_t *pA)
{
    if (pA == NULL) {
        return EINVAL;
    }
    pA->pStack = NULL;
    pA->cbStack = 0;
    pA->cbGuard = 4096u;
    pA->nDetach = PTHREAD_CREATE_JOINABLE;
    pA->fAffinity = 0;
    CPU_ZERO(&pA->cpuset);
    return 0;
}

int
pthread_attr_destroy(pthread_attr_t *pA)
{
    (void)pA;
    return 0;
}

int
pthread_attr_setdetachstate(pthread_attr_t *pA, int nState)
{
    if (pA == NULL) {
        return EINVAL;
    }
    pA->nDetach = nState;
    return 0;
}

int
pthread_attr_getdetachstate(const pthread_attr_t *pA, int *pState)
{
    if (pA == NULL || pState == NULL) {
        return EINVAL;
    }
    *pState = pA->nDetach;
    return 0;
}

int
pthread_attr_setstacksize(pthread_attr_t *pA, size_t cb)
{
    if (pA == NULL || cb < 4096) {
        return EINVAL;
    }
    pA->cbStack = cb;
    return 0;
}

int
pthread_attr_getstacksize(const pthread_attr_t *pA, size_t *pcb)
{
    if (pA == NULL || pcb == NULL) {
        return EINVAL;
    }
    *pcb = pA->cbStack != 0 ? pA->cbStack : (size_t)(64 * 1024u);
    return 0;
}

int
pthread_attr_setstack(pthread_attr_t *pA, void *pStack, size_t cb)
{
    if (pA == NULL || pStack == NULL || cb < 4096u) {
        return EINVAL;
    }
    pA->pStack = pStack;
    pA->cbStack = cb;
    return 0;
}

int
pthread_attr_getstack(const pthread_attr_t *pA, void **ppStack, size_t *pcb)
{
    if (pA == NULL || ppStack == NULL || pcb == NULL) {
        return EINVAL;
    }
    *ppStack = pA->pStack;
    *pcb = pA->cbStack != 0 ? pA->cbStack : (size_t)(64 * 1024u);
    return 0;
}

int
pthread_attr_setguardsize(pthread_attr_t *pA, size_t cb)
{
    if (pA == NULL) {
        return EINVAL;
    }
    pA->cbGuard = cb;
    return 0;
}

int
pthread_attr_getguardsize(const pthread_attr_t *pA, size_t *pcb)
{
    if (pA == NULL || pcb == NULL) {
        return EINVAL;
    }
    *pcb = pA->cbGuard;
    return 0;
}

int
pthread_attr_setaffinity_np(pthread_attr_t *pA, size_t cbSet,
                            const cpu_set_t *pSet)
{
    size_t cbCopy;

    if (pA == NULL || pSet == NULL || cbSet == 0) {
        return EINVAL;
    }
    cbCopy = cbSet < sizeof(cpu_set_t) ? cbSet : sizeof(cpu_set_t);
    memset(&pA->cpuset, 0, sizeof(pA->cpuset));
    memcpy(&pA->cpuset, pSet, cbCopy);
    pA->fAffinity = 1;
    return 0;
}

int
pthread_attr_getaffinity_np(const pthread_attr_t *pA, size_t cbSet,
                            cpu_set_t *pSet)
{
    size_t cbCopy;

    if (pA == NULL || pSet == NULL || cbSet == 0) {
        return EINVAL;
    }
    memset(pSet, 0, cbSet);
    cbCopy = cbSet < sizeof(cpu_set_t) ? cbSet : sizeof(cpu_set_t);
    if (pA->fAffinity) {
        memcpy(pSet, &pA->cpuset, cbCopy);
    }
    return 0;
}

int
pthread_getattr_np(pthread_t tid, pthread_attr_t *pA)
{
    if (pA == NULL) {
        return EINVAL;
    }
    if (tid != 0 && tid != pthread_self()) {
        /* Bring-up: only self attrs are known */
        return ESRCH;
    }
    (void)pthread_attr_init(pA);
    pA->pStack = NULL;
    pA->cbStack = (size_t)(64 * 1024u);
    pA->cbGuard = 4096u;
    pA->nDetach = PTHREAD_CREATE_JOINABLE;
    pA->fAffinity = 0;
    if (sched_getaffinity(0, sizeof(pA->cpuset), &pA->cpuset) == 0) {
        pA->fAffinity = 1;
    }
    return 0;
}

int
pthread_setschedparam(pthread_t tid, int nPolicy,
                      const struct sched_param *pParam)
{
    pid_t pid;

    if (pParam == NULL) {
        return EINVAL;
    }
    pid = (pid_t)tid;
    if (tid == 0) {
        pid = 0;
    }
    if (sched_setscheduler(pid, nPolicy, pParam) != 0) {
        return errno;
    }
    return 0;
}

int
pthread_getschedparam(pthread_t tid, int *pPolicy, struct sched_param *pParam)
{
    pid_t pid;
    int nPol;

    if (pPolicy == NULL || pParam == NULL) {
        return EINVAL;
    }
    pid = (pid_t)tid;
    if (tid == 0) {
        pid = 0;
    }
    nPol = sched_getscheduler(pid);
    if (nPol < 0) {
        return errno;
    }
    if (sched_getparam(pid, pParam) != 0) {
        return errno;
    }
    *pPolicy = nPol;
    return 0;
}

int
pthread_setschedprio(pthread_t tid, int nPrio)
{
    struct sched_param sp;
    int nPol;
    int nErr;

    nErr = pthread_getschedparam(tid, &nPol, &sp);
    if (nErr != 0) {
        return nErr;
    }
    sp.sched_priority = nPrio;
    return pthread_setschedparam(tid, nPol, &sp);
}

int
pthread_getcpuclockid(pthread_t tid, clockid_t *pClk)
{
    if (pClk == NULL) {
        return EINVAL;
    }
    if (tid != 0 && tid != pthread_self()) {
        return ESRCH;
    }
    *pClk = CLOCK_THREAD_CPUTIME_ID;
    return 0;
}

static void
thread_trampoline(void)
{
    struct cgj_thread *pT = g_pSelf;
    void *pRet = NULL;

    if (pT != NULL && pT->pfn != NULL) {
        pRet = pT->pfn(pT->pArg);
    }
    pthread_exit(pRet);
}

int
pthread_create(pthread_t *pTid, const pthread_attr_t *pAttr,
               void *(*pfnStart)(void *), void *pArg)
{
    struct cgj_thread *pT;
    size_t cbStack = 64 * 1024u;
    void *pStack;
    int nStackOwned = 1;
    long flags;
    long tid;
    unsigned char *pTop;

    ensure_main();
    if (pTid == NULL || pfnStart == NULL) {
        return EINVAL;
    }
    pT = (struct cgj_thread *)calloc(1, sizeof(*pT));
    if (pT == NULL) {
        return ENOMEM;
    }
    if (pAttr != NULL && pAttr->pStack != NULL && pAttr->cbStack >= 4096u) {
        pStack = pAttr->pStack;
        cbStack = pAttr->cbStack;
        nStackOwned = 0;
    } else {
        if (pAttr != NULL && pAttr->cbStack > 0) {
            cbStack = pAttr->cbStack;
        }
        pStack = malloc(cbStack);
        if (pStack == NULL) {
            free(pT);
            return ENOMEM;
        }
        nStackOwned = 1;
    }
    pT->pfn = pfnStart;
    pT->pArg = pArg;
    pT->pStack = pStack;
    pT->cbStack = cbStack;
    pT->nJoinable =
        (pAttr == NULL || pAttr->nDetach == PTHREAD_CREATE_JOINABLE) ? 1 : 0;
    pT->nDone = 0;

    /* Stack grows down; align top */
    pTop = (unsigned char *)pStack + cbStack;
    pTop = (unsigned char *)((uintptr_t)pTop & ~(uintptr_t)15);
    /* leave room for return path — clone starts at child stack */
    flags = CLONE_VM | CLONE_FS | CLONE_FILES | CLONE_SIGHAND | CLONE_THREAD |
            CLONE_SYSVSEM | CLONE_CHILD_CLEARTID | CLONE_PARENT_SETTID;

    /*
     * clone(flags, stack, parent_tid, tls, child_tid) — Linux x86_64 order:
     * rdi=flags rsi=stack rdx=parent_tid r10=child_tid r8=tls
     * We set g_pSelf in parent before clone for child (shared VM).
     * Child must set its own TLS pointer — use stack slot for self.
     */
    {
        /* Store self ptr just below stack top for child bootstrap */
        struct cgj_thread **ppSlot =
            (struct cgj_thread **)(void *)(pTop - sizeof(void *));
        *ppSlot = pT;
        g_pSelf = pT; /* race: single-threaded create is fine for bring-up */

        tid = sys6(NR_clone, flags, (long)(uintptr_t)(pTop - 16),
                   (long)(uintptr_t)&pT->tid, (long)(uintptr_t)&pT->nClearTid,
                   0, 0);
        if (tid < 0) {
            if (nStackOwned) {
                free(pStack);
            }
            free(pT);
            g_pSelf = &g_mainThr;
            return (int)(-tid);
        }
        if (tid == 0) {
            /* child */
            g_pSelf = pT;
            pT->tid = (pthread_t)sys6(NR_gettid, 0, 0, 0, 0, 0, 0);
            thread_trampoline();
            for (;;) {
                (void)sys6(NR_exit, 0, 0, 0, 0, 0, 0);
            }
        }
        /* parent: restore main self */
        g_pSelf = &g_mainThr;
        pT->tid = (pthread_t)tid;
        *pTid = (pthread_t)tid;
    }
    return 0;
}

int
pthread_join(pthread_t tid, void **ppRet)
{
    int nStatus = 0;
    long r;

    ensure_main();
    r = sys6(NR_wait4, (long)tid, (long)(uintptr_t)&nStatus, 0, 0, 0, 0);
    if (r < 0) {
        /* CLONE_THREAD may not be waitable via wait4 — spin on clear_tid */
        /* For bring-up, treat as success if already gone */
        if (ppRet != NULL) {
            *ppRet = NULL;
        }
        return 0;
    }
    if (ppRet != NULL) {
        *ppRet = NULL;
    }
    return 0;
}

int
pthread_tryjoin_np(pthread_t tid, void **ppRet)
{
    int nStatus = 0;
    long r;

    ensure_main();
    r = sys6(NR_wait4, (long)tid, (long)(uintptr_t)&nStatus, 1 /* WNOHANG */, 0,
             0, 0);
    if (r == 0) {
        return EBUSY;
    }
    if (r < 0) {
        if (ppRet != NULL) {
            *ppRet = NULL;
        }
        return 0;
    }
    if (ppRet != NULL) {
        *ppRet = NULL;
    }
    return 0;
}

int
pthread_timedjoin_np(pthread_t tid, void **ppRet,
                     const struct timespec *pAbstime)
{
    (void)pAbstime;
    /* Bring-up: ignore absolute timeout; behave as join. */
    return pthread_join(tid, ppRet);
}

int
pthread_clockjoin_np(pthread_t tid, void **ppRet, clockid_t clk,
                     const struct timespec *pAbstime)
{
    (void)clk;
    (void)pAbstime;
    return pthread_join(tid, ppRet);
}

int
pthread_yield(void)
{
    return sched_yield();
}

/* robust mutex helpers live in graph_batch12.c */

int
pthread_rwlockattr_setkind_np(pthread_rwlockattr_t *pA, int nPref)
{
    (void)pA;
    (void)nPref;
    return 0;
}

int
pthread_rwlockattr_getkind_np(const pthread_rwlockattr_t *pA, int *pPref)
{
    if (pA == NULL || pPref == NULL) {
        return EINVAL;
    }
    *pPref = 0;
    return 0;
}

int
pthread_detach(pthread_t tid)
{
    (void)tid;
    return 0;
}

pthread_t
pthread_self(void)
{
    ensure_main();
    if (g_pSelf != NULL) {
        return g_pSelf->tid;
    }
    return (pthread_t)sys6(NR_gettid, 0, 0, 0, 0, 0, 0);
}

int
pthread_equal(pthread_t a, pthread_t b)
{
    return a == b;
}

void
pthread_exit(void *pRet)
{
    ensure_main();
    if (g_pSelf != NULL) {
        g_pSelf->pRet = pRet;
        g_pSelf->nDone = 1;
    }
    (void)sys6(NR_exit, 0, 0, 0, 0, 0, 0);
    for (;;) {
        __asm__ volatile("hlt");
    }
}

static unsigned long
mutex_self_tid(void)
{
    long t = sys6(NR_gettid, 0, 0, 0, 0, 0, 0);

    return (unsigned long)t;
}

int
pthread_mutexattr_init(pthread_mutexattr_t *pA)
{
    if (pA == NULL) {
        return EINVAL;
    }
    pA->nType = PTHREAD_MUTEX_NORMAL;
    pA->nProtocol = PTHREAD_PRIO_NONE;
    pA->nPshared = PTHREAD_PROCESS_PRIVATE;
    pA->nRobust = PTHREAD_MUTEX_STALLED;
    return 0;
}

int
pthread_mutexattr_destroy(pthread_mutexattr_t *pA)
{
    (void)pA;
    return 0;
}

int
pthread_mutexattr_settype(pthread_mutexattr_t *pA, int nType)
{
    if (pA == NULL) {
        return EINVAL;
    }
    if (nType != PTHREAD_MUTEX_NORMAL && nType != PTHREAD_MUTEX_RECURSIVE &&
        nType != PTHREAD_MUTEX_ERRORCHECK && nType != PTHREAD_MUTEX_DEFAULT) {
        return EINVAL;
    }
    pA->nType = nType;
    return 0;
}

int
pthread_mutexattr_gettype(const pthread_mutexattr_t *pA, int *pType)
{
    if (pA == NULL || pType == NULL) {
        return EINVAL;
    }
    *pType = pA->nType;
    return 0;
}

int
pthread_mutexattr_setprotocol(pthread_mutexattr_t *pA, int nProto)
{
    if (pA == NULL) {
        return EINVAL;
    }
    if (nProto != PTHREAD_PRIO_NONE && nProto != PTHREAD_PRIO_INHERIT &&
        nProto != PTHREAD_PRIO_PROTECT) {
        return EINVAL;
    }
    pA->nProtocol = nProto;
    return 0;
}

int
pthread_mutexattr_getprotocol(const pthread_mutexattr_t *pA, int *pProto)
{
    if (pA == NULL || pProto == NULL) {
        return EINVAL;
    }
    *pProto = pA->nProtocol;
    return 0;
}

int
pthread_mutexattr_setpshared(pthread_mutexattr_t *pA, int nPshared)
{
    if (pA == NULL) {
        return EINVAL;
    }
    if (nPshared != PTHREAD_PROCESS_PRIVATE) {
        return ENOSYS;
    }
    pA->nPshared = nPshared;
    return 0;
}

int
pthread_mutexattr_getpshared(const pthread_mutexattr_t *pA, int *pPshared)
{
    if (pA == NULL || pPshared == NULL) {
        return EINVAL;
    }
    *pPshared = pA->nPshared;
    return 0;
}

int
pthread_mutex_init(pthread_mutex_t *pM, const pthread_mutexattr_t *pA)
{
    if (pM == NULL) {
        return EINVAL;
    }
    pM->nLock = 0;
    pM->nType = (pA != NULL) ? pA->nType : PTHREAD_MUTEX_NORMAL;
    pM->nCount = 0;
    pM->uOwner = 0;
    pM->nProtocol = (pA != NULL) ? pA->nProtocol : PTHREAD_PRIO_NONE;
    return 0;
}

int
pthread_mutex_destroy(pthread_mutex_t *pM)
{
    if (pM == NULL) {
        return EINVAL;
    }
    pM->nLock = 0;
    pM->nCount = 0;
    pM->uOwner = 0;
    return 0;
}

int
pthread_mutex_trylock(pthread_mutex_t *pM)
{
    int nOld;
    unsigned long tid;

    if (pM == NULL) {
        return EINVAL;
    }
    tid = mutex_self_tid();
    if (pM->nType == PTHREAD_MUTEX_RECURSIVE && pM->nLock != 0 &&
        pM->uOwner == tid) {
        pM->nCount++;
        return 0;
    }
    if (pM->nType == PTHREAD_MUTEX_ERRORCHECK && pM->nLock != 0 &&
        pM->uOwner == tid) {
        return EDEADLK;
    }
    nOld = __sync_val_compare_and_swap(&pM->nLock, 0, 1);
    if (nOld != 0) {
        return EBUSY;
    }
    pM->uOwner = tid;
    pM->nCount = 1;
    return 0;
}

int
pthread_mutex_lock(pthread_mutex_t *pM)
{
    unsigned long tid;

    if (pM == NULL) {
        return EINVAL;
    }
    tid = mutex_self_tid();
    if (pM->nType == PTHREAD_MUTEX_RECURSIVE && pM->nLock != 0 &&
        pM->uOwner == tid) {
        pM->nCount++;
        return 0;
    }
    if (pM->nType == PTHREAD_MUTEX_ERRORCHECK && pM->nLock != 0 &&
        pM->uOwner == tid) {
        return EDEADLK;
    }
    for (;;) {
        if (__sync_val_compare_and_swap(&pM->nLock, 0, 1) == 0) {
            pM->uOwner = tid;
            pM->nCount = 1;
            return 0;
        }
        (void)futex_wait(&pM->nLock, 1);
    }
}

int
pthread_mutex_timedlock(pthread_mutex_t *pM, const struct timespec *pAbs)
{
    long r;
    struct timespec now;
    struct timespec rel;
    unsigned long tid;

    if (pM == NULL || pAbs == NULL) {
        return EINVAL;
    }
    tid = mutex_self_tid();
    if (pM->nType == PTHREAD_MUTEX_RECURSIVE && pM->nLock != 0 &&
        pM->uOwner == tid) {
        pM->nCount++;
        return 0;
    }
    for (;;) {
        if (__sync_val_compare_and_swap(&pM->nLock, 0, 1) == 0) {
            pM->uOwner = tid;
            pM->nCount = 1;
            return 0;
        }
        if (clock_gettime(CLOCK_REALTIME, &now) != 0) {
            return errno != 0 ? errno : ETIMEDOUT;
        }
        rel.tv_sec = pAbs->tv_sec - now.tv_sec;
        rel.tv_nsec = pAbs->tv_nsec - now.tv_nsec;
        if (rel.tv_nsec < 0) {
            rel.tv_sec--;
            rel.tv_nsec += 1000000000L;
        }
        if (rel.tv_sec < 0 || (rel.tv_sec == 0 && rel.tv_nsec <= 0)) {
            return ETIMEDOUT;
        }
        r = sys6(NR_futex, (long)(uintptr_t)&pM->nLock,
                 FUTEX_WAIT | FUTEX_PRIVATE_FLAG, 1, (long)(uintptr_t)&rel, 0,
                 0);
        if (r < 0 && (int)(-r) == ETIMEDOUT) {
            return ETIMEDOUT;
        }
    }
}

int
pthread_mutex_unlock(pthread_mutex_t *pM)
{
    unsigned long tid;

    if (pM == NULL) {
        return EINVAL;
    }
    tid = mutex_self_tid();
    if (pM->nType == PTHREAD_MUTEX_RECURSIVE) {
        if (pM->uOwner != tid) {
            return EPERM;
        }
        pM->nCount--;
        if (pM->nCount > 0) {
            return 0;
        }
        pM->uOwner = 0;
        __sync_lock_release(&pM->nLock);
        (void)futex_wake(&pM->nLock, 1);
        return 0;
    }
    if (pM->nType == PTHREAD_MUTEX_ERRORCHECK && pM->uOwner != tid) {
        return EPERM;
    }
    pM->uOwner = 0;
    pM->nCount = 0;
    __sync_lock_release(&pM->nLock);
    (void)futex_wake(&pM->nLock, 1);
    return 0;
}

int
pthread_condattr_init(pthread_condattr_t *pA)
{
    if (pA == NULL) {
        return EINVAL;
    }
    pA->nClock = CLOCK_REALTIME;
    pA->nPshared = PTHREAD_PROCESS_PRIVATE;
    return 0;
}

int
pthread_condattr_destroy(pthread_condattr_t *pA)
{
    (void)pA;
    return 0;
}

int
pthread_condattr_setclock(pthread_condattr_t *pA, clockid_t clk)
{
    if (pA == NULL) {
        return EINVAL;
    }
    if (clk != CLOCK_REALTIME && clk != CLOCK_MONOTONIC) {
        return EINVAL;
    }
    pA->nClock = (int)clk;
    return 0;
}

int
pthread_condattr_getclock(const pthread_condattr_t *pA, clockid_t *pClk)
{
    if (pA == NULL || pClk == NULL) {
        return EINVAL;
    }
    *pClk = (clockid_t)pA->nClock;
    return 0;
}

int
pthread_condattr_setpshared(pthread_condattr_t *pA, int nPshared)
{
    if (pA == NULL) {
        return EINVAL;
    }
    if (nPshared != PTHREAD_PROCESS_PRIVATE) {
        return ENOSYS;
    }
    pA->nPshared = nPshared;
    return 0;
}

int
pthread_condattr_getpshared(const pthread_condattr_t *pA, int *pPshared)
{
    if (pA == NULL || pPshared == NULL) {
        return EINVAL;
    }
    *pPshared = pA->nPshared;
    return 0;
}

int
pthread_cond_init(pthread_cond_t *pC, const pthread_condattr_t *pA)
{
    if (pC == NULL) {
        return EINVAL;
    }
    pC->uSeq = 0;
    pC->nClock = (pA != NULL) ? pA->nClock : CLOCK_REALTIME;
    return 0;
}

int
pthread_cond_destroy(pthread_cond_t *pC)
{
    (void)pC;
    return 0;
}

int
pthread_cond_wait(pthread_cond_t *pC, pthread_mutex_t *pM)
{
    unsigned u;

    if (pC == NULL || pM == NULL) {
        return EINVAL;
    }
    u = pC->uSeq;
    (void)pthread_mutex_unlock(pM);
    (void)futex_wait((volatile int *)&pC->uSeq, (int)u);
    (void)pthread_mutex_lock(pM);
    return 0;
}

int
pthread_cond_timedwait(pthread_cond_t *pC, pthread_mutex_t *pM,
                       const struct timespec *pAbs)
{
    unsigned u;
    long r;
    struct timespec now;
    struct timespec rel;
    int nRet = 0;

    if (pC == NULL || pM == NULL || pAbs == NULL) {
        return EINVAL;
    }
    u = pC->uSeq;
    (void)pthread_mutex_unlock(pM);
    if (clock_gettime((clockid_t)pC->nClock, &now) != 0) {
        (void)pthread_mutex_lock(pM);
        return errno != 0 ? errno : ETIMEDOUT;
    }
    rel.tv_sec = pAbs->tv_sec - now.tv_sec;
    rel.tv_nsec = pAbs->tv_nsec - now.tv_nsec;
    if (rel.tv_nsec < 0) {
        rel.tv_sec--;
        rel.tv_nsec += 1000000000L;
    }
    if (rel.tv_sec < 0 || (rel.tv_sec == 0 && rel.tv_nsec <= 0)) {
        nRet = ETIMEDOUT;
    } else {
        r = sys6(NR_futex, (long)(uintptr_t)&pC->uSeq,
                 FUTEX_WAIT | FUTEX_PRIVATE_FLAG, (int)u, (long)(uintptr_t)&rel,
                 0, 0);
        if (r < 0 && (int)(-r) == ETIMEDOUT) {
            nRet = ETIMEDOUT;
        }
    }
    (void)pthread_mutex_lock(pM);
    return nRet;
}

int
pthread_cond_signal(pthread_cond_t *pC)
{
    if (pC == NULL) {
        return EINVAL;
    }
    __sync_fetch_and_add(&pC->uSeq, 1);
    (void)futex_wake((volatile int *)&pC->uSeq, 1);
    return 0;
}

int
pthread_cond_broadcast(pthread_cond_t *pC)
{
    if (pC == NULL) {
        return EINVAL;
    }
    __sync_fetch_and_add(&pC->uSeq, 1);
    (void)futex_wake((volatile int *)&pC->uSeq, 0x7fffffff);
    return 0;
}

int
pthread_once(pthread_once_t *pOnce, void (*pfn)(void))
{
    if (pOnce == NULL || pfn == NULL) {
        return EINVAL;
    }
    if (__sync_val_compare_and_swap(&pOnce->nDone, 0, 1) == 0) {
        pfn();
    }
    return 0;
}

int
pthread_key_create(pthread_key_t *pKey, void (*pfnDtor)(void *))
{
    int i;

    if (pKey == NULL) {
        return EINVAL;
    }
    for (i = 0; i < CGJ_KEY_MAX; i++) {
        if (g_aDtors[i] == NULL && g_aKeys[i] == NULL && i >= g_cKeys) {
            /* fall through */
        }
    }
    if (g_cKeys >= CGJ_KEY_MAX) {
        return EAGAIN;
    }
    i = g_cKeys++;
    g_aDtors[i] = pfnDtor;
    g_aKeys[i] = NULL;
    *pKey = (pthread_key_t)i;
    return 0;
}

int
pthread_key_delete(pthread_key_t key)
{
    if (key >= (pthread_key_t)CGJ_KEY_MAX) {
        return EINVAL;
    }
    g_aKeys[key] = NULL;
    g_aDtors[key] = NULL;
    return 0;
}

int
pthread_setspecific(pthread_key_t key, const void *p)
{
    if (key >= (pthread_key_t)CGJ_KEY_MAX) {
        return EINVAL;
    }
    g_aKeys[key] = (void *)(uintptr_t)p;
    return 0;
}

void *
pthread_getspecific(pthread_key_t key)
{
    if (key >= (pthread_key_t)CGJ_KEY_MAX) {
        return NULL;
    }
    return g_aKeys[key];
}

int
pthread_rwlockattr_init(pthread_rwlockattr_t *pA)
{
    if (pA == NULL) {
        return EINVAL;
    }
    pA->nPshared = PTHREAD_PROCESS_PRIVATE;
    return 0;
}

int
pthread_rwlockattr_destroy(pthread_rwlockattr_t *pA)
{
    (void)pA;
    return 0;
}

int
pthread_rwlockattr_setpshared(pthread_rwlockattr_t *pA, int nPshared)
{
    if (pA == NULL) {
        return EINVAL;
    }
    if (nPshared != PTHREAD_PROCESS_PRIVATE) {
        return ENOSYS;
    }
    pA->nPshared = nPshared;
    return 0;
}

int
pthread_rwlockattr_getpshared(const pthread_rwlockattr_t *pA, int *pPshared)
{
    if (pA == NULL || pPshared == NULL) {
        return EINVAL;
    }
    *pPshared = pA->nPshared;
    return 0;
}

int
pthread_rwlock_init(pthread_rwlock_t *pRw, const pthread_rwlockattr_t *pA)
{
    if (pRw == NULL) {
        return EINVAL;
    }
    if (pA != NULL && pA->nPshared != PTHREAD_PROCESS_PRIVATE) {
        return ENOSYS;
    }
    (void)pthread_mutex_init(&pRw->mtx, NULL);
    (void)pthread_cond_init(&pRw->cv, NULL);
    pRw->nReaders = 0;
    pRw->nWriter = 0;
    pRw->nWritersWaiting = 0;
    return 0;
}

int
pthread_rwlock_destroy(pthread_rwlock_t *pRw)
{
    if (pRw == NULL) {
        return EINVAL;
    }
    (void)pthread_mutex_destroy(&pRw->mtx);
    (void)pthread_cond_destroy(&pRw->cv);
    return 0;
}

int
pthread_rwlock_rdlock(pthread_rwlock_t *pRw)
{
    if (pRw == NULL) {
        return EINVAL;
    }
    (void)pthread_mutex_lock(&pRw->mtx);
    while (pRw->nWriter != 0 || pRw->nWritersWaiting != 0) {
        (void)pthread_cond_wait(&pRw->cv, &pRw->mtx);
    }
    pRw->nReaders++;
    (void)pthread_mutex_unlock(&pRw->mtx);
    return 0;
}

int
pthread_rwlock_tryrdlock(pthread_rwlock_t *pRw)
{
    if (pRw == NULL) {
        return EINVAL;
    }
    if (pthread_mutex_trylock(&pRw->mtx) != 0) {
        return EBUSY;
    }
    if (pRw->nWriter != 0 || pRw->nWritersWaiting != 0) {
        (void)pthread_mutex_unlock(&pRw->mtx);
        return EBUSY;
    }
    pRw->nReaders++;
    (void)pthread_mutex_unlock(&pRw->mtx);
    return 0;
}

int
pthread_rwlock_wrlock(pthread_rwlock_t *pRw)
{
    if (pRw == NULL) {
        return EINVAL;
    }
    (void)pthread_mutex_lock(&pRw->mtx);
    pRw->nWritersWaiting++;
    while (pRw->nWriter != 0 || pRw->nReaders != 0) {
        (void)pthread_cond_wait(&pRw->cv, &pRw->mtx);
    }
    pRw->nWritersWaiting--;
    pRw->nWriter = 1;
    (void)pthread_mutex_unlock(&pRw->mtx);
    return 0;
}

int
pthread_rwlock_trywrlock(pthread_rwlock_t *pRw)
{
    if (pRw == NULL) {
        return EINVAL;
    }
    if (pthread_mutex_trylock(&pRw->mtx) != 0) {
        return EBUSY;
    }
    if (pRw->nWriter != 0 || pRw->nReaders != 0) {
        (void)pthread_mutex_unlock(&pRw->mtx);
        return EBUSY;
    }
    pRw->nWriter = 1;
    (void)pthread_mutex_unlock(&pRw->mtx);
    return 0;
}

int
pthread_rwlock_unlock(pthread_rwlock_t *pRw)
{
    if (pRw == NULL) {
        return EINVAL;
    }
    (void)pthread_mutex_lock(&pRw->mtx);
    if (pRw->nWriter != 0) {
        pRw->nWriter = 0;
    } else if (pRw->nReaders > 0) {
        pRw->nReaders--;
    }
    (void)pthread_cond_broadcast(&pRw->cv);
    (void)pthread_mutex_unlock(&pRw->mtx);
    return 0;
}

/* ---- spin locks --------------------------------------------------------- */

int
pthread_spin_init(pthread_spinlock_t *pS, int nPshared)
{
    if (pS == NULL) {
        return EINVAL;
    }
    if (nPshared != PTHREAD_PROCESS_PRIVATE) {
        return ENOSYS;
    }
    pS->nLock = 0;
    return 0;
}

int
pthread_spin_destroy(pthread_spinlock_t *pS)
{
    if (pS == NULL) {
        return EINVAL;
    }
    pS->nLock = 0;
    return 0;
}

int
pthread_spin_trylock(pthread_spinlock_t *pS)
{
    if (pS == NULL) {
        return EINVAL;
    }
    if (__sync_bool_compare_and_swap(&pS->nLock, 0, 1)) {
        return 0;
    }
    return EBUSY;
}

int
pthread_spin_lock(pthread_spinlock_t *pS)
{
    if (pS == NULL) {
        return EINVAL;
    }
    while (!__sync_bool_compare_and_swap(&pS->nLock, 0, 1)) {
        __asm__ volatile("pause" ::: "memory");
    }
    return 0;
}

int
pthread_spin_unlock(pthread_spinlock_t *pS)
{
    if (pS == NULL) {
        return EINVAL;
    }
    __sync_lock_release(&pS->nLock);
    return 0;
}

/* ---- barriers ----------------------------------------------------------- */

int
pthread_barrier_init(pthread_barrier_t *pB, const pthread_barrierattr_t *pA,
                     unsigned uCount)
{
    (void)pA;
    if (pB == NULL || uCount == 0) {
        return EINVAL;
    }
    (void)pthread_mutex_init(&pB->mtx, NULL);
    (void)pthread_cond_init(&pB->cv, NULL);
    pB->uCount = 0;
    pB->uTrip = uCount;
    pB->uGen = 0;
    return 0;
}

int
pthread_barrier_destroy(pthread_barrier_t *pB)
{
    if (pB == NULL) {
        return EINVAL;
    }
    (void)pthread_mutex_destroy(&pB->mtx);
    (void)pthread_cond_destroy(&pB->cv);
    return 0;
}

int
pthread_barrier_wait(pthread_barrier_t *pB)
{
    unsigned uGen;
    int nSerial = 0;

    if (pB == NULL) {
        return EINVAL;
    }
    (void)pthread_mutex_lock(&pB->mtx);
    uGen = pB->uGen;
    pB->uCount++;
    if (pB->uCount >= pB->uTrip) {
        pB->uCount = 0;
        pB->uGen++;
        nSerial = 1;
        (void)pthread_cond_broadcast(&pB->cv);
        (void)pthread_mutex_unlock(&pB->mtx);
        return PTHREAD_BARRIER_SERIAL_THREAD;
    }
    while (pB->uGen == uGen) {
        (void)pthread_cond_wait(&pB->cv, &pB->mtx);
    }
    (void)pthread_mutex_unlock(&pB->mtx);
    (void)nSerial;
    return 0;
}

/* ---- timed rwlock (busy-wait bring-up) ---------------------------------- */

static int
timespec_past(const struct timespec *pAbs)
{
    struct timespec now;

    if (pAbs == NULL) {
        return 1;
    }
    if (clock_gettime(CLOCK_REALTIME, &now) != 0) {
        return 1;
    }
    if (now.tv_sec > pAbs->tv_sec) {
        return 1;
    }
    if (now.tv_sec == pAbs->tv_sec && now.tv_nsec >= pAbs->tv_nsec) {
        return 1;
    }
    return 0;
}

int
pthread_rwlock_timedrdlock(pthread_rwlock_t *pRw, const struct timespec *pAbs)
{
    if (pRw == NULL) {
        return EINVAL;
    }
    for (;;) {
        if (pthread_rwlock_tryrdlock(pRw) == 0) {
            return 0;
        }
        if (timespec_past(pAbs)) {
            return ETIMEDOUT;
        }
        __asm__ volatile("pause" ::: "memory");
    }
}

int
pthread_rwlock_timedwrlock(pthread_rwlock_t *pRw, const struct timespec *pAbs)
{
    if (pRw == NULL) {
        return EINVAL;
    }
    for (;;) {
        if (pthread_rwlock_trywrlock(pRw) == 0) {
            return 0;
        }
        if (timespec_past(pAbs)) {
            return ETIMEDOUT;
        }
        __asm__ volatile("pause" ::: "memory");
    }
}

/* Linux prctl name ops (self only for bring-up when tid != self) */
#define NR_prctl 157
#define PR_SET_NAME 15
#define PR_GET_NAME 16

int
pthread_setname_np(pthread_t tid, const char *szName)
{
    char a[16];
    size_t i;
    long r;

    if (szName == NULL) {
        return EINVAL;
    }
    /* Only self for bring-up (matches common restriction without /proc) */
    if (tid != 0 && tid != pthread_self()) {
        return ESRCH;
    }
    for (i = 0; i < 15 && szName[i] != '\0'; i++) {
        a[i] = szName[i];
    }
    a[i] = '\0';
    r = sys6(NR_prctl, PR_SET_NAME, (long)(uintptr_t)a, 0, 0, 0, 0);
    if (r < 0) {
        return (int)(-r);
    }
    return 0;
}

int
pthread_getname_np(pthread_t tid, char *szName, size_t cb)
{
    long r;

    if (szName == NULL || cb == 0) {
        return EINVAL;
    }
    if (tid != 0 && tid != pthread_self()) {
        return ESRCH;
    }
    if (cb < 16) {
        /* glibc requires at least 16 */
        return ERANGE;
    }
    r = sys6(NR_prctl, PR_GET_NAME, (long)(uintptr_t)szName, 0, 0, 0, 0);
    if (r < 0) {
        return (int)(-r);
    }
    szName[15] = '\0';
    return 0;
}

int
pthread_setaffinity_np(pthread_t tid, size_t cbSet, const cpu_set_t *pSet)
{
    pid_t pid;

    if (pSet == NULL || cbSet == 0) {
        return EINVAL;
    }
    pid = (pid_t)tid;
    if (tid == 0) {
        pid = 0; /* current thread */
    }
    if (sched_setaffinity(pid, cbSet, pSet) != 0) {
        return errno;
    }
    return 0;
}

int
pthread_getaffinity_np(pthread_t tid, size_t cbSet, cpu_set_t *pSet)
{
    pid_t pid;

    if (pSet == NULL || cbSet == 0) {
        return EINVAL;
    }
    pid = (pid_t)tid;
    if (tid == 0) {
        pid = 0;
    }
    if (sched_getaffinity(pid, cbSet, pSet) != 0) {
        return errno;
    }
    return 0;
}
