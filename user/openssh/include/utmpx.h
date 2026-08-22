/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <utmpx.h> for OpenSSH-portable (HAVE_UTMPX_H).
 * Host POSIX: glibc. DUT: do not include_next libcgj utmpx.h (its file
 * comment contains a star-slash sequence that ends the comment).
 */
#pragma once
#if __STDC_HOSTED__
# include_next <utmpx.h>
#else

#include <sys/types.h>
#include <sys/time.h>

struct utmp;

#ifndef __UT_LINESIZE
# define __UT_LINESIZE 32
#endif
#ifndef __UT_NAMESIZE
# define __UT_NAMESIZE 32
#endif
#ifndef __UT_HOSTSIZE
# define __UT_HOSTSIZE 256
#endif

#ifndef EMPTY
# define EMPTY         0
# define RUN_LVL       1
# define BOOT_TIME     2
# define NEW_TIME      3
# define OLD_TIME      4
# define INIT_PROCESS  5
# define LOGIN_PROCESS 6
# define USER_PROCESS  7
# define DEAD_PROCESS  8
# define ACCOUNTING    9
#endif

struct utmpx {
    short   ut_type;
    pid_t   ut_pid;
    char    ut_line[__UT_LINESIZE];
    char    ut_id[4];
    char    ut_user[__UT_NAMESIZE];
    char    ut_host[__UT_HOSTSIZE];
    struct {
        short e_termination;
        short e_exit;
    } ut_exit;
    long    ut_session;
    struct timeval ut_tv;
    int     ut_addr_v6[4];
    char    ut_unused[20];
};

struct utmpx *getutxent(void);
void          setutxent(void);
void          endutxent(void);
struct utmpx *getutxid(const struct utmpx *pId);
struct utmpx *getutxline(const struct utmpx *pLine);
struct utmpx *pututxline(const struct utmpx *pUtx);
int           utmpxname(const char *szFile);
void          updwtmpx(const char *szFile, const struct utmpx *pUtx);
void          getutmp(const struct utmpx *pUtx, struct utmp *pUt);
void          getutmpx(const struct utmp *pUt, struct utmpx *pUtx);

#endif
