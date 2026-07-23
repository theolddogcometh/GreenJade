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
out_str(FILE *pF, char **ppBuf, size_t *pLeft, size_t *pWrote, const char *sz)
{
    if (sz == NULL) {
        sz = "(null)";
    }
    while (*sz != '\0') {
        out_ch(pF, ppBuf, pLeft, pWrote, *sz++);
    }
}

static void
out_udec(FILE *pF, char **ppBuf, size_t *pLeft, size_t *pWrote,
         unsigned long long u, int nBase, int fUpper)
{
    char aTmp[32];
    const char *szDig = fUpper ? "0123456789ABCDEF" : "0123456789abcdef";
    int i = 0;

    if (u == 0) {
        out_ch(pF, ppBuf, pLeft, pWrote, '0');
        return;
    }
    while (u > 0 && i < (int)sizeof(aTmp)) {
        aTmp[i++] = szDig[u % (unsigned)nBase];
        u /= (unsigned)nBase;
    }
    while (i > 0) {
        out_ch(pF, ppBuf, pLeft, pWrote, aTmp[--i]);
    }
}

static void
out_dec(FILE *pF, char **ppBuf, size_t *pLeft, size_t *pWrote, long long v)
{
    unsigned long long u;

    if (v < 0) {
        out_ch(pF, ppBuf, pLeft, pWrote, '-');
        u = (unsigned long long)(-(v + 1)) + 1ull;
    } else {
        u = (unsigned long long)v;
    }
    out_udec(pF, ppBuf, pLeft, pWrote, u, 10, 0);
}

