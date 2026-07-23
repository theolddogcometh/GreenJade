/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch18: syscall(), BSD helpers (fls/strtonum/
 * getprogname/heapsort/recallocarray), more underscored aliases, Linux
 * sched_setattr/perf_event_open/fsopen family, getdents.
 * Integer/pointer only (no SSE doubles). freezero lives in batch17.
 */
#include <errno.h>
#include <err.h>
#include <fcntl.h>
#include <limits.h>
#include <mqueue.h>
#include <poll.h>
#include <resolv.h>
#include <sched.h>
#include <semaphore.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/msg.h>
#include <sys/resource.h>
#include <sys/select.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/swap.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>

int clone3(void *pClArgs, size_t cbArgs);
int ptrace(int nRequest, pid_t pid, void *pAddr, void *pData);
int sysctl(int *pName, int nNlen, void *pOld, size_t *pOldlen, void *pNew,
           size_t nNewlen);

#define NR_getdents         78
#define NR_getdents64      217
#define NR_perf_event_open 298
#define NR_sched_setattr   314
#define NR_sched_getattr   315
#define NR_rseq            334
#define NR_fsopen          430
#define NR_fsconfig        431
#define NR_fsmount         432
#define NR_fspick          433
#define NR_mount_setattr   442
#define NR_quotactl_fd     443
#define NR_futex_waitv     449
#define NR_cachestat       451

static long
b18_sys6(long nr, long a0, long a1, long a2, long a3, long a4, long a5)
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
b18_sys_ret(long r)
{
    if (r < 0 && r > -4096) {
        errno = (int)(-r);
        return -1;
    }
    return r;
}

long
syscall(long nNr, ...)
{
    va_list ap;
    long a0, a1, a2, a3, a4, a5;

    va_start(ap, nNr);
    a0 = va_arg(ap, long);
    a1 = va_arg(ap, long);
    a2 = va_arg(ap, long);
    a3 = va_arg(ap, long);
    a4 = va_arg(ap, long);
    a5 = va_arg(ap, long);
    va_end(ap);
    return b18_sys_ret(b18_sys6(nNr, a0, a1, a2, a3, a4, a5));
}

int
fls(int n)
{
    unsigned u;
    int nBit;

    if (n == 0) {
        return 0;
    }
    u = (unsigned)n;
    nBit = 0;
    while (u != 0u) {
        nBit++;
        u >>= 1;
    }
    return nBit;
}

int
flsl(long n)
{
    unsigned long u;
    int nBit;

    if (n == 0L) {
        return 0;
    }
    u = (unsigned long)n;
    nBit = 0;
    while (u != 0UL) {
        nBit++;
        u >>= 1;
    }
    return nBit;
}

int
flsll(long long n)
{
    unsigned long long u;
    int nBit;

    if (n == 0LL) {
        return 0;
    }
    u = (unsigned long long)n;
    nBit = 0;
    while (u != 0ULL) {
        nBit++;
        u >>= 1;
    }
    return nBit;
}

static char *s_szProgname = (char *)"greenjade";

const char *
getprogname(void)
{
    return s_szProgname;
}

void
setprogname(const char *sz)
{
    if (sz != NULL && sz[0] != '\0') {
        s_szProgname = (char *)sz;
    }
}

void *
recallocarray(void *p, size_t nOld, size_t nNew, size_t cb)
{
    void *pNew;
    size_t cbOld;
    size_t cbNew;

    if (cb != 0 && nNew > (size_t)-1 / cb) {
        errno = ENOMEM;
        return NULL;
    }
    if (cb != 0 && nOld > (size_t)-1 / cb) {
        errno = ENOMEM;
        return NULL;
    }
    cbNew = nNew * cb;
    cbOld = nOld * cb;
    pNew = reallocarray(p, nNew, cb);
    if (pNew == NULL) {
        return NULL;
    }
    if (cbNew > cbOld) {
        (void)memset((unsigned char *)pNew + cbOld, 0, cbNew - cbOld);
    }
    return pNew;
}

void *
reallocf(void *p, size_t cb)
{
    void *pNew = realloc(p, cb);

    if (pNew == NULL && p != NULL && cb != 0) {
        free(p);
    }
    return pNew;
}

