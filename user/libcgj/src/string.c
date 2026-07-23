/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room string/memory (glibc-shaped names).
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

size_t
strlen(const char *sz)
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
strnlen(const char *sz, size_t nMax)
{
    size_t n = 0;

    if (sz == NULL) {
        return 0;
    }
    while (n < nMax && sz[n] != '\0') {
        n++;
    }
    return n;
}

int
strcmp(const char *szA, const char *szB)
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
strncmp(const char *szA, const char *szB, size_t cb)
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
stpcpy(char *szDst, const char *szSrc)
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
    return szDst + i;
}

char *
strcpy(char *szDst, const char *szSrc)
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
strncpy(char *szDst, const char *szSrc, size_t cb)
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
            while (i < cb) {
                szDst[i++] = '\0';
            }
            break;
        }
    }
    return szDst;
}

char *
strcat(char *szDst, const char *szSrc)
{
    size_t n;

    if (szDst == NULL) {
        return NULL;
    }
    n = strlen(szDst);
    if (szSrc != NULL) {
        (void)strcpy(szDst + n, szSrc);
    }
    return szDst;
}

char *
strchr(const char *sz, int ch)
{
    char c = (char)ch;

    if (sz == NULL) {
        return NULL;
    }
    for (;;) {
        if (*sz == c) {
            return (char *)(uintptr_t)sz;
        }
        if (*sz == '\0') {
            return NULL;
        }
        sz++;
    }
}

char *
strchrnul(const char *sz, int ch)
{
    char c = (char)ch;

    if (sz == NULL) {
        return NULL;
    }
    while (*sz != '\0' && *sz != c) {
        sz++;
    }
    return (char *)(uintptr_t)sz;
}

char *
strrchr(const char *sz, int ch)
{
    const char *pLast = NULL;
    char c = (char)ch;

    if (sz == NULL) {
        return NULL;
    }
    while (*sz != '\0') {
        if (*sz == c) {
            pLast = sz;
        }
        sz++;
    }
    if (c == '\0') {
        return (char *)(uintptr_t)sz;
    }
    return (char *)(uintptr_t)pLast;
}

void *
memcpy(void *pDst, const void *pSrc, size_t cb)
{
    unsigned char *pD = (unsigned char *)pDst;
    const unsigned char *pS = (const unsigned char *)pSrc;
    size_t i;

    if (pD == NULL || pS == NULL) {
        return pDst;
    }
    for (i = 0; i < cb; i++) {
        pD[i] = pS[i];
    }
    return pDst;
}

void *
memmove(void *pDst, const void *pSrc, size_t cb)
{
    unsigned char *pD = (unsigned char *)pDst;
    const unsigned char *pS = (const unsigned char *)pSrc;
    size_t i;

    if (pD == NULL || pS == NULL) {
        return pDst;
    }
    if (pD < pS) {
        for (i = 0; i < cb; i++) {
            pD[i] = pS[i];
        }
    } else if (pD > pS) {
        i = cb;
        while (i > 0) {
            i--;
            pD[i] = pS[i];
        }
    }
    return pDst;
}

void *
memset(void *pDst, int c, size_t cb)
{
    unsigned char *p = (unsigned char *)pDst;
    size_t i;

    if (p == NULL) {
        return pDst;
    }
    for (i = 0; i < cb; i++) {
        p[i] = (unsigned char)c;
    }
    return pDst;
}

int
memcmp(const void *pA, const void *pB, size_t cb)
{
    const unsigned char *pX = (const unsigned char *)pA;
    const unsigned char *pY = (const unsigned char *)pB;
    size_t i;

    if (pX == NULL || pY == NULL) {
        return (pX == pY) ? 0 : (pX == NULL ? -1 : 1);
    }
    for (i = 0; i < cb; i++) {
        if (pX[i] != pY[i]) {
            return (int)pX[i] - (int)pY[i];
        }
    }
    return 0;
}

void *
memchr(const void *p, int c, size_t cb)
{
    const unsigned char *pB = (const unsigned char *)p;
    unsigned char ch = (unsigned char)c;
    size_t i;

    if (pB == NULL) {
        return NULL;
    }
    for (i = 0; i < cb; i++) {
        if (pB[i] == ch) {
            return (void *)(uintptr_t)(pB + i);
        }
    }
    return NULL;
}

