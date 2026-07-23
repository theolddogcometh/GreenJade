/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding string/memory helpers for libgj userspace.
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 */
#include <gj/string.h>
#include <gj/syscalls.h>
#include <stdint.h>

size_t
gj_strlen(const char *sz)
{
    size_t n = 0;

    if (sz == NULL) {
        return 0;
    }
    while (sz[n] != '\0') {
        n++;
    }
    return n;
}

size_t
gj_strnlen(const char *sz, size_t cbMax)
{
    size_t n = 0;

    if (sz == NULL || cbMax == 0) {
        return 0;
    }
    while (n < cbMax && sz[n] != '\0') {
        n++;
    }
    return n;
}

int
gj_strcmp(const char *szA, const char *szB)
{
    size_t i = 0;

    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    while (szA[i] != '\0' && szA[i] == szB[i]) {
        i++;
    }
    return (unsigned char)szA[i] - (unsigned char)szB[i];
}

int
gj_strncmp(const char *szA, const char *szB, size_t cb)
{
    size_t i;

    if (cb == 0) {
        return 0;
    }
    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    for (i = 0; i < cb; i++) {
        unsigned char ca = (unsigned char)szA[i];
        unsigned char cb2 = (unsigned char)szB[i];

        if (ca != cb2) {
            return (int)ca - (int)cb2;
        }
        if (ca == 0) {
            return 0;
        }
    }
    return 0;
}

char *
gj_strcpy(char *szDst, const char *szSrc)
{
    size_t i = 0;

    if (szDst == NULL) {
        return NULL;
    }
    if (szSrc == NULL) {
        szDst[0] = '\0';
        return szDst;
    }
    while (szSrc[i] != '\0') {
        szDst[i] = szSrc[i];
        i++;
    }
    szDst[i] = '\0';
    return szDst;
}

char *
gj_strncpy(char *szDst, const char *szSrc, size_t cb)
{
    size_t i;

    if (szDst == NULL || cb == 0) {
        return szDst;
    }
    for (i = 0; i < cb; i++) {
        if (szSrc != NULL && szSrc[i] != '\0') {
            szDst[i] = szSrc[i];
        } else {
            szDst[i] = '\0';
            i++;
            break;
        }
    }
    for (; i < cb; i++) {
        szDst[i] = '\0';
    }
    return szDst;
}

char *
gj_strcat(char *szDst, const char *szSrc)
{
    size_t nDst;

    if (szDst == NULL) {
        return NULL;
    }
    nDst = gj_strlen(szDst);
    (void)gj_strcpy(szDst + nDst, szSrc != NULL ? szSrc : "");
    return szDst;
}

char *
gj_strncat(char *szDst, const char *szSrc, size_t cb)
{
    size_t nDst;
    size_t i;

    if (szDst == NULL) {
        return NULL;
    }
    if (cb == 0 || szSrc == NULL) {
        return szDst;
    }
    nDst = gj_strlen(szDst);
    for (i = 0; i < cb && szSrc[i] != '\0'; i++) {
        szDst[nDst + i] = szSrc[i];
    }
    szDst[nDst + i] = '\0';
    return szDst;
}

size_t
gj_strlcpy(char *szDst, const char *szSrc, size_t cbDst)
{
    size_t nSrc;
    size_t i;

    nSrc = gj_strlen(szSrc);
    if (szDst == NULL || cbDst == 0) {
        return nSrc;
    }
    i = 0;
    if (szSrc != NULL) {
        while (i + 1u < cbDst && szSrc[i] != '\0') {
            szDst[i] = szSrc[i];
            i++;
        }
    }
    szDst[i] = '\0';
    return nSrc;
}

size_t
gj_strlcat(char *szDst, const char *szSrc, size_t cbDst)
{
    size_t nDst;
    size_t nSrc;
    size_t i;

    nSrc = gj_strlen(szSrc);
    if (szDst == NULL || cbDst == 0) {
        return nSrc;
    }
    nDst = gj_strnlen(szDst, cbDst);
    if (nDst >= cbDst) {
        /* No room / not NUL-terminated within cbDst. */
        return cbDst + nSrc;
    }
    i = 0;
    if (szSrc != NULL) {
        while (nDst + i + 1u < cbDst && szSrc[i] != '\0') {
            szDst[nDst + i] = szSrc[i];
            i++;
        }
    }
    szDst[nDst + i] = '\0';
    return nDst + nSrc;
}

char *
gj_strchr(const char *sz, int ch)
{
    if (sz == NULL) {
        return NULL;
    }
    while (*sz != '\0') {
        if ((unsigned char)*sz == (unsigned char)ch) {
            return (char *)(uintptr_t)sz;
        }
        sz++;
    }
    if (ch == 0) {
        return (char *)(uintptr_t)sz;
    }
    return NULL;
}

char *
gj_strrchr(const char *sz, int ch)
{
    const char *pLast = NULL;

    if (sz == NULL) {
        return NULL;
    }
    while (*sz != '\0') {
        if ((unsigned char)*sz == (unsigned char)ch) {
            pLast = sz;
        }
        sz++;
    }
    if (ch == 0) {
        return (char *)(uintptr_t)sz;
    }
    return (char *)(uintptr_t)pLast;
}

