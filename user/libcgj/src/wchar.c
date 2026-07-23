/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Wide-char helpers, UTF-8 conversion, fwprintf subset (glibc-shaped).
 */
#include <errno.h>
#include <stdarg.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

/* Must match stdio.c flag bits */
#define F_WMEMSTREAM 16
#define F_MEMSTREAM  8

size_t
wcslen(const wchar_t *sz)
{
    size_t n = 0;

    if (sz == NULL) {
        return 0;
    }
    while (sz[n] != 0) {
        n++;
    }
    return n;
}

int
wcscmp(const wchar_t *a, const wchar_t *b)
{
    size_t i = 0;

    if (a == NULL || b == NULL) {
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    }
    while (a[i] != 0 && a[i] == b[i]) {
        i++;
    }
    return (int)a[i] - (int)b[i];
}

wchar_t *
wcscpy(wchar_t *pDst, const wchar_t *szSrc)
{
    size_t i = 0;

    if (pDst == NULL || szSrc == NULL) {
        return pDst;
    }
    while (szSrc[i] != 0) {
        pDst[i] = szSrc[i];
        i++;
    }
    pDst[i] = 0;
    return pDst;
}

wchar_t *
wcsncpy(wchar_t *pDst, const wchar_t *szSrc, size_t c)
{
    size_t i = 0;

    if (pDst == NULL || szSrc == NULL) {
        return pDst;
    }
    while (i < c && szSrc[i] != 0) {
        pDst[i] = szSrc[i];
        i++;
    }
    while (i < c) {
        pDst[i++] = 0;
    }
    return pDst;
}

wchar_t *
wcscat(wchar_t *pDst, const wchar_t *szSrc)
{
    size_t n;

    if (pDst == NULL || szSrc == NULL) {
        return pDst;
    }
    n = wcslen(pDst);
    (void)wcscpy(pDst + n, szSrc);
    return pDst;
}

int
fwide(FILE *pF, int nMode)
{
    if (pF == NULL) {
        return 0;
    }
    if (nMode > 0) {
        if (pF->nWide < 0) {
            return pF->nWide;
        }
        pF->nWide = 1;
    } else if (nMode < 0) {
        if (pF->nWide > 0) {
            return pF->nWide;
        }
        pF->nWide = -1;
    }
    return pF->nWide;
}

wint_t
fputwc(wchar_t wc, FILE *pF)
{
    unsigned char a[sizeof(int)];

    if (pF == NULL) {
        errno = EINVAL;
        return WEOF;
    }
    if ((pF->nFlags & F_WMEMSTREAM) != 0) {
        pF->nWide = 1;
        memcpy(a, &wc, sizeof(int));
        if (fwrite(a, 1, sizeof(int), pF) != sizeof(int)) {
            return WEOF;
        }
        return (wint_t)wc;
    }
    /* Narrow stream: emit UTF-8 for BMP-ish codepoints (bring-up ASCII) */
    pF->nWide = -1;
    if (wc >= 0 && wc < 0x80) {
        if (fputc((int)wc, pF) == EOF) {
            return WEOF;
        }
        return (wint_t)wc;
    }
    if (wc < 0x800) {
        if (fputc(0xC0 | ((wc >> 6) & 0x1F), pF) == EOF) {
            return WEOF;
        }
        if (fputc(0x80 | (wc & 0x3F), pF) == EOF) {
            return WEOF;
        }
        return (wint_t)wc;
    }
    if (fputc(0xE0 | ((wc >> 12) & 0x0F), pF) == EOF) {
        return WEOF;
    }
    if (fputc(0x80 | ((wc >> 6) & 0x3F), pF) == EOF) {
        return WEOF;
    }
    if (fputc(0x80 | (wc & 0x3F), pF) == EOF) {
        return WEOF;
    }
    return (wint_t)wc;
}

wint_t
putwc(wchar_t wc, FILE *pF)
{
    return fputwc(wc, pF);
}

int
fputws(const wchar_t *sz, FILE *pF)
{
    size_t i;

    if (sz == NULL || pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; sz[i] != 0; i++) {
        if (fputwc(sz[i], pF) == WEOF) {
            return -1;
        }
    }
    return 0;
}

wint_t
fgetwc(FILE *pF)
{
    unsigned char a[4];
    int c;
    mbstate_t st;
    wchar_t wc = 0;
    size_t n;
    size_t i;

    if (pF == NULL) {
        return WEOF;
    }
    c = fgetc(pF);
    if (c == EOF) {
        return WEOF;
    }
    a[0] = (unsigned char)c;
    if (a[0] < 0x80) {
        return (wint_t)a[0];
    }
    memset(&st, 0, sizeof(st));
    /* Read up to 3 more bytes for UTF-8 */
    for (i = 1; i < 4; i++) {
        n = mbrtowc(&wc, (const char *)a, i, &st);
        if (n == (size_t)-1) {
            return WEOF;
        }
        if (n != (size_t)-2) {
            return (wint_t)wc;
        }
        c = fgetc(pF);
        if (c == EOF) {
            return WEOF;
        }
        a[i] = (unsigned char)c;
        memset(&st, 0, sizeof(st));
    }
    n = mbrtowc(&wc, (const char *)a, 4, &st);
    if (n == (size_t)-1 || n == (size_t)-2) {
        return WEOF;
    }
    return (wint_t)wc;
}

wint_t
getwc(FILE *pF)
{
    return fgetwc(pF);
}

