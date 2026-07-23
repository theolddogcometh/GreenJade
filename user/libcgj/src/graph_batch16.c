/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16: more underscored syscall/libc aliases,
 * fortify printf/syslog/path helpers, _setjmp/__sigsetjmp, LFS stdio,
 * readdir64_r, pthread underscored aliases, pwd/grp reentrant underscores.
 * Integer/pointer only (no SSE doubles).
 *
 * greppable: CGJ_GRAPH_BATCH16_SOFT_NULL
 * greppable: CGJ_GRAPH_BATCH16_SOFT_ARGS
 * greppable: CGJ_GRAPH_BATCH16_SOFT_EDGE
 *
 * Soft deepen: null/arg guards on user-facing graph nodes; edge
 * hardening only. No multi-def; no API break. Pure C integer/pointer.
 */
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <net/if.h>
#include <pthread.h>
#include <pwd.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/statvfs.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <sys/wait.h>
#include <syslog.h>
#include <time.h>
#include <unistd.h>
#include <wchar.h>

void __chk_fail(void);

/* ---- socket / uio underscored ------------------------------------------- */

int
__socket(int nDomain, int nType, int nProto)
{
    return socket(nDomain, nType, nProto);
}

int
__bind(int nFd, const struct sockaddr *pAddr, socklen_t nLen)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pAddr == NULL) {
        errno = EFAULT;
        return -1;
    }

    return bind(nFd, pAddr, nLen);
}

int
__listen(int nFd, int nBacklog)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return listen(nFd, nBacklog);
}

int
__accept(int nFd, struct sockaddr *pAddr, socklen_t *pLen)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return accept(nFd, pAddr, pLen);
}

int
__accept4(int nFd, struct sockaddr *pAddr, socklen_t *pLen, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return accept4(nFd, pAddr, pLen, nFlags);
}

int
__shutdown(int nFd, int nHow)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return shutdown(nFd, nHow);
}

int
__setsockopt(int nFd, int nLevel, int nOpt, const void *pVal, socklen_t nLen)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return setsockopt(nFd, nLevel, nOpt, pVal, nLen);
}

int
__getsockopt(int nFd, int nLevel, int nOpt, void *pVal, socklen_t *pLen)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pLen == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getsockopt(nFd, nLevel, nOpt, pVal, pLen);
}

int
__getpeername(int nFd, struct sockaddr *pAddr, socklen_t *pLen)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pLen == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getpeername(nFd, pAddr, pLen);
}

int
__getsockname(int nFd, struct sockaddr *pAddr, socklen_t *pLen)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pLen == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getsockname(nFd, pAddr, pLen);
}

ssize_t
__sendto(int nFd, const void *pBuf, size_t cb, int nFlags,
         const struct sockaddr *pAddr, socklen_t nLen)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return sendto(nFd, pBuf, cb, nFlags, pAddr, nLen);
}

ssize_t
__recvfrom(int nFd, void *pBuf, size_t cb, int nFlags, struct sockaddr *pAddr,
           socklen_t *pLen)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return recvfrom(nFd, pBuf, cb, nFlags, pAddr, pLen);
}

ssize_t
__sendmsg(int nFd, const struct msghdr *pMsg, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pMsg == NULL) {
        errno = EFAULT;
        return -1;
    }

    return sendmsg(nFd, pMsg, nFlags);
}

ssize_t
__recvmsg(int nFd, struct msghdr *pMsg, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pMsg == NULL) {
        errno = EFAULT;
        return -1;
    }

    return recvmsg(nFd, pMsg, nFlags);
}

int
__sendmmsg(int nFd, struct mmsghdr *pVec, unsigned nVlen, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pVec == NULL) {
        errno = EFAULT;
        return -1;
    }

    return sendmmsg(nFd, pVec, nVlen, nFlags);
}

int
__recvmmsg(int nFd, struct mmsghdr *pVec, unsigned nVlen, int nFlags,
           struct timespec *pTimeout)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pVec == NULL) {
        errno = EFAULT;
        return -1;
    }

    return recvmmsg(nFd, pVec, nVlen, nFlags, pTimeout);
}

