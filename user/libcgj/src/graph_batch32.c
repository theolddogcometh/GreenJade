/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch32: remaining __GI_* reloc aliases, loader/NSS
 * helpers (__resp/svc_fdset/__check_pf/__lll_lock_wait), pidfd_getfd_np,
 * BSD vis/strnunvis, OpenBSD-style MD5Init/SHA1Init/SHA256_Init stubs,
 * getgrouplist_2. Integer/pointer only (no SSE doubles).
 */
#include <dlfcn.h>
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <sched.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

/* ---- existing surface --------------------------------------------------- */

int sigprocmask(int nHow, const sigset_t *pSet, sigset_t *pOld);
int sigaction(int nSig, const struct sigaction *pAct, struct sigaction *pOld);
void (*signal(int nSig, void (*pHandler)(int)))(int);
int gettimeofday(struct timeval *pTv, void *pTz);
time_t time(time_t *pT);
int clock_gettime(clockid_t clk, struct timespec *pTs);
int nanosleep(const struct timespec *pReq, struct timespec *pRem);
int socket(int nDomain, int nType, int nProto);
int bind(int nFd, const struct sockaddr *pAddr, socklen_t cb);
int connect(int nFd, const struct sockaddr *pAddr, socklen_t cb);
int listen(int nFd, int nBacklog);
int accept(int nFd, struct sockaddr *pAddr, socklen_t *pcb);
ssize_t send(int nFd, const void *pBuf, size_t cb, int nFlags);
ssize_t recv(int nFd, void *pBuf, size_t cb, int nFlags);
ssize_t sendto(int nFd, const void *pBuf, size_t cb, int nFlags,
               const struct sockaddr *pAddr, socklen_t cbAddr);
ssize_t recvfrom(int nFd, void *pBuf, size_t cb, int nFlags,
                 struct sockaddr *pAddr, socklen_t *pcbAddr);
int poll(struct pollfd *pFds, nfds_t nFds, int nTimeout);
int ppoll(struct pollfd *pFds, nfds_t nFds, const struct timespec *pTs,
          const void *pMask);
int select(int nNfds, fd_set *pR, fd_set *pW, fd_set *pE, struct timeval *pTv);
int stat(const char *szPath, struct stat *pSt);
int fstat(int nFd, struct stat *pSt);
int lstat(const char *szPath, struct stat *pSt);
int openat(int nDirfd, const char *szPath, int nFlags, ...);
char *getcwd(char *szBuf, size_t cb);
int chdir(const char *szPath);
int fchdir(int nFd);
pid_t getpid(void);
uid_t getuid(void);
uid_t geteuid(void);
gid_t getgid(void);
gid_t getegid(void);
char *getenv(const char *szName);
int setenv(const char *szName, const char *szVal, int nOverwrite);
int unsetenv(const char *szName);
void qsort(void *pBase, size_t n, size_t cb,
           int (*pfnCmp)(const void *, const void *));
void *bsearch(const void *pKey, const void *pBase, size_t n, size_t cb,
              int (*pfnCmp)(const void *, const void *));
long strtol(const char *sz, char **ppEnd, int nBase);
unsigned long strtoul(const char *sz, char **ppEnd, int nBase);
int atoi(const char *sz);
long atol(const char *sz);
int rand(void);
void srand(unsigned int uSeed);
struct tm *localtime(const time_t *pT);
struct tm *gmtime(const time_t *pT);
size_t strftime(char *szBuf, size_t cb, const char *szFmt,
                const struct tm *pTm);
time_t mktime(struct tm *pTm);
char *strerror(int nErr);
void perror(const char *sz);
int fcntl(int nFd, int nCmd, ...);
int ioctl(int nFd, unsigned long uReq, ...);
int dup(int nFd);
int dup2(int nOld, int nNew);
int pipe(int aFd[2]);
int pipe2(int aFd[2], int nFlags);
pid_t waitpid(pid_t nPid, int *pStatus, int nOptions);
pid_t wait(int *pStatus);
int kill(pid_t nPid, int nSig);
unsigned int sleep(unsigned int nSec);
int usleep(unsigned int nUsec);
int sched_yield(void);
int prctl(int nOption, unsigned long a2, unsigned long a3, unsigned long a4,
          unsigned long a5);
int uname(struct utsname *pBuf);
long sysconf(int nName);
int getopt(int argc, char *const argv[], const char *szOptstring);
void *dlopen(const char *szFile, int nMode);
void *dlsym(void *pHandle, const char *szName);
int dlclose(void *pHandle);
int pidfd_getfd(int nPidfd, int nTargetFd, unsigned int uFlags);
int getgrouplist(const char *szUser, gid_t nGroup, gid_t *pGroups, int *pNg);

