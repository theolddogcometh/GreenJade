/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * glibc graph aliases: *64, __isoc99_*, __xstat shape (x86_64 off_t is 64-bit).
 * Soft deepen: null/path checks, safer O_CREAT mode extraction, version
 * reject for unknown __xstat ver, isoc99 null-format guard.
 */
#include <dirent.h>
#include <errno.h>
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

	if (szPath == NULL) {
		errno = EFAULT;
		return -1;
	}
	/*
	 * O_CREAT and O_TMPFILE require a mode argument. Soft: also accept
	 * when only those bits request it (kernel may still reject TMPFILE).
	 */
	if ((nFlags & O_CREAT) != 0
#if defined(O_TMPFILE) && (O_TMPFILE != 0)
	    || (nFlags & O_TMPFILE) == O_TMPFILE
#endif
	) {
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

	if (szPath == NULL) {
		errno = EFAULT;
		return -1;
	}
	if ((nFlags & O_CREAT) != 0
#if defined(O_TMPFILE) && (O_TMPFILE != 0)
	    || (nFlags & O_TMPFILE) == O_TMPFILE
#endif
	) {
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
	if (nFd < 0) {
		errno = EBADF;
		return (off_t)-1;
	}
	return lseek(nFd, off, nWhence);
}

ssize_t
pread64(int nFd, void *pBuf, size_t cb, off_t off)
{
	if (nFd < 0) {
		errno = EBADF;
		return -1;
	}
	if (pBuf == NULL && cb != 0) {
		errno = EFAULT;
		return -1;
	}
	if (off < 0) {
		errno = EINVAL;
		return -1;
	}
	return pread(nFd, pBuf, cb, off);
}

ssize_t
pwrite64(int nFd, const void *pBuf, size_t cb, off_t off)
{
	if (nFd < 0) {
		errno = EBADF;
		return -1;
	}
	if (pBuf == NULL && cb != 0) {
		errno = EFAULT;
		return -1;
	}
	if (off < 0) {
		errno = EINVAL;
		return -1;
	}
	return pwrite(nFd, pBuf, cb, off);
}

void *
mmap64(void *pAddr, size_t cb, int nProt, int nFlags, int nFd, off_t off)
{
	if (off < 0) {
		errno = EINVAL;
		return MAP_FAILED;
	}
	return mmap(pAddr, cb, nProt, nFlags, nFd, off);
}

int
ftruncate64(int nFd, off_t cbLen)
{
	if (nFd < 0) {
		errno = EBADF;
		return -1;
	}
	if (cbLen < 0) {
		errno = EINVAL;
		return -1;
	}
	return ftruncate(nFd, cbLen);
}

int
truncate64(const char *szPath, off_t cbLen)
{
	if (szPath == NULL) {
		errno = EFAULT;
		return -1;
	}
	if (cbLen < 0) {
		errno = EINVAL;
		return -1;
	}
	return truncate(szPath, cbLen);
}

int
stat64(const char *szPath, struct stat *pSt)
{
	if (szPath == NULL || pSt == NULL) {
		errno = EFAULT;
		return -1;
	}
	return stat(szPath, pSt);
}

int
fstat64(int nFd, struct stat *pSt)
{
	if (pSt == NULL) {
		errno = EFAULT;
		return -1;
	}
	return fstat(nFd, pSt);
}

int
lstat64(const char *szPath, struct stat *pSt)
{
	if (szPath == NULL || pSt == NULL) {
		errno = EFAULT;
		return -1;
	}
	return lstat(szPath, pSt);
}

int
fstatat64(int nDfd, const char *szPath, struct stat *pSt, int nFlags)
{
	if (szPath == NULL || pSt == NULL) {
		errno = EFAULT;
		return -1;
	}
	return fstatat(nDfd, szPath, pSt, nFlags);
}

struct dirent *
readdir64(DIR *pDir)
{
	if (pDir == NULL) {
		errno = EBADF;
		return NULL;
	}
	return readdir(pDir);
}

int
scandir64(const char *szPath, struct dirent ***pppList,
	  int (*pfnFilter)(const struct dirent *),
	  int (*pfnCmp)(const struct dirent **, const struct dirent **))
{
	if (szPath == NULL || pppList == NULL) {
		errno = EFAULT;
		return -1;
	}
	return scandir(szPath, pppList, pfnFilter, pfnCmp);
}

int
glob64(const char *szPattern, int nFlags,
       int (*pfnErr)(const char *, int), glob_t *pGlob)
{
	if (szPattern == NULL || pGlob == NULL) {
		return GLOB_NOSPACE; /* soft: no GLOB_NOMATCH without pattern */
	}
	return glob(szPattern, nFlags, pfnErr, pGlob);
}

void
globfree64(glob_t *pGlob)
{
	if (pGlob == NULL) {
		return;
	}
	globfree(pGlob);
}

int
ftw64(const char *szPath,
      int (*pfn)(const char *, const struct stat *, int), int nFdLimit)
{
	if (szPath == NULL || pfn == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (nFdLimit < 1) {
		nFdLimit = 1;
	}
	return ftw(szPath, pfn, nFdLimit);
}

int
nftw64(const char *szPath,
       int (*pfn)(const char *, const struct stat *, int, struct FTW *),
       int nFdLimit, int nFlags)
{
	if (szPath == NULL || pfn == NULL) {
		errno = EINVAL;
		return -1;
	}
	if (nFdLimit < 1) {
		nFdLimit = 1;
	}
	return nftw(szPath, pfn, nFdLimit, nFlags);
}

int
posix_fadvise64(int nFd, off_t off, off_t cbLen, int nAdvice)
{
	if (nFd < 0) {
		return EBADF;
	}
	if (off < 0 || cbLen < 0) {
		return EINVAL;
	}
	return posix_fadvise(nFd, off, cbLen, nAdvice);
}

/* ---- legacy __xstat versioned entry points ------------------------------ */

/* Linux glibc _STAT_VER for x86_64 is 1; accept 0/1 soft. */
static int
xstat_ver_ok(int nVer)
{
	return nVer == 0 || nVer == 1 || nVer == 3;
}

int
__xstat(int nVer, const char *szPath, struct stat *pSt)
{
	if (!xstat_ver_ok(nVer)) {
		errno = EINVAL;
		return -1;
	}
	if (szPath == NULL || pSt == NULL) {
		errno = EFAULT;
		return -1;
	}
	return stat(szPath, pSt);
}

int
__fxstat(int nVer, int nFd, struct stat *pSt)
{
	if (!xstat_ver_ok(nVer)) {
		errno = EINVAL;
		return -1;
	}
	if (pSt == NULL) {
		errno = EFAULT;
		return -1;
	}
	return fstat(nFd, pSt);
}

int
__lxstat(int nVer, const char *szPath, struct stat *pSt)
{
	if (!xstat_ver_ok(nVer)) {
		errno = EINVAL;
		return -1;
	}
	if (szPath == NULL || pSt == NULL) {
		errno = EFAULT;
		return -1;
	}
	return lstat(szPath, pSt);
}

int
__xstat64(int nVer, const char *szPath, struct stat *pSt)
{
	return __xstat(nVer, szPath, pSt);
}

int
__fxstat64(int nVer, int nFd, struct stat *pSt)
{
	return __fxstat(nVer, nFd, pSt);
}

int
__lxstat64(int nVer, const char *szPath, struct stat *pSt)
{
	return __lxstat(nVer, szPath, pSt);
}

int
__fxstatat(int nVer, int nDfd, const char *szPath, struct stat *pSt, int nFlags)
{
	if (!xstat_ver_ok(nVer)) {
		errno = EINVAL;
		return -1;
	}
	if (szPath == NULL || pSt == NULL) {
		errno = EFAULT;
		return -1;
	}
	return fstatat(nDfd, szPath, pSt, nFlags);
}

int
__fxstatat64(int nVer, int nDfd, const char *szPath, struct stat *pSt,
	     int nFlags)
{
	return __fxstatat(nVer, nDfd, szPath, pSt, nFlags);
}

/* ---- C99 scanf aliases (same implementation) ---------------------------- */

int
__isoc99_scanf(const char *szFmt, ...)
{
	va_list ap;
	int n;

	if (szFmt == NULL) {
		errno = EINVAL;
		return EOF;
	}
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

	if (pF == NULL || szFmt == NULL) {
		errno = EINVAL;
		return EOF;
	}
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

	if (szBuf == NULL || szFmt == NULL) {
		errno = EINVAL;
		return EOF;
	}
	va_start(ap, szFmt);
	n = vsscanf(szBuf, szFmt, ap);
	va_end(ap);
	return n;
}

int
__isoc99_vscanf(const char *szFmt, va_list ap)
{
	if (szFmt == NULL) {
		errno = EINVAL;
		return EOF;
	}
	return vscanf(szFmt, ap);
}

int
__isoc99_vfscanf(FILE *pF, const char *szFmt, va_list ap)
{
	if (pF == NULL || szFmt == NULL) {
		errno = EINVAL;
		return EOF;
	}
	return vfscanf(pF, szFmt, ap);
}

int
__isoc99_vsscanf(const char *szBuf, const char *szFmt, va_list ap)
{
	if (szBuf == NULL || szFmt == NULL) {
		errno = EINVAL;
		return EOF;
	}
	return vsscanf(szBuf, szFmt, ap);
}

/* glibc also exports newfstatat as a public name (NR synonym of fstatat) */
int
newfstatat(int nDfd, const char *szPath, struct stat *pSt, int nFlags)
{
	if (szPath == NULL || pSt == NULL) {
		errno = EFAULT;
		return -1;
	}
	return fstatat(nDfd, szPath, pSt, nFlags);
}
