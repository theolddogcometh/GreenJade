/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Wide numeric conversion, wcstok, and wcsftime (over strftime).
 *
 * greppable: CGJ_WCHAR_NUM_SOFT_ERANGE
 * greppable: CGJ_WCHAR_NUM_SOFT_NOCONV
 * greppable: CGJ_WCHAR_NUM_SOFT_ENDPTR
 *
 * Soft deepen: ERANGE on overflow, no-conversion endptr stays at nptr,
 * optional leading '-' on unsigned (wrap), empty digit after base prefix.
 */
#include <errno.h>
#include <limits.h>
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

/*
 * Shared soft parse: *pp advances; returns 0 ok, -1 no conversion, 1 overflow.
 * pStart is nptr for no-conversion endptr restore.
 */
static int
wcstox_soft_u(const wchar_t **pp, const wchar_t *pStart, int nBase,
              unsigned long long *pOut, int *pfNeg)
{
    const wchar_t *p;
    const wchar_t *pDigits;
    unsigned long long v = 0;
    int fNeg = 0;
    int fAny = 0;
    int d;
    unsigned long long uLim;
    unsigned long long uCut;
    int nCutDigit;

    (void)pStart;
    p = *pp;
    skip_wspace(&p);
    if (*p == L'-') {
        fNeg = 1;
        p++;
    } else if (*p == L'+') {
        p++;
    }
    if (nBase == 0) {
        if (*p == L'0' && (p[1] == L'x' || p[1] == L'X') &&
            wdigit_val(p[2], 16) >= 0) {
            nBase = 16;
            p += 2;
        } else if (*p == L'0') {
            nBase = 8;
            /* consume leading 0 as a digit soft */
            fAny = 1;
            p++;
            pDigits = p;
            while ((d = wdigit_val(*p, 8)) >= 0) {
                fAny = 1;
                if (v > (ULLONG_MAX / 8ull) ||
                    (v == (ULLONG_MAX / 8ull) && (unsigned)d > (ULLONG_MAX % 8ull))) {
                    *pfNeg = fNeg;
                    *pOut = ULLONG_MAX;
                    while (wdigit_val(*p, 8) >= 0) {
                        p++;
                    }
                    *pp = p;
                    return 1;
                }
                v = v * 8ull + (unsigned long long)d;
                p++;
            }
            *pfNeg = fNeg;
            *pOut = v;
            *pp = p;
            return fAny ? 0 : -1;
        } else {
            nBase = 10;
        }
    } else if (nBase == 16 && *p == L'0' && (p[1] == L'x' || p[1] == L'X') &&
               wdigit_val(p[2], 16) >= 0) {
        p += 2;
    }
    if (nBase < 2 || nBase > 36) {
        return -2; /* EINVAL */
    }
    pDigits = p;
    uLim = ULLONG_MAX;
    uCut = uLim / (unsigned long long)nBase;
    nCutDigit = (int)(uLim % (unsigned long long)nBase);
    while ((d = wdigit_val(*p, nBase)) >= 0) {
        fAny = 1;
        if (v > uCut || (v == uCut && d > nCutDigit)) {
            /* greppable: CGJ_WCHAR_NUM_SOFT_ERANGE */
            while (wdigit_val(*p, nBase) >= 0) {
                p++;
            }
            *pfNeg = fNeg;
            *pOut = ULLONG_MAX;
            *pp = p;
            return 1;
        }
        v = v * (unsigned long long)nBase + (unsigned long long)d;
        p++;
    }
    (void)pDigits;
    *pfNeg = fNeg;
    *pOut = v;
    *pp = p;
    return fAny ? 0 : -1;
}