wchar_t *
fgetws(wchar_t *sz, int n, FILE *pF)
{
    int i;

    if (sz == NULL || n <= 0 || pF == NULL) {
        return NULL;
    }
    for (i = 0; i < n - 1; i++) {
        wint_t wc = fgetwc(pF);

        if (wc == WEOF) {
            if (i == 0) {
                return NULL;
            }
            break;
        }
        sz[i] = (wchar_t)wc;
        if (wc == L'\n') {
            i++;
            break;
        }
    }
    sz[i] = 0;
    return sz;
}

wint_t
ungetwc(wint_t wc, FILE *pF)
{
    if (wc == WEOF || pF == NULL) {
        return WEOF;
    }
    if (wc >= 0 && wc < 0x80) {
        if (ungetc((int)wc, pF) == EOF) {
            return WEOF;
        }
        return wc;
    }
    errno = ENOSYS;
    return WEOF;
}

wchar_t *
wcschr(const wchar_t *sz, wchar_t wc)
{
    size_t i;

    if (sz == NULL) {
        return NULL;
    }
    for (i = 0;; i++) {
        if (sz[i] == wc) {
            return (wchar_t *)(sz + i);
        }
        if (sz[i] == 0) {
            return NULL;
        }
    }
}

wchar_t *
wcsrchr(const wchar_t *sz, wchar_t wc)
{
    const wchar_t *pLast = NULL;
    size_t i;

    if (sz == NULL) {
        return NULL;
    }
    for (i = 0;; i++) {
        if (sz[i] == wc) {
            pLast = sz + i;
        }
        if (sz[i] == 0) {
            return (wchar_t *)pLast;
        }
    }
}

wchar_t *
wcsdup(const wchar_t *sz)
{
    size_t n;
    wchar_t *p;

    if (sz == NULL) {
        return NULL;
    }
    n = wcslen(sz);
    p = (wchar_t *)malloc((n + 1) * sizeof(wchar_t));
    if (p == NULL) {
        return NULL;
    }
    return wcscpy(p, sz);
}

/* ---- UTF-8 conversion --------------------------------------------------- */

int
mbsinit(const mbstate_t *pSt)
{
    return (pSt == NULL || pSt->__uCount == 0) ? 1 : 0;
}

size_t
mbrtowc(wchar_t *pWc, const char *pS, size_t n, mbstate_t *pSt)
{
    static mbstate_t s_st;
    mbstate_t *p = pSt != NULL ? pSt : &s_st;
    unsigned char c;
    unsigned int u;
    unsigned int need;

    if (pS == NULL) {
        p->__uCount = 0;
        p->__uValue = 0;
        return 0;
    }
    if (n == 0) {
        return (size_t)-2; /* incomplete */
    }
    c = (unsigned char)pS[0];
    if (p->__uCount == 0) {
        if (c < 0x80) {
            if (pWc != NULL) {
                *pWc = (wchar_t)c;
            }
            return (c == 0) ? 0 : 1;
        }
        if ((c & 0xE0) == 0xC0) {
            need = 1;
            u = c & 0x1F;
        } else if ((c & 0xF0) == 0xE0) {
            need = 2;
            u = c & 0x0F;
        } else if ((c & 0xF8) == 0xF0) {
            need = 3;
            u = c & 0x07;
        } else {
            errno = EILSEQ;
            return (size_t)-1;
        }
        p->__uCount = need;
        p->__uValue = u;
        if (n == 1) {
            return (size_t)-2;
        }
        {
            size_t i;
            size_t used = 1;

            for (i = 1; i <= need && i < n; i++) {
                unsigned char cont = (unsigned char)pS[i];

                if ((cont & 0xC0) != 0x80) {
                    p->__uCount = 0;
                    errno = EILSEQ;
                    return (size_t)-1;
                }
                p->__uValue = (p->__uValue << 6) | (cont & 0x3F);
                used++;
                p->__uCount--;
            }
            if (p->__uCount != 0) {
                return (size_t)-2;
            }
            if (pWc != NULL) {
                *pWc = (wchar_t)p->__uValue;
            }
            p->__uValue = 0;
            return used;
        }
    }
    /* Resume incomplete sequence — limited bring-up: treat as new */
    errno = EILSEQ;
    p->__uCount = 0;
    return (size_t)-1;
}

size_t
wcrtomb(char *pS, wchar_t wc, mbstate_t *pSt)
{
    unsigned int u = (unsigned int)wc;

    if (pSt != NULL) {
        pSt->__uCount = 0;
        pSt->__uValue = 0;
    }
    if (pS == NULL) {
        return 1; /* shift state reset length */
    }
    if (u <= 0x7F) {
        pS[0] = (char)u;
        return 1;
    }
    if (u <= 0x7FF) {
        pS[0] = (char)(0xC0 | (u >> 6));
        pS[1] = (char)(0x80 | (u & 0x3F));
        return 2;
    }
    if (u <= 0xFFFF) {
        pS[0] = (char)(0xE0 | (u >> 12));
        pS[1] = (char)(0x80 | ((u >> 6) & 0x3F));
        pS[2] = (char)(0x80 | (u & 0x3F));
        return 3;
    }
    if (u <= 0x10FFFF) {
        pS[0] = (char)(0xF0 | (u >> 18));
        pS[1] = (char)(0x80 | ((u >> 12) & 0x3F));
        pS[2] = (char)(0x80 | ((u >> 6) & 0x3F));
        pS[3] = (char)(0x80 | (u & 0x3F));
        return 4;
    }
    errno = EILSEQ;
    return (size_t)-1;
}

