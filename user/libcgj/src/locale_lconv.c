/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * localeconv — C locale monetary/numeric conventions (bring-up).
 *
 * greppable: CGJ_LCONV_SOFT_C
 * greppable: CGJ_LCONV_SOFT_CHAR_MAX
 *
 * Soft deepen: CHAR_MAX-unspecified monetary fields, stable C grouping
 * "\3" (groups of 3, no further groups). Pure C freestanding.
 */
#include <limits.h>
#include <locale.h>

static char g_szEmpty[] = "";
static char g_szDot[] = ".";
/* greppable: CGJ_LCONV_SOFT_C — groups of 3; CHAR_MAX terminator implied by "" */
static char g_szGroup[] = "\3";

/* greppable: CGJ_LCONV_SOFT_CHAR_MAX */
#ifndef CHAR_MAX
#define CHAR_MAX 127
#endif

static struct lconv g_lconvC = {
    g_szDot,     /* decimal_point */
    g_szEmpty,   /* thousands_sep */
    g_szGroup,   /* grouping */
    g_szEmpty,   /* int_curr_symbol */
    g_szEmpty,   /* currency_symbol */
    g_szDot,     /* mon_decimal_point */
    g_szEmpty,   /* mon_thousands_sep */
    g_szGroup,   /* mon_grouping */
    g_szEmpty,   /* positive_sign */
    g_szEmpty,   /* negative_sign */
    CHAR_MAX,    /* int_frac_digits (unspecified) */
    CHAR_MAX,    /* frac_digits */
    CHAR_MAX,    /* p_cs_precedes */
    CHAR_MAX,    /* p_sep_by_space */
    CHAR_MAX,    /* n_cs_precedes */
    CHAR_MAX,    /* n_sep_by_space */
    CHAR_MAX,    /* p_sign_posn */
    CHAR_MAX,    /* n_sign_posn */
    CHAR_MAX,    /* int_p_cs_precedes */
    CHAR_MAX,    /* int_p_sep_by_space */
    CHAR_MAX,    /* int_n_cs_precedes */
    CHAR_MAX,    /* int_n_sep_by_space */
    CHAR_MAX,    /* int_p_sign_posn */
    CHAR_MAX     /* int_n_sign_posn */
};

struct lconv *
localeconv(void)
{
    /* C locale only; xlocale soft path does not override lconv yet. */
    return &g_lconvC;
}