long long
strtonum(const char *sz, long long nMin, long long nMax, const char **ppErr)
{
    char *pEnd = NULL;
    long long n;

    if (ppErr != NULL) {
        *ppErr = NULL;
    }
    if (sz == NULL || *sz == '\0') {
        if (ppErr != NULL) {
            *ppErr = "invalid";
        }
        errno = EINVAL;
        return 0;
    }
    errno = 0;
    n = strtoll(sz, &pEnd, 10);
    if (errno == ERANGE || pEnd == sz || (pEnd != NULL && *pEnd != '\0')) {
        if (ppErr != NULL) {
            *ppErr = "invalid";
        }
        errno = EINVAL;
        return 0;
    }
    if (n < nMin) {
        if (ppErr != NULL) {
            *ppErr = "too small";
        }
        errno = ERANGE;
        return 0;
    }
    if (n > nMax) {
        if (ppErr != NULL) {
            *ppErr = "too large";
        }
        errno = ERANGE;
        return 0;
    }
    return n;
}

int
issetugid(void)
{
    if (geteuid() != getuid() || getegid() != getgid()) {
        return 1;
    }
    return 0;
}

void
strmode(mode_t mode, char *sz)
{
    char a[11];

    if (sz == NULL) {
        return;
    }
    if (S_ISREG(mode)) {
        a[0] = '-';
    } else if (S_ISDIR(mode)) {
        a[0] = 'd';
    } else if (S_ISLNK(mode)) {
        a[0] = 'l';
    } else if (S_ISCHR(mode)) {
        a[0] = 'c';
    } else if (S_ISBLK(mode)) {
        a[0] = 'b';
    } else if (S_ISFIFO(mode)) {
        a[0] = 'p';
    } else if (S_ISSOCK(mode)) {
        a[0] = 's';
    } else {
        a[0] = '?';
    }
    a[1] = (mode & S_IRUSR) ? 'r' : '-';
    a[2] = (mode & S_IWUSR) ? 'w' : '-';
    a[3] = (mode & S_IXUSR) ? 'x' : '-';
    a[4] = (mode & S_IRGRP) ? 'r' : '-';
    a[5] = (mode & S_IWGRP) ? 'w' : '-';
    a[6] = (mode & S_IXGRP) ? 'x' : '-';
    a[7] = (mode & S_IROTH) ? 'r' : '-';
    a[8] = (mode & S_IWOTH) ? 'w' : '-';
    a[9] = (mode & S_IXOTH) ? 'x' : '-';
    if (mode & S_ISUID) {
        a[3] = (mode & S_IXUSR) ? 's' : 'S';
    }
    if (mode & S_ISGID) {
        a[6] = (mode & S_IXGRP) ? 's' : 'S';
    }
    if (mode & S_ISVTX) {
        a[9] = (mode & S_IXOTH) ? 't' : 'T';
    }
    a[10] = '\0';
    (void)memcpy(sz, a, 11);
}

static void
heap_sift(unsigned char *pBase, size_t nNel, size_t cbWidth, size_t iRoot,
          int (*pfnCmp)(const void *, const void *), unsigned char *pTmp)
{
    size_t iChild;

    for (;;) {
        iChild = 2u * iRoot + 1u;
        if (iChild >= nNel) {
            break;
        }
        if (iChild + 1u < nNel &&
            pfnCmp(pBase + (iChild + 1u) * cbWidth,
                   pBase + iChild * cbWidth) > 0) {
            iChild++;
        }
        if (pfnCmp(pBase + iRoot * cbWidth, pBase + iChild * cbWidth) >= 0) {
            break;
        }
        (void)memcpy(pTmp, pBase + iRoot * cbWidth, cbWidth);
        (void)memcpy(pBase + iRoot * cbWidth, pBase + iChild * cbWidth,
                     cbWidth);
        (void)memcpy(pBase + iChild * cbWidth, pTmp, cbWidth);
        iRoot = iChild;
    }
}

