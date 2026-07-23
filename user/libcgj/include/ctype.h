/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped ctype.h (subset). Not GNU glibc.
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int isalnum(int c);
int isalpha(int c);
int isdigit(int c);
int isspace(int c);
int isprint(int c);
int iscntrl(int c);
int islower(int c);
int isupper(int c);
int isxdigit(int c);
int ispunct(int c);
int isgraph(int c);
int isblank(int c);
int isascii(int c);
int toascii(int c);
int tolower(int c);
int toupper(int c);

/* glibc internal ctype table accessors (desktop graph) */
const unsigned short **__ctype_b_loc(void);
const int            **__ctype_tolower_loc(void);
const int            **__ctype_toupper_loc(void);
size_t                 __ctype_get_mb_cur_max(void);

#ifdef __cplusplus
}
#endif
