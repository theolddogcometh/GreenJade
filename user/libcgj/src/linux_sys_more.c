/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * More Linux desktop graph SYSCALL wrappers: vfork, clone (fn form), fexecve,
 * openat2, mlock2, pkey_*, swapon/swapoff, quotactl, process_madvise,
 * userfaultfd, seccomp, getdents64, prlimit64.
 * Clean-room public ABI — no GPL source. Integer/pointer only (no SSE).
 */
#include <errno.h>
#include <fcntl.h>
#include <sched.h>
#include <signal.h>
#include <stdint.h>
#include <sys/mman.h>
#include <sys/quota.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

#define NR_clone           56
#define NR_vfork           58
#define NR_exit            60
#define NR_swapon          167
#define NR_swapoff         168
#define NR_quotactl        179
#define NR_getdents64      217
#define NR_prlimit64       302
#define NR_seccomp         317
#define NR_userfaultfd     323
#define NR_mlock2          325
#define NR_pkey_mprotect   329
#define NR_pkey_alloc      330
#define NR_pkey_free       331
#define NR_execveat        322
#define NR_openat2         437
#define NR_process_madvise 440

#ifndef AT_EMPTY_PATH
#define AT_EMPTY_PATH 0x1000
#endif

#ifndef SIGCHLD
#define SIGCHLD 17
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

pid_t
vfork(void)
{
    return (pid_t)sys_ret(sys6(NR_vfork, 0, 0, 0, 0, 0, 0));
}

/*
 * clone(fn, stack, flags, arg, ...): Linux glibc-shaped.
 * Optional trailing args: pid_t *ptid, void *tls, pid_t *ctid.
 * Child stack must be non-NULL when fn is non-NULL (aligned top).
 * Child runs fn(arg) then _exit; parent returns child tid.
 */
int
clone(int (*fn)(void *), void *pStack, int nFlags, void *pArg, ...)
{
    pid_t *pPtid = 0;
    void *pTls = 0;
    pid_t *pCtid = 0;
    long ret;
    uintptr_t *pTop;

    if (fn != NULL) {
        __builtin_va_list ap;

        __builtin_va_start(ap, pArg);
        if (nFlags & (CLONE_PARENT_SETTID | CLONE_CHILD_SETTID | CLONE_CHILD_CLEARTID |
                      CLONE_SETTLS)) {
            pPtid = __builtin_va_arg(ap, pid_t *);
            pTls = __builtin_va_arg(ap, void *);
            pCtid = __builtin_va_arg(ap, pid_t *);
        }
        __builtin_va_end(ap);

        if (pStack == NULL) {
            errno = EINVAL;
            return -1;
        }
        pTop = (uintptr_t *)((((uintptr_t)pStack) & ~(uintptr_t)15) - 16);
        /* child lands here after syscall with RSP=pTop; fake retaddr path */
        pTop[0] = (uintptr_t)pArg;
        pTop[1] = (uintptr_t)fn;

        /* x86_64: flags, stack, parent_tid, child_tid, tls */
        ret = sys6(NR_clone, (long)nFlags, (long)(uintptr_t)pTop,
                   (long)(uintptr_t)pPtid, (long)(uintptr_t)pCtid,
                   (long)(uintptr_t)pTls, 0);
        if (ret == 0) {
            /* Child: reconstruct fn/arg from stack slots we left */
            void *arg;
            int (*child_fn)(void *);
            int st;

            __asm__ volatile("mov 0(%%rsp), %0\n\t"
                             "mov 8(%%rsp), %1"
                             : "=r"(arg), "=r"(child_fn));
            st = child_fn(arg);
            (void)sys6(NR_exit, st, 0, 0, 0, 0, 0);
            for (;;) {
            }
        }
        return (int)sys_ret(ret);
    }

    /* fn NULL: raw clone (flags, stack, ptid, ctid, tls) via optional args */
    {
        __builtin_va_list ap;

        __builtin_va_start(ap, pArg);
        pPtid = __builtin_va_arg(ap, pid_t *);
        pCtid = __builtin_va_arg(ap, pid_t *);
        pTls = __builtin_va_arg(ap, void *);
        __builtin_va_end(ap);
    }
    ret = sys6(NR_clone, (long)nFlags, (long)(uintptr_t)pStack,
               (long)(uintptr_t)pPtid, (long)(uintptr_t)pCtid,
               (long)(uintptr_t)pTls, 0);
    return (int)sys_ret(ret);
}

