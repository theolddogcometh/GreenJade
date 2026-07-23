/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch16: more underscored syscall/libc aliases,
 * fortify printf/syslog/path helpers, _setjmp/__sigsetjmp, LFS stdio,
 * readdir64_r, pthread underscored aliases, pwd/grp reentrant underscores.
 * Integer/pointer only (no SSE doubles).
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
    return bind(nFd, pAddr, nLen);
}

int
__listen(int nFd, int nBacklog)
{
    return listen(nFd, nBacklog);
}

int
__accept(int nFd, struct sockaddr *pAddr, socklen_t *pLen)
{
    return accept(nFd, pAddr, pLen);
}

int
__accept4(int nFd, struct sockaddr *pAddr, socklen_t *pLen, int nFlags)
{
    return accept4(nFd, pAddr, pLen, nFlags);
}

int
__shutdown(int nFd, int nHow)
{
    return shutdown(nFd, nHow);
}

int
__setsockopt(int nFd, int nLevel, int nOpt, const void *pVal, socklen_t nLen)
{
    return setsockopt(nFd, nLevel, nOpt, pVal, nLen);
}

int
__getsockopt(int nFd, int nLevel, int nOpt, void *pVal, socklen_t *pLen)
{
    return getsockopt(nFd, nLevel, nOpt, pVal, pLen);
}

int
__getpeername(int nFd, struct sockaddr *pAddr, socklen_t *pLen)
{
    return getpeername(nFd, pAddr, pLen);
}

int
__getsockname(int nFd, struct sockaddr *pAddr, socklen_t *pLen)
{
    return getsockname(nFd, pAddr, pLen);
}

ssize_t
__sendto(int nFd, const void *pBuf, size_t cb, int nFlags,
         const struct sockaddr *pAddr, socklen_t nLen)
{
    return sendto(nFd, pBuf, cb, nFlags, pAddr, nLen);
}

ssize_t
__recvfrom(int nFd, void *pBuf, size_t cb, int nFlags, struct sockaddr *pAddr,
           socklen_t *pLen)
{
    return recvfrom(nFd, pBuf, cb, nFlags, pAddr, pLen);
}

ssize_t
__sendmsg(int nFd, const struct msghdr *pMsg, int nFlags)
{
    return sendmsg(nFd, pMsg, nFlags);
}

ssize_t
__recvmsg(int nFd, struct msghdr *pMsg, int nFlags)
{
    return recvmsg(nFd, pMsg, nFlags);
}

int
__sendmmsg(int nFd, struct mmsghdr *pVec, unsigned nVlen, int nFlags)
{
    return sendmmsg(nFd, pVec, nVlen, nFlags);
}

int
__recvmmsg(int nFd, struct mmsghdr *pVec, unsigned nVlen, int nFlags,
           struct timespec *pTimeout)
{
    return recvmmsg(nFd, pVec, nVlen, nFlags, pTimeout);
}

ssize_t
__readv(int nFd, const struct iovec *pIov, int nIov)
{
    return readv(nFd, pIov, nIov);
}

ssize_t
__writev(int nFd, const struct iovec *pIov, int nIov)
{
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
    return getcwd(szBuf, cb);
}

int
__chdir(const char *szPath)
{
    return chdir(szPath);
}

int
__fchdir(int nFd)
{
    return fchdir(nFd);
}

int
__mkdir(const char *szPath, mode_t mode)
{
    return mkdir(szPath, mode);
}

int
__rmdir(const char *szPath)
{
    return rmdir(szPath);
}

int
__unlink(const char *szPath)
{
    return unlink(szPath);
}

int
__rename(const char *szOld, const char *szNew)
{
    return rename(szOld, szNew);
}

int
__access(const char *szPath, int nMode)
{
    return access(szPath, nMode);
}

int
__stat(const char *szPath, struct stat *pSt)
{
    return stat(szPath, pSt);
}

int
__fstat(int nFd, struct stat *pSt)
{
    return fstat(nFd, pSt);
}

int
__lstat(const char *szPath, struct stat *pSt)
{
    return lstat(szPath, pSt);
}

int
__fstatat(int nDfd, const char *szPath, struct stat *pSt, int nFlags)
{
    return fstatat(nDfd, szPath, pSt, nFlags);
}

int
__openat(int nDfd, const char *szPath, int nFlags, ...)
{
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
    return read(nFd, pBuf, cb);
}

ssize_t
__libc_write(int nFd, const void *pBuf, size_t cb)
{
    return write(nFd, pBuf, cb);
}

int
__libc_close(int nFd)
{
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
    return lseek(nFd, off, nWhence);
}

int
__libc_fsync(int nFd)
{
    return fsync(nFd);
}

ssize_t
__libc_pread(int nFd, void *pBuf, size_t cb, off_t off)
{
    return pread(nFd, pBuf, cb, off);
}

ssize_t
__libc_pwrite(int nFd, const void *pBuf, size_t cb, off_t off)
{
    return pwrite(nFd, pBuf, cb, off);
}

int
__libc_system(const char *szCmd)
{
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
    return secure_getenv(szName);
}

/* ---- strto*_internal (glibc private integer parse shape) ---------------- */

long
__strtol_internal(const char *sz, char **ppEnd, int nBase, int nGroup)
{
    (void)nGroup;
    return strtol(sz, ppEnd, nBase);
}

unsigned long
__strtoul_internal(const char *sz, char **ppEnd, int nBase, int nGroup)
{
    (void)nGroup;
    return strtoul(sz, ppEnd, nBase);
}

long long
__strtoll_internal(const char *sz, char **ppEnd, int nBase, int nGroup)
{
    (void)nGroup;
    return strtoll(sz, ppEnd, nBase);
}

