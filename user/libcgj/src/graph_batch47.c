/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch47: fortify/chk surface audit gate + missing
 * fortify nodes not already defined elsewhere (no multi-def).
 *
 * Already exported from fortify_chk.c / graph_batch15–23 (skip redefine):
 *   __readlinkat_chk, __getgroups_chk, __gethostname_chk,
 *   __getdomainname_chk, __getlogin_r_chk, __confstr_chk, __wctomb_chk,
 *   __mbsnrtowcs_chk, __wcsnrtombs_chk, __recv_chk, __recvfrom_chk,
 *   __read_chk, __pread_chk, __getwd_chk, __write_chk, __send_chk,
 *   __sendto_chk, __pwrite_chk, __poll_chk, __ppoll_chk, …
 *
 * Soft deepen (unique to this TU):
 *   __pwrite64_chk, __preadv64_chk, __pwritev64_chk,
 *   __select_chk, __ppoll64_chk,
 *   __inet_ntop_chk, __inet_pton_chk,
 *   __libcgj_batch47_fortify_probe (dyn smoke helper),
 *   __libcgj_batch47_marker.
 *
 * Clean-room pure C; integer/pointer only.
 */
#include <errno.h>
#include <poll.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/uio.h>
#include <time.h>
#include <unistd.h>

/* Avoid <sys/socket.h> (comment markers can trip -Werror on some builds). */
#ifndef _GJ_SOCKLEN_T_DEFINED
typedef unsigned int socklen_t;
#endif

void __chk_fail(void);

/* Declarations only — implementations live in other TUs. */
ssize_t pwrite(int nFd, const void *pBuf, size_t cb, off_t off);
int select(int nNfds, fd_set *pR, fd_set *pW, fd_set *pE, struct timeval *pTv);
const char *inet_ntop(int nAf, const void *pSrc, char *szDst, socklen_t cbDst);
int inet_pton(int nAf, const char *szSrc, void *pDst);

const char __libcgj_batch47_marker[] = "libcgj-batch47";

/* ---- pwrite64 fortify (off_t is 64-bit on LP64) -------------------------- */

ssize_t
__pwrite64_chk(int nFd, const void *pBuf, size_t cb, off_t off, size_t cbSrc)
{
    if (cbSrc != (size_t)-1 && cb > cbSrc) {
        __chk_fail();
    }
    return pwrite(nFd, pBuf, cb, off);
}

/* ---- vector I/O fortify (64-bit off alias path) ------------------------- */

static int
b47_iov_bos_ok(const struct iovec *pIov, int nIovcnt, size_t cbTotal)
{
    size_t cb;
    int i;

    if (pIov == NULL || nIovcnt < 0) {
        return 0;
    }
    cb = 0;
    for (i = 0; i < nIovcnt; i++) {
        cb += pIov[i].iov_len;
        if (cbTotal != (size_t)-1 && cb > cbTotal) {
            return 0;
        }
    }
    return 1;
}

ssize_t
__preadv64_chk(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
               size_t cbTotal)
{
    if (!b47_iov_bos_ok(pIov, nIovcnt, cbTotal)) {
        if (pIov == NULL || nIovcnt < 0) {
            errno = EINVAL;
            return -1;
        }
        __chk_fail();
    }
    return preadv(nFd, pIov, nIovcnt, off);
}

ssize_t
__pwritev64_chk(int nFd, const struct iovec *pIov, int nIovcnt, off_t off,
                size_t cbTotal)
{
    if (!b47_iov_bos_ok(pIov, nIovcnt, cbTotal)) {
        if (pIov == NULL || nIovcnt < 0) {
            errno = EINVAL;
            return -1;
        }
        __chk_fail();
    }
    return pwritev(nFd, pIov, nIovcnt, off);
}

/* ---- select fortify (fd_set object size) -------------------------------- */

int
__select_chk(int nNfds, fd_set *pR, fd_set *pW, fd_set *pE,
             struct timeval *pTv, size_t cbFdset)
{
    /* When bos known: require space for FD_SETSIZE bits (classic glibc). */
    if (cbFdset != (size_t)-1 && cbFdset < sizeof(fd_set)) {
        __chk_fail();
    }
    if (nNfds < 0) {
        errno = EINVAL;
        return -1;
    }
    return select(nNfds, pR, pW, pE, pTv);
}

/* ---- ppoll64 fortify (timespec 64-bit path name) ------------------------ */

int
__ppoll64_chk(struct pollfd *pFds, nfds_t nFds, const struct timespec *pTs,
              const void *pMask, size_t cbFds)
{
    if (pFds == NULL && nFds != 0) {
        errno = EFAULT;
        return -1;
    }
    if (cbFds != (size_t)-1 && nFds > 0 &&
        ((size_t)nFds > cbFds / sizeof(struct pollfd))) {
        __chk_fail();
    }
    /* poll.h: ppoll mask is const void * */
    return ppoll(pFds, nFds, pTs, pMask);
}

/* ---- inet_ntop / inet_pton fortify -------------------------------------- */

const char *
__inet_ntop_chk(int nAf, const void *pSrc, char *szDst, socklen_t cbDst,
                size_t cbDstObj)
{
    if (szDst != NULL && cbDstObj != (size_t)-1 &&
        (size_t)cbDst > cbDstObj) {
        __chk_fail();
    }
    return inet_ntop(nAf, pSrc, szDst, cbDst);
}

int
__inet_pton_chk(int nAf, const char *szSrc, void *pDst, size_t cbDstObj)
{
    size_t cbNeed;

    /* AF_INET needs 4, AF_INET6 needs 16; unknown AF: skip size check. */
    if (nAf == 2 /* AF_INET */) {
        cbNeed = 4u;
    } else if (nAf == 10 /* AF_INET6 */) {
        cbNeed = 16u;
    } else {
        cbNeed = 0u;
    }
    if (cbNeed != 0u && cbDstObj != (size_t)-1 && cbDstObj < cbNeed) {
        __chk_fail();
    }
    return inet_pton(nAf, szSrc, pDst);
}

/*
 * Dyn-smoke / audit probe: returns a bitset of soft-deepen fortify nodes
 * present in this TU (never aborts). Bit0=pwrite64,1=preadv64,2=pwritev64,
 * 3=select,4=ppoll64,5=inet_ntop,6=inet_pton.
 */
unsigned
__libcgj_batch47_fortify_probe(void)
{
    unsigned u;

    u = 0u;
    u |= 1u << 0; /* __pwrite64_chk */
    u |= 1u << 1; /* __preadv64_chk */
    u |= 1u << 2; /* __pwritev64_chk */
    u |= 1u << 3; /* __select_chk */
    u |= 1u << 4; /* __ppoll64_chk */
    u |= 1u << 5; /* __inet_ntop_chk */
    u |= 1u << 6; /* __inet_pton_chk */
    return u;
}

unsigned __libcgj_batch47_fortify_probe_alias(void)
    __attribute__((alias("__libcgj_batch47_fortify_probe")));
