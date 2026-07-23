/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Freestanding string/memory helpers for libgj userspace.
 * Pure C11. Dual-licensed MIT OR Apache-2.0.
 */
#include <gj/string.h>
#include <gj/syscalls.h>
#include <stdarg.h>
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

size_t
gj_strspn(const char *sz, const char *szAccept)
{
    size_t n = 0;
    size_t i;
    size_t cAccept;

    if (sz == NULL || szAccept == NULL) {
        return 0;
    }
    cAccept = gj_strlen(szAccept);
    while (sz[n] != '\0') {
        int fOk = 0;

        for (i = 0; i < cAccept; i++) {
            if ((unsigned char)sz[n] == (unsigned char)szAccept[i]) {
                fOk = 1;
                break;
            }
        }
        if (fOk == 0) {
            break;
        }
        n++;
    }
    return n;
}

size_t
gj_strcspn(const char *sz, const char *szReject)
{
    size_t n = 0;
    size_t i;
    size_t cReject;

    if (sz == NULL) {
        return 0;
    }
    if (szReject == NULL || szReject[0] == '\0') {
        return gj_strlen(sz);
    }
    cReject = gj_strlen(szReject);
    while (sz[n] != '\0') {
        for (i = 0; i < cReject; i++) {
            if ((unsigned char)sz[n] == (unsigned char)szReject[i]) {
                return n;
            }
        }
        n++;
    }
    return n;
}

int
gj_tolower(int ch)
{
    if (ch >= 'A' && ch <= 'Z') {
        return ch - 'A' + 'a';
    }
    return ch;
}

int
gj_toupper(int ch)
{
    if (ch >= 'a' && ch <= 'z') {
        return ch - 'a' + 'A';
    }
    return ch;
}

int
gj_strcasecmp(const char *szA, const char *szB)
{
    size_t i = 0;
    int ca;
    int cb2;

    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    for (;;) {
        ca = gj_tolower((unsigned char)szA[i]);
        cb2 = gj_tolower((unsigned char)szB[i]);
        if (ca != cb2 || ca == 0) {
            return ca - cb2;
        }
        i++;
    }
}

int
gj_strncasecmp(const char *szA, const char *szB, size_t cb)
{
    size_t i;
    int ca;
    int cb2;

    if (cb == 0) {
        return 0;
    }
    if (szA == NULL || szB == NULL) {
        return (szA == szB) ? 0 : (szA == NULL ? -1 : 1);
    }
    for (i = 0; i < cb; i++) {
        ca = gj_tolower((unsigned char)szA[i]);
        cb2 = gj_tolower((unsigned char)szB[i]);
        if (ca != cb2 || ca == 0) {
            return ca - cb2;
        }
    }
    return 0;
}

int
gj_isdigit(int ch)
{
    return (ch >= '0' && ch <= '9') ? 1 : 0;
}

