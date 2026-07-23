/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Additional string/memory helpers (mempcpy, ffs, strerror_r, …).
 */
#include <stdint.h>
#include <string.h>
#include <strings.h>

void *
mempcpy(void *pDst, const void *pSrc, size_t cb)
{
    unsigned char *p;

    p = (unsigned char *)memcpy(pDst, pSrc, cb);
    if (p == NULL) {
        return NULL;
    }
    return p + cb;
}

void *
rawmemchr(const void *p, int c)
{
    const unsigned char *pB = (const unsigned char *)p;
    unsigned char ch = (unsigned char)c;

    if (pB == NULL) {
        return NULL;
    }
    /* Unbounded scan (glibc): assumes the byte exists in the object. */
    for (;;) {
        if (*pB == ch) {
            return (void *)(uintptr_t)pB;
        }
        pB++;
    }
}

/* GNU strerror_r: always returns a pointer usable as a message. */
char *
strerror_r(int nErr, char *szBuf, size_t cb)
{
    const char *sz;
    size_t n;
    size_t i;

    sz = strerror(nErr);
    if (szBuf == NULL || cb == 0) {
        return (char *)sz;
    }
    n = strlen(sz);
    if (n >= cb) {
        n = cb - 1;
    }
    for (i = 0; i < n; i++) {
        szBuf[i] = sz[i];
    }
    szBuf[n] = '\0';
    return szBuf;
}

int
ffs(int n)
{
    unsigned u;
    int i;

    if (n == 0) {
        return 0;
    }
    u = (unsigned)n;
    for (i = 1; i <= 32; i++) {
        if (u & 1u) {
            return i;
        }
        u >>= 1;
    }
    return 0;
}

int
ffsl(long n)
{
    unsigned long u;
    int i;
    int nBits = (int)(sizeof(long) * 8);

    if (n == 0) {
        return 0;
    }
    u = (unsigned long)n;
    for (i = 1; i <= nBits; i++) {
        if (u & 1UL) {
            return i;
        }
        u >>= 1;
    }
    return 0;
}

int
ffsll(long long n)
{
    unsigned long long u;
    int i;

    if (n == 0) {
        return 0;
    }
    u = (unsigned long long)n;
    for (i = 1; i <= 64; i++) {
        if (u & 1ULL) {
            return i;
        }
        u >>= 1;
    }
    return 0;
}
