/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room POSIX regex.h — freestanding ERE subset engine. Not GNU glibc.
 */
#pragma once

#include <stddef.h>
#include <sys/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifndef _REGOFF_T_DEFINED
#define _REGOFF_T_DEFINED
typedef ssize_t regoff_t;
#endif

typedef struct {
	size_t re_nsub;
	int    re_cflags;
	char  *re_pat; /* owned NUL-terminated pattern copy */
	int    re_pad;
} regex_t;

typedef struct {
	regoff_t rm_so;
	regoff_t rm_eo;
} regmatch_t;

/* cflags */
#define REG_EXTENDED 1
#define REG_ICASE    2
#define REG_NEWLINE  4
#define REG_NOSUB    8

/* eflags */
#define REG_NOTBOL 1
#define REG_NOTEOL 2

/* error codes (glibc-shaped positive values) */
#define REG_NOMATCH  1
#define REG_BADPAT   2
#define REG_ECOLLATE 3
#define REG_ECTYPE   4
#define REG_EESCAPE  5
#define REG_ESUBREG  6
#define REG_EBRACK   7
#define REG_EPAREN   8
#define REG_EBRACE   9
#define REG_BADBR    10
#define REG_ERANGE   11
#define REG_ESPACE   12
#define REG_BADRPT   13

int    regcomp(regex_t *pPreg, const char *szPattern, int nCflags);
int    regexec(const regex_t *pPreg, const char *szString, size_t nMatch,
               regmatch_t pMatch[], int nEflags);
size_t regerror(int nErrcode, const regex_t *pPreg, char *szErrbuf, size_t cb);
void   regfree(regex_t *pPreg);

#ifdef __cplusplus
}
#endif