/* ---- marker / data ------------------------------------------------------ */

const char __libcgj_batch32_marker[] = "libcgj-batch32";
void *__resp;
fd_set svc_fdset;
uintptr_t __pointer_chk_guard = 0x1badc0deul;
int __rseq_handled = 0;

/* ---- thin helpers ------------------------------------------------------- */

int
pidfd_getfd_np(int nPidfd, int nTargetFd, unsigned int uFlags)
{
    return pidfd_getfd(nPidfd, nTargetFd, uFlags);
}

void
__check_pf(int *pSeenInet, int *pSeenInet6)
{
    if (pSeenInet != NULL) {
        *pSeenInet = 1;
    }
    if (pSeenInet6 != NULL) {
        *pSeenInet6 = 1;
    }
}

void
__lll_lock_wait(void)
{
    /* thin spin-yield bring-up (real wait is futex-based in pthread) */
    (void)sched_yield();
}

int
__nss_database_lookup2(const char *szDb, const char *szAlt1, const char *szAlt2,
                       void **ppResult)
{
    (void)szDb;
    (void)szAlt1;
    (void)szAlt2;
    if (ppResult != NULL) {
        *ppResult = NULL;
    }
    return -1;
}

/* ---- BSD vis family (minimal) ------------------------------------------- */

int
strnvis(char *pDst, size_t cbDst, const char *szSrc, int nFlags)
{
    size_t i = 0;
    size_t j = 0;

    (void)nFlags;
    if (pDst == NULL || cbDst == 0) {
        errno = EINVAL;
        return -1;
    }
    if (szSrc == NULL) {
        pDst[0] = '\0';
        return 0;
    }
    while (szSrc[i] != '\0' && j + 1u < cbDst) {
        unsigned char ch = (unsigned char)szSrc[i++];

        if (ch >= 0x20u && ch < 0x7fu && ch != '\\') {
            pDst[j++] = (char)ch;
        } else if (j + 4u < cbDst) {
            pDst[j++] = '\\';
            pDst[j++] = '0' + ((ch >> 6) & 7);
            pDst[j++] = '0' + ((ch >> 3) & 7);
            pDst[j++] = '0' + (ch & 7);
        } else {
            break;
        }
    }
    pDst[j] = '\0';
    return (int)j;
}

int
stravis(char **ppDst, const char *szSrc, int nFlags)
{
    size_t nSrc = 0;
    size_t cb;
    char *p;

    if (ppDst == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szSrc != NULL) {
        while (szSrc[nSrc] != '\0') {
            nSrc++;
        }
    }
    cb = nSrc * 4u + 1u;
    p = (char *)malloc(cb);
    if (p == NULL) {
        return -1;
    }
    if (strnvis(p, cb, szSrc, nFlags) < 0) {
        free(p);
        return -1;
    }
    *ppDst = p;
    return (int)strlen(p);
}

int
strnunvis(char *pDst, size_t cbDst, const char *szSrc)
{
    size_t i = 0;
    size_t j = 0;

    if (pDst == NULL || cbDst == 0) {
        errno = EINVAL;
        return -1;
    }
    if (szSrc == NULL) {
        pDst[0] = '\0';
        return 0;
    }
    while (szSrc[i] != '\0' && j + 1u < cbDst) {
        if (szSrc[i] == '\\' && szSrc[i + 1] >= '0' && szSrc[i + 1] <= '7' &&
            szSrc[i + 2] >= '0' && szSrc[i + 2] <= '7' &&
            szSrc[i + 3] >= '0' && szSrc[i + 3] <= '7') {
            unsigned char ch =
                (unsigned char)(((szSrc[i + 1] - '0') << 6) |
                                ((szSrc[i + 2] - '0') << 3) |
                                (szSrc[i + 3] - '0'));
            pDst[j++] = (char)ch;
            i += 4u;
        } else {
            pDst[j++] = szSrc[i++];
        }
    }
    pDst[j] = '\0';
    return (int)j;
}

/* ---- OpenBSD libmd-shaped init stubs (context opaque) ------------------- */

void
MD5Init(void *pCtx)
{
    if (pCtx != NULL) {
        memset(pCtx, 0, 92u);
    }
}

void
SHA1Init(void *pCtx)
{
    if (pCtx != NULL) {
        memset(pCtx, 0, 96u);
    }
}

void
SHA256_Init(void *pCtx)
{
    if (pCtx != NULL) {
        memset(pCtx, 0, 112u);
    }
}

