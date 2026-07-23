/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped shadow.h stubs. Not GNU glibc.
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * Public declarations for the libcgj (GreenJade freestanding libc) surface
 * matching glibc-shaped include path <shadow.h>. Constants and type sizes
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

#include <stdio.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct spwd {
    char   *sp_namp;
    char   *sp_pwdp;
    long    sp_lstchg;
    long    sp_min;
    long    sp_max;
    long    sp_warn;
    long    sp_inact;
    long    sp_expire;
    unsigned long sp_flag;
};

struct spwd *getspnam(const char *szName);
struct spwd *getspent(void);
void         setspent(void);
void         endspent(void);
int          putspent(const struct spwd *pSp, FILE *pF);
int          lckpwdf(void);
int          ulckpwdf(void);
struct spwd *fgetspent(FILE *pF);
struct spwd *sgetspent(const char *szLine);
int          getspnam_r(const char *szName, struct spwd *pSp, char *szBuf,
                        size_t cb, struct spwd **ppResult);
int          getspent_r(struct spwd *pSp, char *szBuf, size_t cb,
                        struct spwd **ppResult);
int          fgetspent_r(FILE *pF, struct spwd *pSp, char *szBuf, size_t cb,
                         struct spwd **ppResult);

#ifdef __cplusplus
}
#endif