static size_t
vformat(FILE *pF, char *szBuf, size_t cb, const char *szFmt, va_list ap)
{
    size_t wrote = 0;
    size_t left = cb;
    char *pB = szBuf;
    /* When formatting to buffer (or sizing), always pass &pB so out_ch counts */
    char **ppB = (pF == NULL) ? &pB : NULL;
    const char *p;

    if (szFmt == NULL) {
        return 0;
    }
    for (p = szFmt; *p != '\0'; p++) {
        if (*p != '%') {
            out_ch(pF, ppB, &left, &wrote, *p);
            continue;
        }
        p++;
        if (*p == '\0') {
            break;
        }
        if (*p == '%') {
            out_ch(pF, ppB, &left, &wrote, '%');
            continue;
        }
        if (*p == 's') {
            out_str(pF, ppB, &left, &wrote, va_arg(ap, const char *));
            continue;
        }
        if (*p == 'c') {
            out_ch(pF, ppB, &left, &wrote, (char)va_arg(ap, int));
            continue;
        }
        if (*p == 'd' || *p == 'i') {
            out_dec(pF, ppB, &left, &wrote, va_arg(ap, int));
            continue;
        }
        if (*p == 'u') {
            out_udec(pF, ppB, &left, &wrote,
                     (unsigned long long)va_arg(ap, unsigned), 10, 0);
            continue;
        }
        if (*p == 'x') {
            out_udec(pF, ppB, &left, &wrote,
                     (unsigned long long)va_arg(ap, unsigned), 16, 0);
            continue;
        }
        if (*p == 'X') {
            out_udec(pF, ppB, &left, &wrote,
                     (unsigned long long)va_arg(ap, unsigned), 16, 1);
            continue;
        }
        if (*p == 'p') {
            out_str(pF, ppB, &left, &wrote, "0x");
            out_udec(pF, ppB, &left, &wrote,
                     (unsigned long long)(uintptr_t)va_arg(ap, void *), 16, 0);
            continue;
        }
        if (*p == 'l') {
            p++;
            if (*p == 'd' || *p == 'i') {
                out_dec(pF, ppB, &left, &wrote, va_arg(ap, long));
            } else if (*p == 'u') {
                out_udec(pF, ppB, &left, &wrote,
                         (unsigned long long)va_arg(ap, unsigned long), 10, 0);
            } else if (*p == 'x') {
                out_udec(pF, ppB, &left, &wrote,
                         (unsigned long long)va_arg(ap, unsigned long), 16, 0);
            } else if (*p == 'l' && (p[1] == 'd' || p[1] == 'i')) {
                p++;
                out_dec(pF, ppB, &left, &wrote, va_arg(ap, long long));
            } else if (*p == 'l' && p[1] == 'u') {
                p++;
                out_udec(pF, ppB, &left, &wrote,
                         (unsigned long long)va_arg(ap, unsigned long long), 10,
                         0);
            }
            continue;
        }
        if (*p == 'z') {
            p++;
            if (*p == 'u' || *p == 'd' || *p == 'i') {
                out_udec(pF, ppB, &left, &wrote,
                         (unsigned long long)va_arg(ap, size_t), 10, 0);
            }
            continue;
        }
        out_ch(pF, ppB, &left, &wrote, '%');
        out_ch(pF, ppB, &left, &wrote, *p);
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
};

static int
scan_get(struct scan_src *pSrc)
{
    int c;

    if (pSrc->pF != NULL) {
        return fgetc(pSrc->pF);
    }
    if (pSrc->pS == NULL) {
        return EOF;
    }
    c = (unsigned char)pSrc->pS[pSrc->nPos];
    if (c == '\0') {
        return EOF;
    }
    pSrc->nPos++;
    return c;
}

static void
scan_unget(struct scan_src *pSrc, int c)
{
    if (c == EOF) {
        return;
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
scan_int(struct scan_src *pSrc, int nBase, long long *pOut)
{
    int c;
    int fNeg = 0;
    int fAny = 0;
    unsigned long long u = 0;

    scan_skip_space(pSrc);
    c = scan_get(pSrc);
    if (c == '+' || c == '-') {
        fNeg = (c == '-');
        c = scan_get(pSrc);
    }
    if (nBase == 0) {
        if (c == '0') {
            int c2 = scan_get(pSrc);

            if (c2 == 'x' || c2 == 'X') {
                nBase = 16;
                c = scan_get(pSrc);
            } else {
                nBase = 8;
                scan_unget(pSrc, c2);
                fAny = 1;
            }
        } else {
            nBase = 10;
        }
    }
    for (;;) {
        int d = -1;

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

            if (*p == '*') {
                fSuppress = 1;
                p++;
            }
            while (*p >= '0' && *p <= '9') {
                nWidth = nWidth * 10 + (*p - '0');
                p++;
            }
            if (*p == 'l') {
                p++;
                if (*p == 'd' || *p == 'i') {
                    long long v = 0;

                    if (scan_int(pSrc, 10, &v) != 0) {
                        return (nAssigned > 0) ? nAssigned : EOF;
                    }
                    if (!fSuppress) {
                        *va_arg(ap, long *) = (long)v;
                        nAssigned++;
                    }
                    continue;
                }
                if (*p == 'u') {
                    long long v = 0;

                    if (scan_int(pSrc, 10, &v) != 0) {
                        return (nAssigned > 0) ? nAssigned : EOF;
                    }
                    if (!fSuppress) {
                        *va_arg(ap, unsigned long *) = (unsigned long)v;
                        nAssigned++;
                    }
                    continue;
                }
                if (*p == 'l' && (p[1] == 'd' || p[1] == 'i')) {
                    long long v = 0;

                    p++;
                    if (scan_int(pSrc, 10, &v) != 0) {
                        return (nAssigned > 0) ? nAssigned : EOF;
                    }
                    if (!fSuppress) {
                        *va_arg(ap, long long *) = v;
                        nAssigned++;
                    }
                    continue;
                }
            }
            if (*p == 'd' || *p == 'i') {
                long long v = 0;

                if (scan_int(pSrc, (*p == 'i') ? 0 : 10, &v) != 0) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                if (!fSuppress) {
                    *va_arg(ap, int *) = (int)v;
                    nAssigned++;
                }
                continue;
            }
            if (*p == 'u') {
                long long v = 0;

                if (scan_int(pSrc, 10, &v) != 0) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                if (!fSuppress) {
                    *va_arg(ap, unsigned *) = (unsigned)v;
                    nAssigned++;
                }
                continue;
            }
            if (*p == 'x' || *p == 'X') {
                long long v = 0;

                if (scan_int(pSrc, 16, &v) != 0) {
                    return (nAssigned > 0) ? nAssigned : EOF;
                }
                if (!fSuppress) {
                    *va_arg(ap, unsigned *) = (unsigned)v;
                    nAssigned++;
                }
                continue;
            }
            if (*p == 's') {
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
            if (*p == 'c') {
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
            if (*p == 'n') {
                if (!fSuppress) {
                    *va_arg(ap, int *) = (int)pSrc->nPos;
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
