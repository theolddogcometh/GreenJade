/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Overlay <utmp.h> for OpenSSH-portable (HAVE_UTMP_H).
 * Host POSIX: glibc. DUT: do not include_next libcgj utmp.h (its file
 * comment contains a star-slash sequence that ends the comment).
 */
#pragma once
#if __STDC_HOSTED__
# include_next <utmp.h>
#else

#include <sys/types.h>
#include <time.h>

#ifndef UT_LINESIZE
# define UT_LINESIZE 32
#endif
#ifndef UT_NAMESIZE
# define UT_NAMESIZE 32
#endif
#ifndef UT_HOSTSIZE
# define UT_HOSTSIZE 256
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

struct exit_status {
    short e_termination;
    short e_exit;
};

struct utmp {
    short   ut_type;
    pid_t   ut_pid;
    char    ut_line[UT_LINESIZE];
    char    ut_id[4];
    char    ut_user[UT_NAMESIZE];
    char    ut_host[UT_HOSTSIZE];
    struct exit_status ut_exit;
    long    ut_session;
    struct {
        int tv_sec;
        int tv_usec;
    } ut_tv;
    int     ut_addr_v6[4];
    char    ut_unused[20];
};

#ifndef ut_name
# define ut_name ut_user
#endif
#ifndef ut_time
# define ut_time ut_tv.tv_sec
#endif
#ifndef ut_addr
# define ut_addr ut_addr_v6[0]
#endif

struct lastlog {
    int  ll_time;
    char ll_line[UT_LINESIZE];
    char ll_host[UT_HOSTSIZE];
};

struct utmp *getutent(void);
void         setutent(void);
void         endutent(void);
struct utmp *getutid(const struct utmp *pId);
struct utmp *getutline(const struct utmp *pLine);
struct utmp *pututline(const struct utmp *pUt);
int          utmpname(const char *szFile);
void         updwtmp(const char *szFile, const struct utmp *pUt);
void         login(const struct utmp *pUt);
int          logout(const char *szLine);
void         logwtmp(const char *szLine, const char *szName, const char *szHost);

#endif