char *
gj_strstr(const char *szHay, const char *szNeedle)
{
    size_t cbNeedle;
    size_t i;

    if (szHay == NULL) {
        return NULL;
    }
    if (szNeedle == NULL || szNeedle[0] == '\0') {
        return (char *)(uintptr_t)szHay;
    }
    cbNeedle = gj_strlen(szNeedle);
    for (i = 0; szHay[i] != '\0'; i++) {
        size_t j;

        for (j = 0; j < cbNeedle; j++) {
            if (szHay[i + j] == '\0' || szHay[i + j] != szNeedle[j]) {
                break;
            }
        }
        if (j == cbNeedle) {
            return (char *)(uintptr_t)(szHay + i);
        }
    }
    return NULL;
}

void *
gj_memchr(const void *p, int ch, size_t cb)
{
    const unsigned char *pB = (const unsigned char *)p;
    unsigned char chWant = (unsigned char)ch;
    size_t i;

    if (p == NULL) {
        return NULL;
    }
    for (i = 0; i < cb; i++) {
        if (pB[i] == chWant) {
            return (void *)(uintptr_t)(pB + i);
        }
    }
    return NULL;
}

void *
gj_memcpy(void *pDst, const void *pSrc, size_t cb)
{
    unsigned char *d = (unsigned char *)pDst;
    const unsigned char *s = (const unsigned char *)pSrc;
    size_t i;

    for (i = 0; i < cb; i++) {
        d[i] = s[i];
    }
    return pDst;
}

void *
gj_memmove(void *pDst, const void *pSrc, size_t cb)
{
    unsigned char *d = (unsigned char *)pDst;
    const unsigned char *s = (const unsigned char *)pSrc;
    size_t i;

    if (d == s || cb == 0) {
        return pDst;
    }
    if (d < s) {
        for (i = 0; i < cb; i++) {
            d[i] = s[i];
        }
    } else {
        for (i = cb; i > 0; i--) {
            d[i - 1] = s[i - 1];
        }
    }
    return pDst;
}

void *
gj_memset(void *pDst, int c, size_t cb)
{
    unsigned char *d = (unsigned char *)pDst;
    size_t i;

    for (i = 0; i < cb; i++) {
        d[i] = (unsigned char)c;
    }
    return pDst;
}

int
gj_memcmp(const void *pA, const void *pB, size_t cb)
{
    const unsigned char *a = (const unsigned char *)pA;
    const unsigned char *b = (const unsigned char *)pB;
    size_t i;

    for (i = 0; i < cb; i++) {
        if (a[i] != b[i]) {
            return (int)a[i] - (int)b[i];
        }
    }
    return 0;
}

void
gj_puts(const char *sz)
{
    size_t cb;

    if (sz == NULL) {
        return;
    }
    cb = gj_strlen(sz);
    if (cb > 0) {
        (void)linux_write(1, sz, cb);
    }
}

long
gj_write(int nFd, const void *p, size_t cb)
{
    return linux_write(nFd, p, cb);
}

long
gj_dlog(const char *sz)
{
    if (sz == NULL) {
        return gj_debug_log("", 0);
    }
    return gj_debug_log(sz, (long)gj_strlen(sz));
}

size_t
gj_itoa(long i64Val, char *szBuf, size_t cbBuf)
{
    char aTmp[24];
    size_t n = 0;
    size_t i;
    unsigned long u;
    int fNeg = 0;

    if (szBuf == NULL || cbBuf == 0) {
        return 0;
    }
    if (i64Val < 0) {
        fNeg = 1;
        u = (unsigned long)(-(i64Val + 1)) + 1ul;
    } else {
        u = (unsigned long)i64Val;
    }
    if (u == 0) {
        aTmp[n++] = '0';
    } else {
        while (u > 0 && n < sizeof(aTmp)) {
            aTmp[n++] = (char)('0' + (u % 10ul));
            u /= 10ul;
        }
    }
    i = 0;
    if (fNeg && i < cbBuf - 1) {
        szBuf[i++] = '-';
    }
    while (n > 0 && i < cbBuf - 1) {
        n--;
        szBuf[i++] = aTmp[n];
    }
    szBuf[i] = '\0';
    return i;
}

size_t
gj_utoa(unsigned long uVal, char *szBuf, size_t cbBuf)
{
    char aTmp[24];
    size_t n = 0;
    size_t i;

    if (szBuf == NULL || cbBuf == 0) {
        return 0;
    }
    if (uVal == 0) {
        aTmp[n++] = '0';
    } else {
        while (uVal > 0 && n < sizeof(aTmp)) {
            aTmp[n++] = (char)('0' + (uVal % 10ul));
            uVal /= 10ul;
        }
    }
    i = 0;
    while (n > 0 && i < cbBuf - 1) {
        n--;
        szBuf[i++] = aTmp[n];
    }
    szBuf[i] = '\0';
    return i;
}

size_t
gj_xtoa(unsigned long uVal, char *szBuf, size_t cbBuf,
        int fLeadZero, size_t cMin)
{
    static const char aHex[] = "0123456789abcdef";
    char aTmp[24];
    size_t n = 0;
    size_t i;
    size_t cPad;

    if (szBuf == NULL || cbBuf == 0) {
        return 0;
    }
    if (uVal == 0) {
        aTmp[n++] = '0';
    } else {
        while (uVal > 0 && n < sizeof(aTmp)) {
            aTmp[n++] = aHex[uVal & 0xful];
            uVal >>= 4;
        }
    }
    cPad = 0;
    if (fLeadZero != 0 && cMin > n) {
        cPad = cMin - n;
    }
    i = 0;
    while (cPad > 0 && i < cbBuf - 1) {
        szBuf[i++] = '0';
        cPad--;
    }
    while (n > 0 && i < cbBuf - 1) {
        n--;
        szBuf[i++] = aTmp[n];
    }
    szBuf[i] = '\0';
    return i;
}
