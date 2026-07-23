/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * iconv — identity and common single-byte conversions (no SSE).
 */
#include <ctype.h>
#include <errno.h>
#include <iconv.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

/* Conversion kinds stored in opaque iconv_t */
enum {
    CGJ_IC_IDENT = 1, /* byte-for-byte (UTF-8/UTF-8, ASCII/ASCII, ...) */
    CGJ_IC_L1_UTF8,   /* ISO-8859-1 -> UTF-8 */
    CGJ_IC_UTF8_L1    /* UTF-8 -> ISO-8859-1 (ASCII subset always ok) */
};

struct cgj_iconv {
    int nKind;
};

static void
cgj_canon_cs(const char *szIn, char *szOut, size_t cbOut)
{
    size_t i = 0;
    const char *p;

    if (szIn == NULL || szOut == NULL || cbOut == 0) {
        if (szOut != NULL && cbOut > 0) {
            szOut[0] = '\0';
        }
        return;
    }
    p = szIn;
    /* strip trailing //TRANSLIT or //IGNORE */
    while (*p != '\0' && i + 1 < cbOut) {
        if (p[0] == '/' && p[1] == '/') {
            break;
        }
        if (*p == '-' || *p == '_') {
            p++;
            continue;
        }
        szOut[i++] = (char)tolower((unsigned char)*p);
        p++;
    }
    szOut[i] = '\0';
}

static int
cgj_cs_class(const char *szCanon)
{
    if (szCanon == NULL || szCanon[0] == '\0') {
        return 0;
    }
    if (strcmp(szCanon, "utf8") == 0 || strcmp(szCanon, "utf") == 0) {
        return 1; /* UTF-8 */
    }
    if (strcmp(szCanon, "ascii") == 0 || strcmp(szCanon, "usascii") == 0 ||
        strcmp(szCanon, "ansi_x3.4") == 0 ||
        strcmp(szCanon, "ansi_x3.41968") == 0) {
        return 2; /* ASCII */
    }
    if (strcmp(szCanon, "iso88591") == 0 || strcmp(szCanon, "latin1") == 0 ||
        strcmp(szCanon, "l1") == 0 || strcmp(szCanon, "ibm819") == 0 ||
        strcmp(szCanon, "cp819") == 0) {
        return 3; /* Latin-1 */
    }
    if (strcmp(szCanon, "char") == 0) {
        return 1; /* treat as UTF-8 locale codeset on GreenJade */
    }
    return 0;
}

iconv_t
iconv_open(const char *szTo, const char *szFrom)
{
    char aTo[64];
    char aFrom[64];
    int nTo;
    int nFrom;
    struct cgj_iconv *pCd;

    cgj_canon_cs(szTo, aTo, sizeof(aTo));
    cgj_canon_cs(szFrom, aFrom, sizeof(aFrom));
    nTo = cgj_cs_class(aTo);
    nFrom = cgj_cs_class(aFrom);
    if (nTo == 0 || nFrom == 0) {
        errno = EINVAL;
        return (iconv_t)-1;
    }
    pCd = (struct cgj_iconv *)malloc(sizeof(*pCd));
    if (pCd == NULL) {
        errno = ENOMEM;
        return (iconv_t)-1;
    }
    if (nFrom == nTo || (nFrom == 2 && (nTo == 1 || nTo == 3)) ||
        (nTo == 2 && (nFrom == 1 || nFrom == 3))) {
        /* same class, or ASCII subset of UTF-8/Latin1 (checked at convert) */
        if (nFrom == 3 && nTo == 1) {
            pCd->nKind = CGJ_IC_L1_UTF8;
        } else if (nFrom == 1 && nTo == 3) {
            pCd->nKind = CGJ_IC_UTF8_L1;
        } else if (nFrom == 2 || nTo == 2) {
            pCd->nKind = CGJ_IC_IDENT; /* ASCII path validates bytes */
            if (nFrom != 2 && nTo == 2) {
                /* non-ASCII source into ASCII: use UTF8_L1-like strict */
                if (nFrom == 1) {
                    pCd->nKind = CGJ_IC_UTF8_L1;
                } else {
                    pCd->nKind = CGJ_IC_IDENT;
                }
            }
        } else {
            pCd->nKind = CGJ_IC_IDENT;
        }
    } else if (nFrom == 3 && nTo == 1) {
        pCd->nKind = CGJ_IC_L1_UTF8;
    } else if (nFrom == 1 && nTo == 3) {
        pCd->nKind = CGJ_IC_UTF8_L1;
    } else {
        free(pCd);
        errno = EINVAL;
        return (iconv_t)-1;
    }
    return (iconv_t)pCd;
}

int
iconv_close(iconv_t cd)
{
    if (cd == (iconv_t)-1 || cd == NULL) {
        errno = EBADF;
        return -1;
    }
    free(cd);
    return 0;
}

