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
    (void)memset(p, 0, cb);
}

void
bcopy(const void *pSrc, void *pDst, size_t cb)
{
    (void)memmove(pDst, pSrc, cb);
}

int
bcmp(const void *pA, const void *pB, size_t cb)
{
    return memcmp(pA, pB, cb);
}

char *
index(const char *sz, int ch)
{
    return strchr(sz, ch);
}

char *
rindex(const char *sz, int ch)
{
    return strrchr(sz, ch);
}

void
swab(const void *pSrc, void *pDst, ssize_t cb)
{
    const unsigned char *s = (const unsigned char *)pSrc;
    unsigned char *d = (unsigned char *)pDst;
    ssize_t i;

    if (cb < 0) {
        return;
    }
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
    /* C locale */
    return strcmp(szA, szB);
}

size_t
strxfrm(char *szDst, const char *szSrc, size_t cb)
{
    size_t n;

    if (szSrc == NULL) {
        return 0;
    }
    n = strlen(szSrc);
    if (szDst != NULL && cb > 0) {
        size_t c = (n < cb - 1) ? n : (cb - 1);
        memcpy(szDst, szSrc, c);
        szDst[c] = '\0';
    }
    return n;
}
