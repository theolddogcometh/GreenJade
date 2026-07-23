/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch: mremap, preadv2/pwritev2, epoll_pwait2,
 * process_mrelease, clone3, exit_group/tkill/futex, module/landlock stubs,
 * tmpnam/getw/psignal, gnu_get_libc_*, locale _l string wrappers, etc.
 * Integer/pointer only (no SSE doubles).
 *
 * greppable: CGJ_GRAPH_BATCH3_SOFT_NULL
 * greppable: CGJ_GRAPH_BATCH3_SOFT_ARGS
 * greppable: CGJ_GRAPH_BATCH3_SOFT_EDGE
 *
 * Soft deepen: null/arg guards on user-facing graph nodes; edge
 * hardening only. No multi-def; no API break. Pure C integer/pointer.
 */
#include <errno.h>
#include <fcntl.h>
#include <langinfo.h>
#include <locale.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

#define NR_mremap           25
#define NR_exit_group       231
#define NR_tkill            200
#define NR_futex            202
#define NR_set_tid_address  218
#define NR_ioperm           173
#define NR_iopl             172
#define NR_setfsuid         122
#define NR_setfsgid         123
#define NR_vhangup          153
#define NR_settimeofday     164
#define NR_init_module      175
#define NR_delete_module    176
#define NR_preadv2          327
#define NR_pwritev2         328
#define NR_kcmp             312
#define NR_finit_module     313
#define NR_epoll_pwait2     441
#define NR_process_mrelease 448
#define NR_clone3           435
#define NR_landlock_create_ruleset 444
#define NR_landlock_add_rule       445
#define NR_landlock_restrict_self  446
#define NR_open_tree        428
#define NR_move_mount       429
#define NR_eventfd2         290
#define NR_signalfd4        289
#define NR_getdents64       217

#ifndef MREMAP_MAYMOVE
#define MREMAP_MAYMOVE 1
#endif
#ifndef MREMAP_FIXED
#define MREMAP_FIXED 2
#endif
#ifndef L_tmpnam
#define L_tmpnam 20
#endif
#ifndef CPU_ALLOC_SIZE
#define CPU_ALLOC_SIZE(count) \
    ((((size_t)(count) + 63u) / 64u) * sizeof(unsigned long))
#endif
#ifndef O_SYNC
#define O_SYNC 04010000
#endif

/* Opaque bpf attribute blob (graph node only). */
union bpf_attr {
    int __unused;
};

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

/* ---- mremap / uio v2 / epoll_pwait2 ------------------------------------ */

void *
mremap(void *pOld, size_t cbOld, size_t cbNew, int nFlags, ...)
{
    void *pNew = NULL;
    long r;

    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    /* Soft: fixed remap needs a target; zero new size is kernel-defined. */
    if ((nFlags & MREMAP_FIXED) != 0 && (nFlags & MREMAP_MAYMOVE) == 0 &&
        pOld == NULL) {
        errno = EINVAL;
        return MAP_FAILED;
    }
    if (nFlags & MREMAP_FIXED) {
        va_list ap;

        va_start(ap, nFlags);
        pNew = va_arg(ap, void *);
        va_end(ap);
    }
    r = sys6(NR_mremap, (long)(uintptr_t)pOld, (long)cbOld, (long)cbNew,
             nFlags, (long)(uintptr_t)pNew, 0);
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return MAP_FAILED;
    }
    return (void *)(uintptr_t)r;
}

ssize_t
preadv2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off, int nFlags)
{
    long r;

    /* greppable: CGJ_GRAPH_BATCH3_SOFT_ARGS */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pIov == NULL && nIovcnt != 0) {
        errno = EFAULT;
        return -1;
    }
    if (nIovcnt < 0) {
        errno = EINVAL;
        return -1;
    }
    r = sys6(NR_preadv2, nFd, (long)(uintptr_t)pIov, nIovcnt, (long)off, 0,
             nFlags);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        if (nFlags == 0) {
            return preadv(nFd, pIov, nIovcnt, off);
        }
        errno = ENOSYS;
        return -1;
    }
    return (ssize_t)sys_ret(r);
}