ssize_t
__readv(int nFd, const struct iovec *pIov, int nIov)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pIov == NULL) {
        errno = EFAULT;
        return -1;
    }

    return readv(nFd, pIov, nIov);
}

ssize_t
__writev(int nFd, const struct iovec *pIov, int nIov)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pIov == NULL) {
        errno = EFAULT;
        return -1;
    }

    return writev(nFd, pIov, nIov);
}

/* ---- fd / path / wait / time underscored -------------------------------- */

int
__pipe(int aFd[2])
{
    return pipe(aFd);
}

int
__pipe2(int aFd[2], int nFlags)
{
    return pipe2(aFd, nFlags);
}

int
__dup(int nFd)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return dup(nFd);
}

int
__dup3(int nOld, int nNew, int nFlags)
{
    return dup3(nOld, nNew, nFlags);
}

int
__fcntl(int nFd, int nCmd, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    va_list ap;
    long nArg;

    va_start(ap, nCmd);
    nArg = va_arg(ap, long);
    va_end(ap);
    return fcntl(nFd, nCmd, nArg);
}

int
__fcntl64(int nFd, int nCmd, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    va_list ap;
    long nArg;

    va_start(ap, nCmd);
    nArg = va_arg(ap, long);
    va_end(ap);
    return fcntl(nFd, nCmd, nArg);
}

int
__ioctl(int nFd, unsigned long uReq, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    va_list ap;
    void *pArg;

    va_start(ap, uReq);
    pArg = va_arg(ap, void *);
    va_end(ap);
    return ioctl(nFd, uReq, pArg);
}

int
__nanosleep(const struct timespec *pReq, struct timespec *pRem)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pReq == NULL) {
        errno = EFAULT;
        return -1;
    }

    return nanosleep(pReq, pRem);
}

time_t
__time(time_t *pT)
{
    return time(pT);
}

int
__gettimeofday(struct timeval *pTv, void *pTz)
{
    return gettimeofday(pTv, pTz);
}

pid_t
__wait(int *pStatus)
{
    return wait(pStatus);
}

pid_t
__waitpid(pid_t pid, int *pStatus, int nOptions)
{
    return waitpid(pid, pStatus, nOptions);
}

pid_t
__wait4(pid_t pid, int *pStatus, int nOptions, struct rusage *pRu)
{
    return wait4(pid, pStatus, nOptions, pRu);
}

void *
__mmap(void *pAddr, size_t cb, int nProt, int nFlags, int nFd, off_t off)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return NULL;
    }

    return mmap(pAddr, cb, nProt, nFlags, nFd, off);
}

int
__munmap(void *pAddr, size_t cb)
{
    return munmap(pAddr, cb);
}

int
__mprotect(void *pAddr, size_t cb, int nProt)
{
    return mprotect(pAddr, cb, nProt);
}

int
__brk(void *pAddr)
{
    return brk(pAddr);
}

void *
__sbrk(intptr_t nInc)
{
    return sbrk(nInc);
}

char *
__getcwd(char *szBuf, size_t cb)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szBuf == NULL) {
        return NULL;
    }

    return getcwd(szBuf, cb);
}

int
__chdir(const char *szPath)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    return chdir(szPath);
}

int
__fchdir(int nFd)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return fchdir(nFd);
}

int
__mkdir(const char *szPath, mode_t mode)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    return mkdir(szPath, mode);
}

int
__rmdir(const char *szPath)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    return rmdir(szPath);
}

int
__unlink(const char *szPath)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    return unlink(szPath);
}

int
__rename(const char *szOld, const char *szNew)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szOld == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szNew == NULL) {
        errno = EFAULT;
        return -1;
    }

    return rename(szOld, szNew);
}

int
__access(const char *szPath, int nMode)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    return access(szPath, nMode);
}

int
__stat(const char *szPath, struct stat *pSt)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pSt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return stat(szPath, pSt);
}

int
__fstat(int nFd, struct stat *pSt)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (pSt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return fstat(nFd, pSt);
}

int
__lstat(const char *szPath, struct stat *pSt)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pSt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return lstat(szPath, pSt);
}

