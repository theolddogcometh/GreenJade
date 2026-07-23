/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Additional wide-string helpers (wcsstr, wcsnlen, casecmp, spn/cspn).
 * Note: wcsncmp/wcsncat live in wchar.c — do not duplicate here.
 *
 * greppable: CGJ_WCHAR_MORE_SOFT_NULL
 * greppable: CGJ_WCHAR_MORE_SOFT_CASECMP
 * greppable: CGJ_WCHAR_MORE_SOFT_SPN
 *
 * Soft deepen: null-safe length/span, casecmp stops on first NUL, empty
 * needle is haystack start (POSIX).
 */
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <wchar.h>
/* strtol family */

size_t
wcsnlen(const wchar_t *sz, size_t cMax)
{
    size_t n = 0;

    /* greppable: CGJ_WCHAR_MORE_SOFT_NULL */
    if (sz == NULL || cMax == 0) {
        return 0;
    }
    while (n < cMax && sz[n] != 0) {
        n++;
    }
    return n;
}

wchar_t *
wcsstr(const wchar_t *szHay, const wchar_t *szNeedle)
{
    size_t nNeedle;
    size_t i;

    if (szHay == NULL || szNeedle == NULL) {
        return NULL;
    }
    nNeedle = wcslen(szNeedle);
    if (nNeedle == 0) {
        return (wchar_t *)(uintptr_t)szHay;
    }
    /* Soft: first-char filter before full ncmp. */
    for (i = 0; szHay[i] != 0; i++) {
        if (szHay[i] != szNeedle[0]) {
            continue;
        }
        if (wcsncmp(szHay + i, szNeedle, nNeedle) == 0) {
            return (wchar_t *)(uintptr_t)(szHay + i);
        }
    }
    return NULL;
}

size_t
wcsspn(const wchar_t *sz, const wchar_t *szAccept)
{
    size_t i;

    /* greppable: CGJ_WCHAR_MORE_SOFT_SPN */
    if (sz == NULL) {
        return 0;
    }
    if (szAccept == NULL || szAccept[0] == 0) {
        return 0;
    }
    for (i = 0; sz[i] != 0; i++) {
        if (wcschr(szAccept, sz[i]) == NULL) {
            break;
        }
    }
    return i;
}

size_t
wcscspn(const wchar_t *sz, const wchar_t *szReject)
{
    size_t i;

    if (sz == NULL) {
        return 0;
    }
    if (szReject == NULL || szReject[0] == 0) {
        return wcslen(sz);
    }
    for (i = 0; sz[i] != 0; i++) {
        if (wcschr(szReject, sz[i]) != NULL) {
            break;
        }
    }
    return i;
}

wchar_t *
wcspbrk(const wchar_t *sz, const wchar_t *szAccept)
{
    size_t i;

    if (sz == NULL || szAccept == NULL || szAccept[0] == 0) {
        return NULL;
    }
    for (i = 0; sz[i] != 0; i++) {
        if (wcschr(szAccept, sz[i]) != NULL) {
            return (wchar_t *)(uintptr_t)(sz + i);
        }
    }
    return NULL;
}

int
wcscasecmp(const wchar_t *a, const wchar_t *b)
{
    size_t i;
    wint_t ca;
    wint_t cb;

    /* greppable: CGJ_WCHAR_MORE_SOFT_CASECMP */
    if (a == NULL || b == NULL) {
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    }
    for (i = 0;; i++) {
        ca = towlower((wint_t)a[i]);
        cb = towlower((wint_t)b[i]);
        if (ca != cb) {
            return (ca < cb) ? -1 : 1;
        }
        if (a[i] == 0) {
            return 0;
        }
    }
}

int
wcsncasecmp(const wchar_t *a, const wchar_t *b, size_t c)
{
    size_t i;
    wint_t ca;
    wint_t cb;

    if (c == 0) {
        return 0;
    }
    if (a == NULL || b == NULL) {
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    }
    for (i = 0; i < c; i++) {
        ca = towlower((wint_t)a[i]);
        cb = towlower((wint_t)b[i]);
        if (ca != cb) {
            return (ca < cb) ? -1 : 1;
        }
        if (a[i] == 0) {
            return 0;
        }
    }
    return 0;
}

/* wcstol/wcstoul/wcstoll/wcstoull/wcstok/wcsftime live in wchar_num.c */