size_t
mbsrtowcs(wchar_t *pDst, const char **ppSrc, size_t cLen, mbstate_t *pSt)
{
    mbstate_t st;
    mbstate_t *p = pSt != NULL ? pSt : &st;
    size_t nOut = 0;
    const char *pS;

    if (ppSrc == NULL || *ppSrc == NULL) {
        return 0;
    }
    if (pSt == NULL) {
        memset(&st, 0, sizeof(st));
    }
    pS = *ppSrc;
    while (pDst == NULL || nOut < cLen) {
        wchar_t wc = 0;
        size_t n = mbrtowc(pDst != NULL ? &wc : NULL, pS, 16, p);

        if (n == (size_t)-1) {
            return (size_t)-1;
        }
        if (n == (size_t)-2) {
            errno = EILSEQ;
            return (size_t)-1;
        }
        if (n == 0) {
            if (pDst != NULL && nOut < cLen) {
                pDst[nOut] = 0;
            }
            *ppSrc = NULL;
            return nOut;
        }
        if (pDst != NULL) {
            pDst[nOut] = wc;
        }
        nOut++;
        pS += n;
    }
    *ppSrc = pS;
    return nOut;
}

size_t
wcsrtombs(char *pDst, const wchar_t **ppSrc, size_t cb, mbstate_t *pSt)
{
    mbstate_t st;
    mbstate_t *p = pSt != NULL ? pSt : &st;
    size_t nOut = 0;
    const wchar_t *pW;

    if (ppSrc == NULL || *ppSrc == NULL) {
        return 0;
    }
    if (pSt == NULL) {
        memset(&st, 0, sizeof(st));
    }
    pW = *ppSrc;
    while (*pW != 0) {
        char aTmp[8];
        size_t n = wcrtomb(aTmp, *pW, p);

        if (n == (size_t)-1) {
            return (size_t)-1;
        }
        if (pDst != NULL) {
            if (nOut + n > cb) {
                *ppSrc = pW;
                return nOut;
            }
            memcpy(pDst + nOut, aTmp, n);
        }
        nOut += n;
        pW++;
    }
    if (pDst != NULL) {
        if (nOut < cb) {
            pDst[nOut] = '\0';
        }
    }
    *ppSrc = NULL;
    return nOut;
}

/* Convert at most cbSrc bytes from *ppSrc into wide chars (n-limited source) */
size_t
mbsnrtowcs(wchar_t *pDst, const char **ppSrc, size_t cbSrc, size_t cLen,
           mbstate_t *pSt)
{
    mbstate_t st;
    mbstate_t *p = pSt != NULL ? pSt : &st;
    size_t nOut = 0;
    const char *pS;
    size_t rem;

    if (ppSrc == NULL || *ppSrc == NULL) {
        return 0;
    }
    if (pSt == NULL) {
        memset(&st, 0, sizeof(st));
    }
    pS = *ppSrc;
    rem = cbSrc;
    while ((pDst == NULL || nOut < cLen) && rem > 0) {
        wchar_t wc = 0;
        size_t n = mbrtowc(pDst != NULL ? &wc : NULL, pS, rem, p);

        if (n == (size_t)-1) {
            return (size_t)-1;
        }
        if (n == (size_t)-2) {
            /* incomplete sequence at end of input */
            *ppSrc = pS;
            return nOut;
        }
        if (n == 0) {
            if (pDst != NULL && nOut < cLen) {
                pDst[nOut] = 0;
            }
            *ppSrc = NULL;
            return nOut;
        }
        if (pDst != NULL) {
            pDst[nOut] = wc;
        }
        nOut++;
        pS += n;
        rem -= n;
    }
    *ppSrc = pS;
    return nOut;
}

/* Convert at most cSrc wide chars into multibyte (n-limited source) */
size_t
wcsnrtombs(char *pDst, const wchar_t **ppSrc, size_t cSrc, size_t cb,
           mbstate_t *pSt)
{
    mbstate_t st;
    mbstate_t *p = pSt != NULL ? pSt : &st;
    size_t nOut = 0;
    const wchar_t *pW;
    size_t left;

    if (ppSrc == NULL || *ppSrc == NULL) {
        return 0;
    }
    if (pSt == NULL) {
        memset(&st, 0, sizeof(st));
    }
    pW = *ppSrc;
    left = cSrc;
    while (left > 0 && *pW != 0) {
        char aTmp[8];
        size_t n = wcrtomb(aTmp, *pW, p);

        if (n == (size_t)-1) {
            return (size_t)-1;
        }
        if (pDst != NULL) {
            if (nOut + n > cb) {
                *ppSrc = pW;
                return nOut;
            }
            memcpy(pDst + nOut, aTmp, n);
        }
        nOut += n;
        pW++;
        left--;
    }
    if (left > 0 && *pW == 0) {
        if (pDst != NULL && nOut < cb) {
            pDst[nOut] = '\0';
        }
        *ppSrc = NULL;
        return nOut;
    }
    *ppSrc = pW;
    return nOut;
}

size_t
mbstowcs(wchar_t *pDst, const char *szSrc, size_t cLen)
{
    const char *p = szSrc;

    return mbsrtowcs(pDst, &p, cLen, NULL);
}

size_t
wcstombs(char *pDst, const wchar_t *szSrc, size_t cb)
{
    const wchar_t *p = szSrc;

    return wcsrtombs(pDst, &p, cb, NULL);
}

int
mbtowc(wchar_t *pWc, const char *pS, size_t n)
{
    size_t r;

    if (pS == NULL) {
        return 0;
    }
    r = mbrtowc(pWc, pS, n, NULL);
    if (r == (size_t)-1 || r == (size_t)-2) {
        return -1;
    }
    return (int)r;
}