ssize_t
pwritev2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off, int nFlags)
{
    long r;

    /* greppable: CGJ_GRAPH_BATCH3_SOFT_ARGS */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pIov == NULL && nIovcnt != 0) {
        errno = EFAULT;
        return -1;
    }
    if (nIovcnt < 0) {
        errno = EINVAL;
        return -1;
    }
    r = sys6(NR_pwritev2, nFd, (long)(uintptr_t)pIov, nIovcnt, (long)off, 0,
             nFlags);
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        if (nFlags == 0) {
            return pwritev(nFd, pIov, nIovcnt, off);
        }
        errno = ENOSYS;
        return -1;
    }
    return (ssize_t)sys_ret(r);
}

int
epoll_pwait2(int nEpfd, struct epoll_event *pEvents, int nMaxevents,
             const struct timespec *pTimeout, const void *pSigmask)
{
    long r;
    int ms = -1;

    /* greppable: CGJ_GRAPH_BATCH3_SOFT_ARGS */
    if (nEpfd < 0) {
        errno = EBADF;
        return -1;
    }
    if (nMaxevents <= 0) {
        errno = EINVAL;
        return -1;
    }
    if (pEvents == NULL) {
        errno = EFAULT;
        return -1;
    }
    r = sys6(NR_epoll_pwait2, nEpfd, (long)(uintptr_t)pEvents, nMaxevents,
             (long)(uintptr_t)pTimeout, (long)(uintptr_t)pSigmask,
             (long)sizeof(sigset_t));
    if (r < 0 && r > -4096 && (int)(-r) == ENOSYS) {
        if (pTimeout != NULL) {
            ms = (int)(pTimeout->tv_sec * 1000L + pTimeout->tv_nsec / 1000000L);
            if (ms < 0) {
                ms = -1;
            }
        }
        return epoll_pwait(nEpfd, pEvents, nMaxevents, ms, pSigmask);
    }
    return (int)sys_ret(r);
}

int
process_mrelease(int nPidfd, unsigned int uFlags)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_ARGS */
    if (nPidfd < 0) {
        errno = EBADF;
        return -1;
    }
    return (int)sys_ret(sys6(NR_process_mrelease, nPidfd, (long)uFlags, 0, 0, 0,
                             0));
}

struct clone_args {
    uint64_t flags;
    uint64_t pidfd;
    uint64_t child_tid;
    uint64_t parent_tid;
    uint64_t exit_signal;
    uint64_t stack;
    uint64_t stack_size;
    uint64_t tls;
    uint64_t set_tid;
    uint64_t set_tid_size;
    uint64_t cgroup;
};

long
clone3(struct clone_args *pArgs, size_t cbArgs)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (pArgs == NULL || cbArgs == 0) {
        errno = EINVAL;
        return -1;
    }
    return sys_ret(sys6(NR_clone3, (long)(uintptr_t)pArgs, (long)cbArgs, 0, 0,
                        0, 0));
}

/* ---- process / privilege / module -------------------------------------- */

void
exit_group(int nStatus)
{
    (void)sys6(NR_exit_group, nStatus, 0, 0, 0, 0, 0);
    for (;;) {
        __asm__ volatile("hlt");
    }
}

int
tkill(int nTid, int nSig)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_ARGS */
    if (nTid <= 0) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_tkill, nTid, nSig, 0, 0, 0, 0));
}

long
futex(uint32_t *pUaddr, int nOp, uint32_t uVal, const struct timespec *pTimeout,
      uint32_t *pUaddr2, uint32_t uVal3)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (pUaddr == NULL) {
        errno = EFAULT;
        return -1;
    }
    (void)pUaddr2;
    return sys_ret(sys6(NR_futex, (long)(uintptr_t)pUaddr, nOp, (long)uVal,
                        (long)(uintptr_t)pTimeout, (long)(uintptr_t)pUaddr2,
                        (long)uVal3));
}

