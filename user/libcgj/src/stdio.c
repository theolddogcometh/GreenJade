/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Buffered printf-family + FILE (glibc-shaped). Not GNU glibc.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <wchar.h>

#define F_READ       1
#define F_WRITE      2
#define F_OWNBUF     4
#define F_MEMSTREAM  8
#define F_WMEMSTREAM 16
#define F_COOKIE     32

static FILE g_stdin;
static FILE g_stdout;
static FILE g_stderr;

FILE *stdin = &g_stdin;
FILE *stdout = &g_stdout;
FILE *stderr = &g_stderr;

static void
file_init_std(FILE *pF, int nFd, int nFlags, int nMode)
{
    pF->nFd = nFd;
    pF->nFlags = nFlags;
    pF->nErr = 0;
    pF->nEof = 0;
    pF->nBufMode = nMode;
    pF->nUnget = -1;
    pF->nWide = 0;
    pF->cbBuf = sizeof(pF->aSmall);
    pF->cbWpos = 0;
    pF->cbRlen = 0;
    pF->cbRpos = 0;
    pF->pBuf = pF->aSmall;
    pF->ppMemUser = NULL;
    pF->ppWMemUser = NULL;
    pF->pcbMemUser = NULL;
    pF->pMemData = NULL;
    pF->cbMemCap = 0;
    pF->cbMemLen = 0;
    pF->offMem = 0;
    pF->pCookie = NULL;
    pF->pfnCkRead = NULL;
    pF->pfnCkWrite = NULL;
    pF->pfnCkSeek = NULL;
    pF->pfnCkClose = NULL;
}

static int
memstream_publish(FILE *pF)
{
    if (pF->pcbMemUser == NULL) {
        return 0;
    }
    if ((pF->nFlags & F_WMEMSTREAM) != 0) {
        if (pF->ppWMemUser != NULL) {
            *pF->ppWMemUser = (void *)pF->pMemData;
        }
        *pF->pcbMemUser = pF->cbMemLen; /* wchar count */
    } else if (pF->ppMemUser != NULL) {
        *pF->ppMemUser = pF->pMemData;
        *pF->pcbMemUser = pF->cbMemLen;
    }
    return 0;
}

static int
memstream_grow(FILE *pF, size_t cbNeed)
{
    size_t cbNew;
    char *pNew;

    if (cbNeed <= pF->cbMemCap) {
        return 0;
    }
    cbNew = pF->cbMemCap ? pF->cbMemCap * 2 : 64;
    while (cbNew < cbNeed) {
        cbNew *= 2;
    }
    pNew = (char *)realloc(pF->pMemData, cbNew);
    if (pNew == NULL) {
        pF->nErr = 1;
        return -1;
    }
    pF->pMemData = pNew;
    pF->cbMemCap = cbNew;
    (void)memstream_publish(pF);
    return 0;
}

/* Commit pending write buffer into growable memstream storage. */
static int
memstream_flush(FILE *pF)
{
    size_t cbNeed;
    size_t i;

    if (pF->cbWpos == 0) {
        (void)memstream_publish(pF);
        return 0;
    }
    if ((pF->nFlags & F_WMEMSTREAM) != 0) {
        /* Wide: write buffer holds packed wchar_t units */
        size_t cW = pF->cbWpos / sizeof(int); /* wchar_t == int */
        size_t cbUnit = sizeof(int);
        int *pW;

        if (pF->cbWpos % cbUnit != 0) {
            pF->nErr = 1;
            return -1;
        }
        cbNeed = ((size_t)pF->offMem + cW + 1) * cbUnit;
        if (memstream_grow(pF, cbNeed) != 0) {
            return -1;
        }
        pW = (int *)(void *)pF->pMemData;
        if ((size_t)pF->offMem > pF->cbMemLen) {
            size_t z;

            for (z = pF->cbMemLen; z < (size_t)pF->offMem; z++) {
                pW[z] = 0;
            }
        }
        for (i = 0; i < cW; i++) {
            int wc;

            memcpy(&wc, pF->pBuf + i * cbUnit, cbUnit);
            pW[(size_t)pF->offMem + i] = wc;
        }
        pF->offMem += (off_t)cW;
        if ((size_t)pF->offMem > pF->cbMemLen) {
            pF->cbMemLen = (size_t)pF->offMem;
        }
        pW[pF->cbMemLen] = 0;
        pF->cbWpos = 0;
        return memstream_publish(pF);
    }
    cbNeed = (size_t)pF->offMem + pF->cbWpos + 1; /* +NUL */
    if (memstream_grow(pF, cbNeed) != 0) {
        return -1;
    }
    /* Zero-fill holes if seek past end */
    if ((size_t)pF->offMem > pF->cbMemLen) {
        size_t z;

        for (z = pF->cbMemLen; z < (size_t)pF->offMem; z++) {
            pF->pMemData[z] = '\0';
        }
    }
    for (i = 0; i < pF->cbWpos; i++) {
        pF->pMemData[(size_t)pF->offMem + i] = (char)pF->pBuf[i];
    }
    pF->offMem += (off_t)pF->cbWpos;
    if ((size_t)pF->offMem > pF->cbMemLen) {
        pF->cbMemLen = (size_t)pF->offMem;
    }
    pF->pMemData[pF->cbMemLen] = '\0';
    pF->cbWpos = 0;
    return memstream_publish(pF);
}

/* Called from libcgj_init / first use */
void
_libcgj_stdio_init(void)
{
    static int fDone;

    if (fDone) {
        return;
    }
    fDone = 1;
    file_init_std(&g_stdin, 0, F_READ, _IOLBF);
    file_init_std(&g_stdout, 1, F_WRITE, _IOLBF);
    file_init_std(&g_stderr, 2, F_WRITE, _IONBF);
}

static void
ensure_stdio(void)
{
    if (g_stdin.pBuf == NULL) {
        _libcgj_stdio_init();
    }
}

static int
flush_write(FILE *pF)
{
    size_t off = 0;

    if (pF == NULL) {
        return 0;
    }
    if ((pF->nFlags & F_MEMSTREAM) != 0) {
        return memstream_flush(pF);
    }
    if (pF->cbWpos == 0) {
        return 0;
    }
    while (off < pF->cbWpos) {
        ssize_t w;

        if ((pF->nFlags & F_COOKIE) != 0 && pF->pfnCkWrite != NULL) {
            w = pF->pfnCkWrite(pF->pCookie, (const char *)(pF->pBuf + off),
                               pF->cbWpos - off);
        } else {
            w = write(pF->nFd, pF->pBuf + off, pF->cbWpos - off);
        }
        if (w < 0) {
            pF->nErr = 1;
            return -1;
        }
        if (w == 0) {
            break;
        }
        off += (size_t)w;
    }
    pF->cbWpos = 0;
    return 0;
}

int
fflush(FILE *pF)
{
    ensure_stdio();
    if (pF == NULL) {
        (void)flush_write(stdout);
        (void)flush_write(stderr);
        return 0;
    }
    if (pF->nFlags & F_WRITE) {
        return flush_write(pF);
    }
    /* drop read buffer on flush */
    pF->cbRlen = 0;
    pF->cbRpos = 0;
    return 0;
}

int
setvbuf(FILE *pF, char *pBuf, int nMode, size_t cb)
{
    ensure_stdio();
    if (pF == NULL) {
        return -1;
    }
    (void)fflush(pF);
    if (pF->nFlags & F_OWNBUF) {
        free(pF->pBuf);
        pF->nFlags &= ~F_OWNBUF;
    }
    pF->nBufMode = nMode;
    pF->cbWpos = 0;
    pF->cbRlen = 0;
    pF->cbRpos = 0;
    if (nMode == _IONBF) {
        pF->pBuf = pF->aSmall;
        pF->cbBuf = 1;
        return 0;
    }
    if (pBuf != NULL && cb > 0) {
        pF->pBuf = (unsigned char *)pBuf;
        pF->cbBuf = cb;
        return 0;
    }
    if (cb == 0) {
        cb = BUFSIZ;
    }
    {
        unsigned char *p = (unsigned char *)malloc(cb);

        if (p == NULL) {
            pF->pBuf = pF->aSmall;
            pF->cbBuf = sizeof(pF->aSmall);
            return 0;
        }
        pF->pBuf = p;
        pF->cbBuf = cb;
        pF->nFlags |= F_OWNBUF;
    }
    return 0;
}