int
wctomb(char *pS, wchar_t wc)
{
    size_t r;

    if (pS == NULL) {
        return 0;
    }
    r = wcrtomb(pS, wc, NULL);
    if (r == (size_t)-1) {
        return -1;
    }
    return (int)r;
}

int
mblen(const char *pS, size_t n)
{
    return mbtowc(NULL, pS, n);
}

/* ---- Wide printf subset ------------------------------------------------- */

static void
wout_ch(FILE *pF, wchar_t *pBuf, size_t cCap, size_t *pPos, size_t *pWrote,
        wchar_t wc)
{
    if (pBuf != NULL) {
        if (*pPos + 1 < cCap) {
            pBuf[*pPos] = wc;
        }
        (*pPos)++;
        (*pWrote)++;
        return;
    }
    if (pF != NULL) {
        if (fputwc(wc, pF) != WEOF) {
            (*pWrote)++;
        }
    }
}

static void
wout_str_narrow(FILE *pF, wchar_t *pBuf, size_t cCap, size_t *pPos,
                size_t *pWrote, const char *sz)
{
    mbstate_t st;
    const char *p;

    if (sz == NULL) {
        sz = "(null)";
    }
    memset(&st, 0, sizeof(st));
    p = sz;
    while (*p != '\0') {
        wchar_t wc = 0;
        size_t n = mbrtowc(&wc, p, 8, &st);

        if (n == 0 || n == (size_t)-1 || n == (size_t)-2) {
            break;
        }
        wout_ch(pF, pBuf, cCap, pPos, pWrote, wc);
        p += n;
    }
}

static void
wout_str_wide(FILE *pF, wchar_t *pBuf, size_t cCap, size_t *pPos, size_t *pWrote,
              const wchar_t *sz)
{
    size_t i;

    if (sz == NULL) {
        wout_str_narrow(pF, pBuf, cCap, pPos, pWrote, "(null)");
        return;
    }
    for (i = 0; sz[i] != 0; i++) {
        wout_ch(pF, pBuf, cCap, pPos, pWrote, sz[i]);
    }
}

static void
wout_udec(FILE *pF, wchar_t *pBuf, size_t cCap, size_t *pPos, size_t *pWrote,
          unsigned long long u, int nBase, int fUpper)
{
    wchar_t aTmp[32];
    const char *szDig = fUpper ? "0123456789ABCDEF" : "0123456789abcdef";
    int i = 0;

    if (u == 0) {
        wout_ch(pF, pBuf, cCap, pPos, pWrote, L'0');
        return;
    }
    while (u > 0 && i < (int)(sizeof(aTmp) / sizeof(aTmp[0]))) {
        aTmp[i++] = (wchar_t)szDig[u % (unsigned)nBase];
        u /= (unsigned)nBase;
    }
    while (i > 0) {
        wout_ch(pF, pBuf, cCap, pPos, pWrote, aTmp[--i]);
    }
}

static void
wout_dec(FILE *pF, wchar_t *pBuf, size_t cCap, size_t *pPos, size_t *pWrote,
         long long v)
{
    unsigned long long u;

    if (v < 0) {
        wout_ch(pF, pBuf, cCap, pPos, pWrote, L'-');
        u = (unsigned long long)(-(v + 1)) + 1ull;
    } else {
        u = (unsigned long long)v;
    }
    wout_udec(pF, pBuf, cCap, pPos, pWrote, u, 10, 0);
}

static size_t
vwformat(FILE *pF, wchar_t *pBuf, size_t cCap, const wchar_t *szFmt, va_list ap)
{
    size_t wrote = 0;
    size_t pos = 0;
    const wchar_t *p;

    if (szFmt == NULL) {
        return 0;
    }
    for (p = szFmt; *p != 0; p++) {
        if (*p != L'%') {
            wout_ch(pF, pBuf, cCap, &pos, &wrote, *p);
            continue;
        }
        p++;
        if (*p == 0) {
            break;
        }
        if (*p == L'%') {
            wout_ch(pF, pBuf, cCap, &pos, &wrote, L'%');
            continue;
        }
        if (*p == L'l') {
            p++;
            if (*p == L's') {
                wout_str_wide(pF, pBuf, cCap, &pos, &wrote,
                              va_arg(ap, const wchar_t *));
                continue;
            }
            if (*p == L'c') {
                wout_ch(pF, pBuf, cCap, &pos, &wrote,
                        (wchar_t)va_arg(ap, wint_t));
                continue;
            }
            if (*p == L'd' || *p == L'i') {
                wout_dec(pF, pBuf, cCap, &pos, &wrote, va_arg(ap, long));
                continue;
            }
            if (*p == L'u') {
                wout_udec(pF, pBuf, cCap, &pos, &wrote,
                          (unsigned long long)va_arg(ap, unsigned long), 10, 0);
                continue;
            }
            if (*p == L'x') {
                wout_udec(pF, pBuf, cCap, &pos, &wrote,
                          (unsigned long long)va_arg(ap, unsigned long), 16, 0);
                continue;
            }
            /* fall through for ll */
            if (*p == L'l' && (p[1] == L'd' || p[1] == L'i')) {
                p++;
                wout_dec(pF, pBuf, cCap, &pos, &wrote, va_arg(ap, long long));
                continue;
            }
        }
        if (*p == L's') {
            wout_str_narrow(pF, pBuf, cCap, &pos, &wrote,
                            va_arg(ap, const char *));
            continue;
        }
        if (*p == L'c') {
            wout_ch(pF, pBuf, cCap, &pos, &wrote, (wchar_t)va_arg(ap, int));
            continue;
        }
        if (*p == L'd' || *p == L'i') {
            wout_dec(pF, pBuf, cCap, &pos, &wrote, va_arg(ap, int));
            continue;
        }
        if (*p == L'u') {
            wout_udec(pF, pBuf, cCap, &pos, &wrote,
                      (unsigned long long)va_arg(ap, unsigned), 10, 0);
            continue;
        }
        if (*p == L'x') {
            wout_udec(pF, pBuf, cCap, &pos, &wrote,
                      (unsigned long long)va_arg(ap, unsigned), 16, 0);
            continue;
        }
        if (*p == L'X') {
            wout_udec(pF, pBuf, cCap, &pos, &wrote,
                      (unsigned long long)va_arg(ap, unsigned), 16, 1);
            continue;
        }
        if (*p == L'p') {
            wout_str_narrow(pF, pBuf, cCap, &pos, &wrote, "0x");
            wout_udec(pF, pBuf, cCap, &pos, &wrote,
                      (unsigned long long)(uintptr_t)va_arg(ap, void *), 16, 0);
            continue;
        }
        wout_ch(pF, pBuf, cCap, &pos, &wrote, L'%');
        wout_ch(pF, pBuf, cCap, &pos, &wrote, *p);
    }
    if (pBuf != NULL && cCap > 0) {
        if (pos < cCap) {
            pBuf[pos] = 0;
        } else {
            pBuf[cCap - 1] = 0;
        }
    }
    return wrote;
}

