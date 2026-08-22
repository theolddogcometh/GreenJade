/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/time.h (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <sys/time.h>. Constants and type sizes
 * follow Linux LP64 (x86_64 / aarch64 product) where ABI numbers matter.
 *
 * Design notes
 * ------------
 * Pure C11 headers only — no inline runtime beyond macros. Implementations
 * live under user/libcgj/src/. Symbol versions are described by libc.map
 * (GLIBC_2.* nodes) for staged libc.so.6.
 *
 * Non-goals
 * ---------
 * Full POSIX/Linux completeness; stubs and soft fills may return ENOSYS
 * until the hybrid ABI path is wired. See docs/GLIBC_COMPAT.md.
 *
 * Soft residual lean (this unit; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   - timeval/gettimeofday surface; soft fills may ENOSYS until hybrid ABI
 *   - Soft!=product: libcgj sys/time surface != UDX/DDI product close
 *   - product = UDX/DDI + hot/cold Linux ABI; freestanding rtl/USB SKIP
 *   - agent PASS != close Dual DoD; no stamp storms; bar v2026.08.04.75 parent-only
 *   - dual MIT OR Apache-2.0; no GPL; no Linux .ko product AC (G-AC-1)
 * Grep: libcgj sys/time.h: soft residual lean Soft!=product
 */
#pragma once

#include <sys/types.h>
#include <time.h>
/* glibc sys/time.h pulls select under _GNU_SOURCE / __USE_MISC. */
#include <sys/select.h>

#ifdef __cplusplus
extern "C" {
#endif

/* timeval already in time.h for gettimeofday */

#ifndef timerisset
#define timerisset(pTv) ((pTv)->tv_sec || (pTv)->tv_usec)
#endif
#ifndef timerclear
#define timerclear(pTv) ((pTv)->tv_sec = (pTv)->tv_usec = 0)
#endif
#ifndef timercmp
#define timercmp(pA, pB, CMP) \
    (((pA)->tv_sec == (pB)->tv_sec) \
         ? ((pA)->tv_usec CMP (pB)->tv_usec) \
         : ((pA)->tv_sec CMP (pB)->tv_sec))
#endif
#ifndef timeradd
#define timeradd(pA, pB, pR) do { \
    (pR)->tv_sec = (pA)->tv_sec + (pB)->tv_sec; \
    (pR)->tv_usec = (pA)->tv_usec + (pB)->tv_usec; \
    if ((pR)->tv_usec >= 1000000) { \
        ++(pR)->tv_sec; \
        (pR)->tv_usec -= 1000000; \
    } \
} while (0)
#endif
#ifndef timersub
#define timersub(pA, pB, pR) do { \
    (pR)->tv_sec = (pA)->tv_sec - (pB)->tv_sec; \
    (pR)->tv_usec = (pA)->tv_usec - (pB)->tv_usec; \
    if ((pR)->tv_usec < 0) { \
        --(pR)->tv_sec; \
        (pR)->tv_usec += 1000000; \
    } \
} while (0)
#endif
#ifndef TIMEVAL_TO_TIMESPEC
#define TIMEVAL_TO_TIMESPEC(pTv, pTs) do { \
    (pTs)->tv_sec = (pTv)->tv_sec; \
    (pTs)->tv_nsec = (pTv)->tv_usec * 1000L; \
} while (0)
#endif
#ifndef TIMESPEC_TO_TIMEVAL
#define TIMESPEC_TO_TIMEVAL(pTv, pTs) do { \
    (pTv)->tv_sec = (pTs)->tv_sec; \
    (pTv)->tv_usec = (pTs)->tv_nsec / 1000; \
} while (0)
#endif
#ifndef timespeccmp
#define timespeccmp(pA, pB, CMP) \
    (((pA)->tv_sec == (pB)->tv_sec) \
         ? ((pA)->tv_nsec CMP (pB)->tv_nsec) \
         : ((pA)->tv_sec CMP (pB)->tv_sec))
#endif
#ifndef timespecclear
#define timespecclear(pTs) ((pTs)->tv_sec = (pTs)->tv_nsec = 0)
#endif
#ifndef timespecisset
#define timespecisset(pTs) ((pTs)->tv_sec || (pTs)->tv_nsec)
#endif

#define ITIMER_REAL    0
#define ITIMER_VIRTUAL 1
#define ITIMER_PROF    2

struct itimerval {
    struct timeval it_interval;
    struct timeval it_value;
};

int getitimer(int nWhich, struct itimerval *pCur);
int setitimer(int nWhich, const struct itimerval *pNew,
              struct itimerval *pOld);

/* adjtime(3) — clock slew (bring-up: query zeros; set may EPERM) */
int adjtime(const struct timeval *pDelta, struct timeval *pOldDelta);

int utimes(const char *szPath, const struct timeval aTv[2]);
int lutimes(const char *szPath, const struct timeval aTv[2]);
int futimes(int nFd, const struct timeval aTv[2]);
int futimens(int nFd, const struct timespec aTs[2]);
int utimensat(int nDfd, const char *szPath, const struct timespec aTs[2],
              int nFlags);

#ifdef __cplusplus
}
#endif