void
setbuf(FILE *pF, char *pBuf)
{
    if (pBuf == NULL) {
        (void)setvbuf(pF, NULL, _IONBF, 0);
    } else {
        (void)setvbuf(pF, pBuf, _IOFBF, BUFSIZ);
    }
}

void
setbuffer(FILE *pF, char *pBuf, size_t cb)
{
    if (pBuf == NULL) {
        (void)setvbuf(pF, NULL, _IONBF, 0);
    } else {
        (void)setvbuf(pF, pBuf, _IOFBF, cb);
    }
}

void
setlinebuf(FILE *pF)
{
    (void)setvbuf(pF, NULL, _IOLBF, 0);
}

static int
putc_buf(FILE *pF, unsigned char c)
{
    if (pF->nBufMode == _IONBF) {
        if ((pF->nFlags & F_MEMSTREAM) != 0) {
            pF->pBuf[0] = c;
            pF->cbWpos = 1;
            return flush_write(pF);
        } else if ((pF->nFlags & F_COOKIE) != 0 && pF->pfnCkWrite != NULL) {
            ssize_t w = pF->pfnCkWrite(pF->pCookie, (const char *)&c, 1);

            if (w != 1) {
                pF->nErr = 1;
                return -1;
            }
            return 0;
        } else {
            ssize_t w = write(pF->nFd, &c, 1);

            if (w != 1) {
                pF->nErr = 1;
                return -1;
            }
            return 0;
        }
    }
    if (pF->cbWpos >= pF->cbBuf) {
        if (flush_write(pF) != 0) {
            return -1;
        }
    }
    pF->pBuf[pF->cbWpos++] = c;
    if (pF->nBufMode == _IOLBF && c == '\n') {
        return flush_write(pF);
    }
    if (pF->cbWpos >= pF->cbBuf) {
        return flush_write(pF);
    }
    return 0;
}

static void
out_ch(FILE *pF, char **ppBuf, size_t *pLeft, size_t *pWrote, char c)
{
    if (ppBuf != NULL) {
        /* Buffer or sizing mode (ppBuf set; *ppBuf may be NULL to count only) */
        if (*ppBuf != NULL && pLeft != NULL && *pLeft > 1) {
            **ppBuf = c;
            (*ppBuf)++;
            (*pLeft)--;
        }
        (*pWrote)++;
        return;
    }
    if (pF != NULL) {
        if (putc_buf(pF, (unsigned char)c) == 0) {
            (*pWrote)++;
        }
    }
}

static void
out_nstr(FILE *pF, char **ppBuf, size_t *pLeft, size_t *pWrote,
         const char *sz, int nLen)
{
    int i;

    if (sz == NULL || nLen <= 0) {
        return;
    }
    for (i = 0; i < nLen; i++) {
        out_ch(pF, ppBuf, pLeft, pWrote, sz[i]);
    }
}

static void
out_fill(FILE *pF, char **ppBuf, size_t *pLeft, size_t *pWrote, char ch, int n)
{
    int i;

    for (i = 0; i < n; i++) {
        out_ch(pF, ppBuf, pLeft, pWrote, ch);
    }
}

static void
fmt_ull(char *szDig, int *pN, unsigned long long u, int nBase, int fUpper)
{
    const char *szMap = fUpper ? "0123456789ABCDEF" : "0123456789abcdef";
    char aTmp[72];
    int n = 0;
    int i;

    if (nBase < 2 || nBase > 16) {
        nBase = 10;
    }
    if (u == 0) {
        szDig[0] = '0';
        *pN = 1;
        return;
    }
    while (u > 0 && n < (int)sizeof(aTmp)) {
        aTmp[n++] = szMap[u % (unsigned)nBase];
        u /= (unsigned)nBase;
    }
    for (i = 0; i < n; i++) {
        szDig[i] = aTmp[n - 1 - i];
    }
    *pN = n;
}

static void
out_field(FILE *pF, char **ppBuf, size_t *pLeft, size_t *pWrote,
          const char *szPre, const char *szDig, int nDig, int nWidth,
          int fLeft, int fZero)
{
    int nPre = 0;
    int nPad;
    int nBody;

    if (szPre != NULL) {
        while (szPre[nPre] != '\0') {
            nPre++;
        }
    }
    nBody = nPre + nDig;
    nPad = (nWidth > nBody) ? nWidth - nBody : 0;
    if (fLeft) {
        out_nstr(pF, ppBuf, pLeft, pWrote, szPre, nPre);
        out_nstr(pF, ppBuf, pLeft, pWrote, szDig, nDig);
        out_fill(pF, ppBuf, pLeft, pWrote, ' ', nPad);
    } else if (fZero) {
        out_nstr(pF, ppBuf, pLeft, pWrote, szPre, nPre);
        out_fill(pF, ppBuf, pLeft, pWrote, '0', nPad);
        out_nstr(pF, ppBuf, pLeft, pWrote, szDig, nDig);
    } else {
        out_fill(pF, ppBuf, pLeft, pWrote, ' ', nPad);
        out_nstr(pF, ppBuf, pLeft, pWrote, szPre, nPre);
        out_nstr(pF, ppBuf, pLeft, pWrote, szDig, nDig);
    }
}

/*
 * Fixed-point / scientific double for OpenSSH %f %e %g (fingerprints use
 * integers; servconf debug uses %f). Not a full dtoa.
 */
static int
fmt_double(char *szOut, size_t cbOut, double dVal, int nPrec, int fUpper,
           char chConv)
{
    union {
        double d;
        uint64_t u64;
    } ub;
    char aInt[32];
    char aFrac[32];
    int nInt = 0;
    int nExp = 0;
    int fNeg;
    int nExpF = 0;
    int i;
    int nPrecUse;
    int fSci = 0;
    int fStrip = 0;
    uint64_t uInt;
    uint64_t uFrac = 0;
    uint64_t uScale = 1;
    double dAbs;
    double dFrac;
    size_t o = 0;

    if (szOut == NULL || cbOut < 8) {
        return -1;
    }
    nPrecUse = (nPrec < 0) ? 6 : nPrec;
    if (nPrecUse > 18) {
        nPrecUse = 18;
    }
    ub.d = dVal;
    fNeg = (int)(ub.u64 >> 63);
    if (((ub.u64 >> 52) & 0x7ffULL) == 0x7ffULL) {
        const char *sz;
        int n;

        if ((ub.u64 & 0x000fffffffffffffULL) != 0ULL) {
            sz = fUpper ? "NAN" : "nan";
        } else {
            sz = fUpper ? "INF" : "inf";
        }
        if (fNeg && o + 1 < cbOut) {
            szOut[o++] = '-';
        }
        for (n = 0; sz[n] != '\0' && o + 1 < cbOut; n++) {
            szOut[o++] = sz[n];
        }
        szOut[o] = '\0';
        return (int)o;
    }
    dAbs = fNeg ? -dVal : dVal;
    if (chConv == 'g' || chConv == 'G') {
        fStrip = 1;
        if (nPrec < 0) {
            nPrecUse = 6;
        }
        if (nPrecUse < 1) {
            nPrecUse = 1;
        }
        if (dAbs != 0.0) {
            double dT = dAbs;

            nExpF = 0;
            while (dT >= 10.0 && nExpF < 400) {
                dT *= 0.1;
                nExpF++;
            }
            while (dT < 1.0 && dT > 0.0 && nExpF > -400) {
                dT *= 10.0;
                nExpF--;
            }
            if (nExpF < -4 || nExpF >= nPrecUse) {
                fSci = 1;
            }
        }
    } else if (chConv == 'e' || chConv == 'E') {
        fSci = 1;
    }
    if (fSci && dAbs != 0.0) {
        while (dAbs >= 10.0 && nExp < 400) {
            dAbs *= 0.1;
            nExp++;
        }
        while (dAbs < 1.0 && nExp > -400) {
            dAbs *= 10.0;
            nExp--;
        }
    }
    if (dAbs >= 1.0e18) {
        dAbs = 1.0e18;
    }
    uInt = (uint64_t)dAbs;
    dFrac = dAbs - (double)uInt;
    for (i = 0; i < nPrecUse; i++) {
        uScale *= 10ULL;
    }
    if (nPrecUse > 0) {
        uFrac = (uint64_t)(dFrac * (double)uScale + 0.5);
        if (uFrac >= uScale) {
            uInt += 1ULL;
            uFrac -= uScale;
        }
    } else if (dFrac >= 0.5) {
        uInt += 1ULL;
    }
    fmt_ull(aInt, &nInt, uInt, 10, 0);
    if (nPrecUse > 0) {
        char aTmp[24];
        int nF = 0;
        uint64_t uF = uFrac;

        if (uF == 0) {
            for (i = 0; i < nPrecUse; i++) {
                aFrac[i] = '0';
            }
        } else {
            while (uF > 0 && nF < 24) {
                aTmp[nF++] = (char)('0' + (uF % 10ULL));
                uF /= 10ULL;
            }
            while (nF < nPrecUse) {
                aTmp[nF++] = '0';
            }
            for (i = 0; i < nPrecUse; i++) {
                aFrac[i] = aTmp[nPrecUse - 1 - i];
            }
        }
        if (fStrip) {
            while (nPrecUse > 0 && aFrac[nPrecUse - 1] == '0') {
                nPrecUse--;
            }
        }
    }
    if (fNeg && o + 1 < cbOut) {
        szOut[o++] = '-';
    }
    for (i = 0; i < nInt && o + 1 < cbOut; i++) {
        szOut[o++] = aInt[i];
    }
    if (nPrecUse > 0 && o + 1 < cbOut) {
        szOut[o++] = '.';
        for (i = 0; i < nPrecUse && o + 1 < cbOut; i++) {
            szOut[o++] = aFrac[i];
        }
    }
    if (fSci) {
        char aE[8];
        int nE = 0;
        int nAe = (nExp < 0) ? -nExp : nExp;

        if (o + 1 < cbOut) {
            szOut[o++] = fUpper ? 'E' : 'e';
        }
        if (o + 1 < cbOut) {
            szOut[o++] = (nExp < 0) ? '-' : '+';
        }
        if (nAe < 10 && o + 1 < cbOut) {
            szOut[o++] = '0';
        }
        fmt_ull(aE, &nE, (unsigned long long)nAe, 10, 0);
        for (i = 0; i < nE && o + 1 < cbOut; i++) {
            szOut[o++] = aE[i];
        }
    }
    szOut[o] = '\0';
    return (int)o;
}