int
vfwprintf(FILE *pF, const wchar_t *szFmt, va_list ap)
{
    if (pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    return (int)vwformat(pF, NULL, 0, szFmt, ap);
}

int
fwprintf(FILE *pF, const wchar_t *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vfwprintf(pF, szFmt, ap);
    va_end(ap);
    return n;
}

int
vwprintf(const wchar_t *szFmt, va_list ap)
{
    return vfwprintf(stdout, szFmt, ap);
}

int
wprintf(const wchar_t *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vwprintf(szFmt, ap);
    va_end(ap);
    return n;
}

int
vswprintf(wchar_t *pBuf, size_t cLen, const wchar_t *szFmt, va_list ap)
{
    if (pBuf == NULL || cLen == 0) {
        errno = EINVAL;
        return -1;
    }
    return (int)vwformat(NULL, pBuf, cLen, szFmt, ap);
}

int
swprintf(wchar_t *pBuf, size_t cLen, const wchar_t *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vswprintf(pBuf, cLen, szFmt, ap);
    va_end(ap);
    return n;
}

wint_t
btowc(int c)
{
    if (c == EOF) {
        return WEOF;
    }
    if (c < 0 || c > 0x7F) {
        return WEOF;
    }
    return (wint_t)c;
}

int
wctob(wint_t wc)
{
    if (wc == WEOF || wc < 0 || wc > 0x7F) {
        return EOF;
    }
    return (int)wc;
}

int
iswdigit(wint_t wc)
{
    return (wc >= L'0' && wc <= L'9') ? 1 : 0;
}

int
iswalpha(wint_t wc)
{
    return ((wc >= L'A' && wc <= L'Z') || (wc >= L'a' && wc <= L'z')) ? 1 : 0;
}

int
iswalnum(wint_t wc)
{
    return (iswalpha(wc) || iswdigit(wc)) ? 1 : 0;
}

int
iswspace(wint_t wc)
{
    return (wc == L' ' || wc == L'\t' || wc == L'\n' || wc == L'\r' ||
            wc == L'\f' || wc == L'\v')
               ? 1
               : 0;
}

int
iswblank(wint_t wc)
{
    return (wc == L' ' || wc == L'\t') ? 1 : 0;
}

int
iswcntrl(wint_t wc)
{
    return ((wc >= 0 && wc < 0x20) || wc == 0x7F) ? 1 : 0;
}

int
iswprint(wint_t wc)
{
    return (wc >= 0x20 && wc != 0x7F) ? 1 : 0;
}

int
iswgraph(wint_t wc)
{
    return (iswprint(wc) && wc != L' ') ? 1 : 0;
}

int
iswlower(wint_t wc)
{
    return (wc >= L'a' && wc <= L'z') ? 1 : 0;
}

int
iswupper(wint_t wc)
{
    return (wc >= L'A' && wc <= L'Z') ? 1 : 0;
}

int
iswpunct(wint_t wc)
{
    return (iswgraph(wc) && !iswalnum(wc)) ? 1 : 0;
}

int
iswxdigit(wint_t wc)
{
    return (iswdigit(wc) || (wc >= L'a' && wc <= L'f') ||
            (wc >= L'A' && wc <= L'F'))
               ? 1
               : 0;
}

wint_t
towlower(wint_t wc)
{
    if (wc >= L'A' && wc <= L'Z') {
        return wc + (L'a' - L'A');
    }
    return wc;
}

wint_t
towupper(wint_t wc)
{
    if (wc >= L'a' && wc <= L'z') {
        return wc - (L'a' - L'A');
    }
    return wc;
}

/* wctype property ids (ASCII-centric bring-up; values stable within libcgj) */
#define WCT_ALNUM  1u
#define WCT_ALPHA  2u
#define WCT_BLANK  3u
#define WCT_CNTRL  4u
#define WCT_DIGIT  5u
#define WCT_GRAPH  6u
#define WCT_LOWER  7u
#define WCT_PRINT  8u
#define WCT_PUNCT  9u
#define WCT_SPACE  10u
#define WCT_UPPER  11u
#define WCT_XDIGIT 12u

wctype_t
wctype(const char *szProp)
{
    if (szProp == NULL) {
        return 0;
    }
    if (strcmp(szProp, "alnum") == 0) {
        return WCT_ALNUM;
    }
    if (strcmp(szProp, "alpha") == 0) {
        return WCT_ALPHA;
    }
    if (strcmp(szProp, "blank") == 0) {
        return WCT_BLANK;
    }
    if (strcmp(szProp, "cntrl") == 0) {
        return WCT_CNTRL;
    }
    if (strcmp(szProp, "digit") == 0) {
        return WCT_DIGIT;
    }
    if (strcmp(szProp, "graph") == 0) {
        return WCT_GRAPH;
    }
    if (strcmp(szProp, "lower") == 0) {
        return WCT_LOWER;
    }
    if (strcmp(szProp, "print") == 0) {
        return WCT_PRINT;
    }
    if (strcmp(szProp, "punct") == 0) {
        return WCT_PUNCT;
    }
    if (strcmp(szProp, "space") == 0) {
        return WCT_SPACE;
    }
    if (strcmp(szProp, "upper") == 0) {
        return WCT_UPPER;
    }
    if (strcmp(szProp, "xdigit") == 0) {
        return WCT_XDIGIT;
    }
    return 0;
}

int
iswctype(wint_t wc, wctype_t desc)
{
    switch (desc) {
    case WCT_ALNUM:
        return iswalnum(wc);
    case WCT_ALPHA:
        return iswalpha(wc);
    case WCT_BLANK:
        return iswblank(wc);
    case WCT_CNTRL:
        return iswcntrl(wc);
    case WCT_DIGIT:
        return iswdigit(wc);
    case WCT_GRAPH:
        return iswgraph(wc);
    case WCT_LOWER:
        return iswlower(wc);
    case WCT_PRINT:
        return iswprint(wc);
    case WCT_PUNCT:
        return iswpunct(wc);
    case WCT_SPACE:
        return iswspace(wc);
    case WCT_UPPER:
        return iswupper(wc);
    case WCT_XDIGIT:
        return iswxdigit(wc);
    default:
        return 0;
    }
}

/* wctrans: 1=tolower, 2=toupper */
wctrans_t
wctrans(const char *szProp)
{
    if (szProp == NULL) {
        return 0;
    }
    if (strcmp(szProp, "tolower") == 0) {
        return 1;
    }
    if (strcmp(szProp, "toupper") == 0) {
        return 2;
    }
    return 0;
}

wint_t
towctrans(wint_t wc, wctrans_t desc)
{
    if (desc == 1) {
        return towlower(wc);
    }
    if (desc == 2) {
        return towupper(wc);
    }
    return wc;
}

wint_t
putwchar(wchar_t wc)
{
    return fputwc(wc, stdout);
}

wint_t
getwchar(void)
{
    return fgetwc(stdin);
}

int
wcsncmp(const wchar_t *a, const wchar_t *b, size_t c)
{
    size_t i = 0;

    if (a == NULL || b == NULL) {
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    }
    while (i < c && a[i] != 0 && a[i] == b[i]) {
        i++;
    }
    if (i == c) {
        return 0;
    }
    return (int)a[i] - (int)b[i];
}

wchar_t *
wcsncat(wchar_t *pDst, const wchar_t *szSrc, size_t c)
{
    size_t n;
    size_t i;

    if (pDst == NULL || szSrc == NULL) {
        return pDst;
    }
    n = wcslen(pDst);
    i = 0;
    while (i < c && szSrc[i] != 0) {
        pDst[n + i] = szSrc[i];
        i++;
    }
    pDst[n + i] = 0;
    return pDst;
}

/* ---- wmem* / coll ------------------------------------------------------- */

wchar_t *
wmemcpy(wchar_t *pDst, const wchar_t *pSrc, size_t c)
{
    size_t i;

    if (pDst == NULL || pSrc == NULL) {
        return pDst;
    }
    for (i = 0; i < c; i++) {
        pDst[i] = pSrc[i];
    }
    return pDst;
}

wchar_t *
wmemmove(wchar_t *pDst, const wchar_t *pSrc, size_t c)
{
    size_t i;

    if (pDst == NULL || pSrc == NULL) {
        return pDst;
    }
    if (pDst < pSrc) {
        for (i = 0; i < c; i++) {
            pDst[i] = pSrc[i];
        }
    } else if (pDst > pSrc) {
        i = c;
        while (i > 0) {
            i--;
            pDst[i] = pSrc[i];
        }
    }
    return pDst;
}

wchar_t *
wmemset(wchar_t *pDst, wchar_t wc, size_t c)
{
    size_t i;

    if (pDst == NULL) {
        return pDst;
    }
    for (i = 0; i < c; i++) {
        pDst[i] = wc;
    }
    return pDst;
}

int
wmemcmp(const wchar_t *a, const wchar_t *b, size_t c)
{
    size_t i;

    if (a == NULL || b == NULL) {
        return (a == b) ? 0 : (a == NULL ? -1 : 1);
    }
    for (i = 0; i < c; i++) {
        if (a[i] != b[i]) {
            return (int)a[i] - (int)b[i];
        }
    }
    return 0;
}

wchar_t *
wmemchr(const wchar_t *p, wchar_t wc, size_t c)
{
    size_t i;

    if (p == NULL) {
        return NULL;
    }
    for (i = 0; i < c; i++) {
        if (p[i] == wc) {
            return (wchar_t *)(p + i);
        }
    }
    return NULL;
}

int
wcscoll(const wchar_t *a, const wchar_t *b)
{
    /* C locale: same as wcscmp */
    return wcscmp(a, b);
}

size_t
wcsxfrm(wchar_t *pDst, const wchar_t *szSrc, size_t c)
{
    size_t n;

    if (szSrc == NULL) {
        return 0;
    }
    n = wcslen(szSrc);
    if (pDst != NULL && c > 0) {
        size_t i;
        size_t lim = (n < c) ? n : (c - 1);

        for (i = 0; i < lim; i++) {
            pDst[i] = szSrc[i];
        }
        pDst[lim] = 0;
    }
    return n;
}

/* ---- Wide scanf subset -------------------------------------------------- */

struct wscan_src {
    FILE           *pF;
    const wchar_t  *pS;
    size_t          nPos;
};

static wint_t
wscan_get(struct wscan_src *pSrc)
{
    wint_t wc;

    if (pSrc->pF != NULL) {
        return fgetwc(pSrc->pF);
    }
    if (pSrc->pS == NULL) {
        return WEOF;
    }
    wc = (wint_t)pSrc->pS[pSrc->nPos];
    if (wc == 0) {
        return WEOF;
    }
    pSrc->nPos++;
    return wc;
}

static void
wscan_unget(struct wscan_src *pSrc, wint_t wc)
{
    if (wc == WEOF) {
        return;
    }
    if (pSrc->pF != NULL) {
        (void)ungetwc(wc, pSrc->pF);
        return;
    }
    if (pSrc->nPos > 0) {
        pSrc->nPos--;
    }
}

static void
wscan_skip_space(struct wscan_src *pSrc)
{
    wint_t wc;

    for (;;) {
        wc = wscan_get(pSrc);
        if (wc == WEOF) {
            return;
        }
        if (!iswspace(wc)) {
            wscan_unget(pSrc, wc);
            return;
        }
    }
}

static int
wscan_int(struct wscan_src *pSrc, int nBase, long long *pOut)
{
    wint_t wc;
    int fNeg = 0;
    int fAny = 0;
    unsigned long long u = 0;

    wscan_skip_space(pSrc);
    wc = wscan_get(pSrc);
    if (wc == L'+' || wc == L'-') {
        fNeg = (wc == L'-');
        wc = wscan_get(pSrc);
    }
    if (nBase == 0) {
        if (wc == L'0') {
            wint_t w2 = wscan_get(pSrc);

            if (w2 == L'x' || w2 == L'X') {
                nBase = 16;
                wc = wscan_get(pSrc);
            } else {
                nBase = 8;
                wscan_unget(pSrc, w2);
                fAny = 1;
            }
        } else {
            nBase = 10;
        }
    }
    for (;;) {
        int d = -1;

        if (wc >= L'0' && wc <= L'9') {
            d = (int)(wc - L'0');
        } else if (wc >= L'a' && wc <= L'f') {
            d = 10 + (int)(wc - L'a');
        } else if (wc >= L'A' && wc <= L'F') {
            d = 10 + (int)(wc - L'A');
        }
        if (d < 0 || d >= nBase) {
            break;
        }
        fAny = 1;
        u = u * (unsigned)nBase + (unsigned)d;
        wc = wscan_get(pSrc);
    }
    if (wc != WEOF) {
        wscan_unget(pSrc, wc);
    }
    if (!fAny) {
        return -1;
    }
    *pOut = fNeg ? -(long long)u : (long long)u;
    return 0;
}

static int
vwscan(struct wscan_src *pSrc, const wchar_t *szFmt, va_list ap)
{
    int nAssigned = 0;
    const wchar_t *p;

    if (szFmt == NULL) {
        return EOF;
    }
    for (p = szFmt; *p != 0; p++) {
        if (iswspace(*p)) {
            wscan_skip_space(pSrc);
            while (iswspace(*p)) {
                p++;
            }
            p--;
            continue;
        }
        if (*p != L'%') {
            wint_t wc = wscan_get(pSrc);

            if (wc == WEOF) {
                return (nAssigned > 0) ? nAssigned : EOF;
            }
            if ((wchar_t)wc != *p) {
                wscan_unget(pSrc, wc);
                return nAssigned;
            }
            continue;
        }
        p++;
        if (*p == 0) {
            break;
        }
        if (*p == L'%') {
            wint_t wc = wscan_get(pSrc);

            if (wc != L'%') {
                if (wc != WEOF) {
                    wscan_unget(pSrc, wc);
                }
                return nAssigned;
            }
            continue;
        }
        {
            int fSuppress = 0;
            int nWidth = 0;

            if (*p == L'*') {
                fSuppress = 1;
                p++;
            }
            while (*p >= L'0' && *p <= L'9') {
                nWidth = nWidth * 10 + (int)(*p - L'0');
                p++;
            }
            if (*p == L'l') {
                p++;
                if (*p == L'd' || *p == L'i') {
                    long long v = 0;

                    if (wscan_int(pSrc, 10, &v) != 0) {
                        return (nAssigned > 0) ? nAssigned : EOF;
                    }
                    if (!fSuppress) {
                        *va_arg(ap, long *) = (long)v;
                        nAssigned++;
                    }
                    continue;
                }
                if (*p == L'u') {
                    long long v = 0;

                    if (wscan_int(pSrc, 10, &v) != 0) {
                        return (nAssigned > 0) ? nAssigned : EOF;
                    }
                    if (!fSuppress) {
                        *va_arg(ap, unsigned long *) = (unsigned long)v;
                        nAssigned++;
                    }
                    continue;
                }
                if (*p == L's') {
                    wchar_t *pOut = fSuppress ? NULL : va_arg(ap, wchar_t *);
                    int n = 0;
                    wint_t wc;

                    wscan_skip_space(pSrc);
                    for (;;) {
                        if (nWidth > 0 && n >= nWidth) {
                            break;
                        }
                        wc = wscan_get(pSrc);
                        if (wc == WEOF || iswspace(wc)) {
                            if (wc != WEOF) {
                                wscan_unget(pSrc, wc);
                            }
                            break;
                        }
                        if (pOut != NULL) {
                            pOut[n] = (wchar_t)wc;
                        }
                        n++;
                    }
                    if (n == 0) {
                        return (nAssigned > 0) ? nAssigned : EOF;
                    }
                    if (pOut != NULL) {
                        pOut[n] = 0;
                        nAssigned++;
                    }
                    continue;
                }
                if (*p == L'c') {
                    wint_t wc = wscan_get(pSrc);

                    if (wc == WEOF) {
                        return (nAssigned > 0) ? nAssigned : EOF;
                    }
                    if (!fSuppress) {
                        *va_arg(ap, wchar_t *) = (wchar_t)wc;
                        nAssigned++;
                    }
                    continue;
                }
            }
            if (*p == L'd' || *p == L'i') {
                long long v = 0;

                if (wscan_int(pSrc, (*p == L'i') ? 0 : 10, &v) != 0) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                if (!fSuppress) {
                    *va_arg(ap, int *) = (int)v;
                    nAssigned++;
                }
                continue;
            }
            if (*p == L'u') {
                long long v = 0;

                if (wscan_int(pSrc, 10, &v) != 0) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                if (!fSuppress) {
                    *va_arg(ap, unsigned *) = (unsigned)v;
                    nAssigned++;
                }
                continue;
            }
            if (*p == L'x' || *p == L'X') {
                long long v = 0;

                if (wscan_int(pSrc, 16, &v) != 0) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                if (!fSuppress) {
                    *va_arg(ap, unsigned *) = (unsigned)v;
                    nAssigned++;
                }
                continue;
            }
            if (*p == L's') {
                char *pOut = fSuppress ? NULL : va_arg(ap, char *);
                int n = 0;
                wint_t wc;
                mbstate_t st;

                memset(&st, 0, sizeof(st));
                wscan_skip_space(pSrc);
                for (;;) {
                    char aMb[8];
                    size_t cb;

                    if (nWidth > 0 && n >= nWidth) {
                        break;
                    }
                    wc = wscan_get(pSrc);
                    if (wc == WEOF || iswspace(wc)) {
                        if (wc != WEOF) {
                            wscan_unget(pSrc, wc);
                        }
                        break;
                    }
                    cb = wcrtomb(aMb, (wchar_t)wc, &st);
                    if (cb == (size_t)-1) {
                        return nAssigned;
                    }
                    if (pOut != NULL) {
                        size_t k;

                        for (k = 0; k < cb; k++) {
                            pOut[n++] = aMb[k];
                        }
                    } else {
                        n += (int)cb;
                    }
                }
                if (n == 0) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                if (pOut != NULL) {
                    pOut[n] = '\0';
                    nAssigned++;
                }
                continue;
            }
            if (*p == L'c') {
                wint_t wc = wscan_get(pSrc);
                char aMb[8];
                size_t cb;
                mbstate_t st;

                if (wc == WEOF) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                if (!fSuppress) {
                    char *pOut = va_arg(ap, char *);

                    memset(&st, 0, sizeof(st));
                    cb = wcrtomb(aMb, (wchar_t)wc, &st);
                    if (cb == (size_t)-1 || cb == 0) {
                        pOut[0] = (char)wc;
                    } else {
                        pOut[0] = aMb[0];
                    }
                    nAssigned++;
                }
                continue;
            }
            if (*p == L'n') {
                if (!fSuppress) {
                    *va_arg(ap, int *) = (int)pSrc->nPos;
                }
                continue;
            }
            /* unknown conversion */
            return nAssigned;
        }
    }
    return nAssigned;
}

int
vfwscanf(FILE *pF, const wchar_t *szFmt, va_list ap)
{
    struct wscan_src src;

    if (pF == NULL || szFmt == NULL) {
        errno = EINVAL;
        return EOF;
    }
    src.pF = pF;
    src.pS = NULL;
    src.nPos = 0;
    return vwscan(&src, szFmt, ap);
}

int
fwscanf(FILE *pF, const wchar_t *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vfwscanf(pF, szFmt, ap);
    va_end(ap);
    return n;
}

int
vwscanf(const wchar_t *szFmt, va_list ap)
{
    return vfwscanf(stdin, szFmt, ap);
}

int
wscanf(const wchar_t *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vwscanf(szFmt, ap);
    va_end(ap);
    return n;
}

int
vswscanf(const wchar_t *szBuf, const wchar_t *szFmt, va_list ap)
{
    struct wscan_src src;

    if (szBuf == NULL || szFmt == NULL) {
        errno = EINVAL;
        return EOF;
    }
    src.pF = NULL;
    src.pS = szBuf;
    src.nPos = 0;
    return vwscan(&src, szFmt, ap);
}

int
swscanf(const wchar_t *szBuf, const wchar_t *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vswscanf(szBuf, szFmt, ap);
    va_end(ap);
    return n;
}
