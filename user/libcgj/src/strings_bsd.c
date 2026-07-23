/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * BSD strings + C-locale strcoll/strxfrm.
 */
#include <stddef.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

void
bzero(void *p, size_t cb)
{
    if (p == NULL || cb == 0) {
        return;
    }
    (void)memset(p, 0, cb);
}

void
bcopy(const void *pSrc, void *pDst, size_t cb)
{
    if (pDst == NULL || pSrc == NULL || cb == 0) {
        return;
    }
    (void)memmove(pDst, pSrc, cb);
}

int
bcmp(const void *pA, const void *pB, size_t cb)
{
    if (cb == 0) {
        return 0;
    }
    if (pA == NULL || pB == NULL) {
        return (pA == pB) ? 0 : 1;
    }
    return memcmp(pA, pB, cb);
}

char *
index(const char *sz, int ch)
{
    if (sz == NULL) {
        return NULL;
    }
    return strchr(sz, ch);
}

char *
rindex(const char *sz, int ch)
{
    if (sz == NULL) {
        return NULL;
    }
    return strrchr(sz, ch);
}

void
swab(const void *pSrc, void *pDst, ssize_t cb)
{
    const unsigned char *s = (const unsigned char *)pSrc;
    unsigned char *d = (unsigned char *)pDst;
    ssize_t i;

    if (s == NULL || d == NULL || cb < 2) {
        return;
    }
    /* Odd trailing byte left untouched (POSIX). */
    for (i = 0; i + 1 < cb; i += 2) {
        unsigned char a = s[i];
        unsigned char b = s[i + 1];

        d[i] = b;
        d[i + 1] = a;
    }
}

int
strcoll(const char *szA, const char *szB)
{
    /* C locale — identical to strcmp; soft null-safe. */
    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    return strcmp(szA, szB);
}

size_t
strxfrm(char *szDst, const char *szSrc, size_t cb)
{
    size_t n;

    if (szSrc == NULL) {
        if (szDst != NULL && cb > 0) {
            szDst[0] = '\0';
        }
        return 0;
    }
    n = strlen(szSrc);
    /* POSIX: if cb == 0, szDst may be NULL; return length only. */
    if (szDst != NULL && cb > 0) {
        size_t c = (n < cb - 1) ? n : (cb - 1);

        if (c > 0) {
            memcpy(szDst, szSrc, c);
        }
        szDst[c] = '\0';
    }
    return n;
}