static size_t
vformat(FILE *pF, char *szBuf, size_t cb, const char *szFmt, va_list ap)
{
    size_t wrote = 0;
    size_t left = cb;
    char *pB = szBuf;
    char **ppB = (pF == NULL) ? &pB : NULL;
    const char *p;

    if (szFmt == NULL) {
        return 0;
    }
    /* Consume the conversion char; continue must not skip the next byte. */
    for (p = szFmt; *p != '\0'; ) {
        int fLeft = 0;
        int fPlus = 0;
        int fSpace = 0;
        int fHash = 0;
        int fZero = 0;
        int nWidth = 0;
        int nPrec = -1;
        int nLenMod = 0;
        char chConv;
        char aDig[96];
        char aPre[8];
        int nDig = 0;
        int nPre = 0;

        if (*p != '%') {
            out_ch(pF, ppB, &left, &wrote, *p);
            p++;
            continue;
        }
        p++;
        if (*p == '\0') {
            break;
        }
        /* flags */
        for (;;) {
            if (*p == '-') {
                fLeft = 1;
            } else if (*p == '+') {
                fPlus = 1;
            } else if (*p == ' ') {
                fSpace = 1;
            } else if (*p == '#') {
                fHash = 1;
            } else if (*p == '0') {
                fZero = 1;
            } else {
                break;
            }
            p++;
        }
        if (*p == '*') {
            nWidth = va_arg(ap, int);
            p++;
            if (nWidth < 0) {
                fLeft = 1;
                nWidth = -nWidth;
            }
        } else {
            while (*p >= '0' && *p <= '9') {
                nWidth = nWidth * 10 + (*p - '0');
                p++;
            }
        }
        if (*p == '.') {
            p++;
            nPrec = 0;
            if (*p == '*') {
                nPrec = va_arg(ap, int);
                p++;
                if (nPrec < 0) {
                    nPrec = -1;
                }
            } else {
                while (*p >= '0' && *p <= '9') {
                    nPrec = nPrec * 10 + (*p - '0');
                    p++;
                }
            }
        }
        if (*p == 'h') {
            p++;
            if (*p == 'h') {
                nLenMod = -2;
                p++;
            } else {
                nLenMod = -1;
            }
        } else if (*p == 'l') {
            p++;
            if (*p == 'l') {
                nLenMod = 2;
                p++;
            } else {
                nLenMod = 1;
            }
        } else if (*p == 'z' || *p == 't' || *p == 'j') {
            nLenMod = 3;
            p++;
        }
        if (*p == '\0') {
            break;
        }
        chConv = *p;
        p++;
        if (fLeft) {
            fZero = 0;
        }
        if (chConv == '%') {
            out_ch(pF, ppB, &left, &wrote, '%');
            continue;
        }
        if (chConv == 'n') {
            if (nLenMod == 2) {
                *va_arg(ap, long long *) = (long long)wrote;
            } else if (nLenMod == 1) {
                *va_arg(ap, long *) = (long)wrote;
            } else {
                *va_arg(ap, int *) = (int)wrote;
            }
            continue;
        }
        if (chConv == 's') {
            const char *sz = va_arg(ap, const char *);
            int nLen;
            int nPad;
            int i;

            if (sz == NULL) {
                sz = "(null)";
            }
            nLen = 0;
            /* nPrec is a max; stop even without a NUL. */
            if (nPrec >= 0) {
                while (nLen < nPrec && sz[nLen] != '\0') {
                    nLen++;
                }
            } else {
                while (sz[nLen] != '\0') {
                    nLen++;
                }
            }
            nPad = (nWidth > nLen) ? nWidth - nLen : 0;
            if (!fLeft) {
                out_fill(pF, ppB, &left, &wrote, ' ', nPad);
            }
            for (i = 0; i < nLen; i++) {
                out_ch(pF, ppB, &left, &wrote, sz[i]);
            }
            if (fLeft) {
                out_fill(pF, ppB, &left, &wrote, ' ', nPad);
            }
            continue;
        }
        if (chConv == 'c') {
            char ch = (char)va_arg(ap, int);
            int nPad = (nWidth > 1) ? nWidth - 1 : 0;

            if (!fLeft) {
                out_fill(pF, ppB, &left, &wrote, ' ', nPad);
            }
            out_ch(pF, ppB, &left, &wrote, ch);
            if (fLeft) {
                out_fill(pF, ppB, &left, &wrote, ' ', nPad);
            }
            continue;
        }
        if (chConv == 'p') {
            unsigned long long uP;

            uP = (unsigned long long)(uintptr_t)va_arg(ap, void *);
            aPre[0] = '0';
            aPre[1] = 'x';
            aPre[2] = '\0';
            nPre = 2;
            fmt_ull(aDig, &nDig, uP, 16, 0);
            if (nPrec == 0 && uP == 0ULL) {
                nDig = 0;
            }
            while (nPrec > nDig && nDig < (int)sizeof(aDig) - 1) {
                int i;

                for (i = nDig; i > 0; i--) {
                    aDig[i] = aDig[i - 1];
                }
                aDig[0] = '0';
                nDig++;
            }
            out_field(pF, ppB, &left, &wrote, aPre, aDig, nDig, nWidth, fLeft,
                      fZero && nPrec < 0);
            continue;
        }
        if (chConv == 'd' || chConv == 'i') {
            long long nVal;
            unsigned long long u;
            int fNeg = 0;

            if (nLenMod == 2) {
                nVal = va_arg(ap, long long);
            } else if (nLenMod == 1) {
                nVal = va_arg(ap, long);
            } else if (nLenMod == 3) {
                nVal = (long long)va_arg(ap, ptrdiff_t);
            } else {
                nVal = va_arg(ap, int);
            }
            if (nVal < 0) {
                fNeg = 1;
                u = (unsigned long long)(-(nVal + 1)) + 1ull;
            } else {
                u = (unsigned long long)nVal;
            }
            fmt_ull(aDig, &nDig, u, 10, 0);
            if (nPrec == 0 && u == 0ULL) {
                nDig = 0;
            }
            while (nPrec > nDig && nDig < (int)sizeof(aDig) - 1) {
                int i;

                for (i = nDig; i > 0; i--) {
                    aDig[i] = aDig[i - 1];
                }
                aDig[0] = '0';
                nDig++;
            }
            if (fNeg) {
                aPre[nPre++] = '-';
            } else if (fPlus) {
                aPre[nPre++] = '+';
            } else if (fSpace) {
                aPre[nPre++] = ' ';
            }
            aPre[nPre] = '\0';
            out_field(pF, ppB, &left, &wrote, aPre, aDig, nDig, nWidth, fLeft,
                      fZero && nPrec < 0);
            continue;
        }
        if (chConv == 'u' || chConv == 'x' || chConv == 'X' || chConv == 'o') {
            unsigned long long u;
            int nBase = 10;
            int fUpper = (chConv == 'X');

            if (chConv == 'x' || chConv == 'X') {
                nBase = 16;
            } else if (chConv == 'o') {
                nBase = 8;
            }
            if (nLenMod == 2) {
                u = va_arg(ap, unsigned long long);
            } else if (nLenMod == 1) {
                u = va_arg(ap, unsigned long);
            } else if (nLenMod == 3) {
                u = va_arg(ap, size_t);
            } else {
                u = va_arg(ap, unsigned);
            }
            fmt_ull(aDig, &nDig, u, nBase, fUpper);
            if (nPrec == 0 && u == 0ULL) {
                nDig = 0;
            }
            while (nPrec > nDig && nDig < (int)sizeof(aDig) - 1) {
                int i;

                for (i = nDig; i > 0; i--) {
                    aDig[i] = aDig[i - 1];
                }
                aDig[0] = '0';
                nDig++;
            }
            if (fHash && u != 0ULL) {
                if (nBase == 16) {
                    aPre[nPre++] = '0';
                    aPre[nPre++] = fUpper ? 'X' : 'x';
                } else if (nBase == 8 && (nDig == 0 || aDig[0] != '0')) {
                    aPre[nPre++] = '0';
                }
            }
            aPre[nPre] = '\0';
            out_field(pF, ppB, &left, &wrote, aPre, aDig, nDig, nWidth, fLeft,
                      fZero && nPrec < 0);
            continue;
        }
        if (chConv == 'f' || chConv == 'F' || chConv == 'e' || chConv == 'E' ||
            chConv == 'g' || chConv == 'G') {
            double dVal;
            char aNum[128];
            int nNum;
            int fUpper = (chConv == 'F' || chConv == 'E' || chConv == 'G');
            const char *szBody;
            int nBody;
            int nPad;
            int fNegNum = 0;

            if (nLenMod == 2) {
                dVal = (double)va_arg(ap, long double);
            } else {
                dVal = va_arg(ap, double);
            }
            nNum = fmt_double(aNum, sizeof(aNum), dVal, nPrec, fUpper, chConv);
            if (nNum < 0) {
                continue;
            }
            szBody = aNum;
            nBody = nNum;
            if (nBody > 0 && aNum[0] == '-') {
                fNegNum = 1;
                szBody = aNum + 1;
                nBody--;
            }
            if (fNegNum) {
                aPre[nPre++] = '-';
            } else if (fPlus) {
                aPre[nPre++] = '+';
            } else if (fSpace) {
                aPre[nPre++] = ' ';
            }
            aPre[nPre] = '\0';
            nPad = nWidth - (nPre + nBody);
            if (nPad < 0) {
                nPad = 0;
            }
            if (fLeft) {
                out_nstr(pF, ppB, &left, &wrote, aPre, nPre);
                out_nstr(pF, ppB, &left, &wrote, szBody, nBody);
                out_fill(pF, ppB, &left, &wrote, ' ', nPad);
            } else if (fZero && aNum[0] != 'n' && aNum[0] != 'N' &&
                       aNum[0] != 'i' && aNum[0] != 'I' &&
                       !(fNegNum && (aNum[1] == 'n' || aNum[1] == 'N' ||
                                     aNum[1] == 'i' || aNum[1] == 'I'))) {
                out_nstr(pF, ppB, &left, &wrote, aPre, nPre);
                out_fill(pF, ppB, &left, &wrote, '0', nPad);
                out_nstr(pF, ppB, &left, &wrote, szBody, nBody);
            } else {
                out_fill(pF, ppB, &left, &wrote, ' ', nPad);
                out_nstr(pF, ppB, &left, &wrote, aPre, nPre);
                out_nstr(pF, ppB, &left, &wrote, szBody, nBody);
            }
            continue;
        }
        out_ch(pF, ppB, &left, &wrote, '%');
        out_ch(pF, ppB, &left, &wrote, chConv);
    }
    if (szBuf != NULL && cb > 0) {
        if (left == 0) {
            szBuf[cb - 1] = '\0';
        } else {
            *pB = '\0';
        }
    }
    return wrote;
}

