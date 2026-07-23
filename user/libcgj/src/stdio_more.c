/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * dprintf/fgetpos/flockfile/unlocked I/O/tmpfile/mkostemp.
 */
#include <errno.h>
#include <fcntl.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
vdprintf(int nFd, const char *szFmt, va_list ap)
{
    char aStack[1024];
    char *pBuf = aStack;
    char *pHeap = NULL;
    int n;
    int nNeed;
    ssize_t w;
    size_t cbWrote = 0;
    va_list ap2;

    if (szFmt == NULL) {
        errno = EINVAL;
        return -1;
    }
    va_copy(ap2, ap);
    n = vsnprintf(aStack, sizeof(aStack), szFmt, ap);
    if (n < 0) {
        va_end(ap2);
        return -1;
    }
    if (n >= (int)sizeof(aStack)) {
        /* Soft deepen: heap-expand when the stack scratch truncates. */
        nNeed = n + 1;
        pHeap = (char *)malloc((size_t)nNeed);
        if (pHeap == NULL) {
            /* Fall back: write truncated stack buffer. */
            n = (int)sizeof(aStack) - 1;
            va_end(ap2);
        } else {
            n = vsnprintf(pHeap, (size_t)nNeed, szFmt, ap2);
            va_end(ap2);
            if (n < 0) {
                free(pHeap);
                return -1;
            }
            pBuf = pHeap;
        }
    } else {
        va_end(ap2);
    }
    while (cbWrote < (size_t)n) {
        w = write(nFd, pBuf + cbWrote, (size_t)n - cbWrote);
        if (w < 0) {
            if (errno == EINTR) {
                continue;
            }
            free(pHeap);
            return -1;
        }
        if (w == 0) {
            break;
        }
        cbWrote += (size_t)w;
    }
    free(pHeap);
    return (int)cbWrote;
}

int
dprintf(int nFd, const char *szFmt, ...)
{
    va_list ap;
    int n;

    va_start(ap, szFmt);
    n = vdprintf(nFd, szFmt, ap);
    va_end(ap);
    return n;
}

int
fgetpos(FILE *pF, fpos_t *pPos)
{
    off_t off;

    if (pF == NULL || pPos == NULL) {
        errno = EINVAL;
        return -1;
    }
    off = ftello(pF);
    if (off < 0) {
        return -1;
    }
    *pPos = (fpos_t)off;
    return 0;
}

int
fsetpos(FILE *pF, const fpos_t *pPos)
{
    if (pF == NULL || pPos == NULL) {
        errno = EINVAL;
        return -1;
    }
    return fseeko(pF, (off_t)*pPos, SEEK_SET);
}

/* Bring-up: FILE not multi-threaded; locks are no-ops */
void
flockfile(FILE *pF)
{
    (void)pF;
}

void
funlockfile(FILE *pF)
{
    (void)pF;
}

int
ftrylockfile(FILE *pF)
{
    (void)pF;
    return 0;
}

int
getc_unlocked(FILE *pF)
{
    return fgetc(pF);
}

int
putc_unlocked(int ch, FILE *pF)
{
    return fputc(ch, pF);
}

int
getchar_unlocked(void)
{
    return fgetc(stdin);
}

int
putchar_unlocked(int ch)
{
    return fputc(ch, stdout);
}

size_t
fread_unlocked(void *p, size_t cb, size_t n, FILE *pF)
{
    return fread(p, cb, n, pF);
}

size_t
fwrite_unlocked(const void *p, size_t cb, size_t n, FILE *pF)
{
    return fwrite(p, cb, n, pF);
}

int
fputc_unlocked(int ch, FILE *pF)
{
    return fputc(ch, pF);
}

int
fputs_unlocked(const char *sz, FILE *pF)
{
    return fputs(sz, pF);
}

int
fgetc_unlocked(FILE *pF)
{
    return fgetc(pF);
}

char *
fgets_unlocked(char *sz, int n, FILE *pF)
{
    return fgets(sz, n, pF);
}

void
clearerr_unlocked(FILE *pF)
{
    clearerr(pF);
}

int
feof_unlocked(FILE *pF)
{
    return feof(pF);
}

int
ferror_unlocked(FILE *pF)
{
    return ferror(pF);
}

int
fileno_unlocked(FILE *pF)
{
    return fileno(pF);
}

FILE *
tmpfile(void)
{
    char aPath[] = "/tmp/cgjXXXXXX";
    int nFd;
    FILE *pF;

    nFd = mkstemp(aPath);
    if (nFd < 0) {
        return NULL;
    }
    (void)unlink(aPath);
    pF = fdopen(nFd, "w+");
    if (pF == NULL) {
        (void)close(nFd);
    }
    return pF;
}

int
mkostemp(char *szTemplate, int nFlags)
{
    int nFd;
    int i;
    size_t n;
    char *pX;
    int fl;

    if (szTemplate == NULL) {
        errno = EINVAL;
        return -1;
    }
    n = strlen(szTemplate);
    if (n < 6) {
        errno = EINVAL;
        return -1;
    }
    pX = szTemplate + n - 6;
    for (i = 0; i < 6; i++) {
        if (pX[i] != 'X') {
            errno = EINVAL;
            return -1;
        }
    }
    /* reuse mkstemp then apply extra flags */
    nFd = mkstemp(szTemplate);
    if (nFd < 0) {
        return -1;
    }
    if ((nFlags & O_CLOEXEC) != 0) {
        (void)fcntl(nFd, F_SETFD, FD_CLOEXEC);
    }
    fl = fcntl(nFd, F_GETFL);
    if (fl >= 0) {
        int nAdd = 0;

        if ((nFlags & O_APPEND) != 0) {
            nAdd |= O_APPEND;
        }
#ifdef O_SYNC
        if ((nFlags & O_SYNC) != 0) {
            nAdd |= O_SYNC;
        }
#endif
#ifdef O_DSYNC
        if ((nFlags & O_DSYNC) != 0) {
            nAdd |= O_DSYNC;
        }
#endif
        if (nAdd != 0) {
            (void)fcntl(nFd, F_SETFL, fl | nAdd);
        }
    }
    return nFd;
}
