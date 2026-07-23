/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Fortify / _FORTIFY_SOURCE graph nodes — length-aware wrappers.
 * Bring-up: object size not tracked; fail only on obvious overflow.
 */
#include <errno.h>
#include <fcntl.h>
#include <poll.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

void
__chk_fail(void)
{
    static const char sz[] = "libcgj: fortify check failed\n";

    (void)write(2, sz, sizeof(sz) - 1);
    abort();
}

void
__fortify_fail(const char *szMsg)
{
    static const char szPfx[] = "libcgj: fortify: ";
    const char *p = (szMsg != NULL) ? szMsg : "fail";

    (void)write(2, szPfx, sizeof(szPfx) - 1);
    while (*p) {
        (void)write(2, p, 1);
        p++;
    }
    (void)write(2, "\n", 1);
    abort();
}

void *
__memcpy_chk(void *pDst, const void *pSrc, size_t cb, size_t cbDst)
{
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return memcpy(pDst, pSrc, cb);
}

void *
__memmove_chk(void *pDst, const void *pSrc, size_t cb, size_t cbDst)
{
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return memmove(pDst, pSrc, cb);
}

void *
__memset_chk(void *pDst, int c, size_t cb, size_t cbDst)
{
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return memset(pDst, c, cb);
}

void *
__mempcpy_chk(void *pDst, const void *pSrc, size_t cb, size_t cbDst)
{
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return mempcpy(pDst, pSrc, cb);
}

char *
__strcpy_chk(char *szDst, const char *szSrc, size_t cbDst)
{
    size_t n;

    if (szSrc == NULL) {
        return strcpy(szDst, szSrc);
    }
    n = strlen(szSrc) + 1;
    if (cbDst != (size_t)-1 && n > cbDst) {
        __chk_fail();
    }
    return strcpy(szDst, szSrc);
}

char *
__stpcpy_chk(char *szDst, const char *szSrc, size_t cbDst)
{
    size_t n;

    if (szSrc == NULL) {
        return stpcpy(szDst, szSrc);
    }
    n = strlen(szSrc) + 1;
    if (cbDst != (size_t)-1 && n > cbDst) {
        __chk_fail();
    }
    return stpcpy(szDst, szSrc);
}

char *
__strncpy_chk(char *szDst, const char *szSrc, size_t cb, size_t cbDst)
{
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return strncpy(szDst, szSrc, cb);
}

char *
__strcat_chk(char *szDst, const char *szSrc, size_t cbDst)
{
    size_t nDst;
    size_t nSrc;

    if (szDst == NULL || szSrc == NULL) {
        return strcat(szDst, szSrc);
    }
    nDst = strlen(szDst);
    nSrc = strlen(szSrc);
    if (cbDst != (size_t)-1 && nDst + nSrc + 1 > cbDst) {
        __chk_fail();
    }
    return strcat(szDst, szSrc);
}

int
__printf_chk(int nFlag, const char *szFmt, ...)
{
    va_list ap;
    int n;

    (void)nFlag;
    va_start(ap, szFmt);
    n = vprintf(szFmt, ap);
    va_end(ap);
    return n;
}

int
__fprintf_chk(FILE *pF, int nFlag, const char *szFmt, ...)
{
    va_list ap;
    int n;

    (void)nFlag;
    va_start(ap, szFmt);
    n = vfprintf(pF, szFmt, ap);
    va_end(ap);
    return n;
}

int
__sprintf_chk(char *szBuf, int nFlag, size_t cbDst, const char *szFmt, ...)
{
    va_list ap;
    int n;

    (void)nFlag;
    va_start(ap, szFmt);
    n = vsnprintf(szBuf, cbDst, szFmt, ap);
    va_end(ap);
    return n;
}

int
__vsprintf_chk(char *szBuf, int nFlag, size_t cbDst, const char *szFmt,
               va_list ap)
{
    (void)nFlag;
    return vsnprintf(szBuf, cbDst, szFmt, ap);
}

int
__snprintf_chk(char *szBuf, size_t cb, int nFlag, size_t cbDst,
               const char *szFmt, ...)
{
    va_list ap;
    int n;
    size_t lim = cb;

    (void)nFlag;
    if (cbDst != (size_t)-1 && lim > cbDst) {
        lim = cbDst;
    }
    va_start(ap, szFmt);
    n = vsnprintf(szBuf, lim, szFmt, ap);
    va_end(ap);
    return n;
}

int
__vsnprintf_chk(char *szBuf, size_t cb, int nFlag, size_t cbDst,
                const char *szFmt, va_list ap)
{
    size_t lim = cb;

    (void)nFlag;
    if (cbDst != (size_t)-1 && lim > cbDst) {
        lim = cbDst;
    }
    return vsnprintf(szBuf, lim, szFmt, ap);
}

char *
__fgets_chk(char *sz, size_t cbDst, int n, FILE *pF)
{
    if (n > 0 && cbDst != (size_t)-1 && (size_t)n > cbDst) {
        __chk_fail();
    }
    return fgets(sz, n, pF);
}

size_t
__fread_chk(void *p, size_t cb, size_t n, size_t cbDst, FILE *pF)
{
    size_t total;

    if (cb != 0 && n > (size_t)-1 / cb) {
        __chk_fail();
    }
    total = cb * n;
    if (cbDst != (size_t)-1 && total > cbDst) {
        __chk_fail();
    }
    return fread(p, cb, n, pF);
}

ssize_t
__read_chk(int nFd, void *pBuf, size_t cb, size_t cbDst)
{
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return read(nFd, pBuf, cb);
}

ssize_t
__recv_chk(int nFd, void *pBuf, size_t cb, int nFlags, size_t cbDst)
{
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return recv(nFd, pBuf, cb, nFlags);
}

ssize_t
__recvfrom_chk(int nFd, void *pBuf, size_t cb, int nFlags,
               struct sockaddr *pAddr, socklen_t *pAlen, size_t cbDst)
{
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return recvfrom(nFd, pBuf, cb, nFlags, pAddr, pAlen);
}

char *
__getcwd_chk(char *szBuf, size_t cb, size_t cbDst)
{
    if (cbDst != (size_t)-1 && cb > cbDst) {
        __chk_fail();
    }
    return getcwd(szBuf, cb);
}

char *
__realpath_chk(const char *szPath, char *szResolved, size_t cbResolved)
{
    char *p;

    (void)cbResolved;
    p = realpath(szPath, szResolved);
    return p;
}

int
__poll_chk(struct pollfd *pFds, nfds_t nFds, int nTimeoutMs, size_t cbFds)
{
    if (cbFds != (size_t)-1 &&
        (size_t)nFds > cbFds / sizeof(struct pollfd)) {
        __chk_fail();
    }
    return poll(pFds, nFds, nTimeoutMs);
}

int
__ppoll_chk(struct pollfd *pFds, nfds_t nFds, const struct timespec *pTs,
            const void *pMask, size_t cbFds)
{
    if (cbFds != (size_t)-1 &&
        (size_t)nFds > cbFds / sizeof(struct pollfd)) {
        __chk_fail();
    }
    return ppoll(pFds, nFds, pTs, pMask);
}

/* open with O_CREAT must supply mode — fortify open_2 rejects O_CREAT alone. */
int
__open_2(const char *szPath, int nFlags)
{
    if (nFlags & O_CREAT) {
        __fortify_fail("invalid open call: O_CREAT without mode");
    }
    return open(szPath, nFlags);
}

long
__fdelt_chk(long d)
{
    if (d < 0 || d >= 1024) {
        __chk_fail();
    }
    return d;
}
