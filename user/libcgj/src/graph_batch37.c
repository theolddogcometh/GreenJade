/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch37: fortify preadv/pwritev chk, __creat64 alias,
 * and freestanding ENOSYS stubs for qecvt/fcvt/gcvt (no FP math — symbols
 * only; -mgeneral-regs-only safe: parameters unused). Clean-room ABI.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <unistd.h>

ssize_t preadv(int nFd, const struct iovec *pIov, int nIovcnt, off_t off);
ssize_t pwritev(int nFd, const struct iovec *pIov, int nIovcnt, off_t off);
int creat(const char *szPath, mode_t mode);

const char __libcgj_batch37_marker[] = "libcgj-batch37";

/* ---- fortify-style bounded vector I/O ----------------------------------- */

ssize_t
__preadv_chk(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
             size_t cbTotal)
{
    size_t cb = 0;
    int i;

    if (pIov == NULL || nIovcnt < 0) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; i < nIovcnt; i++) {
        cb += pIov[i].iov_len;
        if (cb > cbTotal) {
            errno = EINVAL;
            return -1;
        }
    }
    return preadv(nFd, pIov, nIovcnt, off);
}

ssize_t
__pwritev_chk(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
              size_t cbTotal)
{
    size_t cb = 0;
    int i;

    if (pIov == NULL || nIovcnt < 0) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; i < nIovcnt; i++) {
        cb += pIov[i].iov_len;
        if (cb > cbTotal) {
            errno = EINVAL;
            return -1;
        }
    }
    return pwritev(nFd, pIov, nIovcnt, off);
}

/* ---- LFS creat underscored (creat64 already may exist) ------------------ */

int
__creat64(const char *szPath, mode_t mode)
{
    return creat(szPath, mode);
}

/*
 * qecvt/fcvt/gcvt: desktop graphs probe these. No FP math under freestanding
 * -mno-sse; return ENOSYS. Parameters deliberately unused.
 *
 * Note: x86_64 SysV would pass doubles in XMM; gcc with -mgeneral-regs-only
 * still accepts unused formal params on this toolchain when no FP ops occur.
 */

char *
qecvt(long double ldVal, int nNd, int *pDec, int *pSign)
{
    (void)ldVal;
    (void)nNd;
    if (pDec != NULL) {
        *pDec = 0;
    }
    if (pSign != NULL) {
        *pSign = 0;
    }
    errno = ENOSYS;
    return NULL;
}

char *
fcvt(double dVal, int nNd, int *pDec, int *pSign)
{
    (void)dVal;
    (void)nNd;
    if (pDec != NULL) {
        *pDec = 0;
    }
    if (pSign != NULL) {
        *pSign = 0;
    }
    errno = ENOSYS;
    return NULL;
}

char *
ecvt(double dVal, int nNd, int *pDec, int *pSign)
{
    (void)dVal;
    (void)nNd;
    if (pDec != NULL) {
        *pDec = 0;
    }
    if (pSign != NULL) {
        *pSign = 0;
    }
    errno = ENOSYS;
    return NULL;
}

char *
gcvt(double dVal, int nNd, char *szBuf)
{
    (void)dVal;
    (void)nNd;
    if (szBuf != NULL) {
        szBuf[0] = '\0';
    }
    errno = ENOSYS;
    return NULL;
}

int
ecvt_r(double dVal, int nNd, int *pDec, int *pSign, char *szBuf, size_t cb)
{
    (void)dVal;
    (void)nNd;
    (void)cb;
    if (pDec != NULL) {
        *pDec = 0;
    }
    if (pSign != NULL) {
        *pSign = 0;
    }
    if (szBuf != NULL && cb > 0u) {
        szBuf[0] = '\0';
    }
    errno = ENOSYS;
    return -1;
}

int
fcvt_r(double dVal, int nNd, int *pDec, int *pSign, char *szBuf, size_t cb)
{
    (void)dVal;
    (void)nNd;
    (void)cb;
    if (pDec != NULL) {
        *pDec = 0;
    }
    if (pSign != NULL) {
        *pSign = 0;
    }
    if (szBuf != NULL && cb > 0u) {
        szBuf[0] = '\0';
    }
    errno = ENOSYS;
    return -1;
}

