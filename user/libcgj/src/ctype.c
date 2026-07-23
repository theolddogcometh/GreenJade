/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room ctype (ASCII C locale). Not GNU glibc.
 *
 * greppable: CGJ_CTYPE_SOFT_ASCII
 * greppable: CGJ_CTYPE_SOFT_RANGE
 *
 * Soft deepen: classifiers treat only unsigned-char range 0..255 as
 * classifiable; EOF and out-of-range values are non-members (to* are
 * identity). Matches freestanding C-locale expectations without tables.
 * isascii/toascii live in graph batch surface — not redefined here.
 */
#include <ctype.h>

/* True when nC is a classifiable byte (not EOF / not > UCHAR_MAX soft). */
static int
ctype_soft_in_range(int nC)
{
    /* greppable: CGJ_CTYPE_SOFT_RANGE */
    return nC >= 0 && nC <= 0xff;
}

int
isalnum(int nC)
{
    return isalpha(nC) || isdigit(nC);
}

int
isalpha(int nC)
{
    return islower(nC) || isupper(nC);
}

int
isdigit(int nC)
{
    if (!ctype_soft_in_range(nC)) {
        return 0;
    }
    return nC >= '0' && nC <= '9';
}

int
isspace(int nC)
{
    if (!ctype_soft_in_range(nC)) {
        return 0;
    }
    return nC == ' ' || nC == '\t' || nC == '\n' || nC == '\r' || nC == '\f' ||
           nC == '\v';
}

int
isprint(int nC)
{
    if (!ctype_soft_in_range(nC)) {
        return 0;
    }
    return nC >= 0x20 && nC <= 0x7e;
}

int
iscntrl(int nC)
{
    if (!ctype_soft_in_range(nC)) {
        return 0;
    }
    return (nC >= 0 && nC < 0x20) || nC == 0x7f;
}

int
islower(int nC)
{
    if (!ctype_soft_in_range(nC)) {
        return 0;
    }
    return nC >= 'a' && nC <= 'z';
}

int
isupper(int nC)
{
    if (!ctype_soft_in_range(nC)) {
        return 0;
    }
    return nC >= 'A' && nC <= 'Z';
}

int
isxdigit(int nC)
{
    if (!ctype_soft_in_range(nC)) {
        return 0;
    }
    return isdigit(nC) || (nC >= 'a' && nC <= 'f') || (nC >= 'A' && nC <= 'F');
}

int
ispunct(int nC)
{
    return isprint(nC) && !isalnum(nC) && !isspace(nC);
}

int
isgraph(int nC)
{
    return isprint(nC) && nC != ' ';
}

int
isblank(int nC)
{
    if (!ctype_soft_in_range(nC)) {
        return 0;
    }
    return nC == ' ' || nC == '\t';
}

int
tolower(int nC)
{
    if (isupper(nC)) {
        return nC + ('a' - 'A');
    }
    return nC;
}

int
toupper(int nC)
{
    if (islower(nC)) {
        return nC - ('a' - 'A');
    }
    return nC;
}