long
set_tid_address(int *pTidptr)
{
    return sys_ret(sys6(NR_set_tid_address, (long)(uintptr_t)pTidptr, 0, 0, 0,
                        0, 0));
}

int
ioperm(unsigned long uFrom, unsigned long uNum, int nTurnOn)
{
    return (int)sys_ret(sys6(NR_ioperm, (long)uFrom, (long)uNum, nTurnOn, 0, 0,
                             0));
}

int
iopl(int nLevel)
{
    return (int)sys_ret(sys6(NR_iopl, nLevel, 0, 0, 0, 0, 0));
}

int
setfsuid(uid_t uid)
{
    return (int)sys_ret(sys6(NR_setfsuid, (long)uid, 0, 0, 0, 0, 0));
}

int
setfsgid(gid_t gid)
{
    return (int)sys_ret(sys6(NR_setfsgid, (long)gid, 0, 0, 0, 0, 0));
}

int
vhangup(void)
{
    return (int)sys_ret(sys6(NR_vhangup, 0, 0, 0, 0, 0, 0));
}

int
settimeofday(const struct timeval *pTv, const void *pTz)
{
    return (int)sys_ret(sys6(NR_settimeofday, (long)(uintptr_t)pTv,
                             (long)(uintptr_t)pTz, 0, 0, 0, 0));
}

int
stime(const time_t *pT)
{
    struct timeval tv;

    if (pT == NULL) {
        errno = EFAULT;
        return -1;
    }
    tv.tv_sec = *pT;
    tv.tv_usec = 0;
    return settimeofday(&tv, NULL);
}

useconds_t
ualarm(useconds_t uValue, useconds_t uInterval)
{
    struct itimerval itNew;
    struct itimerval itOld;

    (void)memset(&itNew, 0, sizeof(itNew));
    itNew.it_value.tv_sec = (time_t)(uValue / 1000000u);
    itNew.it_value.tv_usec = (long)(uValue % 1000000u);
    itNew.it_interval.tv_sec = (time_t)(uInterval / 1000000u);
    itNew.it_interval.tv_usec = (long)(uInterval % 1000000u);
    if (setitimer(ITIMER_REAL, &itNew, &itOld) != 0) {
        return 0;
    }
    return (useconds_t)(itOld.it_value.tv_sec * 1000000L +
                        itOld.it_value.tv_usec);
}

int
init_module(void *pModuleImage, unsigned long cbLen, const char *szParamValues)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (pModuleImage == NULL || cbLen == 0) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_init_module, (long)(uintptr_t)pModuleImage,
                             (long)cbLen, (long)(uintptr_t)szParamValues, 0, 0,
                             0));
}

int
delete_module(const char *szName, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (szName == NULL || szName[0] == '\0') {
        errno = ENOENT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_delete_module, (long)(uintptr_t)szName, nFlags,
                             0, 0, 0, 0));
}

int
finit_module(int nFd, const char *szParamValues, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_ARGS */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    return (int)sys_ret(sys6(NR_finit_module, nFd, (long)(uintptr_t)szParamValues,
                             nFlags, 0, 0, 0));
}

int
kcmp(pid_t nPid1, pid_t nPid2, int nType, unsigned long uIdx1,
     unsigned long uIdx2)
{
    return (int)sys_ret(sys6(NR_kcmp, nPid1, nPid2, nType, (long)uIdx1,
                             (long)uIdx2, 0));
}

int
bpf(int nCmd, union bpf_attr *pAttr, unsigned uSize)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */

    (void)nCmd;
    (void)pAttr;
    (void)uSize;
    errno = ENOSYS;
    return -1;
}

/* landlock / mount_api graph nodes */
int
landlock_create_ruleset(const void *pAttr, size_t cb, uint32_t uFlags)
{
    return (int)sys_ret(sys6(NR_landlock_create_ruleset, (long)(uintptr_t)pAttr,
                             (long)cb, (long)uFlags, 0, 0, 0));
}

