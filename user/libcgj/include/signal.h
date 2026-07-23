/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped signal.h (subset). Not GNU glibc.
 */
#pragma once

#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int sig_atomic_t;

/* Linux x86_64: 1024 signals → 16 × unsigned long */
typedef struct {
    unsigned long __val[16];
} sigset_t;

#define SIG_DFL ((void (*)(int))0)
#define SIG_IGN ((void (*)(int))1)
#define SIG_ERR ((void (*)(int))-1)

#define SIGHUP   1
#define SIGINT   2
#define SIGQUIT  3
#define SIGILL   4
#define SIGTRAP  5
#define SIGABRT  6
#define SIGBUS   7
#define SIGFPE   8
#define SIGKILL  9
#define SIGUSR1 10
#define SIGSEGV 11
#define SIGUSR2 12
#define SIGPIPE 13
#define SIGALRM 14
#define SIGTERM 15
#define SIGCHLD 17
#define SIGCONT 18
#define SIGSTOP 19
#define SIGTSTP 20

typedef void (*sighandler_t)(int);

typedef union sigval {
    int   sival_int;
    void *sival_ptr;
} sigval_t;

#define SIGEV_SIGNAL    0
#define SIGEV_NONE      1
#define SIGEV_THREAD    2
#define SIGEV_THREAD_ID 4

/* Linux x86_64 sigevent is 64 bytes */
struct sigevent {
    sigval_t sigev_value;
    int      sigev_signo;
    int      sigev_notify;
    union {
        int   _pad[12];
        int   _tid;
        struct {
            void (*_function)(sigval_t);
            void *_attribute; /* pthread_attr_t * */
        } _sigev_thread;
    } _sigev_un;
};

#define sigev_notify_function   _sigev_un._sigev_thread._function
#define sigev_notify_attributes _sigev_un._sigev_thread._attribute
#define sigev_notify_thread_id  _sigev_un._tid

typedef struct {
    int      si_signo;
    int      si_errno;
    int      si_code;
    int      __pad0;
    pid_t    si_pid;
    uid_t    si_uid;
    sigval_t si_value;
    int      si_status;
    long     si_band;
    void    *si_addr;
    int      si_fd;
    short    si_addr_lsb;
    short    __pad2;
    void    *si_call_addr;
    int      si_syscall;
    unsigned si_arch;
    char     __pad[40];
} siginfo_t;

#define SI_USER   0
#define SI_KERNEL 0x80
#define SI_QUEUE  (-1)
#define SI_TIMER  (-2)
#define SI_MESGQ  (-3)
#define SI_ASYNCIO (-4)
#define SI_SIGIO  (-5)
#define SI_TKILL  (-6)

/* sigprocmask how */
#define SIG_BLOCK   0
#define SIG_UNBLOCK 1
#define SIG_SETMASK 2

/* sa_flags */
#define SA_NOCLDSTOP 0x00000001
#define SA_NOCLDWAIT 0x00000002
#define SA_SIGINFO   0x00000004
#define SA_ONSTACK   0x08000000
#define SA_RESTART   0x10000000
#define SA_NODEFER   0x40000000
#define SA_RESETHAND 0x80000000

struct sigaction {
    sighandler_t sa_handler;
    unsigned long sa_flags;
    void (*sa_restorer)(void);
    sigset_t sa_mask;
};

sighandler_t signal(int nSig, sighandler_t pHandler);
sighandler_t bsd_signal(int nSig, sighandler_t pHandler);
sighandler_t sysv_signal(int nSig, sighandler_t pHandler);
sighandler_t __sysv_signal(int nSig, sighandler_t pHandler);
int          sigblock(int nMask);
int          sigsetmask(int nMask);
int          siggetmask(void);
int          sigpause(int nSig);
int          __sigpause(int nSig);
int          raise(int nSig);
int          kill(int nPid, int nSig);
int          tgkill(int nTgid, int nTid, int nSig);
int          sigemptyset(sigset_t *pSet);
int          sigfillset(sigset_t *pSet);
int          sigaddset(sigset_t *pSet, int nSig);
int          sigdelset(sigset_t *pSet, int nSig);
int          sigismember(const sigset_t *pSet, int nSig);
int          sigprocmask(int nHow, const sigset_t *pSet, sigset_t *pOldset);
int          sigaction(int nSig, const struct sigaction *pAct,
                       struct sigaction *pOldact);
int          sigpending(sigset_t *pSet);
int          sigsuspend(const sigset_t *pSet);
int          sigwait(const sigset_t *pSet, int *pSig);
int          sigwaitinfo(const sigset_t *pSet, siginfo_t *pInfo);
struct timespec;
int          sigtimedwait(const sigset_t *pSet, siginfo_t *pInfo,
                          const struct timespec *pTimeout);
int          sigqueue(pid_t nPid, int nSig, const union sigval value);

/* Alternate signal stack (also used by ucontext) */
#ifndef _CGJ_STACK_T_DEFINED
#define _CGJ_STACK_T_DEFINED
typedef struct {
    void  *ss_sp;
    int    ss_flags;
    size_t ss_size;
} stack_t;
#endif
#ifndef SS_ONSTACK
#define SS_ONSTACK 1
#endif
#ifndef SS_DISABLE
#define SS_DISABLE 2
#endif
int          sigaltstack(const stack_t *pSs, stack_t *pOss);
int          sigisemptyset(const sigset_t *pSet);
int          sigorset(sigset_t *pDst, const sigset_t *pA, const sigset_t *pB);
int          sigandset(sigset_t *pDst, const sigset_t *pA, const sigset_t *pB);
int          sighold(int nSig);
int          sigrelse(int nSig);
int          sigignore(int nSig);
void       (*sigset(int nSig, void (*pfn)(int)))(int);
int          gsignal(int nSig);
int          ssignal(int nSig, void (*pfn)(int));

#ifdef __cplusplus
}
#endif