void *
memmem(const void *pHay, size_t cbHay, const void *pNeedle, size_t cbNeedle)
{
    const unsigned char *pH = (const unsigned char *)pHay;
    const unsigned char *pN = (const unsigned char *)pNeedle;
    size_t i;

    if (cbNeedle == 0) {
        return (void *)(uintptr_t)pH;
    }
    if (pH == NULL || pN == NULL || cbNeedle > cbHay) {
        return NULL;
    }
    for (i = 0; i + cbNeedle <= cbHay; i++) {
        if (memcmp(pH + i, pN, cbNeedle) == 0) {
            return (void *)(uintptr_t)(pH + i);
        }
    }
    return NULL;
}

char *
strncat(char *szDst, const char *szSrc, size_t cb)
{
    size_t n;
    size_t i;

    if (szDst == NULL) {
        return NULL;
    }
    n = strlen(szDst);
    if (szSrc == NULL || cb == 0) {
        return szDst;
    }
    for (i = 0; i < cb && szSrc[i] != '\0'; i++) {
        szDst[n + i] = szSrc[i];
    }
    szDst[n + i] = '\0';
    return szDst;
}

char *
strstr(const char *szHay, const char *szNeedle)
{
    size_t nNeedle;
    size_t i;

    if (szHay == NULL || szNeedle == NULL) {
        return NULL;
    }
    nNeedle = strlen(szNeedle);
    if (nNeedle == 0) {
        return (char *)(uintptr_t)szHay;
    }
    for (i = 0; szHay[i] != '\0'; i++) {
        if (strncmp(szHay + i, szNeedle, nNeedle) == 0) {
            return (char *)(uintptr_t)(szHay + i);
        }
    }
    return NULL;
}

char *
strdup(const char *sz)
{
    size_t n;
    char *p;

    if (sz == NULL) {
        return NULL;
    }
    n = strlen(sz);
    p = (char *)malloc(n + 1);
    if (p == NULL) {
        return NULL;
    }
    (void)memcpy(p, sz, n + 1);
    return p;
}

char *
strndup(const char *sz, size_t n)
{
    size_t i;
    char *p;

    if (sz == NULL) {
        return NULL;
    }
    i = 0;
    while (i < n && sz[i] != '\0') {
        i++;
    }
    p = (char *)malloc(i + 1);
    if (p == NULL) {
        return NULL;
    }
    if (i > 0) {
        (void)memcpy(p, sz, i);
    }
    p[i] = '\0';
    return p;
}

size_t
strspn(const char *sz, const char *szAccept)
{
    size_t i = 0;

    if (sz == NULL || szAccept == NULL) {
        return 0;
    }
    while (sz[i] != '\0' && strchr(szAccept, (unsigned char)sz[i]) != NULL) {
        i++;
    }
    return i;
}

size_t
strcspn(const char *sz, const char *szReject)
{
    size_t i = 0;

    if (sz == NULL) {
        return 0;
    }
    if (szReject == NULL) {
        return strlen(sz);
    }
    while (sz[i] != '\0' && strchr(szReject, (unsigned char)sz[i]) == NULL) {
        i++;
    }
    return i;
}

char *
strpbrk(const char *sz, const char *szAccept)
{
    size_t i;

    if (sz == NULL || szAccept == NULL) {
        return NULL;
    }
    for (i = 0; sz[i] != '\0'; i++) {
        if (strchr(szAccept, (unsigned char)sz[i]) != NULL) {
            return (char *)(uintptr_t)(sz + i);
        }
    }
    return NULL;
}

char *
strtok_r(char *sz, const char *szDelim, char **ppSave)
{
    char *pStart;
    char *pEnd;

    if (ppSave == NULL || szDelim == NULL) {
        return NULL;
    }
    if (sz != NULL) {
        *ppSave = sz;
    }
    if (*ppSave == NULL) {
        return NULL;
    }
    pStart = *ppSave;
    pStart += strspn(pStart, szDelim);
    if (*pStart == '\0') {
        *ppSave = NULL;
        return NULL;
    }
    pEnd = pStart + strcspn(pStart, szDelim);
    if (*pEnd != '\0') {
        *pEnd = '\0';
        *ppSave = pEnd + 1;
    } else {
        *ppSave = NULL;
    }
    return pStart;
}