int
landlock_add_rule(int nRulesetFd, int nRuleType, const void *pAttr,
                  uint32_t uFlags)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (nRulesetFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pAttr == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_landlock_add_rule, nRulesetFd, nRuleType,
                             (long)(uintptr_t)pAttr, (long)uFlags, 0, 0));
}

int
landlock_restrict_self(int nRulesetFd, uint32_t uFlags)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (nRulesetFd < 0) {
        errno = EBADF;
        return -1;
    }

    return (int)sys_ret(sys6(NR_landlock_restrict_self, nRulesetFd,
                             (long)uFlags, 0, 0, 0, 0));
}

int
open_tree(int nDfd, const char *szPath, unsigned int uFlags)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_open_tree, nDfd, (long)(uintptr_t)szPath,
                             (long)uFlags, 0, 0, 0));
}

int
move_mount(int nFromDfd, const char *szFromPath, int nToDfd,
           const char *szToPath, unsigned int uFlags)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (szFromPath == NULL || szToPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    return (int)sys_ret(sys6(NR_move_mount, nFromDfd, (long)(uintptr_t)szFromPath,
                             nToDfd, (long)(uintptr_t)szToPath, (long)uFlags,
                             0));
}

int
eventfd2(unsigned int uInitval, int nFlags)
{
    return (int)sys_ret(sys6(NR_eventfd2, (long)uInitval, nFlags, 0, 0, 0, 0));
}

int
signalfd4(int nFd, const sigset_t *pMask, size_t cbMask, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (pMask == NULL || cbMask == 0) {
        errno = EINVAL;
        return -1;
    }
    return (int)sys_ret(sys6(NR_signalfd4, nFd, (long)(uintptr_t)pMask,
                             (long)cbMask, nFlags, 0, 0));
}

/* ---- tmpnam / getw / psignal / xmknod / sched cpualloc ----------------- */

cpu_set_t *
__sched_cpualloc(size_t nCount)
{
    size_t cb;
    cpu_set_t *p;

    if (nCount == 0) {
        nCount = 1;
    }
    cb = CPU_ALLOC_SIZE(nCount);
    p = (cpu_set_t *)malloc(cb);
    if (p != NULL) {
        (void)memset(p, 0, cb);
    }
    return p;
}

void
__sched_cpufree(cpu_set_t *pSet)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (pSet == NULL) {
        return;
    }
    free(pSet);
}

int
getw(FILE *pF)
{
    int v = 0;
    size_t n;

    if (pF == NULL) {
        return EOF;
    }
    n = fread(&v, 1, sizeof(v), pF);
    if (n != sizeof(v)) {
        return EOF;
    }
    return v;
}

int
putw(int nW, FILE *pF)
{
    if (pF == NULL) {
        return EOF;
    }
    if (fwrite(&nW, 1, sizeof(nW), pF) != sizeof(nW)) {
        return EOF;
    }
    return 0;
}

char *
tmpnam(char *szBuf)
{
    static char aStatic[L_tmpnam > 0 ? L_tmpnam : 20];
    static unsigned uSeq;
    char *p = (szBuf != NULL) ? szBuf : aStatic;
    pid_t pid = getpid();

    uSeq++;
    (void)snprintf(p, (size_t)(L_tmpnam > 0 ? L_tmpnam : 20),
                   "/tmp/gj%lx%x", (unsigned long)pid, uSeq);
    return p;
}

char *
tmpnam_r(char *szBuf)
{
    if (szBuf == NULL) {
        return NULL;
    }
    return tmpnam(szBuf);
}

char *
tempnam(const char *szDir, const char *szPfx)
{
    char aPath[256];
    const char *szD = (szDir != NULL && szDir[0] != '\0') ? szDir : "/tmp";
    const char *szP = (szPfx != NULL && szPfx[0] != '\0') ? szPfx : "gj";
    static unsigned uSeq;
    char *p;

    uSeq++;
    (void)snprintf(aPath, sizeof(aPath), "%s/%s%lx%x", szD, szP,
                   (unsigned long)getpid(), uSeq);
    p = (char *)malloc(strlen(aPath) + 1);
    if (p == NULL) {
        return NULL;
    }
    memcpy(p, aPath, strlen(aPath) + 1);
    return p;
}