int
getgrouplist_2(const char *szUser, gid_t nGroup, gid_t **ppGroups)
{
    gid_t *p;
    int n = 1;

    if (ppGroups == NULL) {
        errno = EINVAL;
        return -1;
    }
    p = (gid_t *)malloc(sizeof(gid_t));
    if (p == NULL) {
        return -1;
    }
    p[0] = nGroup;
    *ppGroups = p;
    (void)szUser;
    (void)getgrouplist; /* keep link surface warm */
    return n;
}

/* ---- remaining __GI_* aliases ------------------------------------------- */

int
__GI_sigprocmask(int nHow, const sigset_t *pSet, sigset_t *pOld)
{
    return sigprocmask(nHow, pSet, pOld);
}

int
__GI_sigaction(int nSig, const struct sigaction *pAct, struct sigaction *pOld)
{
    return sigaction(nSig, pAct, pOld);
}

void (*__GI_signal(int nSig, void (*pHandler)(int)))(int)
{
    return signal(nSig, pHandler);
}

time_t
__GI_time(time_t *pT)
{
    return time(pT);
}

int
__GI_clock_gettime(clockid_t clk, struct timespec *pTs)
{
    return clock_gettime(clk, pTs);
}

int
__GI_nanosleep(const struct timespec *pReq, struct timespec *pRem)
{
    return nanosleep(pReq, pRem);
}

int
__GI_bind(int nFd, const struct sockaddr *pAddr, socklen_t cb)
{
    return bind(nFd, pAddr, cb);
}

int
__GI_connect(int nFd, const struct sockaddr *pAddr, socklen_t cb)
{
    return connect(nFd, pAddr, cb);
}

int
__GI_listen(int nFd, int nBacklog)
{
    return listen(nFd, nBacklog);
}

int
__GI_accept(int nFd, struct sockaddr *pAddr, socklen_t *pcb)
{
    return accept(nFd, pAddr, pcb);
}

ssize_t
__GI_send(int nFd, const void *pBuf, size_t cb, int nFlags)
{
    return send(nFd, pBuf, cb, nFlags);
}

ssize_t
__GI_recv(int nFd, void *pBuf, size_t cb, int nFlags)
{
    return recv(nFd, pBuf, cb, nFlags);
}

ssize_t
__GI_sendto(int nFd, const void *pBuf, size_t cb, int nFlags,
            const struct sockaddr *pAddr, socklen_t cbAddr)
{
    return sendto(nFd, pBuf, cb, nFlags, pAddr, cbAddr);
}

ssize_t
__GI_recvfrom(int nFd, void *pBuf, size_t cb, int nFlags,
              struct sockaddr *pAddr, socklen_t *pcbAddr)
{
    return recvfrom(nFd, pBuf, cb, nFlags, pAddr, pcbAddr);
}

int
__GI_poll(struct pollfd *pFds, nfds_t nFds, int nTimeout)
{
    return poll(pFds, nFds, nTimeout);
}

int
__GI_ppoll(struct pollfd *pFds, nfds_t nFds, const struct timespec *pTs,
           const void *pMask)
{
    return ppoll(pFds, nFds, pTs, pMask);
}

int
__GI_select(int nNfds, fd_set *pR, fd_set *pW, fd_set *pE, struct timeval *pTv)
{
    return select(nNfds, pR, pW, pE, pTv);
}

int
__GI_stat(const char *szPath, struct stat *pSt)
{
    return stat(szPath, pSt);
}

int
__GI_fstat(int nFd, struct stat *pSt)
{
    return fstat(nFd, pSt);
}

int
__GI_lstat(const char *szPath, struct stat *pSt)
{
    return lstat(szPath, pSt);
}