int
heapsort(void *pBase, size_t nNel, size_t cbWidth,
         int (*pfnCmp)(const void *, const void *))
{
    unsigned char *p;
    unsigned char *pTmp;
    size_t i;

    if (pBase == NULL || pfnCmp == NULL || cbWidth == 0) {
        errno = EINVAL;
        return -1;
    }
    if (nNel < 2) {
        return 0;
    }
    p = (unsigned char *)pBase;
    pTmp = (unsigned char *)malloc(cbWidth);
    if (pTmp == NULL) {
        return -1;
    }
    for (i = nNel / 2u; i > 0;) {
        i--;
        heap_sift(p, nNel, cbWidth, i, pfnCmp, pTmp);
    }
    while (nNel > 1u) {
        nNel--;
        (void)memcpy(pTmp, p, cbWidth);
        (void)memcpy(p, p + nNel * cbWidth, cbWidth);
        (void)memcpy(p + nNel * cbWidth, pTmp, cbWidth);
        heap_sift(p, nNel, cbWidth, 0, pfnCmp, pTmp);
    }
    free(pTmp);
    return 0;
}

int
mergesort(void *pBase, size_t nNel, size_t cbWidth,
          int (*pfnCmp)(const void *, const void *))
{
    unsigned char *p;
    unsigned char *pTmp;
    size_t cb;
    size_t uWidth;
    size_t i;

    if (pBase == NULL || pfnCmp == NULL || cbWidth == 0) {
        errno = EINVAL;
        return -1;
    }
    if (nNel < 2) {
        return 0;
    }
    if (cbWidth > (size_t)-1 / nNel) {
        errno = ENOMEM;
        return -1;
    }
    p = (unsigned char *)pBase;
    cb = nNel * cbWidth;
    pTmp = (unsigned char *)malloc(cb);
    if (pTmp == NULL) {
        return -1;
    }
    for (uWidth = 1; uWidth < nNel; uWidth *= 2u) {
        for (i = 0; i < nNel; i += 2u * uWidth) {
            size_t iLeft = i;
            size_t iMid = i + uWidth;
            size_t iRight = i + 2u * uWidth;
            size_t iA, iB, iD;

            if (iMid > nNel) {
                iMid = nNel;
            }
            if (iRight > nNel) {
                iRight = nNel;
            }
            iA = iLeft;
            iB = iMid;
            iD = iLeft;
            while (iA < iMid && iB < iRight) {
                if (pfnCmp(p + iA * cbWidth, p + iB * cbWidth) <= 0) {
                    (void)memcpy(pTmp + iD * cbWidth, p + iA * cbWidth,
                                 cbWidth);
                    iA++;
                } else {
                    (void)memcpy(pTmp + iD * cbWidth, p + iB * cbWidth,
                                 cbWidth);
                    iB++;
                }
                iD++;
            }
            while (iA < iMid) {
                (void)memcpy(pTmp + iD * cbWidth, p + iA * cbWidth, cbWidth);
                iA++;
                iD++;
            }
            while (iB < iRight) {
                (void)memcpy(pTmp + iD * cbWidth, p + iB * cbWidth, cbWidth);
                iB++;
                iD++;
            }
        }
        (void)memcpy(p, pTmp, cb);
    }
    free(pTmp);
    return 0;
}

struct __res_state *
__res_state(void)
{
    return (struct __res_state *)&_res;
}

void
errc(int nEval, int nErr, const char *szFmt, ...)
{
    va_list ap;

    errno = nErr;
    va_start(ap, szFmt);
    verr(nEval, szFmt, ap);
    va_end(ap);
}

void
warnc(int nErr, const char *szFmt, ...)
{
    va_list ap;
    int nSaved = errno;

    errno = nErr;
    va_start(ap, szFmt);
    vwarn(szFmt, ap);
    va_end(ap);
    errno = nSaved;
}

int
sched_setattr(pid_t pid, struct sched_attr *pAttr, unsigned uFlags)
{
    return (int)b18_sys_ret(b18_sys6(NR_sched_setattr, (long)pid,
                                     (long)(uintptr_t)pAttr, (long)uFlags, 0, 0,
                                     0));
}

int
sched_getattr(pid_t pid, struct sched_attr *pAttr, unsigned uSize,
              unsigned uFlags)
{
    return (int)b18_sys_ret(b18_sys6(NR_sched_getattr, (long)pid,
                                     (long)(uintptr_t)pAttr, (long)uSize,
                                     (long)uFlags, 0, 0));
}