int
mkstemps(char *szTemplate, int nSuffixLen)
{
    size_t n;
    size_t i;
    int nFd;
    static const char aX[] = "0123456789abcdefghijklmnopqrstuvwxyz";
    unsigned uSeed;

    if (szTemplate == NULL || nSuffixLen < 0) {
        errno = EINVAL;
        return -1;
    }
    n = strlen(szTemplate);
    if ((size_t)nSuffixLen >= n || n < 6u + (size_t)nSuffixLen) {
        errno = EINVAL;
        return -1;
    }
    /* require trailing XXXXXX before suffix */
    for (i = n - (size_t)nSuffixLen - 6; i < n - (size_t)nSuffixLen; i++) {
        if (szTemplate[i] != 'X') {
            errno = EINVAL;
            return -1;
        }
    }
    uSeed = (unsigned)getpid() ^ (unsigned)(uintptr_t)szTemplate;
    for (i = 0; i < 256u; i++) {
        size_t j;
        unsigned u = uSeed + (unsigned)i * 1103515245u;

        for (j = 0; j < 6; j++) {
            szTemplate[n - (size_t)nSuffixLen - 6 + j] = aX[u % 36u];
            u /= 36u;
        }
        nFd = open(szTemplate, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (nFd >= 0) {
            return nFd;
        }
        if (errno != EEXIST) {
            return -1;
        }
    }
    errno = EEXIST;
    return -1;
}

int
mkstemps64(char *szTemplate, int nSuffixLen)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (szTemplate == NULL) {
        errno = EFAULT;
        return -1;
    }

    return mkstemps(szTemplate, nSuffixLen);
}

int
mkostemps(char *szTemplate, int nSuffixLen, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (szTemplate == NULL) {
        errno = EFAULT;
        return -1;
    }

    int nFd;
    int nExtra;

    nFd = mkstemps(szTemplate, nSuffixLen);
    if (nFd < 0) {
        return -1;
    }
    nExtra = nFlags & (O_CLOEXEC | O_APPEND | O_SYNC);
    if (nExtra != 0) {
        int nCur = fcntl(nFd, F_GETFL);
        if (nCur >= 0) {
            (void)fcntl(nFd, F_SETFL, nCur | (nExtra & ~O_CLOEXEC));
        }
        if (nFlags & O_CLOEXEC) {
            (void)fcntl(nFd, F_SETFD, FD_CLOEXEC);
        }
    }
    return nFd;
}

void
psignal(int nSig, const char *szS)
{
    const char *szMsg = strsignal(nSig);

    if (szS != NULL && szS[0] != '\0') {
        (void)fprintf(stderr, "%s: %s\n", szS,
                      szMsg != NULL ? szMsg : "Unknown signal");
    } else {
        (void)fprintf(stderr, "%s\n", szMsg != NULL ? szMsg : "Unknown signal");
    }
}

void
psiginfo(const siginfo_t *pInfo, const char *szS)
{
    if (pInfo == NULL) {
        psignal(0, szS);
        return;
    }
    psignal(pInfo->si_signo, szS);
}

int
__xmknod(int nVer, const char *szPath, mode_t mode, dev_t *pDev)
{
    (void)nVer;
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pDev == NULL) {
        errno = EFAULT;
        return -1;
    }
    return mknod(szPath, mode, *pDev);
}

int
__xmknodat(int nVer, int nDfd, const char *szPath, mode_t mode, dev_t *pDev)
{
    (void)nVer;
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pDev == NULL) {
        errno = EFAULT;
        return -1;
    }
    return mknodat(nDfd, szPath, mode, *pDev);
}

