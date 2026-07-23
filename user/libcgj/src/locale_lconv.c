/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * localeconv — C locale monetary/numeric conventions (bring-up).
 */
#include <locale.h>

static char g_szEmpty[] = "";
static char g_szDot[] = ".";
static char g_szGroup[] = "\3";

static struct lconv g_lconvC = {
    g_szDot,    /* decimal_point */
    g_szEmpty,  /* thousands_sep */
    g_szGroup,  /* grouping */
    g_szEmpty,  /* int_curr_symbol */
    g_szEmpty,  /* currency_symbol */
    g_szDot,    /* mon_decimal_point */
    g_szEmpty,  /* mon_thousands_sep */
    g_szGroup,  /* mon_grouping */
    g_szEmpty,  /* positive_sign */
    g_szEmpty,  /* negative_sign */
    127,        /* int_frac_digits (CHAR_MAX = unspecified) */
    127,        /* frac_digits */
    127,        /* p_cs_precedes */
    127,        /* p_sep_by_space */
    127,        /* n_cs_precedes */
    127,        /* n_sep_by_space */
    127,        /* p_sign_posn */
    127,        /* n_sign_posn */
    127,        /* int_p_cs_precedes */
    127,        /* int_p_sep_by_space */
    127,        /* int_n_cs_precedes */
    127,        /* int_n_sep_by_space */
    127,        /* int_p_sign_posn */
    127         /* int_n_sign_posn */
};

struct lconv *
localeconv(void)
{
    return &g_lconvC;
}