int
qecvt_r(long double ldVal, int nNd, int *pDec, int *pSign, char *szBuf,
        size_t cb)
{
    (void)ldVal;
    (void)nNd;
    (void)cb;
    if (pDec != NULL) {
        *pDec = 0;
    }
    if (pSign != NULL) {
        *pSign = 0;
    }
    if (szBuf != NULL && cb > 0u) {
        szBuf[0] = '\0';
    }
    errno = ENOSYS;
    return -1;
}

int
qfcvt_r(long double ldVal, int nNd, int *pDec, int *pSign, char *szBuf,
        size_t cb)
{
    return qecvt_r(ldVal, nNd, pDec, pSign, szBuf, cb);
}

char *
qgcvt(long double ldVal, int nNd, char *szBuf)
{
    (void)ldVal;
    (void)nNd;
    if (szBuf != NULL) {
        szBuf[0] = '\0';
    }
    errno = ENOSYS;
    return NULL;
}

/*
 * qfcvt: classic non-reentrant long-double fcvt-shaped stub.
 * Missing from product surface (qfcvt_r exists). No FP math.
 */
char *
qfcvt(long double ldVal, int nNd, int *pDec, int *pSign)
{
    (void)ldVal;
    (void)nNd;
    if (pDec != NULL) {
        *pDec = 0;
    }
    if (pSign != NULL) {
        *pSign = 0;
    }
    errno = ENOSYS;
    return NULL;
}

/* ---- underscored aliases (unique; bases above) -------------------------- */

char *__ecvt(double dVal, int nNd, int *pDec, int *pSign)
    __attribute__((alias("ecvt")));
char *__fcvt(double dVal, int nNd, int *pDec, int *pSign)
    __attribute__((alias("fcvt")));
char *__gcvt(double dVal, int nNd, char *szBuf)
    __attribute__((alias("gcvt")));
char *__qecvt(long double ldVal, int nNd, int *pDec, int *pSign)
    __attribute__((alias("qecvt")));
char *__qfcvt(long double ldVal, int nNd, int *pDec, int *pSign)
    __attribute__((alias("qfcvt")));
char *__qgcvt(long double ldVal, int nNd, char *szBuf)
    __attribute__((alias("qgcvt")));
int __ecvt_r(double dVal, int nNd, int *pDec, int *pSign, char *szBuf,
             size_t cb) __attribute__((alias("ecvt_r")));
int __fcvt_r(double dVal, int nNd, int *pDec, int *pSign, char *szBuf,
             size_t cb) __attribute__((alias("fcvt_r")));
int __qecvt_r(long double ldVal, int nNd, int *pDec, int *pSign, char *szBuf,
              size_t cb) __attribute__((alias("qecvt_r")));
int __qfcvt_r(long double ldVal, int nNd, int *pDec, int *pSign, char *szBuf,
              size_t cb) __attribute__((alias("qfcvt_r")));

/* preadv2 fortify-shaped bound check (unique; bases may live elsewhere) */
ssize_t preadv2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
                int nFlags);
ssize_t pwritev2(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
                 int nFlags);

ssize_t
__preadv2_chk(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
              int nFlags, size_t cbTotal)
{
    size_t cb = 0;
    int i;

    if (pIov == NULL || nIovcnt < 0) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; i < nIovcnt; i++) {
        cb += pIov[i].iov_len;
        if (cb > cbTotal) {
            errno = EINVAL;
            return -1;
        }
    }
    return preadv2(nFd, pIov, nIovcnt, off, nFlags);
}

ssize_t
__pwritev2_chk(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
               int nFlags, size_t cbTotal)
{
    size_t cb = 0;
    int i;

    if (pIov == NULL || nIovcnt < 0) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; i < nIovcnt; i++) {
        cb += pIov[i].iov_len;
        if (cb > cbTotal) {
            errno = EINVAL;
            return -1;
        }
    }
    return pwritev2(nFd, pIov, nIovcnt, off, nFlags);
}

