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
    char aBuf[1024];
    int n;
    ssize_t w;

    if (szFmt == NULL) {
        errno = EINVAL;
        return -1;
    }
    n = vsnprintf(aBuf, sizeof(aBuf), szFmt, ap);
    if (n < 0) {
        return -1;
    }
    if (n >= (int)sizeof(aBuf)) {
        /* truncated: still write what we have */
        n = (int)sizeof(aBuf) - 1;
    }
    w = write(nFd, aBuf, (size_t)n);
    if (w < 0) {
        return -1;
    }
    return (int)w;
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
    if ((nFlags & O_APPEND) != 0) {
        int fl = fcntl(nFd, F_GETFL);
        if (fl >= 0) {
            (void)fcntl(nFd, F_SETFL, fl | O_APPEND);
        }
    }
    return nFd;
}