int
__GI_openat(int nDirfd, const char *szPath, int nFlags, ...)
{
    mode_t mode = 0;
    va_list ap;

    if ((nFlags & O_CREAT) != 0) {
        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    return openat(nDirfd, szPath, nFlags, mode);
}

char *
__GI_getcwd(char *szBuf, size_t cb)
{
    return getcwd(szBuf, cb);
}

int
__GI_chdir(const char *szPath)
{
    return chdir(szPath);
}

int
__GI_fchdir(int nFd)
{
    return fchdir(nFd);
}

pid_t
__GI_getpid(void)
{
    return getpid();
}

uid_t
__GI_getuid(void)
{
    return getuid();
}

uid_t
__GI_geteuid(void)
{
    return geteuid();
}

gid_t
__GI_getgid(void)
{
    return getgid();
}

gid_t
__GI_getegid(void)
{
    return getegid();
}

char *
__GI_getenv(const char *szName)
{
    return getenv(szName);
}

int
__GI_setenv(const char *szName, const char *szVal, int nOverwrite)
{
    return setenv(szName, szVal, nOverwrite);
}

int
__GI_unsetenv(const char *szName)
{
    return unsetenv(szName);
}

void
__GI_qsort(void *pBase, size_t n, size_t cb,
           int (*pfnCmp)(const void *, const void *))
{
    qsort(pBase, n, cb, pfnCmp);
}

void *
__GI_bsearch(const void *pKey, const void *pBase, size_t n, size_t cb,
             int (*pfnCmp)(const void *, const void *))
{
    return bsearch(pKey, pBase, n, cb, pfnCmp);
}

long
__GI_strtol(const char *sz, char **ppEnd, int nBase)
{
    return strtol(sz, ppEnd, nBase);
}

unsigned long
__GI_strtoul(const char *sz, char **ppEnd, int nBase)
{
    return strtoul(sz, ppEnd, nBase);
}

int
__GI_atoi(const char *sz)
{
    return atoi(sz);
}

long
__GI_atol(const char *sz)
{
    return atol(sz);
}

int
__GI_rand(void)
{
    return rand();
}

void
__GI_srand(unsigned int uSeed)
{
    srand(uSeed);
}

struct tm *
__GI_localtime(const time_t *pT)
{
    return localtime(pT);
}

struct tm *
__GI_gmtime(const time_t *pT)
{
    return gmtime(pT);
}

size_t
__GI_strftime(char *szBuf, size_t cb, const char *szFmt, const struct tm *pTm)
{
    return strftime(szBuf, cb, szFmt, pTm);
}

time_t
__GI_mktime(struct tm *pTm)
{
    return mktime(pTm);
}

char *
__GI_strerror(int nErr)
{
    return strerror(nErr);
}

void
__GI_perror(const char *sz)
{
    perror(sz);
}

int
__GI_fcntl(int nFd, int nCmd, ...)
{
    va_list ap;
    long aArg = 0;
    int n;

    va_start(ap, nCmd);
    aArg = va_arg(ap, long);
    va_end(ap);
    n = fcntl(nFd, nCmd, aArg);
    return n;
}

int
__GI_ioctl(int nFd, unsigned long uReq, ...)
{
    va_list ap;
    void *pArg;
    int n;

    va_start(ap, uReq);
    pArg = va_arg(ap, void *);
    va_end(ap);
    n = ioctl(nFd, uReq, pArg);
    return n;
}

int
__GI_dup(int nFd)
{
    return dup(nFd);
}

int
__GI_dup2(int nOld, int nNew)
{
    return dup2(nOld, nNew);
}

int
__GI_pipe(int aFd[2])
{
    return pipe(aFd);
}

int
__GI_pipe2(int aFd[2], int nFlags)
{
    return pipe2(aFd, nFlags);
}

pid_t
__GI_waitpid(pid_t nPid, int *pStatus, int nOptions)
{
    return waitpid(nPid, pStatus, nOptions);
}

pid_t
__GI_wait(int *pStatus)
{
    return wait(pStatus);
}

int
__GI_kill(pid_t nPid, int nSig)
{
    return kill(nPid, nSig);
}

unsigned int
__GI_sleep(unsigned int nSec)
{
    return sleep(nSec);
}

int
__GI_usleep(unsigned int nUsec)
{
    return usleep(nUsec);
}

int
__GI_sched_yield(void)
{
    return sched_yield();
}

int
__GI_prctl(int nOption, unsigned long a2, unsigned long a3, unsigned long a4,
           unsigned long a5)
{
    return prctl(nOption, a2, a3, a4, a5);
}

int
__GI_uname(struct utsname *pBuf)
{
    return uname(pBuf);
}

long
__GI_sysconf(int nName)
{
    return sysconf(nName);
}

int
__GI_getopt(int argc, char *const argv[], const char *szOptstring)
{
    return getopt(argc, argv, szOptstring);
}

void *
__GI_dlopen(const char *szFile, int nMode)
{
    return dlopen(szFile, nMode);
}

void *
__GI_dlsym(void *pHandle, const char *szName)
{
    return dlsym(pHandle, szName);
}

int
__GI_dlclose(void *pHandle)
{
    return dlclose(pHandle);
}

/* ---- soft deepen: pidfd helper probe (unique) --------------------------- */

int
pidfd_getfd_np_check(int nPidfd, int nTargetFd, unsigned int uFlags)
{
    if (nPidfd < 0 || nTargetFd < 0) {
        errno = EINVAL;
        return -1;
    }
    return pidfd_getfd(nPidfd, nTargetFd, uFlags);
}

int __pidfd_getfd_np_check(int nPidfd, int nTargetFd, unsigned int uFlags)
    __attribute__((alias("pidfd_getfd_np_check")));
