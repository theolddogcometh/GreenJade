/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room <lastlog.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * OpenSSH-portable HAVE_LASTLOG_H. struct lastlog lives in <utmp.h>
 * (ll_time, ll_line, ll_host). This header is the include path.
 *
 * Dual DoD B OPEN. Soft!=product. G-AC-1. No stamp bump.
 */
#pragma once

#include <sys/time.h>
#include <sys/types.h>
#include <utmp.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * BSD lastlogx (OpenSSH HAVE_GETLASTLOGXBYNAME unset). Linux lastlog(5)
 * is struct lastlog in <utmp.h>; this is the extended retrieval shape.
 */
struct lastlogx {
    struct timeval ll_tv;
    char           ll_line[UT_LINESIZE];
    char           ll_host[UT_HOSTSIZE];
};

struct lastlogx *getlastlogx(uid_t uid, struct lastlogx *pLl);
struct lastlogx *getlastlogxbyname(const char *szName, struct lastlogx *pLl);

#ifdef __cplusplus
}
#endif