int
__fstatat(int nDfd, const char *szPath, struct stat *pSt, int nFlags)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nDfd < 0) {
        errno = EBADF;
        return -1;
    }
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pSt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return fstatat(nDfd, szPath, pSt, nFlags);
}

int
__openat(int nDfd, const char *szPath, int nFlags, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nDfd < 0) {
        errno = EBADF;
        return -1;
    }
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    mode_t mode = 0;

    if (nFlags & O_CREAT) {
        va_list ap;
        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    return openat(nDfd, szPath, nFlags, mode);
}

int
__openat64(int nDfd, const char *szPath, int nFlags, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nDfd < 0) {
        errno = EBADF;
        return -1;
    }
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    mode_t mode = 0;

    if (nFlags & O_CREAT) {
        va_list ap;
        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    return openat(nDfd, szPath, nFlags, mode);
}

/* ---- __libc_* aliases --------------------------------------------------- */

int
__libc_open(const char *szPath, int nFlags, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }

    mode_t mode = 0;

    if (nFlags & O_CREAT) {
        va_list ap;
        va_start(ap, nFlags);
        mode = (mode_t)va_arg(ap, int);
        va_end(ap);
    }
    return open(szPath, nFlags, mode);
}

ssize_t
__libc_read(int nFd, void *pBuf, size_t cb)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return read(nFd, pBuf, cb);
}

ssize_t
__libc_write(int nFd, const void *pBuf, size_t cb)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return write(nFd, pBuf, cb);
}

int
__libc_close(int nFd)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return close(nFd);
}

pid_t
__libc_fork(void)
{
    return fork();
}

off_t
__libc_lseek(int nFd, off_t off, int nWhence)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return lseek(nFd, off, nWhence);
}

int
__libc_fsync(int nFd)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return fsync(nFd);
}

ssize_t
__libc_pread(int nFd, void *pBuf, size_t cb, off_t off)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return pread(nFd, pBuf, cb, off);
}

ssize_t
__libc_pwrite(int nFd, const void *pBuf, size_t cb, off_t off)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return pwrite(nFd, pBuf, cb, off);
}

int
__libc_system(const char *szCmd)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szCmd == NULL) {
        errno = EFAULT;
        return -1;
    }

    return system(szCmd);
}

void *
__libc_reallocarray(void *p, size_t n, size_t cb)
{
    return reallocarray(p, n, cb);
}

char *
__secure_getenv(const char *szName)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szName == NULL) {
        return NULL;
    }

    return secure_getenv(szName);
}

/* ---- strto*_internal (glibc private integer parse shape) ---------------- */

long
__strtol_internal(const char *sz, char **ppEnd, int nBase, int nGroup)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nGroup;
    return strtol(sz, ppEnd, nBase);
}

unsigned long
__strtoul_internal(const char *sz, char **ppEnd, int nBase, int nGroup)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nGroup;
    return strtoul(sz, ppEnd, nBase);
}

long long
__strtoll_internal(const char *sz, char **ppEnd, int nBase, int nGroup)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nGroup;
    return strtoll(sz, ppEnd, nBase);
}

unsigned long long
__strtoull_internal(const char *sz, char **ppEnd, int nBase, int nGroup)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (sz == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nGroup;
    return strtoull(sz, ppEnd, nBase);
}

/* ---- wchar / time underscored ------------------------------------------- */

size_t
__mbrlen(const char *pS, size_t n, mbstate_t *pSt)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pS == NULL) {
        return 0;
    }
    if (pSt == NULL) {
        return 0;
    }

    return mbrtowc(NULL, pS, n, pSt);
}

int
__mbsinit(const mbstate_t *pSt)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pSt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return mbsinit(pSt);
}

struct tm *
__localtime_r(const time_t *pT, struct tm *pTm)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pT == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pTm == NULL) {
        errno = EFAULT;
        return -1;
    }

    return localtime_r(pT, pTm);
}

/* __fxstatat lives in aliases_glibc.c */

int
__statfs64(const char *szPath, struct statfs *pBuf)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szPath == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    return statfs(szPath, pBuf);
}

/* ---- isoc99 printf family (identity) ------------------------------------ */

