/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped sys/timex.h (subset). Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <sys/timex.h>. Constants and type sizes
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
 */
#pragma once

#include <sys/time.h>
#include <sys/types.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ADJ_OFFSET            0x0001
#define ADJ_FREQUENCY         0x0002
#define ADJ_MAXERROR          0x0004
#define ADJ_ESTERROR          0x0008
#define ADJ_STATUS            0x0010
#define ADJ_TIMECONST         0x0020
#define ADJ_TAI               0x0080
#define ADJ_SETOFFSET         0x0100
#define ADJ_MICRO             0x1000
#define ADJ_NANO              0x2000
#define ADJ_TICK              0x4000
#define ADJ_OFFSET_SINGLESHOT 0x8001
#define ADJ_OFFSET_SS_READ    0xa001

#define STA_PLL       0x0001
#define STA_PPSFREQ   0x0002
#define STA_PPSTIME   0x0004
#define STA_FLL       0x0008
#define STA_INS       0x0010
#define STA_DEL       0x0020
#define STA_UNSYNC    0x0040
#define STA_FREQHOLD  0x0080
#define STA_PPSSIGNAL 0x0100
#define STA_PPSJITTER 0x0200
#define STA_PPSWANDER 0x0400
#define STA_PPSERROR  0x0800
#define STA_CLOCKERR  0x1000
#define STA_NANO      0x2000
#define STA_MODE      0x4000
#define STA_CLK       0x8000

#define TIME_OK    0
#define TIME_INS   1
#define TIME_DEL   2
#define TIME_OOP   3
#define TIME_WAIT  4
#define TIME_ERROR 5
#define TIME_BAD   TIME_ERROR

struct timex {
    unsigned int modes;
    long         offset;
    long         freq;
    long         maxerror;
    long         esterror;
    int          status;
    long         constant;
    long         precision;
    long         tolerance;
    struct timeval time;
    long         tick;
    long         ppsfreq;
    long         jitter;
    int          shift;
    long         stabil;
    long         jitcnt;
    long         calcnt;
    long         errcnt;
    long         stbcnt;
    int          tai;
    int          __pad[11];
};

int adjtimex(struct timex *pBuf);
int clock_adjtime(clockid_t clk, struct timex *pBuf);
int ntp_adjtime(struct timex *pBuf);

#ifdef __cplusplus
}
#endif