int
vsnprintf(char *szBuf, size_t cb, const char *szFmt, va_list ap)
{
    return (int)vformat(NULL, szBuf, cb, szFmt, ap);
}

int
snprintf(char *szBuf, size_t cb, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vsnprintf(szBuf, cb, szFmt, ap);
    va_end(ap);
    return n;
}

int
vasprintf(char **ppBuf, const char *szFmt, va_list ap)
{
    va_list ap2;
    int n;
    char *p;
    size_t cb;

    if (ppBuf == NULL || szFmt == NULL) {
        errno = EINVAL;
        return -1;
    }
    va_copy(ap2, ap);
    n = vsnprintf(NULL, 0, szFmt, ap2);
    va_end(ap2);
    if (n < 0) {
        *ppBuf = NULL;
        return -1;
    }
    cb = (size_t)n + 1u;
    p = (char *)malloc(cb);
    if (p == NULL) {
        *ppBuf = NULL;
        return -1;
    }
    n = vsnprintf(p, cb, szFmt, ap);
    if (n < 0) {
        free(p);
        *ppBuf = NULL;
        return -1;
    }
    *ppBuf = p;
    return n;
}

int
asprintf(char **ppBuf, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vasprintf(ppBuf, szFmt, ap);
    va_end(ap);
    return n;
}

int
vsprintf(char *szBuf, const char *szFmt, va_list ap)
{
    return vsnprintf(szBuf, (size_t)-1, szFmt, ap);
}

int
sprintf(char *szBuf, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vsprintf(szBuf, szFmt, ap);
    va_end(ap);
    return n;
}

int
vfprintf(FILE *pF, const char *szFmt, va_list ap)
{
    int n;

    ensure_stdio();
    if (pF == NULL) {
        pF = stdout;
    }
    n = (int)vformat(pF, NULL, 0, szFmt, ap);
    if (pF->nBufMode == _IOLBF) {
        (void)flush_write(pF);
    }
    return n;
}

int
vprintf(const char *szFmt, va_list ap)
{
    return vfprintf(stdout, szFmt, ap);
}

int
fprintf(FILE *pF, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vfprintf(pF, szFmt, ap);
    va_end(ap);
    return n;
}

int
printf(const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vfprintf(stdout, szFmt, ap);
    va_end(ap);
    return n;
}

int
puts(const char *sz)
{
    ensure_stdio();
    if (sz == NULL) {
        sz = "(null)";
    }
    if (fputs(sz, stdout) < 0) {
        return EOF;
    }
    if (fputc('\n', stdout) == EOF) {
        return EOF;
    }
    return 0;
}

int
fputs(const char *sz, FILE *pF)
{
    size_t n;

    ensure_stdio();
    if (pF == NULL) {
        pF = stdout;
    }
    if (sz == NULL) {
        return EOF;
    }
    n = strlen(sz);
    if (fwrite(sz, 1, n, pF) != n) {
        return EOF;
    }
    return 0;
}

int
putchar(int ch)
{
    return fputc(ch, stdout);
}

int
fputc(int ch, FILE *pF)
{
    ensure_stdio();
    if (pF == NULL) {
        pF = stdout;
    }
    if (putc_buf(pF, (unsigned char)ch) != 0) {
        return EOF;
    }
    return (unsigned char)ch;
}

static int
refill_read(FILE *pF)
{
    ssize_t r;

    pF->cbRpos = 0;
    pF->cbRlen = 0;
    if ((pF->nFlags & F_COOKIE) != 0 && pF->pfnCkRead != NULL) {
        r = pF->pfnCkRead(pF->pCookie, (char *)pF->pBuf, pF->cbBuf);
    } else {
        r = read(pF->nFd, pF->pBuf, pF->cbBuf);
    }
    if (r < 0) {
        pF->nErr = 1;
        return -1;
    }
    if (r == 0) {
        pF->nEof = 1;
        return 0;
    }
    pF->cbRlen = (size_t)r;
    return 1;
}