ssize_t
getdirentries(int nFd, char *szBuf, size_t cb, off_t *pBasep)
{
    ssize_t n;

    /* greppable: CGJ_GRAPH_BATCH3_SOFT_ARGS */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (szBuf == NULL && cb != 0) {
        errno = EFAULT;
        return -1;
    }
    if (pBasep != NULL) {
        *pBasep = lseek(nFd, 0, SEEK_CUR);
    }
    n = (ssize_t)sys_ret(sys6(NR_getdents64, nFd, (long)(uintptr_t)szBuf,
                              (long)cb, 0, 0, 0));
    if (n > 0 && pBasep != NULL) {
        *pBasep = lseek(nFd, 0, SEEK_CUR);
    }
    return n;
}

ssize_t
getdirentries64(int nFd, char *szBuf, size_t cb, off_t *pBasep)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getdirentries(nFd, szBuf, cb, pBasep);
}

/* ---- string/locale graph ----------------------------------------------- */

long
strtoq(const char *sz, char **ppEnd, int nBase)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        errno = EINVAL;
        return 0;
    }
    return strtoll(sz, ppEnd, nBase);
}

unsigned long
strtouq(const char *sz, char **ppEnd, int nBase)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        errno = EINVAL;
        return 0;
    }
    return strtoull(sz, ppEnd, nBase);
}

int
l64a_r(long n, char *szBuf, int nBuflen)
{
    char *p;
    size_t cb;

    if (szBuf == NULL || nBuflen < 1) {
        errno = EINVAL;
        return -1;
    }
    p = l64a(n);
    cb = strlen(p);
    if ((int)cb + 1 > nBuflen) {
        errno = ERANGE;
        return -1;
    }
    memcpy(szBuf, p, cb + 1);
    return 0;
}

int
strcasecmp_l(const char *szA, const char *szB, locale_t loc)
{
    (void)loc;
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    return strcasecmp(szA, szB);
}

int
strncasecmp_l(const char *szA, const char *szB, size_t n, locale_t loc)
{
    (void)loc;
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (n == 0) {
        return 0;
    }
    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    return strncasecmp(szA, szB, n);
}

size_t
strftime_l(char *szBuf, size_t cb, const char *szFmt, const struct tm *pTm,
           locale_t loc)
{
    (void)loc;
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (szBuf == NULL || szFmt == NULL || pTm == NULL || cb == 0) {
        return 0;
    }
    return strftime(szBuf, cb, szFmt, pTm);
}

char *
nl_langinfo_l(nl_item item, locale_t loc)
{
    (void)loc;
    return nl_langinfo(item);
}

char *
__xpg_basename(char *szPath)
{
    /* greppable: CGJ_GRAPH_BATCH3_SOFT_NULL */
    if (szPath == NULL) {
        return NULL;
    }

    return basename(szPath);
}

int
__xpg_strerror_r(int nErr, char *szBuf, size_t cb)
{
    char *p;

    if (szBuf == NULL || cb == 0) {
        return EINVAL;
    }
    p = strerror(nErr);
    if (p == NULL) {
        return EINVAL;
    }
    if (strlen(p) + 1 > cb) {
        (void)strncpy(szBuf, p, cb - 1);
        szBuf[cb - 1] = '\0';
        return ERANGE;
    }
    memcpy(szBuf, p, strlen(p) + 1);
    return 0;
}

int
rpmatch(const char *sz)
{
    if (sz == NULL) {
        return -1;
    }
    if (sz[0] == 'y' || sz[0] == 'Y') {
        return 1;
    }
    if (sz[0] == 'n' || sz[0] == 'N') {
        return 0;
    }
    return -1;
}

int
glob_pattern_p(const char *szPattern, int nQuote)
{
    const char *p;

    (void)nQuote;
    if (szPattern == NULL) {
        return 0;
    }
    for (p = szPattern; *p != '\0'; p++) {
        if (*p == '*' || *p == '?' || *p == '[') {
            return 1;
        }
    }
    return 0;
}

int
getpt(void)
{
    return posix_openpt(O_RDWR | O_NOCTTY);
}

/* h_errno location for reentrant netdb graphs */
extern int h_errno;

int *
__h_errno_location(void)
{
    return &h_errno;
}
