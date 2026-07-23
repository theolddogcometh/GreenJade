/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped shadow.h stubs. Not GNU glibc.
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