char *
strsep(char **ppString, const char *szDelim)
{
    char *pStart;
    char *pEnd;

    if (ppString == NULL || *ppString == NULL) {
        return NULL;
    }
    pStart = *ppString;
    if (szDelim == NULL) {
        *ppString = NULL;
        return pStart;
    }
    pEnd = pStart + strcspn(pStart, szDelim);
    if (*pEnd != '\0') {
        *pEnd = '\0';
        *ppString = pEnd + 1;
    } else {
        *ppString = NULL;
    }
    return pStart;
}

int
strcasecmp(const char *szA, const char *szB)
{
    size_t i;
    int ca;
    int cb;

    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    for (i = 0;; i++) {
        ca = (unsigned char)szA[i];
        cb = (unsigned char)szB[i];
        if (ca >= 'A' && ca <= 'Z') {
            ca = ca - 'A' + 'a';
        }
        if (cb >= 'A' && cb <= 'Z') {
            cb = cb - 'A' + 'a';
        }
        if (ca != cb || ca == 0) {
            return ca - cb;
        }
    }
}

int
strncasecmp(const char *szA, const char *szB, size_t n)
{
    size_t i;
    int ca;
    int cb;

    if (n == 0) {
        return 0;
    }
    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    for (i = 0; i < n; i++) {
        ca = (unsigned char)szA[i];
        cb = (unsigned char)szB[i];
        if (ca >= 'A' && ca <= 'Z') {
            ca = ca - 'A' + 'a';
        }
        if (cb >= 'A' && cb <= 'Z') {
            cb = cb - 'A' + 'a';
        }
        if (ca != cb || ca == 0) {
            return ca - cb;
        }
    }
    return 0;
}

/* POSIX basename/dirname — may modify path in place */
static char g_szDot[] = ".";
static char g_szSlash[] = "/";

char *
basename(char *szPath)
{
    char *p;
    char *pLast;

    if (szPath == NULL || szPath[0] == '\0') {
        return g_szDot;
    }
    /* strip trailing slashes */
    p = szPath + strlen(szPath);
    while (p > szPath && p[-1] == '/') {
        p--;
        *p = '\0';
    }
    if (szPath[0] == '\0') {
        return g_szSlash;
    }
    pLast = strrchr(szPath, '/');
    if (pLast == NULL) {
        return szPath;
    }
    if (pLast[1] == '\0') {
        return g_szSlash;
    }
    return pLast + 1;
}

char *
dirname(char *szPath)
{
    char *p;

    if (szPath == NULL || szPath[0] == '\0') {
        return g_szDot;
    }
    p = szPath + strlen(szPath);
    while (p > szPath && p[-1] == '/') {
        p--;
        *p = '\0';
    }
    p = strrchr(szPath, '/');
    if (p == NULL) {
        szPath[0] = '.';
        szPath[1] = '\0';
        return szPath;
    }
    if (p == szPath) {
        p[1] = '\0';
        return szPath;
    }
    *p = '\0';
    return szPath;
}

/* BSD/macOS-shaped safe copy: always NUL-terminates when cbDst > 0 */
size_t
strlcpy(char *szDst, const char *szSrc, size_t cbDst)
{
    size_t nSrc;
    size_t i;

    if (szSrc == NULL) {
        if (szDst != NULL && cbDst > 0) {
            szDst[0] = '\0';
        }
        return 0;
    }
    nSrc = strlen(szSrc);
    if (szDst == NULL || cbDst == 0) {
        return nSrc;
    }
    i = 0;
    while (i + 1 < cbDst && szSrc[i] != '\0') {
        szDst[i] = szSrc[i];
        i++;
    }
    szDst[i] = '\0';
    return nSrc;
}

