/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph nodes: inttypes, qsort_r, _Exit, group_member, getwd,
 * a64l/l64a. Integer-only (no SSE doubles).
 */
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void
_Exit(int nCode)
{
    _exit(nCode);
}

intmax_t
imaxabs(intmax_t n)
{
    return (n < 0) ? -n : n;
}

imaxdiv_t
imaxdiv(intmax_t nNumer, intmax_t nDenom)
{
    imaxdiv_t r;

    r.quot = nNumer / nDenom;
    r.rem = nNumer % nDenom;
    return r;
}

intmax_t
strtoimax(const char *sz, char **ppEnd, int nBase)
{
    return (intmax_t)strtoll(sz, ppEnd, nBase);
}

uintmax_t
strtoumax(const char *sz, char **ppEnd, int nBase)
{
    return (uintmax_t)strtoull(sz, ppEnd, nBase);
}

int
group_member(gid_t gid)
{
    gid_t aGids[64];
    int n;
    int i;

    if (gid == getegid() || gid == getgid()) {
        return 1;
    }
    n = getgroups((int)(sizeof(aGids) / sizeof(aGids[0])), aGids);
    if (n < 0) {
        return 0;
    }
    for (i = 0; i < n; i++) {
        if (aGids[i] == gid) {
            return 1;
        }
    }
    return 0;
}

char *
getwd(char *szBuf)
{
    if (szBuf == NULL) {
        errno = EINVAL;
        return NULL;
    }
    /* Historical PATH_MAX bring-up buffer size. */
    return getcwd(szBuf, 4096u);
}

/* GNU qsort_r: comparator gets caller cookie as third arg. */
void
qsort_r(void *pBase, size_t n, size_t cb,
        int (*pfnCmp)(const void *, const void *, void *), void *pArg)
{
    size_t i, j;
    unsigned char *p = (unsigned char *)pBase;

    if (pBase == NULL || pfnCmp == NULL || cb == 0 || n < 2) {
        return;
    }
    for (i = 1; i < n; i++) {
        j = i;
        while (j > 0 &&
               pfnCmp(p + (j - 1) * cb, p + j * cb, pArg) > 0) {
            size_t k;
            for (k = 0; k < cb; k++) {
                unsigned char t = p[(j - 1) * cb + k];
                p[(j - 1) * cb + k] = p[j * cb + k];
                p[j * cb + k] = t;
            }
            j--;
        }
    }
}

/* a64l / l64a — radix-64 encode/decode of long (glibc shape). */
static int
a64_digit(int c)
{
    if (c == '.') {
        return 0;
    }
    if (c == '/') {
        return 1;
    }
    if (c >= '0' && c <= '9') {
        return c - '0' + 2;
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 12;
    }
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 38;
    }
    return -1;
}

static char
a64_char(int n)
{
    static const char aTab[] =
        "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

    if (n < 0 || n > 63) {
        return '.';
    }
    return aTab[n];
}

long
a64l(const char *sz)
{
    long v = 0;
    int i;

    if (sz == NULL) {
        return 0;
    }
    for (i = 0; i < 6 && sz[i] != '\0'; i++) {
        int d = a64_digit((unsigned char)sz[i]);

        if (d < 0) {
            break;
        }
        v |= ((long)d) << (6 * i);
    }
    return v;
}

char *
l64a(long n)
{
    static char aBuf[7];
    unsigned long u;
    int i;

    u = (unsigned long)n;
    for (i = 0; i < 6; i++) {
        aBuf[i] = a64_char((int)(u & 63u));
        u >>= 6;
        if (u == 0) {
            aBuf[i + 1] = '\0';
            return aBuf;
        }
    }
    aBuf[6] = '\0';
    return aBuf;
}