static size_t
iconv_ident(char **ppIn, size_t *pInLeft, char **ppOut, size_t *pOutLeft,
            int fAsciiOnly)
{
    size_t nCopy;
    size_t i;
    unsigned char ch;

    if (ppIn == NULL || *ppIn == NULL) {
        /* reset state — no shift state */
        return 0;
    }
    if (ppOut == NULL || *ppOut == NULL || pInLeft == NULL ||
        pOutLeft == NULL) {
        errno = EINVAL;
        return (size_t)-1;
    }
    nCopy = *pInLeft;
    if (nCopy > *pOutLeft) {
        nCopy = *pOutLeft;
    }
    if (fAsciiOnly) {
        for (i = 0; i < nCopy; i++) {
            ch = (unsigned char)(*ppIn)[i];
            if (ch >= 0x80u) {
                if (i > 0) {
                    memcpy(*ppOut, *ppIn, i);
                    *ppIn += i;
                    *ppOut += i;
                    *pInLeft -= i;
                    *pOutLeft -= i;
                }
                errno = EILSEQ;
                return (size_t)-1;
            }
        }
    }
    if (nCopy > 0) {
        memcpy(*ppOut, *ppIn, nCopy);
        *ppIn += nCopy;
        *ppOut += nCopy;
        *pInLeft -= nCopy;
        *pOutLeft -= nCopy;
    }
    if (*pInLeft > 0 && *pOutLeft == 0) {
        errno = E2BIG;
        return (size_t)-1;
    }
    return 0;
}

static size_t
iconv_l1_utf8(char **ppIn, size_t *pInLeft, char **ppOut, size_t *pOutLeft)
{
    unsigned char ch;
    size_t nIrrev = 0;

    if (ppIn == NULL || *ppIn == NULL) {
        return 0;
    }
    if (ppOut == NULL || *ppOut == NULL || pInLeft == NULL ||
        pOutLeft == NULL) {
        errno = EINVAL;
        return (size_t)-1;
    }
    while (*pInLeft > 0) {
        ch = (unsigned char)(*ppIn)[0];
        if (ch < 0x80u) {
            if (*pOutLeft < 1) {
                errno = E2BIG;
                return (size_t)-1;
            }
            (*ppOut)[0] = (char)ch;
            (*ppOut)++;
            (*pOutLeft)--;
        } else {
            if (*pOutLeft < 2) {
                errno = E2BIG;
                return (size_t)-1;
            }
            (*ppOut)[0] = (char)(0xC0u | (ch >> 6));
            (*ppOut)[1] = (char)(0x80u | (ch & 0x3Fu));
            *ppOut += 2;
            *pOutLeft -= 2;
            nIrrev++;
        }
        (*ppIn)++;
        (*pInLeft)--;
    }
    return nIrrev;
}

static size_t
iconv_utf8_l1(char **ppIn, size_t *pInLeft, char **ppOut, size_t *pOutLeft)
{
    unsigned char c0;
    unsigned char c1;
    unsigned u;

    if (ppIn == NULL || *ppIn == NULL) {
        return 0;
    }
    if (ppOut == NULL || *ppOut == NULL || pInLeft == NULL ||
        pOutLeft == NULL) {
        errno = EINVAL;
        return (size_t)-1;
    }
    while (*pInLeft > 0) {
        c0 = (unsigned char)(*ppIn)[0];
        if (c0 < 0x80u) {
            if (*pOutLeft < 1) {
                errno = E2BIG;
                return (size_t)-1;
            }
            (*ppOut)[0] = (char)c0;
            (*ppOut)++;
            (*pOutLeft)--;
            (*ppIn)++;
            (*pInLeft)--;
            continue;
        }
        if ((c0 & 0xE0u) == 0xC0u) {
            if (*pInLeft < 2) {
                errno = EINVAL;
                return (size_t)-1;
            }
            c1 = (unsigned char)(*ppIn)[1];
            if ((c1 & 0xC0u) != 0x80u) {
                errno = EILSEQ;
                return (size_t)-1;
            }
            u = ((unsigned)(c0 & 0x1Fu) << 6) | (unsigned)(c1 & 0x3Fu);
            if (u > 0xFFu || u < 0x80u) {
                /* outside Latin-1 or overlong */
                errno = EILSEQ;
                return (size_t)-1;
            }
            if (*pOutLeft < 1) {
                errno = E2BIG;
                return (size_t)-1;
            }
            (*ppOut)[0] = (char)u;
            (*ppOut)++;
            (*pOutLeft)--;
            *ppIn += 2;
            *pInLeft -= 2;
            continue;
        }
        errno = EILSEQ;
        return (size_t)-1;
    }
    return 0;
}

size_t
iconv(iconv_t cd, char **ppIn, size_t *pInLeft, char **ppOut, size_t *pOutLeft)
{
    struct cgj_iconv *pCd = (struct cgj_iconv *)cd;

    if (cd == (iconv_t)-1 || pCd == NULL) {
        errno = EBADF;
        return (size_t)-1;
    }
    switch (pCd->nKind) {
    case CGJ_IC_IDENT:
        return iconv_ident(ppIn, pInLeft, ppOut, pOutLeft, 0);
    case CGJ_IC_L1_UTF8:
        return iconv_l1_utf8(ppIn, pInLeft, ppOut, pOutLeft);
    case CGJ_IC_UTF8_L1:
        return iconv_utf8_l1(ppIn, pInLeft, ppOut, pOutLeft);
    default:
        errno = EBADF;
        return (size_t)-1;
    }
}