int
__isoc99_printf(const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vprintf(szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc99_fprintf(FILE *pF, const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vfprintf(pF, szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc99_sprintf(char *szBuf, const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vsprintf(szBuf, szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc99_snprintf(char *szBuf, size_t cb, const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vsnprintf(szBuf, cb, szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc99_vprintf(const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vprintf(szFmt, ap);
}

int
__isoc99_vfprintf(FILE *pF, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vfprintf(pF, szFmt, ap);
}

int
__isoc99_vsprintf(char *szBuf, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vsprintf(szBuf, szFmt, ap);
}

int
__isoc99_vsnprintf(char *szBuf, size_t cb, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    return vsnprintf(szBuf, cb, szFmt, ap);
}

/* ---- fortify more ------------------------------------------------------- */

int
__asprintf_chk(int nFlag, char **ppBuf, const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (ppBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    (void)nFlag;
    va_start(ap, szFmt);
    n = vasprintf(ppBuf, szFmt, ap);
    va_end(ap);
    return n;
}

int
__vasprintf_chk(int nFlag, char **ppBuf, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (ppBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nFlag;
    return vasprintf(ppBuf, szFmt, ap);
}

int
__dprintf_chk(int nFd, int nFlag, const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;
    int n;

    (void)nFlag;
    va_start(ap, szFmt);
    n = vdprintf(nFd, szFmt, ap);
    va_end(ap);
    return n;
}

int
__vdprintf_chk(int nFd, int nFlag, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nFlag;
    return vdprintf(nFd, szFmt, ap);
}

void
__syslog_chk(int nFlag, int nPri, const char *szFmt, ...)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    va_list ap;

    (void)nFlag;
    va_start(ap, szFmt);
    vsyslog(nPri, szFmt, ap);
    va_end(ap);
}

void
__vsyslog_chk(int nPri, int nFlag, const char *szFmt, va_list ap)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szFmt == NULL) {
        errno = EFAULT;
        return -1;
    }

    (void)nFlag;
    vsyslog(nPri, szFmt, ap);
}

char *
__stpncpy_chk(char *szDst, const char *szSrc, size_t cb, size_t cbDst)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szDst == NULL) {
        return NULL;
    }
    if (szSrc == NULL) {
        return NULL;
    }

    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return stpncpy(szDst, szSrc, cb);
}

ssize_t
__pread_chk(int nFd, void *pBuf, size_t cb, off_t off, size_t cbDst)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return pread(nFd, pBuf, cb, off);
}

ssize_t
__pread64_chk(int nFd, void *pBuf, size_t cb, off_t off, size_t cbDst)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (nFd < 0) {
        errno = EBADF;
        return -1;
    }

    return __pread_chk(nFd, pBuf, cb, off, cbDst);
}

int
__gethostname_chk(char *szBuf, size_t cb, size_t cbDst)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return gethostname(szBuf, cb);
}

int
__getlogin_r_chk(char *szBuf, size_t cb, size_t cbDst)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }

    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return getlogin_r(szBuf, cb);
}

/* ---- setjmp aliases (no mask) ------------------------------------------- */

int
_setjmp(jmp_buf env)
{
    return setjmp(env);
}

int
__sigsetjmp(sigjmp_buf env, int nSavemask)
{
    (void)nSavemask;
    return sigsetjmp(env, 0);
}

/* ---- LFS stdio / dirent ------------------------------------------------- */

int
fseeko64(FILE *pF, off_t off, int nWhence)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return fseeko(pF, off, nWhence);
}

off_t
ftello64(FILE *pF)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pF == NULL) {
        return EOF;
    }

    return ftello(pF);
}

FILE *
tmpfile64(void)
{
    return tmpfile();
}

int
readdir64_r(DIR *pDir, struct dirent *pEntry, struct dirent **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pDir == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pEntry == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }

    return readdir_r(pDir, pEntry, ppResult);
}

/* ---- pwd/grp underscored reentrant -------------------------------------- */

int
__getpwnam_r(const char *szName, struct passwd *pPwd, char *szBuf, size_t cb,
             struct passwd **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pPwd == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getpwnam_r(szName, pPwd, szBuf, cb, ppResult);
}

