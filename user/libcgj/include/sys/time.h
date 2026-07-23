/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/time.h (subset). Not GNU glibc.
 */
#pragma once

#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/* timeval already in time.h for gettimeofday */

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
