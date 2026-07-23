/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * iconv — identity and common single-byte conversions (no SSE).
 *
 * greppable: CGJ_ICONV_SOFT_IGNORE
 * greppable: CGJ_ICONV_SOFT_ALIASES
 * greppable: CGJ_ICONV_SOFT_UTF8_VALID
 *
 * Soft deepen: //IGNORE skip bad input, more charset aliases, soft UTF-8
 * structural check on identity UTF-8 paths. Pure C freestanding.
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
    CGJ_IC_UTF8_L1,   /* UTF-8 -> ISO-8859-1 (ASCII subset always ok) */
    CGJ_IC_UTF8_CHECK /* UTF-8 -> UTF-8 with soft structural validation */
};

struct cgj_iconv {
    int nKind;
    int fIgnore; /* greppable: CGJ_ICONV_SOFT_IGNORE */
    int fAsciiOut;
};

static void
cgj_canon_cs(const char *szIn, char *szOut, size_t cbOut, int *pIgnore)
{
    size_t i = 0;
    const char *p;

    if (pIgnore != NULL) {
        *pIgnore = 0;
    }
    if (szIn == NULL || szOut == NULL || cbOut == 0) {
        if (szOut != NULL && cbOut > 0) {
            szOut[0] = '\0';
        }
        return;
    }
    p = szIn;
    while (*p != '\0' && i + 1 < cbOut) {
        if (p[0] == '/' && p[1] == '/') {
            /* greppable: CGJ_ICONV_SOFT_IGNORE */
            if (pIgnore != NULL) {
                const char *q = p + 2;

                while (*q != '\0') {
                    if ((q[0] == 'I' || q[0] == 'i') &&
                        (q[1] == 'G' || q[1] == 'g') &&
                        (q[2] == 'N' || q[2] == 'n') &&
                        (q[3] == 'O' || q[3] == 'o') &&
                        (q[4] == 'R' || q[4] == 'r') &&
                        (q[5] == 'E' || q[5] == 'e')) {
                        *pIgnore = 1;
                        break;
                    }
                    if (q[0] == '/' && q[1] == '/') {
                        q += 2;
                        continue;
                    }
                    q++;
                }
            }
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
    /* greppable: CGJ_ICONV_SOFT_ALIASES */
    if (strcmp(szCanon, "utf8") == 0 || strcmp(szCanon, "utf") == 0 ||
        strcmp(szCanon, "utf8mb4") == 0 || strcmp(szCanon, "unicode") == 0) {
        return 1; /* UTF-8 */
    }
    if (strcmp(szCanon, "ascii") == 0 || strcmp(szCanon, "usascii") == 0 ||
        strcmp(szCanon, "ansi_x3.4") == 0 ||
        strcmp(szCanon, "ansi_x3.41968") == 0 ||
        strcmp(szCanon, "iso646us") == 0) {
        return 2; /* ASCII */
    }
    if (strcmp(szCanon, "iso88591") == 0 || strcmp(szCanon, "latin1") == 0 ||
        strcmp(szCanon, "l1") == 0 || strcmp(szCanon, "ibm819") == 0 ||
        strcmp(szCanon, "cp819") == 0 || strcmp(szCanon, "iso88591:1987") == 0 ||
        strcmp(szCanon, "csisolatin1") == 0 ||
        strcmp(szCanon, "windows1252") == 0 || strcmp(szCanon, "cp1252") == 0) {
        return 3; /* Latin-1 (cp1252 soft-as-L1) */
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
    int fIgnTo = 0;
    int fIgnFrom = 0;
    struct cgj_iconv *pCd;

    cgj_canon_cs(szTo, aTo, sizeof(aTo), &fIgnTo);
    cgj_canon_cs(szFrom, aFrom, sizeof(aFrom), &fIgnFrom);
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
    pCd->fIgnore = (fIgnTo || fIgnFrom) ? 1 : 0;
    pCd->fAsciiOut = (nTo == 2) ? 1 : 0;
    pCd->nKind = CGJ_IC_IDENT;

    if (nFrom == 3 && nTo == 1) {
        pCd->nKind = CGJ_IC_L1_UTF8;
    } else if (nFrom == 1 && nTo == 3) {
        pCd->nKind = CGJ_IC_UTF8_L1;
    } else if (nFrom == 1 && nTo == 2) {
        pCd->nKind = CGJ_IC_UTF8_L1; /* UTF-8 → ASCII: reject >0x7f */
        pCd->fAsciiOut = 1;
    } else if (nFrom == 2 && nTo == 1) {
        pCd->nKind = CGJ_IC_IDENT; /* ASCII ⊂ UTF-8 */
    } else if (nFrom == 2 && nTo == 3) {
        pCd->nKind = CGJ_IC_IDENT;
    } else if (nFrom == 3 && nTo == 2) {
        pCd->nKind = CGJ_IC_IDENT;
        pCd->fAsciiOut = 1;
    } else if (nFrom == 1 && nTo == 1) {
        /* greppable: CGJ_ICONV_SOFT_UTF8_VALID */
        pCd->nKind = CGJ_IC_UTF8_CHECK;
    } else if (nFrom == nTo) {
        pCd->nKind = CGJ_IC_IDENT;
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
            int fAsciiOnly, int fIgnore)
{
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
        unsigned char ch = (unsigned char)(*ppIn)[0];

        if (fAsciiOnly && ch >= 0x80u) {
            if (fIgnore) {
                (*ppIn)++;
                (*pInLeft)--;
                nIrrev++;
                continue;
            }
            errno = EILSEQ;
            return (size_t)-1;
        }
        if (*pOutLeft < 1) {
            errno = E2BIG;
            return (size_t)-1;
        }
        (*ppOut)[0] = (char)ch;
        (*ppOut)++;
        (*pOutLeft)--;
        (*ppIn)++;
        (*pInLeft)--;
    }
    return nIrrev;
}

/* Soft UTF-8 well-formed check + copy (no CESU-8 / overlong). */
static size_t
iconv_utf8_check(char **ppIn, size_t *pInLeft, char **ppOut, size_t *pOutLeft,
                 int fIgnore)
{
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
        unsigned char c0 = (unsigned char)(*ppIn)[0];
        size_t nNeed = 1;
        unsigned uMin = 0;
        size_t i;

        if (c0 < 0x80u) {
            nNeed = 1;
        } else if ((c0 & 0xE0u) == 0xC0u) {
            nNeed = 2;
            uMin = 0x80u;
            if ((c0 & 0xFEu) == 0xC0u) {
                /* overlong 2-byte */
                if (fIgnore) {
                    (*ppIn)++;
                    (*pInLeft)--;
                    nIrrev++;
                    continue;
                }
                errno = EILSEQ;
                return (size_t)-1;
            }
        } else if ((c0 & 0xF0u) == 0xE0u) {
            nNeed = 3;
            uMin = 0x800u;
        } else if ((c0 & 0xF8u) == 0xF0u) {
            nNeed = 4;
            uMin = 0x10000u;
            if (c0 > 0xF4u) {
                if (fIgnore) {
                    (*ppIn)++;
                    (*pInLeft)--;
                    nIrrev++;
                    continue;
                }
                errno = EILSEQ;
                return (size_t)-1;
            }
        } else {
            if (fIgnore) {
                (*ppIn)++;
                (*pInLeft)--;
                nIrrev++;
                continue;
            }
            errno = EILSEQ;
            return (size_t)-1;
        }
        if (*pInLeft < nNeed) {
            errno = EINVAL;
            return (size_t)-1;
        }
        for (i = 1; i < nNeed; i++) {
            unsigned char cx = (unsigned char)(*ppIn)[i];

            if ((cx & 0xC0u) != 0x80u) {
                if (fIgnore) {
                    (*ppIn)++;
                    (*pInLeft)--;
                    nIrrev++;
                    goto next;
                }
                errno = EILSEQ;
                return (size_t)-1;
            }
        }
        if (nNeed >= 2) {
            unsigned u = c0 & ((1u << (7 - nNeed)) - 1u);

            for (i = 1; i < nNeed; i++) {
                u = (u << 6) | ((unsigned)(*ppIn)[i] & 0x3Fu);
            }
            if (u < uMin || (u >= 0xD800u && u <= 0xDFFFu) || u > 0x10FFFFu) {
                if (fIgnore) {
                    (*ppIn)++;
                    (*pInLeft)--;
                    nIrrev++;
                    goto next;
                }
                errno = EILSEQ;
                return (size_t)-1;
            }
        }
        if (*pOutLeft < nNeed) {
            errno = E2BIG;
            return (size_t)-1;
        }
        memcpy(*ppOut, *ppIn, nNeed);
        *ppOut += nNeed;
        *pOutLeft -= nNeed;
        *ppIn += nNeed;
        *pInLeft -= nNeed;
    next:
        (void)0;
    }
    return nIrrev;
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
iconv_utf8_l1(char **ppIn, size_t *pInLeft, char **ppOut, size_t *pOutLeft,
              int fAsciiOnly, int fIgnore)
{
    unsigned char c0;
    unsigned char c1;
    unsigned u;
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
                if (fIgnore) {
                    (*ppIn)++;
                    (*pInLeft)--;
                    nIrrev++;
                    continue;
                }
                errno = EILSEQ;
                return (size_t)-1;
            }
            u = ((unsigned)(c0 & 0x1Fu) << 6) | (unsigned)(c1 & 0x3Fu);
            if (u > 0xFFu || u < 0x80u || (fAsciiOnly && u > 0x7Fu)) {
                if (fIgnore) {
                    *ppIn += 2;
                    *pInLeft -= 2;
                    nIrrev++;
                    continue;
                }
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
        if (fIgnore) {
            (*ppIn)++;
            (*pInLeft)--;
            nIrrev++;
            continue;
        }
        errno = EILSEQ;
        return (size_t)-1;
    }
    return nIrrev;
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
        return iconv_ident(ppIn, pInLeft, ppOut, pOutLeft, pCd->fAsciiOut,
                           pCd->fIgnore);
    case CGJ_IC_UTF8_CHECK:
        return iconv_utf8_check(ppIn, pInLeft, ppOut, pOutLeft, pCd->fIgnore);
    case CGJ_IC_L1_UTF8:
        return iconv_l1_utf8(ppIn, pInLeft, ppOut, pOutLeft);
    case CGJ_IC_UTF8_L1:
        return iconv_utf8_l1(ppIn, pInLeft, ppOut, pOutLeft, pCd->fAsciiOut,
                             pCd->fIgnore);
    default:
        errno = EBADF;
        return (size_t)-1;
    }
}