long
perf_event_open(void *pAttr, pid_t pid, int nCpu, int nGroupFd,
                unsigned long uFlags)
{
    return b18_sys_ret(b18_sys6(NR_perf_event_open, (long)(uintptr_t)pAttr,
                                (long)pid, (long)nCpu, (long)nGroupFd,
                                (long)uFlags, 0));
}

int
fsopen(const char *szFsName, unsigned uFlags)
{
    return (int)b18_sys_ret(b18_sys6(NR_fsopen, (long)(uintptr_t)szFsName,
                                     (long)uFlags, 0, 0, 0, 0));
}

int
fsconfig(int nFd, unsigned uCmd, const char *szKey, const void *pVal,
         int nAux)
{
    return (int)b18_sys_ret(b18_sys6(NR_fsconfig, (long)nFd, (long)uCmd,
                                     (long)(uintptr_t)szKey,
                                     (long)(uintptr_t)pVal, (long)nAux, 0));
}

int
fsmount(int nFd, unsigned uFlags, unsigned uMsFlags)
{
    return (int)b18_sys_ret(b18_sys6(NR_fsmount, (long)nFd, (long)uFlags,
                                     (long)uMsFlags, 0, 0, 0));
}

int
fspick(int nDfd, const char *szPath, unsigned uFlags)
{
    return (int)b18_sys_ret(b18_sys6(NR_fspick, (long)nDfd,
                                     (long)(uintptr_t)szPath, (long)uFlags, 0, 0,
                                     0));
}

int
mount_setattr(int nDfd, const char *szPath, unsigned uFlags, void *pAttr,
              size_t cbAttr)
{
    return (int)b18_sys_ret(b18_sys6(NR_mount_setattr, (long)nDfd,
                                     (long)(uintptr_t)szPath, (long)uFlags,
                                     (long)(uintptr_t)pAttr, (long)cbAttr, 0));
}

int
rseq(void *pRseq, uint32_t cbRseq, int nFlags, uint32_t uSig)
{
    return (int)b18_sys_ret(b18_sys6(NR_rseq, (long)(uintptr_t)pRseq,
                                     (long)cbRseq, (long)nFlags, (long)uSig, 0,
                                     0));
}

int
quotactl_fd(int nFd, int nCmd, int nId, void *pAddr)
{
    return (int)b18_sys_ret(b18_sys6(NR_quotactl_fd, (long)nFd, (long)nCmd,
                                     (long)nId, (long)(uintptr_t)pAddr, 0, 0));
}

int
cachestat(unsigned nFd, void *pCstatRange, void *pCstat, unsigned uFlags)
{
    return (int)b18_sys_ret(b18_sys6(NR_cachestat, (long)nFd,
                                     (long)(uintptr_t)pCstatRange,
                                     (long)(uintptr_t)pCstat, (long)uFlags, 0,
                                     0));
}

int
futex_waitv(void *pWaiters, unsigned nNrWaiters, unsigned uFlags,
            struct timespec *pTimeout, clockid_t clockid)
{
    return (int)b18_sys_ret(b18_sys6(NR_futex_waitv, (long)(uintptr_t)pWaiters,
                                     (long)nNrWaiters, (long)uFlags,
                                     (long)(uintptr_t)pTimeout, (long)clockid,
                                     0));
}

int
getdents(int nFd, void *pDirp, unsigned cb)
{
    return (int)b18_sys_ret(b18_sys6(NR_getdents, (long)nFd,
                                     (long)(uintptr_t)pDirp, (long)cb, 0, 0,
                                     0));
}

int
__getdents(int nFd, void *pDirp, unsigned cb)
{
    return getdents(nFd, pDirp, cb);
}

int
__getdents64(int nFd, void *pDirp, size_t cb)
{
    return (int)getdents64(nFd, pDirp, cb);
}

int
__stat64(const char *szPath, struct stat *pSt)
{
    return stat(szPath, pSt);
}

int
__lstat64(const char *szPath, struct stat *pSt)
{
    return lstat(szPath, pSt);
}