int
__getpwuid_r(uid_t uid, struct passwd *pPwd, char *szBuf, size_t cb,
             struct passwd **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pPwd == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getpwuid_r(uid, pPwd, szBuf, cb, ppResult);
}

int
__getgrnam_r(const char *szName, struct group *pGrp, char *szBuf, size_t cb,
             struct group **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pGrp == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getgrnam_r(szName, pGrp, szBuf, cb, ppResult);
}

int
__getgrgid_r(gid_t gid, struct group *pGrp, char *szBuf, size_t cb,
             struct group **ppResult)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pGrp == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (szBuf == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (ppResult == NULL) {
        errno = EFAULT;
        return -1;
    }

    return getgrgid_r(gid, pGrp, szBuf, cb, ppResult);
}

/* ---- pthread underscored ------------------------------------------------ */

int
__pthread_mutex_lock(pthread_mutex_t *pM)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pM == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_mutex_lock(pM);
}

int
__pthread_mutex_unlock(pthread_mutex_t *pM)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pM == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_mutex_unlock(pM);
}

int
__pthread_mutex_trylock(pthread_mutex_t *pM)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pM == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_mutex_trylock(pM);
}

int
__pthread_key_create(pthread_key_t *pKey, void (*pfnDtor)(void *))
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pKey == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_key_create(pKey, pfnDtor);
}

void *
__pthread_getspecific(pthread_key_t key)
{
    return pthread_getspecific(key);
}

int
__pthread_setspecific(pthread_key_t key, const void *pVal)
{
    return pthread_setspecific(key, pVal);
}

int
__pthread_once(pthread_once_t *pOnce, void (*pfn)(void))
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pOnce == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_once(pOnce, pfn);
}

int
__pthread_rwlock_rdlock(pthread_rwlock_t *pRw)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pRw == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_rwlock_rdlock(pRw);
}

int
__pthread_rwlock_wrlock(pthread_rwlock_t *pRw)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pRw == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_rwlock_wrlock(pRw);
}

int
__pthread_rwlock_unlock(pthread_rwlock_t *pRw)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pRw == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_rwlock_unlock(pRw);
}

int
__pthread_cond_wait(pthread_cond_t *pC, pthread_mutex_t *pM)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pC == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pM == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_cond_wait(pC, pM);
}

int
__pthread_cond_signal(pthread_cond_t *pC)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pC == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_cond_signal(pC);
}

int
__pthread_cond_broadcast(pthread_cond_t *pC)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pC == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_cond_broadcast(pC);
}

int
__pthread_create(pthread_t *pTid, const pthread_attr_t *pAttr,
                 void *(*pfnStart)(void *), void *pArg)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pTid == NULL) {
        errno = EFAULT;
        return -1;
    }
    if (pAttr == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_create(pTid, pAttr, pfnStart, pArg);
}

int
__pthread_join(pthread_t tid, void **ppRet)
{
    return pthread_join(tid, ppRet);
}

int
__pthread_detach(pthread_t tid)
{
    return pthread_detach(tid);
}

pthread_t
__pthread_self(void)
{
    return pthread_self();
}

int
__pthread_equal(pthread_t a, pthread_t b)
{
    return pthread_equal(a, b);
}

void
__pthread_exit(void *pRet)
{
    pthread_exit(pRet);
}

int
__pthread_kill(pthread_t tid, int nSig)
{
    return pthread_kill(tid, nSig);
}

int
__pthread_sigmask(int nHow, const sigset_t *pSet, sigset_t *pOld)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (pSet == NULL) {
        errno = EFAULT;
        return -1;
    }

    return pthread_sigmask(nHow, pSet, pOld);
}

int
__pthread_atfork(void (*pfnPrepare)(void), void (*pfnParent)(void),
                 void (*pfnChild)(void))
{
    return pthread_atfork(pfnPrepare, pfnParent, pfnChild);
}

/* ---- net/if aliases (if_freenameindex lives in if.c) -------------------- */

unsigned int
__if_nametoindex(const char *szName)
{
    /* greppable: CGJ_GRAPH_BATCH16_SOFT_NULL */
    if (szName == NULL) {
        errno = EFAULT;
        return -1;
    }

    return if_nametoindex(szName);
}
