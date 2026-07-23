/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room glibc-shaped <locale.h> for libcgj (GreenJade freestanding libc).
 * Not GNU glibc source; dual MIT OR Apache-2.0 only.
 *
 * Scope
 * -----
 * setlocale, localeconv, and newlocale/uselocale/duplocale/freelocale for
 * C-locale objects. locale_t is shared with string *_l and ctype.
 *
 * Design notes
 * ------------
 * Bring-up is C/POSIX locale only; non-C names may still return "C".
 * lconv fields reflect C locale numeric conventions.
 *
 * Non-goals
 * ---------
 * Full ICU/tzdata-backed locale databases.
 */
#pragma once

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define LC_CTYPE    0
#define LC_NUMERIC  1
#define LC_TIME     2
#define LC_COLLATE  3
#define LC_MONETARY 4
#define LC_MESSAGES 5
#define LC_ALL      6

/* newlocale category masks (glibc-shaped) */
#define LC_CTYPE_MASK    (1 << LC_CTYPE)
#define LC_NUMERIC_MASK  (1 << LC_NUMERIC)
#define LC_TIME_MASK     (1 << LC_TIME)
#define LC_COLLATE_MASK  (1 << LC_COLLATE)
#define LC_MONETARY_MASK (1 << LC_MONETARY)
#define LC_MESSAGES_MASK (1 << LC_MESSAGES)
#define LC_ALL_MASK \
    (LC_CTYPE_MASK | LC_NUMERIC_MASK | LC_TIME_MASK | LC_COLLATE_MASK | \
     LC_MONETARY_MASK | LC_MESSAGES_MASK)

struct lconv {
    char *decimal_point;
    char *thousands_sep;
    char *grouping;
    char *int_curr_symbol;
    char *currency_symbol;
    char *mon_decimal_point;
    char *mon_thousands_sep;
    char *mon_grouping;
    char *positive_sign;
    char *negative_sign;
    char  int_frac_digits;
    char  frac_digits;
    char  p_cs_precedes;
    char  p_sep_by_space;
    char  n_cs_precedes;
    char  n_sep_by_space;
    char  p_sign_posn;
    char  n_sign_posn;
    char  int_p_cs_precedes;
    char  int_p_sep_by_space;
    char  int_n_cs_precedes;
    char  int_n_sep_by_space;
    char  int_p_sign_posn;
    char  int_n_sign_posn;
};

/* Opaque per-thread/object locale (C only for bring-up) */
struct __locale_struct;
typedef struct __locale_struct *locale_t;

#define LC_GLOBAL_LOCALE ((locale_t)(uintptr_t)-1)

char *setlocale(int nCategory, const char *szLocale);
struct lconv *localeconv(void);

locale_t newlocale(int nMask, const char *szLocale, locale_t base);
locale_t duplocale(locale_t loc);
void     freelocale(locale_t loc);
locale_t uselocale(locale_t newloc);

#ifdef __cplusplus
}
#endif