int
fexecve(int nFd, char *const aArgv[], char *const aEnvp[])
{
    int r;

    r = (int)sys_ret(sys6(NR_execveat, nFd, (long)(uintptr_t)"",
                          (long)(uintptr_t)aArgv, (long)(uintptr_t)aEnvp,
                          AT_EMPTY_PATH, 0));
    if (r < 0 && errno == ENOSYS) {
        /* no fallback without /proc */
        errno = ENOSYS;
    }
    return r;
}

int
openat2(int nDfd, const char *szPath, const struct open_how *pHow, size_t cb)
{
    return (int)sys_ret(sys6(NR_openat2, nDfd, (long)(uintptr_t)szPath,
                             (long)(uintptr_t)pHow, (long)cb, 0, 0));
}

int
mlock2(const void *pAddr, size_t cb, unsigned int uFlags)
{
    return (int)sys_ret(sys6(NR_mlock2, (long)(uintptr_t)pAddr, (long)cb,
                             (long)uFlags, 0, 0, 0));
}

int
pkey_alloc(unsigned int uFlags, unsigned int uAccessRights)
{
    return (int)sys_ret(sys6(NR_pkey_alloc, (long)uFlags, (long)uAccessRights,
                             0, 0, 0, 0));
}

int
pkey_free(int nPkey)
{
    return (int)sys_ret(sys6(NR_pkey_free, nPkey, 0, 0, 0, 0, 0));
}

int
pkey_mprotect(void *pAddr, size_t cb, int nProt, int nPkey)
{
    return (int)sys_ret(sys6(NR_pkey_mprotect, (long)(uintptr_t)pAddr, (long)cb,
                             nProt, nPkey, 0, 0));
}

int
swapon(const char *szPath, int nFlags)
{
    return (int)sys_ret(sys6(NR_swapon, (long)(uintptr_t)szPath, nFlags, 0, 0,
                             0, 0));
}

int
swapoff(const char *szPath)
{
    return (int)sys_ret(sys6(NR_swapoff, (long)(uintptr_t)szPath, 0, 0, 0, 0,
                             0));
}

int
quotactl(int nCmd, const char *szSpecial, int nId, caddr_t pAddr)
{
    return (int)sys_ret(sys6(NR_quotactl, nCmd, (long)(uintptr_t)szSpecial,
                             nId, (long)(uintptr_t)pAddr, 0, 0));
}

ssize_t
process_madvise(int nPidfd, const struct iovec *pIov, size_t nVlen, int nAdvice,
                unsigned int uFlags)
{
    return (ssize_t)sys_ret(sys6(NR_process_madvise, nPidfd,
                                 (long)(uintptr_t)pIov, (long)nVlen, nAdvice,
                                 (long)uFlags, 0));
}

int
userfaultfd(int nFlags)
{
    return (int)sys_ret(sys6(NR_userfaultfd, nFlags, 0, 0, 0, 0, 0));
}

int
seccomp(unsigned int uOp, unsigned int uFlags, void *pArgs)
{
    return (int)sys_ret(sys6(NR_seccomp, (long)uOp, (long)uFlags,
                             (long)(uintptr_t)pArgs, 0, 0, 0));
}

ssize_t
getdents64(int nFd, void *pDirp, size_t cb)
{
    return (ssize_t)sys_ret(sys6(NR_getdents64, nFd, (long)(uintptr_t)pDirp,
                                (long)cb, 0, 0, 0));
}

int
prlimit64(pid_t nPid, int nResource, const struct rlimit *pNew,
          struct rlimit *pOld)
{
    return (int)sys_ret(sys6(NR_prlimit64, nPid, nResource,
                             (long)(uintptr_t)pNew, (long)(uintptr_t)pOld, 0,
                             0));
}