unsigned long long
__strtoull_internal(const char *sz, char **ppEnd, int nBase, int nGroup)
{
    (void)nGroup;
    return strtoull(sz, ppEnd, nBase);
}

/* ---- wchar / time underscored ------------------------------------------- */

size_t
__mbrlen(const char *pS, size_t n, mbstate_t *pSt)
{
    return mbrtowc(NULL, pS, n, pSt);
}

int
__mbsinit(const mbstate_t *pSt)
{
    return mbsinit(pSt);
}

struct tm *
__localtime_r(const time_t *pT, struct tm *pTm)
{
    return localtime_r(pT, pTm);
}

/* __fxstatat lives in aliases_glibc.c */

int
__statfs64(const char *szPath, struct statfs *pBuf)
{
    return statfs(szPath, pBuf);
}

/* ---- isoc99 printf family (identity) ------------------------------------ */

int
__isoc99_printf(const char *szFmt, ...)
{
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
    return vprintf(szFmt, ap);
}

int
__isoc99_vfprintf(FILE *pF, const char *szFmt, va_list ap)
{
    return vfprintf(pF, szFmt, ap);
}

int
__isoc99_vsprintf(char *szBuf, const char *szFmt, va_list ap)
{
    return vsprintf(szBuf, szFmt, ap);
}

int
__isoc99_vsnprintf(char *szBuf, size_t cb, const char *szFmt, va_list ap)
{
    return vsnprintf(szBuf, cb, szFmt, ap);
}

/* ---- fortify more ------------------------------------------------------- */

int
__asprintf_chk(int nFlag, char **ppBuf, const char *szFmt, ...)
{
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
    (void)nFlag;
    return vasprintf(ppBuf, szFmt, ap);
}

int
__dprintf_chk(int nFd, int nFlag, const char *szFmt, ...)
{
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
    (void)nFlag;
    return vdprintf(nFd, szFmt, ap);
}

void
__syslog_chk(int nFlag, int nPri, const char *szFmt, ...)
{
    va_list ap;

    (void)nFlag;
    va_start(ap, szFmt);
    vsyslog(nPri, szFmt, ap);
    va_end(ap);
}

void
__vsyslog_chk(int nPri, int nFlag, const char *szFmt, va_list ap)
{
    (void)nFlag;
    vsyslog(nPri, szFmt, ap);
}

char *
__stpncpy_chk(char *szDst, const char *szSrc, size_t cb, size_t cbDst)
{
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return stpncpy(szDst, szSrc, cb);
}

ssize_t
__pread_chk(int nFd, void *pBuf, size_t cb, off_t off, size_t cbDst)
{
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return pread(nFd, pBuf, cb, off);
}

ssize_t
__pread64_chk(int nFd, void *pBuf, size_t cb, off_t off, size_t cbDst)
{
    return __pread_chk(nFd, pBuf, cb, off, cbDst);
}

int
__gethostname_chk(char *szBuf, size_t cb, size_t cbDst)
{
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return gethostname(szBuf, cb);
}

int
__getlogin_r_chk(char *szBuf, size_t cb, size_t cbDst)
{
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
    return fseeko(pF, off, nWhence);
}

off_t
ftello64(FILE *pF)
{
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
    return readdir_r(pDir, pEntry, ppResult);
}

/* ---- pwd/grp underscored reentrant -------------------------------------- */

int
__getpwnam_r(const char *szName, struct passwd *pPwd, char *szBuf, size_t cb,
             struct passwd **ppResult)
{
    return getpwnam_r(szName, pPwd, szBuf, cb, ppResult);
}

int
__getpwuid_r(uid_t uid, struct passwd *pPwd, char *szBuf, size_t cb,
             struct passwd **ppResult)
{
    return getpwuid_r(uid, pPwd, szBuf, cb, ppResult);
}

int
__getgrnam_r(const char *szName, struct group *pGrp, char *szBuf, size_t cb,
             struct group **ppResult)
{
    return getgrnam_r(szName, pGrp, szBuf, cb, ppResult);
}

int
__getgrgid_r(gid_t gid, struct group *pGrp, char *szBuf, size_t cb,
             struct group **ppResult)
{
    return getgrgid_r(gid, pGrp, szBuf, cb, ppResult);
}

/* ---- pthread underscored ------------------------------------------------ */

int
__pthread_mutex_lock(pthread_mutex_t *pM)
{
    return pthread_mutex_lock(pM);
}

int
__pthread_mutex_unlock(pthread_mutex_t *pM)
{
    return pthread_mutex_unlock(pM);
}

int
__pthread_mutex_trylock(pthread_mutex_t *pM)
{
    return pthread_mutex_trylock(pM);
}

int
__pthread_key_create(pthread_key_t *pKey, void (*pfnDtor)(void *))
{
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
    return pthread_once(pOnce, pfn);
}

int
__pthread_rwlock_rdlock(pthread_rwlock_t *pRw)
{
    return pthread_rwlock_rdlock(pRw);
}

int
__pthread_rwlock_wrlock(pthread_rwlock_t *pRw)
{
    return pthread_rwlock_wrlock(pRw);
}

int
__pthread_rwlock_unlock(pthread_rwlock_t *pRw)
{
    return pthread_rwlock_unlock(pRw);
}

int
__pthread_cond_wait(pthread_cond_t *pC, pthread_mutex_t *pM)
{
    return pthread_cond_wait(pC, pM);
}

int
__pthread_cond_signal(pthread_cond_t *pC)
{
    return pthread_cond_signal(pC);
}

int
__pthread_cond_broadcast(pthread_cond_t *pC)
{
    return pthread_cond_broadcast(pC);
}

int
__pthread_create(pthread_t *pTid, const pthread_attr_t *pAttr,
                 void *(*pfnStart)(void *), void *pArg)
{
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
    return if_nametoindex(szName);
}