int
__execve(const char *szPath, char *const aArgv[], char *const aEnvp[])
{
    return execve(szPath, aArgv, aEnvp);
}

pid_t
__vfork(void)
{
    return vfork();
}

int
__clone3(void *pClArgs, size_t cbArgs)
{
    return clone3(pClArgs, cbArgs);
}

int
__ppoll(struct pollfd *pFds, nfds_t nFds, const struct timespec *pTs,
        const sigset_t *pMask)
{
    return ppoll(pFds, nFds, pTs, pMask);
}

int
__pselect(int nNfds, fd_set *pR, fd_set *pW, fd_set *pE,
          const struct timespec *pTs, const sigset_t *pMask)
{
    return pselect(nNfds, pR, pW, pE, pTs, pMask);
}

int
__sigwaitinfo(const sigset_t *pSet, siginfo_t *pInfo)
{
    return sigwaitinfo(pSet, pInfo);
}

int
mkstemp64(char *szTemplate)
{
    return mkstemp(szTemplate);
}

int
mkostemp64(char *szTemplate, int nFlags)
{
    return mkostemp(szTemplate, nFlags);
}

int
__uname(struct utsname *pU)
{
    return uname(pU);
}

int
__gethostname(char *sz, size_t cb)
{
    return gethostname(sz, cb);
}

int
__sethostname(const char *sz, size_t cb)
{
    return sethostname(sz, cb);
}

int
__getdomainname(char *sz, size_t cb)
{
    return getdomainname(sz, cb);
}

int
__setdomainname(const char *sz, size_t cb)
{
    return setdomainname(sz, cb);
}

int
__daemon(int nNochdir, int nNoclose)
{
    return daemon(nNochdir, nNoclose);
}

int
__initgroups(const char *szUser, gid_t gid)
{
    return initgroups(szUser, gid);
}

int
__getgrouplist(const char *szUser, gid_t gid, gid_t *pGroups, int *pNg)
{
    return getgrouplist(szUser, gid, pGroups, pNg);
}

int
__setgroups(size_t n, const gid_t *pList)
{
    return setgroups(n, pList);
}

int
__getgroups(int n, gid_t *pList)
{
    return getgroups(n, pList);
}

int
__nice(int nInc)
{
    return nice(nInc);
}

int
__chroot(const char *szPath)
{
    return chroot(szPath);
}

int
__acct(const char *szPath)
{
    return acct(szPath);
}

int
__swapon(const char *szPath, int nFlags)
{
    return swapon(szPath, nFlags);
}

int
__swapoff(const char *szPath)
{
    return swapoff(szPath);
}

int
__reboot(int nCmd)
{
    return reboot(0xfee1dead, 672274793, nCmd, NULL);
}

int settimeofday(const struct timeval *pTv, const void *pTz);

int
__settimeofday(const struct timeval *pTv, const void *pTz)
{
    return settimeofday(pTv, pTz);
}

int
__clock_settime(clockid_t clk, const struct timespec *pTs)
{
    return clock_settime(clk, pTs);
}

int
__clock_getres(clockid_t clk, struct timespec *pTs)
{
    return clock_getres(clk, pTs);
}

int
__clock_nanosleep(clockid_t clk, int nFlags, const struct timespec *pReq,
                  struct timespec *pRem)
{
    return clock_nanosleep(clk, nFlags, pReq, pRem);
}

int
__prctl(int nOption, ...)
{
    va_list ap;
    unsigned long a1, a2, a3, a4;

    va_start(ap, nOption);
    a1 = va_arg(ap, unsigned long);
    a2 = va_arg(ap, unsigned long);
    a3 = va_arg(ap, unsigned long);
    a4 = va_arg(ap, unsigned long);
    va_end(ap);
    return prctl(nOption, a1, a2, a3, a4);
}

int
__ptrace(int nRequest, ...)
{
    va_list ap;
    pid_t pid;
    void *pAddr;
    void *pData;

    va_start(ap, nRequest);
    pid = va_arg(ap, pid_t);
    pAddr = va_arg(ap, void *);
    pData = va_arg(ap, void *);
    va_end(ap);
    return ptrace(nRequest, pid, pAddr, pData);
}

