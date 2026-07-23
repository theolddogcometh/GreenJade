/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <time.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * clock_gettime/settime/getres, nanosleep, timers, calendar conversion
 * (gmtime/localtime/mktime/strftime), and CLOCK_* / TIMER_ABSTIME constants
 * matching Linux clock ids for hybrid syscalls.
 *
 * Design notes
 * ------------
 * timespec/timeval/tm layouts are LP64 Linux-shaped. CLOCKS_PER_SEC is 1e6
 * (glibc). Bring-up timezone is often UTC/C only.
 *
 * Non-goals
 * ---------
 * Full tzdb leap-second policy and every obscure clock id.
 * See docs/GLIBC_COMPAT.md.
 */
#pragma once

#include <signal.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Clock ids / TIMER_ABSTIME (Linux) --------------------------------- */

#define CLOCK_REALTIME           0
#define CLOCK_MONOTONIC          1
#define CLOCK_PROCESS_CPUTIME_ID 2
#define CLOCK_THREAD_CPUTIME_ID  3

/* timer_settime / clock_nanosleep absolute flag */
#define TIMER_ABSTIME 1

#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC ((clock_t)1000000)
#endif

typedef void *timer_t;

clock_t clock(void);

/* ---- Time structures --------------------------------------------------- */

struct timespec {
    time_t tv_sec;
    long   tv_nsec;
};

struct timeval {
    time_t tv_sec;
    long   tv_usec;
};

struct itimerspec {
    struct timespec it_interval;
    struct timespec it_value;
};

struct tm {
    int tm_sec;
    int tm_min;
    int tm_hour;
    int tm_mday;
    int tm_mon;
    int tm_year;
    int tm_wday;
    int tm_yday;
    int tm_isdst;
    long tm_gmtoff;
    const char *tm_zone;
};

time_t time(time_t *pT);
int    clock_gettime(clockid_t clk, struct timespec *pTs);
int    __clock_gettime(clockid_t clk, struct timespec *pTs);
int    clock_settime(clockid_t clk, const struct timespec *pTs);
int    clock_getres(clockid_t clk, struct timespec *pTs);
int    clock_nanosleep(clockid_t clk, int nFlags, const struct timespec *pReq,
                       struct timespec *pRem);
int    clock_getcpuclockid(pid_t pid, clockid_t *pClk);
int    gettimeofday(struct timeval *pTv, void *pTz);
int    nanosleep(const struct timespec *pReq, struct timespec *pRem);

/* C11 timespec_get base */
#ifndef TIME_UTC
#define TIME_UTC 1
#endif
int timespec_get(struct timespec *pTs, int nBase);
int timespec_getres(struct timespec *pTs, int nBase);

/* POSIX interval timers (glibc-shaped) */
int timer_create(clockid_t clk, struct sigevent *pEv, timer_t *pTimerid);
int timer_delete(timer_t timerid);
int timer_settime(timer_t timerid, int nFlags,
                  const struct itimerspec *pNew, struct itimerspec *pOld);
int timer_gettime(timer_t timerid, struct itimerspec *pCur);
int timer_getoverrun(timer_t timerid);
struct tm *gmtime_r(const time_t *pT, struct tm *pTm);
struct tm *localtime_r(const time_t *pT, struct tm *pTm);
struct tm *gmtime(const time_t *pT);
struct tm *localtime(const time_t *pT);
char   *asctime_r(const struct tm *pTm, char *szBuf);
char   *asctime(const struct tm *pTm);
char   *ctime_r(const time_t *pT, char *szBuf);
char   *ctime(const time_t *pT);
size_t  strftime(char *szBuf, size_t cb, const char *szFmt,
                 const struct tm *pTm);
char   *strptime(const char *szBuf, const char *szFmt, struct tm *pTm);
time_t  timegm(struct tm *pTm);
time_t  mktime(struct tm *pTm);
void    tzset(void);

extern char  *tzname[2];
extern long   timezone;
extern int    daylight;

time_t  timelocal(struct tm *pTm);
char   *strptime_l(const char *szBuf, const char *szFmt, struct tm *pTm,
                   void *loc);
extern int getdate_err;
struct tm *getdate(const char *sz);
int getdate_r(const char *sz, struct tm *pTm);

#ifdef __cplusplus
}
#endif