size_t
strlcat(char *szDst, const char *szSrc, size_t cbDst)
{
    size_t nDst;
    size_t nSrc;
    size_t i;

    if (szSrc == NULL) {
        return (szDst != NULL) ? strnlen(szDst, cbDst) : 0;
    }
    nSrc = strlen(szSrc);
    if (szDst == NULL || cbDst == 0) {
        return nSrc;
    }
    nDst = strnlen(szDst, cbDst);
    if (nDst == cbDst) {
        return cbDst + nSrc;
    }
    i = 0;
    while (nDst + i + 1 < cbDst && szSrc[i] != '\0') {
        szDst[nDst + i] = szSrc[i];
        i++;
    }
    szDst[nDst + i] = '\0';
    return nDst + nSrc;
}

void *
memrchr(const void *p, int c, size_t cb)
{
    const unsigned char *pB;
    unsigned char ch;

    if (p == NULL || cb == 0) {
        return NULL;
    }
    ch = (unsigned char)c;
    pB = (const unsigned char *)p + cb;
    while (cb > 0) {
        cb--;
        pB--;
        if (*pB == ch) {
            return (void *)(uintptr_t)pB;
        }
    }
    return NULL;
}

void
explicit_bzero(void *p, size_t cb)
{
    volatile unsigned char *pV;
    size_t i;

    if (p == NULL || cb == 0) {
        return;
    }
    pV = (volatile unsigned char *)p;
    for (i = 0; i < cb; i++) {
        pV[i] = 0;
    }
}

char *
stpncpy(char *szDst, const char *szSrc, size_t cb)
{
    size_t i = 0;

    if (szDst == NULL) {
        return NULL;
    }
    if (szSrc == NULL) {
        while (i < cb) {
            szDst[i++] = '\0';
        }
        return szDst;
    }
    while (i < cb && szSrc[i] != '\0') {
        szDst[i] = szSrc[i];
        i++;
    }
    if (i < cb) {
        size_t j = i;
        while (j < cb) {
            szDst[j++] = '\0';
        }
        return szDst + i;
    }
    return szDst + cb;
}

char *
strcasestr(const char *szHay, const char *szNeedle)
{
    size_t nN;
    size_t i;

    if (szHay == NULL || szNeedle == NULL) {
        return NULL;
    }
    if (szNeedle[0] == '\0') {
        return (char *)(uintptr_t)szHay;
    }
    nN = strlen(szNeedle);
    for (i = 0; szHay[i] != '\0'; i++) {
        if (strncasecmp(szHay + i, szNeedle, nN) == 0) {
            return (char *)(uintptr_t)(szHay + i);
        }
    }
    return NULL;
}

/* glibc strverscmp: digit runs compare as numbers, else byte order */
int
strverscmp(const char *szA, const char *szB)
{
    size_t i = 0;

    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    for (;;) {
        unsigned char ca = (unsigned char)szA[i];
        unsigned char cb = (unsigned char)szB[i];
        int digA;
        int digB;

        if (ca == 0 && cb == 0) {
            return 0;
        }
        digA = (ca >= '0' && ca <= '9');
        digB = (cb >= '0' && cb <= '9');
        if (digA && digB) {
            /* skip leading zeros in this digit run */
            size_t za = i;
            size_t zb = i;
            size_t ea;
            size_t eb;
            size_t la;
            size_t lb;

            while (szA[za] == '0') {
                za++;
            }
            while (szB[zb] == '0') {
                zb++;
            }
            ea = za;
            eb = zb;
            while (szA[ea] >= '0' && szA[ea] <= '9') {
                ea++;
            }
            while (szB[eb] >= '0' && szB[eb] <= '9') {
                eb++;
            }
            la = ea - za;
            lb = eb - zb;
            if (la != lb) {
                return (la > lb) ? 1 : -1;
            }
            while (za < ea) {
                if (szA[za] != szB[zb]) {
                    return (unsigned char)szA[za] - (unsigned char)szB[zb];
                }
                za++;
                zb++;
            }
            /* Equal numeric value: longer zero-prefix sorts first (glibc). */
            {
                size_t nZa = za - i;
                size_t nZb = zb - i;

                if (nZa != nZb) {
                    return (nZa > nZb) ? -1 : 1;
                }
            }
            i = ea;
            /* Both sides advanced the same digit-run length only if ea-i
             * matched; if one side had more trailing non-digits, continue
             * from the longer end via ea/eb max. */
            if (eb > ea) {
                i = eb;
            }
            continue;
        }
        if (ca != cb) {
            return (int)ca - (int)cb;
        }
        i++;
    }
}
