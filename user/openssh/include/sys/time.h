/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <sys/time.h> for OpenSSH-portable (timercmp/timerisset in misc.c).
 * Host POSIX: glibc already has these. DUT: libcgj sys/time.h does not.
 */
#pragma once
#include_next <sys/time.h>

#ifndef timerisset
# define timerisset(pTv) ((pTv)->tv_sec || (pTv)->tv_usec)
#endif
#ifndef timerclear
# define timerclear(pTv) ((pTv)->tv_sec = (pTv)->tv_usec = 0)
#endif
#ifndef timercmp
# define timercmp(pA, pB, CMP) \
    (((pA)->tv_sec == (pB)->tv_sec) \
         ? ((pA)->tv_usec CMP (pB)->tv_usec) \
         : ((pA)->tv_sec CMP (pB)->tv_sec))
#endif
#ifndef timeradd
# define timeradd(pA, pB, pR) do { \
    (pR)->tv_sec = (pA)->tv_sec + (pB)->tv_sec; \
    (pR)->tv_usec = (pA)->tv_usec + (pB)->tv_usec; \
    if ((pR)->tv_usec >= 1000000) { \
        ++(pR)->tv_sec; \
        (pR)->tv_usec -= 1000000; \
    } \
} while (0)
#endif
#ifndef timersub
# define timersub(pA, pB, pR) do { \
    (pR)->tv_sec = (pA)->tv_sec - (pB)->tv_sec; \
    (pR)->tv_usec = (pA)->tv_usec - (pB)->tv_usec; \
    if ((pR)->tv_usec < 0) { \
        --(pR)->tv_sec; \
        (pR)->tv_usec += 1000000; \
    } \
} while (0)
#endif
#ifndef TIMEVAL_TO_TIMESPEC
# define TIMEVAL_TO_TIMESPEC(pTv, pTs) do { \
    (pTs)->tv_sec = (pTv)->tv_sec; \
    (pTs)->tv_nsec = (pTv)->tv_usec * 1000L; \
} while (0)
#endif
#ifndef TIMESPEC_TO_TIMEVAL
# define TIMESPEC_TO_TIMEVAL(pTv, pTs) do { \
    (pTv)->tv_sec = (pTs)->tv_sec; \
    (pTv)->tv_usec = (pTs)->tv_nsec / 1000; \
} while (0)
#endif