int
__getdtablesize(void)
{
    return getdtablesize();
}

int
__sysctl(int *pName, int nNlen, void *pOld, size_t *pOldlen, void *pNew,
         size_t nNewlen)
{
    return sysctl(pName, nNlen, pOld, pOldlen, pNew, nNewlen);
}

void
__openlog(const char *szIdent, int nOption, int nFacility)
{
    openlog(szIdent, nOption, nFacility);
}

void
__closelog(void)
{
    closelog();
}

int
__setlogmask(int nMask)
{
    return setlogmask(nMask);
}

void
__syslog(int nPri, const char *szFmt, ...)
{
    va_list ap;

    va_start(ap, szFmt);
    vsyslog(nPri, szFmt, ap);
    va_end(ap);
}

void
__vsyslog(int nPri, const char *szFmt, va_list ap)
{
    vsyslog(nPri, szFmt, ap);
}

int
__timer_create(clockid_t clk, struct sigevent *pEv, timer_t *pTimer)
{
    return timer_create(clk, pEv, pTimer);
}

int
__timer_delete(timer_t timer)
{
    return timer_delete(timer);
}

int
__timer_settime(timer_t timer, int nFlags, const struct itimerspec *pVal,
                struct itimerspec *pOld)
{
    return timer_settime(timer, nFlags, pVal, pOld);
}

int
__timer_gettime(timer_t timer, struct itimerspec *pVal)
{
    return timer_gettime(timer, pVal);
}

mqd_t
__mq_open(const char *szName, int nOflag, ...)
{
    va_list ap;
    mode_t mode = 0;
    struct mq_attr *pAttr = NULL;

    if (nOflag & O_CREAT) {
        va_start(ap, nOflag);
        mode = (mode_t)va_arg(ap, int);
        pAttr = va_arg(ap, struct mq_attr *);
        va_end(ap);
        return mq_open(szName, nOflag, mode, pAttr);
    }
    return mq_open(szName, nOflag);
}

int
__mq_notify(mqd_t mq, const struct sigevent *pEv)
{
    return mq_notify(mq, pEv);
}

int
__sem_wait(sem_t *pSem)
{
    return sem_wait(pSem);
}

int
__sem_post(sem_t *pSem)
{
    return sem_post(pSem);
}

int
__shmget(key_t key, size_t cb, int nFlag)
{
    return shmget(key, cb, nFlag);
}

void *
__shmat(int nId, const void *pAddr, int nFlag)
{
    return shmat(nId, pAddr, nFlag);
}

int
__shmdt(const void *pAddr)
{
    return shmdt(pAddr);
}

int
__shmctl(int nId, int nCmd, struct shmid_ds *pBuf)
{
    return shmctl(nId, nCmd, pBuf);
}

int
__semget(key_t key, int nNsems, int nFlag)
{
    return semget(key, nNsems, nFlag);
}

int
__semop(int nId, struct sembuf *pOps, size_t n)
{
    return semop(nId, pOps, n);
}

int
__semctl(int nId, int nSemnum, int nCmd, ...)
{
    va_list ap;
    long nArg;

    va_start(ap, nCmd);
    nArg = va_arg(ap, long);
    va_end(ap);
    return semctl(nId, nSemnum, nCmd, nArg);
}

int
__msgget(key_t key, int nFlag)
{
    return msgget(key, nFlag);
}

int
__msgsnd(int nId, const void *pMsg, size_t cb, int nFlag)
{
    return msgsnd(nId, pMsg, cb, nFlag);
}

ssize_t
__msgrcv(int nId, void *pMsg, size_t cb, long nType, int nFlag)
{
    return msgrcv(nId, pMsg, cb, nType, nFlag);
}

int
__msgctl(int nId, int nCmd, struct msqid_ds *pBuf)
{
    return msgctl(nId, nCmd, pBuf);
}

void
__gmon_start__(void)
{
}

int
_dl_find_object(void *pAddr, void *pResult)
{
    (void)pAddr;
    (void)pResult;
    return -1;
}

void
__libc_dynarray_at_failed(size_t nIndex)
{
    (void)nIndex;
    abort();
}
