/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * sigset helpers + sigprocmask + sigaction (kernel pass-through, soft deepen).
 */
#include <errno.h>
#include <signal.h>
#include <stdint.h>
#include <string.h>

#define NR_rt_sigaction   13
#define NR_rt_sigprocmask 14
#define SIGSET_NWORDS     16
/* Linux kernel sigset is _NSIG/8 bytes (64 signals → 8); userspace is larger */
#define KERNEL_SIGSET_SZ  8
/* Userspace sigset supports up to 1024; kernel only honors first 64. */
#define CGJ_SIG_MAX       1024

/* Linux kernel sigaction layout (x86_64): mask follows restorer; kernel
 * only consumes KERNEL_SIGSET_SZ bytes of mask via the size argument. */
struct kernel_sigaction {
    void          *ksa_handler;
    unsigned long  ksa_flags;
    void          *ksa_restorer;
    unsigned long  ksa_mask[SIGSET_NWORDS];
};

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

static int
sig_valid(int nSig)
{
    return nSig >= 1 && nSig <= CGJ_SIG_MAX;
}

int
sigemptyset(sigset_t *pSet)
{
    size_t i;

    if (pSet == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; i < SIGSET_NWORDS; i++) {
        pSet->__val[i] = 0UL;
    }
    return 0;
}

int
sigfillset(sigset_t *pSet)
{
    size_t i;

    if (pSet == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; i < SIGSET_NWORDS; i++) {
        pSet->__val[i] = ~0UL;
    }
    return 0;
}

int
sigaddset(sigset_t *pSet, int nSig)
{
    unsigned uBit;
    size_t iWord;

    if (pSet == NULL || !sig_valid(nSig)) {
        errno = EINVAL;
        return -1;
    }
    uBit = (unsigned)(nSig - 1);
    iWord = uBit / (8u * (unsigned)sizeof(unsigned long));
    if (iWord >= SIGSET_NWORDS) {
        errno = EINVAL;
        return -1;
    }
    pSet->__val[iWord] |=
        1UL << (uBit % (8u * (unsigned)sizeof(unsigned long)));
    return 0;
}

int
sigdelset(sigset_t *pSet, int nSig)
{
    unsigned uBit;
    size_t iWord;

    if (pSet == NULL || !sig_valid(nSig)) {
        errno = EINVAL;
        return -1;
    }
    uBit = (unsigned)(nSig - 1);
    iWord = uBit / (8u * (unsigned)sizeof(unsigned long));
    if (iWord >= SIGSET_NWORDS) {
        errno = EINVAL;
        return -1;
    }
    pSet->__val[iWord] &=
        ~(1UL << (uBit % (8u * (unsigned)sizeof(unsigned long))));
    return 0;
}

int
sigismember(const sigset_t *pSet, int nSig)
{
    unsigned uBit;
    size_t iWord;
    unsigned long uMask;

    if (pSet == NULL || !sig_valid(nSig)) {
        errno = EINVAL;
        return -1;
    }
    uBit = (unsigned)(nSig - 1);
    iWord = uBit / (8u * (unsigned)sizeof(unsigned long));
    if (iWord >= SIGSET_NWORDS) {
        errno = EINVAL;
        return -1;
    }
    uMask = 1UL << (uBit % (8u * (unsigned)sizeof(unsigned long)));
    return (pSet->__val[iWord] & uMask) ? 1 : 0;
}

int
sigprocmask(int nHow, const sigset_t *pSet, sigset_t *pOldset)
{
    long r;

    if (nHow != SIG_BLOCK && nHow != SIG_UNBLOCK && nHow != SIG_SETMASK) {
        errno = EINVAL;
        return -1;
    }
    if (pSet == NULL && pOldset == NULL) {
        return 0;
    }
    r = sys6(NR_rt_sigprocmask, nHow, (long)(uintptr_t)pSet,
             (long)(uintptr_t)pOldset, KERNEL_SIGSET_SZ, 0, 0);
    return sys_ret(r);
}

int
sigaction(int nSig, const struct sigaction *pAct, struct sigaction *pOldact)
{
    struct kernel_sigaction ksa;
    struct kernel_sigaction kold;
    long r;
    size_t i;

    if (nSig < 1 || nSig == SIGKILL || nSig == SIGSTOP || nSig > CGJ_SIG_MAX) {
        errno = EINVAL;
        return -1;
    }
    if (pAct == NULL && pOldact == NULL) {
        errno = EINVAL;
        return -1;
    }
    memset(&ksa, 0, sizeof(ksa));
    memset(&kold, 0, sizeof(kold));
    if (pAct != NULL) {
        ksa.ksa_handler = (void *)(uintptr_t)pAct->sa_handler;
        ksa.ksa_flags = (unsigned long)pAct->sa_flags;
        /* Pass restorer if caller set one; soft path has no default restorer. */
        ksa.ksa_restorer = (void *)(uintptr_t)pAct->sa_restorer;
        for (i = 0; i < SIGSET_NWORDS; i++) {
            ksa.ksa_mask[i] = pAct->sa_mask.__val[i];
        }
    }
    r = sys6(NR_rt_sigaction, nSig, pAct ? (long)(uintptr_t)&ksa : 0,
             pOldact ? (long)(uintptr_t)&kold : 0, KERNEL_SIGSET_SZ, 0, 0);
    if (sys_ret(r) < 0) {
        return -1;
    }
    if (pOldact != NULL) {
        memset(pOldact, 0, sizeof(*pOldact));
        pOldact->sa_handler = (sighandler_t)kold.ksa_handler;
        pOldact->sa_flags = (unsigned long)kold.ksa_flags;
        pOldact->sa_restorer = (void (*)(void))(uintptr_t)kold.ksa_restorer;
        for (i = 0; i < SIGSET_NWORDS; i++) {
            pOldact->sa_mask.__val[i] = kold.ksa_mask[i];
        }
    }
    return 0;
}
