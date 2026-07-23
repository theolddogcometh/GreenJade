/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Wide numeric conversion, wcstok, and wcsftime (over strftime).
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <time.h>
#include <wchar.h>

static int
wdigit_val(wchar_t wc, int nBase)
{
    int n;

    if (wc >= L'0' && wc <= L'9') {
        n = (int)(wc - L'0');
    } else if (wc >= L'a' && wc <= L'z') {
        n = (int)(wc - L'a') + 10;
    } else if (wc >= L'A' && wc <= L'Z') {
        n = (int)(wc - L'A') + 10;
    } else {
        return -1;
    }
    if (n >= nBase) {
        return -1;
    }
    return n;
}

static void
skip_wspace(const wchar_t **pp)
{
    const wchar_t *p = *pp;

    while (*p != 0 && iswspace((wint_t)*p)) {
        p++;
    }
    *pp = p;
}

long
wcstol(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    long v = 0;
    int fNeg = 0;
    int d;
    const wchar_t *p;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0;
    }
    p = sz;
    skip_wspace(&p);
    if (*p == L'-') {
        fNeg = 1;
        p++;
    } else if (*p == L'+') {
        p++;
    }
    if (nBase == 0) {
        if (*p == L'0' && (p[1] == L'x' || p[1] == L'X')) {
            nBase = 16;
            p += 2;
        } else if (*p == L'0') {
            nBase = 8;
            p++;
        } else {
            nBase = 10;
        }
    } else if (nBase == 16 && *p == L'0' && (p[1] == L'x' || p[1] == L'X')) {
        p += 2;
    }
    if (nBase < 2 || nBase > 36) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            *ppEnd = (wchar_t *)(uintptr_t)sz;
        }
        return 0;
    }
    while ((d = wdigit_val(*p, nBase)) >= 0) {
        v = v * (long)nBase + d;
        p++;
    }
    if (ppEnd != NULL) {
        *ppEnd = (wchar_t *)(uintptr_t)p;
    }
    return fNeg ? -v : v;
}

unsigned long
wcstoul(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    unsigned long v = 0;
    int d;
    const wchar_t *p;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0;
    }
    p = sz;
    skip_wspace(&p);
    if (*p == L'+') {
        p++;
    }
    if (nBase == 0) {
        if (*p == L'0' && (p[1] == L'x' || p[1] == L'X')) {
            nBase = 16;
            p += 2;
        } else if (*p == L'0') {
            nBase = 8;
            p++;
        } else {
            nBase = 10;
        }
    } else if (nBase == 16 && *p == L'0' && (p[1] == L'x' || p[1] == L'X')) {
        p += 2;
    }
    if (nBase < 2 || nBase > 36) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            *ppEnd = (wchar_t *)(uintptr_t)sz;
        }
        return 0;
    }
    while ((d = wdigit_val(*p, nBase)) >= 0) {
        v = v * (unsigned long)nBase + (unsigned long)d;
        p++;
    }
    if (ppEnd != NULL) {
        *ppEnd = (wchar_t *)(uintptr_t)p;
    }
    return v;
}

long long
wcstoll(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    long long v = 0;
    int fNeg = 0;
    int d;
    const wchar_t *p;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0;
    }
    p = sz;
    skip_wspace(&p);
    if (*p == L'-') {
        fNeg = 1;
        p++;
    } else if (*p == L'+') {
        p++;
    }
    if (nBase == 0) {
        if (*p == L'0' && (p[1] == L'x' || p[1] == L'X')) {
            nBase = 16;
            p += 2;
        } else if (*p == L'0') {
            nBase = 8;
            p++;
        } else {
            nBase = 10;
        }
    } else if (nBase == 16 && *p == L'0' && (p[1] == L'x' || p[1] == L'X')) {
        p += 2;
    }
    if (nBase < 2 || nBase > 36) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            *ppEnd = (wchar_t *)(uintptr_t)sz;
        }
        return 0;
    }
    while ((d = wdigit_val(*p, nBase)) >= 0) {
        v = v * (long long)nBase + d;
        p++;
    }
    if (ppEnd != NULL) {
        *ppEnd = (wchar_t *)(uintptr_t)p;
    }
    return fNeg ? -v : v;
}

unsigned long long
wcstoull(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    unsigned long long v = 0;
    int d;
    const wchar_t *p;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0;
    }
    p = sz;
    skip_wspace(&p);
    if (*p == L'+') {
        p++;
    }
    if (nBase == 0) {
        if (*p == L'0' && (p[1] == L'x' || p[1] == L'X')) {
            nBase = 16;
            p += 2;
        } else if (*p == L'0') {
            nBase = 8;
            p++;
        } else {
            nBase = 10;
        }
    } else if (nBase == 16 && *p == L'0' && (p[1] == L'x' || p[1] == L'X')) {
        p += 2;
    }
    if (nBase < 2 || nBase > 36) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            *ppEnd = (wchar_t *)(uintptr_t)sz;
        }
        return 0;
    }
    while ((d = wdigit_val(*p, nBase)) >= 0) {
        v = v * (unsigned long long)nBase + (unsigned long long)d;
        p++;
    }
    if (ppEnd != NULL) {
        *ppEnd = (wchar_t *)(uintptr_t)p;
    }
    return v;
}

wchar_t *
wcstok(wchar_t *sz, const wchar_t *szDelim, wchar_t **ppSave)
{
    wchar_t *pStart;
    wchar_t *p;

    if (ppSave == NULL || szDelim == NULL) {
        return NULL;
    }
    if (sz != NULL) {
        *ppSave = sz;
    }
    if (*ppSave == NULL) {
        return NULL;
    }
    p = *ppSave;
    while (*p != 0 && wcschr(szDelim, *p) != NULL) {
        p++;
    }
    if (*p == 0) {
        *ppSave = p;
        return NULL;
    }
    pStart = p;
    while (*p != 0 && wcschr(szDelim, *p) == NULL) {
        p++;
    }
    if (*p != 0) {
        *p = 0;
        p++;
    }
    *ppSave = p;
    return pStart;
}

size_t
wcsftime(wchar_t *pBuf, size_t cMax, const wchar_t *szFmt,
         const struct tm *pTm)
{
    char aFmt[256];
    char aOut[512];
    size_t i;
    size_t n;
    size_t j;

    if (pBuf == NULL || cMax == 0 || szFmt == NULL || pTm == NULL) {
        return 0;
    }
    /* Bring-up: format string is ASCII-compatible wide chars only. */
    for (i = 0; i + 1 < sizeof(aFmt) && szFmt[i] != 0; i++) {
        if (szFmt[i] < 0 || szFmt[i] > 127) {
            aFmt[i] = '?';
        } else {
            aFmt[i] = (char)szFmt[i];
        }
    }
    aFmt[i] = '\0';
    n = strftime(aOut, sizeof(aOut), aFmt, pTm);
    if (n == 0) {
        pBuf[0] = 0;
        return 0;
    }
    if (n + 1 > cMax) {
        pBuf[0] = 0;
        return 0;
    }
    for (j = 0; j < n; j++) {
        pBuf[j] = (wchar_t)(unsigned char)aOut[j];
    }
    pBuf[n] = 0;
    return n;
}