int
gj_isalpha(int ch)
{
    return ((ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z')) ? 1 : 0;
}

int
gj_isalnum(int ch)
{
    return (gj_isdigit(ch) || gj_isalpha(ch)) ? 1 : 0;
}

int
gj_isspace(int ch)
{
    return (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' ||
            ch == '\f' || ch == '\v')
               ? 1
               : 0;
}

int
gj_isxdigit(int ch)
{
    return (gj_isdigit(ch) || (ch >= 'a' && ch <= 'f') ||
            (ch >= 'A' && ch <= 'F'))
               ? 1
               : 0;
}

int
gj_isprint(int ch)
{
    return (ch >= 0x20 && ch <= 0x7e) ? 1 : 0;
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

void *
gj_memccpy(void *pDst, const void *pSrc, int ch, size_t cb)
{
    unsigned char *d = (unsigned char *)pDst;
    const unsigned char *s = (const unsigned char *)pSrc;
    unsigned char chWant = (unsigned char)ch;
    size_t i;

    if (d == NULL || s == NULL) {
        return NULL;
    }
    for (i = 0; i < cb; i++) {
        d[i] = s[i];
        if (s[i] == chWant) {
            return (void *)(d + i + 1);
        }
    }
    return NULL;
}

/*
 * Digit value for base 2..36. Returns -1 if not a valid digit for nBase.
 */
static int
s_digit_val(int ch, int nBase)
{
    int n;

    if (ch >= '0' && ch <= '9') {
        n = ch - '0';
    } else if (ch >= 'a' && ch <= 'z') {
        n = ch - 'a' + 10;
    } else if (ch >= 'A' && ch <= 'Z') {
        n = ch - 'A' + 10;
    } else {
        return -1;
    }
    if (n >= nBase) {
        return -1;
    }
    return n;
}

unsigned long
gj_strtoul(const char *sz, char **ppEnd, int nBase)
{
    const char *p;
    unsigned long u = 0;
    int nBaseUse;
    int nDig;
    int fAny = 0;

    if (ppEnd != NULL) {
        *ppEnd = (char *)(uintptr_t)sz;
    }
    if (sz == NULL) {
        return 0;
    }
    p = sz;
    while (gj_isspace((unsigned char)*p) != 0) {
        p++;
    }
    if (*p == '+') {
        p++;
    }
    nBaseUse = nBase;
    if (nBaseUse == 0) {
        if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
            nBaseUse = 16;
            p += 2;
        } else if (p[0] == '0') {
            nBaseUse = 8;
            p++;
            fAny = 1; /* lone "0" is a valid convert */
        } else {
            nBaseUse = 10;
        }
    } else if (nBaseUse == 16 && p[0] == '0' &&
               (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }
    if (nBaseUse < 2 || nBaseUse > 36) {
        return 0;
    }
    for (;;) {
        nDig = s_digit_val((unsigned char)*p, nBaseUse);
        if (nDig < 0) {
            break;
        }
        /* Soft: wrap on overflow rather than set errno (no errno). */
        u = u * (unsigned long)nBaseUse + (unsigned long)nDig;
        fAny = 1;
        p++;
    }
    if (ppEnd != NULL) {
        *ppEnd = (char *)(uintptr_t)(fAny != 0 ? p : sz);
    }
    return u;
}

long
gj_strtol(const char *sz, char **ppEnd, int nBase)
{
    const char *p;
    int fNeg = 0;
    unsigned long u;
    char *pEndLocal = NULL;

    if (ppEnd != NULL) {
        *ppEnd = (char *)(uintptr_t)sz;
    }
    if (sz == NULL) {
        return 0;
    }
    p = sz;
    while (gj_isspace((unsigned char)*p) != 0) {
        p++;
    }
    if (*p == '-') {
        fNeg = 1;
        p++;
    } else if (*p == '+') {
        p++;
    }
    u = gj_strtoul(p, &pEndLocal, nBase);
    if (ppEnd != NULL) {
        if (pEndLocal == p && u == 0) {
            *ppEnd = (char *)(uintptr_t)sz;
        } else {
            *ppEnd = pEndLocal;
        }
    }
    if (fNeg != 0) {
        return -(long)u;
    }
    return (long)u;
}

long
gj_atol(const char *sz)
{
    return gj_strtol(sz, NULL, 10);
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
gj_read(int nFd, void *p, size_t cb)
{
    return linux_read(nFd, p, cb);
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

/*
 * Soft snprintf-lite: append one char / string into bounded buffer.
 * *pOut advances; *pNeed counts full logical length; cbCap includes room for NUL.
 */
static void
s_sn_putc(char *szBuf, size_t cbCap, size_t *pOut, size_t *pNeed, char ch)
{
    if (*pOut + 1u < cbCap) {
        szBuf[*pOut] = ch;
    }
    (*pOut)++;
    (*pNeed)++;
}

static void
s_sn_puts(char *szBuf, size_t cbCap, size_t *pOut, size_t *pNeed,
          const char *sz)
{
    size_t i;

    if (sz == NULL) {
        sz = "(null)";
    }
    for (i = 0; sz[i] != '\0'; i++) {
        s_sn_putc(szBuf, cbCap, pOut, pNeed, sz[i]);
    }
}

size_t
gj_snprintf(char *szBuf, size_t cbBuf, const char *szFmt, ...)
{
    va_list ap;
    size_t iOut = 0;
    size_t iNeed = 0;
    size_t i;
    char aNum[32];
    size_t nNum;

    if (szFmt == NULL) {
        if (szBuf != NULL && cbBuf > 0) {
            szBuf[0] = '\0';
        }
        return 0;
    }
    va_start(ap, szFmt);
    for (i = 0; szFmt[i] != '\0'; i++) {
        if (szFmt[i] != '%') {
            s_sn_putc(szBuf, cbBuf, &iOut, &iNeed, szFmt[i]);
            continue;
        }
        i++;
        if (szFmt[i] == '\0') {
            break;
        }
        if (szFmt[i] == '%') {
            s_sn_putc(szBuf, cbBuf, &iOut, &iNeed, '%');
        } else if (szFmt[i] == 's') {
            s_sn_puts(szBuf, cbBuf, &iOut, &iNeed, va_arg(ap, const char *));
        } else if (szFmt[i] == 'c') {
            s_sn_putc(szBuf, cbBuf, &iOut, &iNeed,
                      (char)va_arg(ap, int));
        } else if (szFmt[i] == 'd' || szFmt[i] == 'i') {
            nNum = gj_itoa(va_arg(ap, long), aNum, sizeof(aNum));
            (void)nNum;
            s_sn_puts(szBuf, cbBuf, &iOut, &iNeed, aNum);
        } else if (szFmt[i] == 'u') {
            nNum = gj_utoa(va_arg(ap, unsigned long), aNum, sizeof(aNum));
            (void)nNum;
            s_sn_puts(szBuf, cbBuf, &iOut, &iNeed, aNum);
        } else if (szFmt[i] == 'x' || szFmt[i] == 'X') {
            nNum = gj_xtoa(va_arg(ap, unsigned long), aNum, sizeof(aNum), 0,
                           0);
            (void)nNum;
            s_sn_puts(szBuf, cbBuf, &iOut, &iNeed, aNum);
        } else if (szFmt[i] == 'p') {
            void *p = va_arg(ap, void *);

            s_sn_puts(szBuf, cbBuf, &iOut, &iNeed, "0x");
            nNum = gj_xtoa((unsigned long)(uintptr_t)p, aNum, sizeof(aNum), 1,
                           sizeof(void *) * 2u);
            (void)nNum;
            s_sn_puts(szBuf, cbBuf, &iOut, &iNeed, aNum);
        } else {
            /* Unknown conversion: emit literally. */
            s_sn_putc(szBuf, cbBuf, &iOut, &iNeed, '%');
            s_sn_putc(szBuf, cbBuf, &iOut, &iNeed, szFmt[i]);
        }
    }
    va_end(ap);
    if (szBuf != NULL && cbBuf > 0) {
        if (iOut < cbBuf) {
            szBuf[iOut] = '\0';
        } else {
            szBuf[cbBuf - 1u] = '\0';
        }
    }
    return iNeed;
}
