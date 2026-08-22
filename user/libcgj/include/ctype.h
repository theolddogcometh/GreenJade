/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <ctype.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * ASCII C-locale character class and case mapping (is/to) plus glibc
 * internal table accessors used by some desktop/dynlink graphs.
 *
 * Design notes
 * ------------
 * Arguments are int so EOF (-1) is representable; only unsigned char range
 * 0..255 plus EOF is defined — other values are undefined (same as ISO C).
 * Bring-up is C locale only; wide-character classes live in <wchar.h> / wctype.
 *
 * Non-goals
 * ---------
 * Full locale-dependent ctype tables beyond C; isw* family is separate.
 *
 * Soft residual (C2 libcgj ctype.h; Soft!=product; G-AC-1; Dual DoD A/B OPEN):
 *   soft     = ASCII C-locale is* to* + __ctype_*_loc table accessors;
 *              bring-up may share process tables (not full TLS locale tables)
 *   product  = UDX/DDI + hot/cold ABI hosts; not freestanding class; not .ko AC
 *   honesty  = C-locale tables != full glibc locale product; agent PASS != Dual
 *              DoD close; stamp-free bar honesty (no version stamp)
 *   law      = dual MIT OR Apache-2.0; no GPL; H2 no stamp storms
 * greppable: libcgj: ctype soft residual Soft!=product
 */
#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* ---- Character classes / case (C locale, ASCII) ------------------------- */

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

/*
 * glibc internal ctype table accessors (desktop graph).
 * Return pointers to thread-oriented tables; bring-up may share process tables.
 */
const unsigned short **__ctype_b_loc(void);
const int            **__ctype_tolower_loc(void);
const int            **__ctype_toupper_loc(void);
size_t                 __ctype_get_mb_cur_max(void); /* MB_CUR_MAX for C locale */

#ifdef __cplusplus
}
#endif