int
fgetc(FILE *pF)
{
    ensure_stdio();
    if (pF == NULL) {
        pF = stdin;
    }
    if (pF->nUnget >= 0) {
        int c = pF->nUnget;

        pF->nUnget = -1;
        return c;
    }
    if (pF->nBufMode == _IONBF) {
        unsigned char c;
        ssize_t r;

        if ((pF->nFlags & F_COOKIE) != 0 && pF->pfnCkRead != NULL) {
            r = pF->pfnCkRead(pF->pCookie, (char *)&c, 1);
        } else {
            r = read(pF->nFd, &c, 1);
        }
        if (r != 1) {
            pF->nEof = 1;
            return EOF;
        }
        return (int)c;
    }
    if (pF->cbRpos >= pF->cbRlen) {
        if (refill_read(pF) <= 0) {
            return EOF;
        }
    }
    return (int)pF->pBuf[pF->cbRpos++];
}

int
getchar(void)
{
    return fgetc(stdin);
}

int
ungetc(int ch, FILE *pF)
{
    ensure_stdio();
    if (pF == NULL || ch == EOF) {
        return EOF;
    }
    if (pF->nUnget >= 0) {
        return EOF;
    }
    pF->nUnget = (unsigned char)ch;
    pF->nEof = 0;
    return (unsigned char)ch;
}

char *
fgets(char *sz, int n, FILE *pF)
{
    int i = 0;
    int c;

    ensure_stdio();
    if (sz == NULL || n <= 0 || pF == NULL) {
        return NULL;
    }
    while (i < n - 1) {
        c = fgetc(pF);
        if (c == EOF) {
            if (i == 0) {
                return NULL;
            }
            break;
        }
        sz[i++] = (char)c;
        if (c == '\n') {
            break;
        }
    }
    sz[i] = '\0';
    return sz;
}

int
fileno(FILE *pF)
{
    ensure_stdio();
    if (pF == NULL) {
        errno = EBADF;
        return -1;
    }
    return pF->nFd;
}

size_t
fwrite(const void *p, size_t cb, size_t n, FILE *pF)
{
    const unsigned char *pB = (const unsigned char *)p;
    size_t total;
    size_t i;

    ensure_stdio();
    if (pF == NULL || p == NULL || cb == 0 || n == 0) {
        return 0;
    }
    total = cb * n;
    for (i = 0; i < total; i++) {
        if (putc_buf(pF, pB[i]) != 0) {
            return i / cb;
        }
    }
    return n;
}

size_t
fread(void *p, size_t cb, size_t n, FILE *pF)
{
    unsigned char *pB = (unsigned char *)p;
    size_t total;
    size_t i;

    ensure_stdio();
    if (pF == NULL || p == NULL || cb == 0 || n == 0) {
        return 0;
    }
    total = cb * n;
    for (i = 0; i < total; i++) {
        int c = fgetc(pF);

        if (c == EOF) {
            return i / cb;
        }
        pB[i] = (unsigned char)c;
    }
    return n;
}

static FILE *
file_open_fd(int nFd, int nFlags)
{
    FILE *pF = (FILE *)malloc(sizeof(FILE));

    if (pF == NULL) {
        (void)close(nFd);
        return NULL;
    }
    file_init_std(pF, nFd, nFlags | F_OWNBUF, _IOFBF);
    {
        unsigned char *p = (unsigned char *)malloc(BUFSIZ);

        if (p != NULL) {
            pF->pBuf = p;
            pF->cbBuf = BUFSIZ;
        } else {
            pF->pBuf = pF->aSmall;
            pF->cbBuf = sizeof(pF->aSmall);
            pF->nFlags &= ~F_OWNBUF;
        }
    }
    return pF;
}