long
wcstol(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    const wchar_t *p;
    unsigned long long u;
    int fNeg = 0;
    int nSt;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0;
    }
    p = sz;
    nSt = wcstox_soft_u(&p, sz, nBase, &u, &fNeg);
    if (nSt == -2) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            /* greppable: CGJ_WCHAR_NUM_SOFT_ENDPTR */
            *ppEnd = (wchar_t *)(uintptr_t)sz;
        }
        return 0;
    }
    if (nSt < 0) {
        /* greppable: CGJ_WCHAR_NUM_SOFT_NOCONV */
        if (ppEnd != NULL) {
            *ppEnd = (wchar_t *)(uintptr_t)sz;
        }
        return 0;
    }
    if (ppEnd != NULL) {
        *ppEnd = (wchar_t *)(uintptr_t)p;
    }
    if (nSt > 0) {
        errno = ERANGE;
        return fNeg ? LONG_MIN : LONG_MAX;
    }
    if (fNeg) {
        if (u > (unsigned long long)LONG_MAX + 1ull) {
            errno = ERANGE;
            return LONG_MIN;
        }
        if (u == (unsigned long long)LONG_MAX + 1ull) {
            return LONG_MIN;
        }
        return -(long)u;
    }
    if (u > (unsigned long long)LONG_MAX) {
        errno = ERANGE;
        return LONG_MAX;
    }
    return (long)u;
}

unsigned long
wcstoul(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    const wchar_t *p;
    unsigned long long u;
    int fNeg = 0;
    int nSt;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0;
    }
    p = sz;
    nSt = wcstox_soft_u(&p, sz, nBase, &u, &fNeg);
    if (nSt == -2) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            *ppEnd = (wchar_t *)(uintptr_t)sz;
        }
        return 0;
    }
    if (nSt < 0) {
        if (ppEnd != NULL) {
            *ppEnd = (wchar_t *)(uintptr_t)sz;
        }
        return 0;
    }
    if (ppEnd != NULL) {
        *ppEnd = (wchar_t *)(uintptr_t)p;
    }
    if (nSt > 0 || u > (unsigned long long)ULONG_MAX) {
        errno = ERANGE;
        return ULONG_MAX;
    }
    if (fNeg) {
        /* POSIX: convert then negate in unsigned domain. */
        return (unsigned long)(0ul - (unsigned long)u);
    }
    return (unsigned long)u;
}

long long
wcstoll(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    const wchar_t *p;
    unsigned long long u;
    int fNeg = 0;
    int nSt;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0;
    }
    p = sz;
    nSt = wcstox_soft_u(&p, sz, nBase, &u, &fNeg);
    if (nSt == -2) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            *ppEnd = (wchar_t *)(uintptr_t)sz;
        }
        return 0;
    }
    if (nSt < 0) {
        if (ppEnd != NULL) {
            *ppEnd = (wchar_t *)(uintptr_t)sz;
        }
        return 0;
    }
    if (ppEnd != NULL) {
        *ppEnd = (wchar_t *)(uintptr_t)p;
    }
    if (nSt > 0) {
        errno = ERANGE;
        return fNeg ? LLONG_MIN : LLONG_MAX;
    }
    if (fNeg) {
        if (u > (unsigned long long)LLONG_MAX + 1ull) {
            errno = ERANGE;
            return LLONG_MIN;
        }
        if (u == (unsigned long long)LLONG_MAX + 1ull) {
            return LLONG_MIN;
        }
        return -(long long)u;
    }
    if (u > (unsigned long long)LLONG_MAX) {
        errno = ERANGE;
        return LLONG_MAX;
    }
    return (long long)u;
}

unsigned long long
wcstoull(const wchar_t *sz, wchar_t **ppEnd, int nBase)
{
    const wchar_t *p;
    unsigned long long u;
    int fNeg = 0;
    int nSt;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0;
    }
    p = sz;
    nSt = wcstox_soft_u(&p, sz, nBase, &u, &fNeg);
    if (nSt == -2) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            *ppEnd = (wchar_t *)(uintptr_t)sz;
        }
        return 0;
    }
    if (nSt < 0) {
        if (ppEnd != NULL) {
            *ppEnd = (wchar_t *)(uintptr_t)sz;
        }
        return 0;
    }
    if (ppEnd != NULL) {
        *ppEnd = (wchar_t *)(uintptr_t)p;
    }
    if (nSt > 0) {
        errno = ERANGE;
        return ULLONG_MAX;
    }
    if (fNeg) {
        return 0ull - u;
    }
    return u;
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
