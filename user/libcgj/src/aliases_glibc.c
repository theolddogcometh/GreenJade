/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * glibc graph aliases: *64, __isoc99_*, __xstat shape (x86_64 off_t is 64-bit).
 */
#include <dirent.h>
#include <fcntl.h>
#include <ftw.h>
#include <glob.h>
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

/* ---- 64-bit LFS aliases (identity on x86_64) ---------------------------- */

int
open64(const char *szPath, int nFlags, ...)
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

int
openat64(int nDfd, const char *szPath, int nFlags, ...)
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

off_t
lseek64(int nFd, off_t off, int nWhence)
{
    return lseek(nFd, off, nWhence);
}

ssize_t
pread64(int nFd, void *pBuf, size_t cb, off_t off)
{
    return pread(nFd, pBuf, cb, off);
}

ssize_t
pwrite64(int nFd, const void *pBuf, size_t cb, off_t off)
{
    return pwrite(nFd, pBuf, cb, off);
}

void *
mmap64(void *pAddr, size_t cb, int nProt, int nFlags, int nFd, off_t off)
{
    return mmap(pAddr, cb, nProt, nFlags, nFd, off);
}

int
ftruncate64(int nFd, off_t cbLen)
{
    return ftruncate(nFd, cbLen);
}

int
truncate64(const char *szPath, off_t cbLen)
{
    return truncate(szPath, cbLen);
}

int
stat64(const char *szPath, struct stat *pSt)
{
    return stat(szPath, pSt);
}

int
fstat64(int nFd, struct stat *pSt)
{
    return fstat(nFd, pSt);
}

int
lstat64(const char *szPath, struct stat *pSt)
{
    return lstat(szPath, pSt);
}

int
fstatat64(int nDfd, const char *szPath, struct stat *pSt, int nFlags)
{
    return fstatat(nDfd, szPath, pSt, nFlags);
}

struct dirent *
readdir64(DIR *pDir)
{
    return readdir(pDir);
}

int
scandir64(const char *szPath, struct dirent ***pppList,
          int (*pfnFilter)(const struct dirent *),
          int (*pfnCmp)(const struct dirent **, const struct dirent **))
{
    return scandir(szPath, pppList, pfnFilter, pfnCmp);
}

int
glob64(const char *szPattern, int nFlags,
       int (*pfnErr)(const char *, int), glob_t *pGlob)
{
    return glob(szPattern, nFlags, pfnErr, pGlob);
}

void
globfree64(glob_t *pGlob)
{
    globfree(pGlob);
}

int
ftw64(const char *szPath,
      int (*pfn)(const char *, const struct stat *, int), int nFdLimit)
{
    return ftw(szPath, pfn, nFdLimit);
}

int
nftw64(const char *szPath,
       int (*pfn)(const char *, const struct stat *, int, struct FTW *),
       int nFdLimit, int nFlags)
{
    return nftw(szPath, pfn, nFdLimit, nFlags);
}

int
posix_fadvise64(int nFd, off_t off, off_t cbLen, int nAdvice)
{
    return posix_fadvise(nFd, off, cbLen, nAdvice);
}

/* ---- legacy __xstat versioned entry points ------------------------------ */

int
__xstat(int nVer, const char *szPath, struct stat *pSt)
{
    (void)nVer;
    return stat(szPath, pSt);
}

int
__fxstat(int nVer, int nFd, struct stat *pSt)
{
    (void)nVer;
    return fstat(nFd, pSt);
}

int
__lxstat(int nVer, const char *szPath, struct stat *pSt)
{
    (void)nVer;
    return lstat(szPath, pSt);
}

int
__xstat64(int nVer, const char *szPath, struct stat *pSt)
{
    (void)nVer;
    return stat(szPath, pSt);
}

int
__fxstat64(int nVer, int nFd, struct stat *pSt)
{
    (void)nVer;
    return fstat(nFd, pSt);
}

int
__lxstat64(int nVer, const char *szPath, struct stat *pSt)
{
    (void)nVer;
    return lstat(szPath, pSt);
}

int
__fxstatat(int nVer, int nDfd, const char *szPath, struct stat *pSt, int nFlags)
{
    (void)nVer;
    return fstatat(nDfd, szPath, pSt, nFlags);
}

int
__fxstatat64(int nVer, int nDfd, const char *szPath, struct stat *pSt,
             int nFlags)
{
    (void)nVer;
    return fstatat(nDfd, szPath, pSt, nFlags);
}

/* ---- C99 scanf aliases (same implementation) ---------------------------- */

int
__isoc99_scanf(const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vscanf(szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc99_fscanf(FILE *pF, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vfscanf(pF, szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc99_sscanf(const char *szBuf, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vsscanf(szBuf, szFmt, ap);
    va_end(ap);
    return n;
}

int
__isoc99_vscanf(const char *szFmt, va_list ap)
{
    return vscanf(szFmt, ap);
}

int
__isoc99_vfscanf(FILE *pF, const char *szFmt, va_list ap)
{
    return vfscanf(pF, szFmt, ap);
}

int
__isoc99_vsscanf(const char *szBuf, const char *szFmt, va_list ap)
{
    return vsscanf(szBuf, szFmt, ap);
}

/* glibc also exports newfstatat as a public name (NR synonym of fstatat) */
int
newfstatat(int nDfd, const char *szPath, struct stat *pSt, int nFlags)
{
    return fstatat(nDfd, szPath, pSt, nFlags);
}