FILE *
fopen(const char *szPath, const char *szMode)
{
    int nFlags = O_RDONLY;
    int nMode = 0644;
    int nFd;
    int fBits = F_READ;

    ensure_stdio();
    if (szPath == NULL || szMode == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (szMode[0] == 'w') {
        nFlags = O_WRONLY | O_CREAT | O_TRUNC;
        fBits = F_WRITE;
        if (szMode[1] == '+') {
            nFlags = O_RDWR | O_CREAT | O_TRUNC;
            fBits = F_READ | F_WRITE;
        }
    } else if (szMode[0] == 'a') {
        nFlags = O_WRONLY | O_CREAT | O_APPEND;
        fBits = F_WRITE;
        if (szMode[1] == '+') {
            nFlags = O_RDWR | O_CREAT | O_APPEND;
            fBits = F_READ | F_WRITE;
        }
    } else if (szMode[0] == 'r') {
        nFlags = O_RDONLY;
        fBits = F_READ;
        if (szMode[1] == '+') {
            nFlags = O_RDWR;
            fBits = F_READ | F_WRITE;
        }
    }
    nFd = open(szPath, nFlags, nMode);
    if (nFd < 0) {
        return NULL;
    }
    return file_open_fd(nFd, fBits);
}

FILE *
fdopen(int nFd, const char *szMode)
{
    int fBits = F_READ;

    ensure_stdio();
    if (nFd < 0 || szMode == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (szMode[0] == 'w' || szMode[0] == 'a') {
        fBits = F_WRITE;
        if (szMode[1] == '+') {
            fBits = F_READ | F_WRITE;
        }
    } else if (szMode[0] == 'r' && szMode[1] == '+') {
        fBits = F_READ | F_WRITE;
    }
    return file_open_fd(nFd, fBits);
}

int
fclose(FILE *pF)
{
    int n = 0;

    ensure_stdio();
    if (pF == NULL) {
        return EOF;
    }
    (void)fflush(pF);
    if (pF == stdin || pF == stdout || pF == stderr) {
        return 0;
    }
    if ((pF->nFlags & (F_MEMSTREAM | F_WMEMSTREAM)) != 0) {
        /* User owns pMemData via *ppMemUser / *ppWMemUser after close */
        pF->pMemData = NULL;
        pF->ppMemUser = NULL;
        pF->ppWMemUser = NULL;
        pF->pcbMemUser = NULL;
    } else if ((pF->nFlags & F_COOKIE) != 0) {
        if (pF->pfnCkClose != NULL) {
            n = pF->pfnCkClose(pF->pCookie);
        }
    } else if (pF->nFd >= 0) {
        n = close(pF->nFd);
    }
    if (pF->nFlags & F_OWNBUF) {
        free(pF->pBuf);
    }
    free(pF);
    return n;
}

static int
file_seek(FILE *pF, off_t off, int nWhence)
{
    ensure_stdio();
    if (pF == NULL) {
        return -1;
    }
    (void)fflush(pF);
    pF->cbRlen = 0;
    pF->cbRpos = 0;
    pF->nUnget = -1;
    pF->nEof = 0;
    if ((pF->nFlags & F_MEMSTREAM) != 0) {
        off_t nBase = 0;
        off_t nNew;

        if (nWhence == SEEK_SET) {
            nBase = 0;
        } else if (nWhence == SEEK_CUR) {
            nBase = pF->offMem;
        } else if (nWhence == SEEK_END) {
            nBase = (off_t)pF->cbMemLen;
        } else {
            errno = EINVAL;
            return -1;
        }
        nNew = nBase + off;
        if (nNew < 0) {
            errno = EINVAL;
            return -1;
        }
        pF->offMem = nNew;
        return 0;
    }
    if ((pF->nFlags & F_COOKIE) != 0) {
        off_t o = off;

        if (pF->pfnCkSeek == NULL) {
            errno = ESPIPE;
            return -1;
        }
        if (pF->pfnCkSeek(pF->pCookie, &o, nWhence) != 0) {
            return -1;
        }
        pF->offMem = o;
        return 0;
    }
    if (lseek(pF->nFd, off, nWhence) < 0) {
        return -1;
    }
    return 0;
}

int
fseek(FILE *pF, long off, int nWhence)
{
    return file_seek(pF, (off_t)off, nWhence);
}

long
ftell(FILE *pF)
{
    off_t off;

    ensure_stdio();
    if (pF == NULL) {
        return -1L;
    }
    (void)fflush(pF);
    if ((pF->nFlags & F_MEMSTREAM) != 0) {
        return (long)pF->offMem;
    }
    off = lseek(pF->nFd, 0, SEEK_CUR);
    if (off < 0) {
        return -1L;
    }
    /* unread buffered bytes sit ahead of kernel pos */
    if (pF->cbRlen > pF->cbRpos) {
        off -= (off_t)(pF->cbRlen - pF->cbRpos);
    }
    return (long)off;
}

int
fseeko(FILE *pF, off_t off, int nWhence)
{
    return file_seek(pF, off, nWhence);
}

off_t
ftello(FILE *pF)
{
    off_t off;

    ensure_stdio();
    if (pF == NULL) {
        return (off_t)-1;
    }
    (void)fflush(pF);
    if ((pF->nFlags & F_MEMSTREAM) != 0) {
        return pF->offMem;
    }
    off = lseek(pF->nFd, 0, SEEK_CUR);
    if (off < 0) {
        return (off_t)-1;
    }
    if (pF->cbRlen > pF->cbRpos) {
        off -= (off_t)(pF->cbRlen - pF->cbRpos);
    }
    return off;
}

FILE *
freopen(const char *szPath, const char *szMode, FILE *pF)
{
    FILE *pNew;

    ensure_stdio();
    if (pF == NULL) {
        errno = EINVAL;
        return NULL;
    }
    /* Close underlying fd/buffer but keep FILE object for std streams */
    (void)fflush(pF);
    if (pF->nFd >= 0 && pF != stdin && pF != stdout && pF != stderr) {
        (void)close(pF->nFd);
        pF->nFd = -1;
    } else if (pF->nFd >= 0) {
        (void)close(pF->nFd);
        pF->nFd = -1;
    }
    if ((pF->nFlags & F_OWNBUF) != 0 && pF->pBuf != NULL &&
        pF->pBuf != pF->aSmall) {
        free(pF->pBuf);
        pF->pBuf = pF->aSmall;
        pF->cbBuf = sizeof(pF->aSmall);
        pF->nFlags &= ~F_OWNBUF;
    }
    pF->cbWpos = 0;
    pF->cbRlen = 0;
    pF->cbRpos = 0;
    pF->nUnget = -1;
    pF->nErr = 0;
    pF->nEof = 0;

    pNew = fopen(szPath, szMode);
    if (pNew == NULL) {
        return NULL;
    }
    /* Steal the new fd into pF and free the temporary FILE */
    pF->nFd = pNew->nFd;
    pF->nFlags = pNew->nFlags;
    pF->nBufMode = pNew->nBufMode;
    pNew->nFd = -1;
    (void)fclose(pNew);
    return pF;
}

ssize_t
getdelim(char **ppLine, size_t *pcb, int nDelim, FILE *pF)
{
    size_t cbCap;
    size_t cbLen = 0;
    char *pLine;
    int ch;

    ensure_stdio();
    if (ppLine == NULL || pcb == NULL || pF == NULL) {
        errno = EINVAL;
        return -1;
    }
    if (*ppLine == NULL || *pcb == 0) {
        cbCap = 128;
        pLine = (char *)malloc(cbCap);
        if (pLine == NULL) {
            return -1;
        }
        *ppLine = pLine;
        *pcb = cbCap;
    } else {
        pLine = *ppLine;
        cbCap = *pcb;
    }

    for (;;) {
        ch = fgetc(pF);
        if (ch == EOF) {
            if (cbLen == 0) {
                return -1;
            }
            pLine[cbLen] = '\0';
            return (ssize_t)cbLen;
        }
        if (cbLen + 2 > cbCap) {
            size_t cbNew = cbCap * 2;
            char *pGrow;

            if (cbNew < cbLen + 2) {
                cbNew = cbLen + 32;
            }
            pGrow = (char *)realloc(pLine, cbNew);
            if (pGrow == NULL) {
                return -1;
            }
            pLine = pGrow;
            *ppLine = pLine;
            *pcb = cbNew;
            cbCap = cbNew;
        }
        pLine[cbLen++] = (char)ch;
        if (ch == nDelim) {
            pLine[cbLen] = '\0';
            return (ssize_t)cbLen;
        }
    }
}

ssize_t
getline(char **ppLine, size_t *pcb, FILE *pF)
{
    return getdelim(ppLine, pcb, '\n', pF);
}

void
rewind(FILE *pF)
{
    (void)fseek(pF, 0, SEEK_SET);
    if (pF != NULL) {
        pF->nErr = 0;
    }
}

void
clearerr(FILE *pF)
{
    if (pF != NULL) {
        pF->nErr = 0;
        pF->nEof = 0;
    }
}

int
ferror(FILE *pF)
{
    return (pF != NULL) ? pF->nErr : 0;
}

int
feof(FILE *pF)
{
    return (pF != NULL) ? pF->nEof : 0;
}

void
perror(const char *sz)
{
    char aBuf[48];
    int n;

    ensure_stdio();
    if (sz != NULL && sz[0] != '\0') {
        (void)fputs(sz, stderr);
        (void)fputs(": ", stderr);
    }
    n = snprintf(aBuf, sizeof(aBuf), "errno=%d\n", errno);
    if (n > 0) {
        (void)fputs(aBuf, stderr);
    }
    (void)fflush(stderr);
}

int
remove(const char *szPath)
{
    return unlink(szPath);
}

/*
 * fopencookie: custom stream via caller I/O callbacks (F_COOKIE).
 */
FILE *
fopencookie(void *pCookie, const char *szMode, cookie_io_functions_t io)
{
    FILE *pF;
    int nBits = F_READ;
    unsigned char *pWbuf;

    ensure_stdio();
    if (szMode == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (szMode[0] == 'w' || szMode[0] == 'a') {
        nBits = F_WRITE;
        if (szMode[1] == '+') {
            nBits = F_READ | F_WRITE;
        }
    } else if (szMode[0] == 'r' && szMode[1] == '+') {
        nBits = F_READ | F_WRITE;
    } else if (szMode[0] != 'r') {
        errno = EINVAL;
        return NULL;
    }
    pF = (FILE *)malloc(sizeof(FILE));
    if (pF == NULL) {
        return NULL;
    }
    file_init_std(pF, -1, nBits | F_COOKIE | F_OWNBUF, _IOFBF);
    pWbuf = (unsigned char *)malloc(BUFSIZ);
    if (pWbuf != NULL) {
        pF->pBuf = pWbuf;
        pF->cbBuf = BUFSIZ;
    } else {
        pF->pBuf = pF->aSmall;
        pF->cbBuf = sizeof(pF->aSmall);
        pF->nFlags &= ~F_OWNBUF;
    }
    pF->pCookie = pCookie;
    pF->pfnCkRead = io.read;
    pF->pfnCkWrite = io.write;
    pF->pfnCkSeek = io.seek;
    pF->pfnCkClose = io.close;
    return pF;
}

/*
 * fmemopen: bring-up via memfd seeded from the buffer (read/write through
 * normal FILE buffering). Does not write back into caller buffer on close.
 */
FILE *
fmemopen(void *pBuf, size_t cb, const char *szMode)
{
    int nFd;
    int nBits = F_READ;
    FILE *pF;

    ensure_stdio();
    if (szMode == NULL || (pBuf == NULL && cb != 0)) {
        errno = EINVAL;
        return NULL;
    }
    nFd = memfd_create("cgj-fmemopen", 0);
    if (nFd < 0) {
        return NULL;
    }
    if (pBuf != NULL && cb > 0 &&
        (szMode[0] == 'r' || szMode[0] == 'a' ||
         (szMode[0] == 'w' && szMode[1] == '+'))) {
        if (pwrite(nFd, pBuf, cb, 0) < 0) {
            (void)close(nFd);
            return NULL;
        }
    }
    if (szMode[0] == 'w' && szMode[1] != '+') {
        (void)ftruncate(nFd, (off_t)cb);
    } else if (szMode[0] == 'r' || szMode[0] == 'a') {
        (void)ftruncate(nFd, (off_t)cb);
    }
    (void)lseek(nFd, 0, SEEK_SET);
    if (szMode[0] == 'w' || szMode[0] == 'a') {
        nBits = F_WRITE;
        if (szMode[1] == '+') {
            nBits = F_READ | F_WRITE;
        }
    } else if (szMode[0] == 'r' && szMode[1] == '+') {
        nBits = F_READ | F_WRITE;
    }
    pF = file_open_fd(nFd, nBits);
    return pF;
}

/*
 * open_memstream: growable in-memory write stream. fflush updates
 * *ppBuf / *pcb to a NUL-terminated buffer the caller free()s.
 */
FILE *
open_memstream(char **ppBuf, size_t *pcb)
{
    FILE *pF;
    unsigned char *pWbuf;

    ensure_stdio();
    if (ppBuf == NULL || pcb == NULL) {
        errno = EINVAL;
        return NULL;
    }
    pF = (FILE *)malloc(sizeof(FILE));
    if (pF == NULL) {
        return NULL;
    }
    file_init_std(pF, -1, F_WRITE | F_MEMSTREAM | F_OWNBUF, _IOFBF);
    pWbuf = (unsigned char *)malloc(BUFSIZ);
    if (pWbuf != NULL) {
        pF->pBuf = pWbuf;
        pF->cbBuf = BUFSIZ;
    } else {
        pF->pBuf = pF->aSmall;
        pF->cbBuf = sizeof(pF->aSmall);
        pF->nFlags &= ~F_OWNBUF;
    }
    pF->pMemData = (char *)malloc(1);
    if (pF->pMemData == NULL) {
        if (pF->nFlags & F_OWNBUF) {
            free(pF->pBuf);
        }
        free(pF);
        return NULL;
    }
    pF->pMemData[0] = '\0';
    pF->cbMemCap = 1;
    pF->cbMemLen = 0;
    pF->offMem = 0;
    pF->ppMemUser = ppBuf;
    pF->pcbMemUser = pcb;
    *ppBuf = pF->pMemData;
    *pcb = 0;
    return pF;
}

/*
 * open_wmemstream: growable wide stream. Unit is wchar_t (int).
 * Prefer fputwc/fputws; fflush publishes *ppBuf / *pcb (wchar count).
 */
FILE *
open_wmemstream(wchar_t **ppBuf, size_t *pcb)
{
    FILE *pF;
    unsigned char *pWbuf;
    int *pW0;

    ensure_stdio();
    if (ppBuf == NULL || pcb == NULL) {
        errno = EINVAL;
        return NULL;
    }
    pF = (FILE *)malloc(sizeof(FILE));
    if (pF == NULL) {
        return NULL;
    }
    file_init_std(pF, -1, F_WRITE | F_MEMSTREAM | F_WMEMSTREAM | F_OWNBUF,
                  _IOFBF);
    pF->nWide = 1;
    pWbuf = (unsigned char *)malloc(BUFSIZ);
    if (pWbuf != NULL) {
        pF->pBuf = pWbuf;
        pF->cbBuf = BUFSIZ - (BUFSIZ % sizeof(int));
    } else {
        pF->pBuf = pF->aSmall;
        pF->cbBuf = sizeof(pF->aSmall) - (sizeof(pF->aSmall) % sizeof(int));
        pF->nFlags &= ~F_OWNBUF;
    }
    pF->pMemData = (char *)malloc(sizeof(int));
    if (pF->pMemData == NULL) {
        if (pF->nFlags & F_OWNBUF) {
            free(pF->pBuf);
        }
        free(pF);
        return NULL;
    }
    pW0 = (int *)(void *)pF->pMemData;
    pW0[0] = 0;
    pF->cbMemCap = sizeof(int);
    pF->cbMemLen = 0;
    pF->offMem = 0;
    pF->ppWMemUser = (void **)ppBuf;
    pF->pcbMemUser = pcb;
    *ppBuf = (wchar_t *)(void *)pF->pMemData;
    *pcb = 0;
    return pF;
}

/* rename(2) lives in syscall_linux.c */

/* ---- narrow scanf subset ------------------------------------------------ */

struct scan_src {
    FILE       *pF;
    const char *pS;
    size_t      nPos;
    size_t      nCount;
};

static int
scan_get(struct scan_src *pSrc)
{
    int c;

    if (pSrc->pF != NULL) {
        c = fgetc(pSrc->pF);
        if (c != EOF) {
            pSrc->nCount++;
        }
        return c;
    }
    if (pSrc->pS == NULL) {
        return EOF;
    }
    c = (unsigned char)pSrc->pS[pSrc->nPos];
    if (c == '\0') {
        return EOF;
    }
    pSrc->nPos++;
    pSrc->nCount++;
    return c;
}

static void
scan_unget(struct scan_src *pSrc, int c)
{
    if (c == EOF) {
        return;
    }
    if (pSrc->nCount > 0) {
        pSrc->nCount--;
    }
    if (pSrc->pF != NULL) {
        (void)ungetc(c, pSrc->pF);
        return;
    }
    if (pSrc->nPos > 0) {
        pSrc->nPos--;
    }
}

static int
scan_isspace(int c)
{
    return (c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' ||
            c == '\v');
}

static void
scan_skip_space(struct scan_src *pSrc)
{
    int c;

    for (;;) {
        c = scan_get(pSrc);
        if (c == EOF) {
            return;
        }
        if (!scan_isspace(c)) {
            scan_unget(pSrc, c);
            return;
        }
    }
}

static int
scan_int(struct scan_src *pSrc, int nBase, int nWidth, long long *pOut)
{
    int c;
    int fNeg = 0;
    int fAny = 0;
    int nUsed = 0;
    int nMax = (nWidth > 0) ? nWidth : 1000000;
    unsigned long long u = 0;

    scan_skip_space(pSrc);
    c = scan_get(pSrc);
    if (c == '+' || c == '-') {
        fNeg = (c == '-');
        nUsed++;
        if (nUsed >= nMax) {
            *pOut = 0;
            return -1;
        }
        c = scan_get(pSrc);
    }
    if (nBase == 0) {
        if (c == '0') {
            int c2;

            nUsed++;
            fAny = 1;
            if (nUsed >= nMax) {
                *pOut = 0;
                return 0;
            }
            c2 = scan_get(pSrc);
            if (c2 == 'x' || c2 == 'X') {
                nBase = 16;
                nUsed++;
                if (nUsed >= nMax) {
                    *pOut = 0;
                    return 0;
                }
                c = scan_get(pSrc);
            } else {
                nBase = 8;
                scan_unget(pSrc, c2);
                c = '0';
                nUsed--;
            }
        } else {
            nBase = 10;
        }
    } else if (nBase == 16 && c == '0') {
        int c2;

        if (nUsed + 1 < nMax) {
            c2 = scan_get(pSrc);
            if (c2 == 'x' || c2 == 'X') {
                nUsed += 2;
                if (nUsed >= nMax) {
                    *pOut = 0;
                    return 0;
                }
                c = scan_get(pSrc);
            } else {
                scan_unget(pSrc, c2);
            }
        }
    }
    for (;;) {
        int d = -1;

        if (nUsed >= nMax) {
            break;
        }
        if (c >= '0' && c <= '9') {
            d = c - '0';
        } else if (c >= 'a' && c <= 'f') {
            d = 10 + (c - 'a');
        } else if (c >= 'A' && c <= 'F') {
            d = 10 + (c - 'A');
        }
        if (d < 0 || d >= nBase) {
            break;
        }
        fAny = 1;
        u = u * (unsigned)nBase + (unsigned)d;
        nUsed++;
        c = scan_get(pSrc);
    }
    if (c != EOF) {
        scan_unget(pSrc, c);
    }
    if (!fAny) {
        return -1;
    }
    *pOut = fNeg ? -(long long)u : (long long)u;
    return 0;
}

static void
scan_store_int(va_list ap, int nLenMod, int fSigned, long long nVal)
{
    if (nLenMod == 2) {
        if (fSigned) {
            *va_arg(ap, long long *) = nVal;
        } else {
            *va_arg(ap, unsigned long long *) = (unsigned long long)nVal;
        }
    } else if (nLenMod == 1) {
        if (fSigned) {
            *va_arg(ap, long *) = (long)nVal;
        } else {
            *va_arg(ap, unsigned long *) = (unsigned long)nVal;
        }
    } else if (nLenMod == 3) {
        if (fSigned) {
            *va_arg(ap, ptrdiff_t *) = (ptrdiff_t)nVal;
        } else {
            *va_arg(ap, size_t *) = (size_t)nVal;
        }
    } else if (nLenMod == -1) {
        if (fSigned) {
            *va_arg(ap, short *) = (short)nVal;
        } else {
            *va_arg(ap, unsigned short *) = (unsigned short)nVal;
        }
    } else if (nLenMod == -2) {
        if (fSigned) {
            *va_arg(ap, signed char *) = (signed char)nVal;
        } else {
            *va_arg(ap, unsigned char *) = (unsigned char)nVal;
        }
    } else if (fSigned) {
        *va_arg(ap, int *) = (int)nVal;
    } else {
        *va_arg(ap, unsigned *) = (unsigned)nVal;
    }
}

static int
vscan(struct scan_src *pSrc, const char *szFmt, va_list ap)
{
    int nAssigned = 0;
    const char *p;

    if (szFmt == NULL) {
        return EOF;
    }
    for (p = szFmt; *p != '\0'; p++) {
        if (scan_isspace((unsigned char)*p)) {
            scan_skip_space(pSrc);
            while (scan_isspace((unsigned char)*p)) {
                p++;
            }
            p--;
            continue;
        }
        if (*p != '%') {
            int c = scan_get(pSrc);

            if (c == EOF) {
                return (nAssigned > 0) ? nAssigned : EOF;
            }
            if (c != (unsigned char)*p) {
                scan_unget(pSrc, c);
                return nAssigned;
            }
            continue;
        }
        p++;
        if (*p == '\0') {
            break;
        }
        if (*p == '%') {
            int c = scan_get(pSrc);

            if (c != '%') {
                if (c != EOF) {
                    scan_unget(pSrc, c);
                }
                return nAssigned;
            }
            continue;
        }
        {
            int fSuppress = 0;
            int nWidth = 0;
            int nLenMod = 0;
            int nBase;
            int fSigned;
            long long v = 0;
            char chConv;

            if (*p == '*') {
                fSuppress = 1;
                p++;
            }
            while (*p >= '0' && *p <= '9') {
                nWidth = nWidth * 10 + (*p - '0');
                p++;
            }
            if (*p == 'h') {
                p++;
                if (*p == 'h') {
                    nLenMod = -2;
                    p++;
                } else {
                    nLenMod = -1;
                }
            } else if (*p == 'l') {
                p++;
                if (*p == 'l') {
                    nLenMod = 2;
                    p++;
                } else {
                    nLenMod = 1;
                }
            } else if (*p == 'z' || *p == 't' || *p == 'j') {
                nLenMod = 3;
                p++;
            }
            if (*p == '\0') {
                break;
            }
            chConv = *p;
            if (chConv == 'p') {
                if (scan_int(pSrc, 16, nWidth, &v) != 0) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                if (!fSuppress) {
                    *va_arg(ap, void **) = (void *)(uintptr_t)v;
                    nAssigned++;
                }
                continue;
            }
            if (chConv == 'd' || chConv == 'i' || chConv == 'u' ||
                chConv == 'x' || chConv == 'X' || chConv == 'o') {
                if (chConv == 'd') {
                    nBase = 10;
                    fSigned = 1;
                } else if (chConv == 'i') {
                    nBase = 0;
                    fSigned = 1;
                } else if (chConv == 'u') {
                    nBase = 10;
                    fSigned = 0;
                } else if (chConv == 'o') {
                    nBase = 8;
                    fSigned = 0;
                } else {
                    nBase = 16;
                    fSigned = 0;
                }
                if (scan_int(pSrc, nBase, nWidth, &v) != 0) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                if (!fSuppress) {
                    scan_store_int(ap, nLenMod, fSigned, v);
                    nAssigned++;
                }
                continue;
            }
            if (chConv == 's') {
                char *pOut = fSuppress ? NULL : va_arg(ap, char *);
                int n = 0;
                int c;

                scan_skip_space(pSrc);
                for (;;) {
                    if (nWidth > 0 && n >= nWidth) {
                        break;
                    }
                    c = scan_get(pSrc);
                    if (c == EOF || scan_isspace(c)) {
                        if (c != EOF) {
                            scan_unget(pSrc, c);
                        }
                        break;
                    }
                    if (pOut != NULL) {
                        pOut[n] = (char)c;
                    }
                    n++;
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
            if (chConv == 'c') {
                int c = scan_get(pSrc);
                int nNeed = (nWidth > 0) ? nWidth : 1;
                int i;

                if (c == EOF) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                if (!fSuppress) {
                    char *pOut = va_arg(ap, char *);

                    pOut[0] = (char)c;
                    for (i = 1; i < nNeed; i++) {
                        c = scan_get(pSrc);
                        if (c == EOF) {
                            break;
                        }
                        pOut[i] = (char)c;
                    }
                    nAssigned++;
                } else {
                    for (i = 1; i < nNeed; i++) {
                        if (scan_get(pSrc) == EOF) {
                            break;
                        }
                    }
                }
                continue;
            }
            if (chConv == '[') {
                unsigned char aSet[256];
                int fNegate = 0;
                int n = 0;
                int c;
                int i;
                char *pOut = fSuppress ? NULL : va_arg(ap, char *);

                p++;
                for (i = 0; i < 256; i++) {
                    aSet[i] = 0;
                }
                if (*p == '^') {
                    fNegate = 1;
                    p++;
                }
                if (*p == ']') {
                    aSet[(unsigned char)']'] = 1;
                    p++;
                }
                while (*p != '\0' && *p != ']') {
                    unsigned char chLo = (unsigned char)*p;

                    p++;
                    if (*p == '-' && p[1] != '\0' && p[1] != ']') {
                        unsigned char chHi = (unsigned char)p[1];
                        unsigned int uCh;

                        p += 2;
                        if (chLo > chHi) {
                            unsigned char chT = chLo;

                            chLo = chHi;
                            chHi = chT;
                        }
                        for (uCh = chLo; uCh <= chHi; uCh++) {
                            aSet[uCh] = 1;
                        }
                    } else {
                        aSet[chLo] = 1;
                    }
                }
                for (;;) {
                    if (nWidth > 0 && n >= nWidth) {
                        break;
                    }
                    c = scan_get(pSrc);
                    if (c == EOF) {
                        break;
                    }
                    {
                        int fIn = aSet[(unsigned char)c];

                        if (fNegate) {
                            fIn = !fIn;
                        }
                        if (!fIn) {
                            scan_unget(pSrc, c);
                            break;
                        }
                    }
                    if (pOut != NULL) {
                        pOut[n] = (char)c;
                    }
                    n++;
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
            if (chConv == 'n') {
                if (!fSuppress) {
                    scan_store_int(ap, nLenMod, 1, (long long)pSrc->nCount);
                }
                continue;
            }
            if (chConv == 'f' || chConv == 'F' || chConv == 'e' ||
                chConv == 'E' || chConv == 'g' || chConv == 'G') {
                char aNum[64];
                int n = 0;
                int c;
                double dV;
                char *pEnd = NULL;

                scan_skip_space(pSrc);
                for (;;) {
                    if (nWidth > 0 && n >= nWidth) {
                        break;
                    }
                    c = scan_get(pSrc);
                    if (c == EOF) {
                        break;
                    }
                    if ((c >= '0' && c <= '9') || c == '.' || c == '+' ||
                        c == '-' || c == 'e' || c == 'E' || c == 'x' ||
                        c == 'X' || (c >= 'a' && c <= 'f') ||
                        (c >= 'A' && c <= 'F')) {
                        if (n + 1 < (int)sizeof(aNum)) {
                            aNum[n++] = (char)c;
                        }
                    } else {
                        scan_unget(pSrc, c);
                        break;
                    }
                }
                aNum[n] = '\0';
                if (n == 0) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                dV = strtod(aNum, &pEnd);
                if (pEnd == aNum) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                if (!fSuppress) {
                    if (nLenMod == 1 || nLenMod == 2) {
                        *va_arg(ap, double *) = dV;
                    } else {
                        *va_arg(ap, float *) = (float)dV;
                    }
                    nAssigned++;
                }
                continue;
            }
            return nAssigned;
        }
    }
    return nAssigned;
}

int
vfscanf(FILE *pF, const char *szFmt, va_list ap)
{
    struct scan_src src;

    ensure_stdio();
    if (pF == NULL || szFmt == NULL) {
        errno = EINVAL;
        return EOF;
    }
    src.pF = pF;
    src.pS = NULL;
    src.nPos = 0;
    src.nCount = 0;
    return vscan(&src, szFmt, ap);
}

int
fscanf(FILE *pF, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vfscanf(pF, szFmt, ap);
    va_end(ap);
    return n;
}

int
vscanf(const char *szFmt, va_list ap)
{
    return vfscanf(stdin, szFmt, ap);
}

int
scanf(const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vscanf(szFmt, ap);
    va_end(ap);
    return n;
}

int
vsscanf(const char *szBuf, const char *szFmt, va_list ap)
{
    struct scan_src src;

    if (szBuf == NULL || szFmt == NULL) {
        errno = EINVAL;
        return EOF;
    }
    src.pF = NULL;
    src.pS = szBuf;
    src.nPos = 0;
    src.nCount = 0;
    return vscan(&src, szFmt, ap);
}

int
sscanf(const char *szBuf, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vsscanf(szBuf, szFmt, ap);
    va_end(ap);
    return n;
}
