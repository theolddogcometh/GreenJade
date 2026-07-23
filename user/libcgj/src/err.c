/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * err/warn family — print to stderr and optionally exit.
 */
#include <err.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void
vwarnx(const char *szFmt, va_list ap)
{
    if (szFmt != NULL) {
        (void)vfprintf(stderr, szFmt, ap);
        (void)fputc('\n', stderr);
    }
}

void
vwarn(const char *szFmt, va_list ap)
{
    int e = errno;

    if (szFmt != NULL) {
        (void)vfprintf(stderr, szFmt, ap);
        (void)fputs(": ", stderr);
    }
    (void)fputs(strerror(e), stderr);
    (void)fputc('\n', stderr);
}

void
verrx(int nEval, const char *szFmt, va_list ap)
{
    vwarnx(szFmt, ap);
    exit(nEval);
}

void
verr(int nEval, const char *szFmt, va_list ap)
{
    vwarn(szFmt, ap);
    exit(nEval);
}

void
warnx(const char *szFmt, ...)
{
    va_list ap;

    va_start(ap, szFmt);
    vwarnx(szFmt, ap);
    va_end(ap);
}

void
warn(const char *szFmt, ...)
{
    va_list ap;

    va_start(ap, szFmt);
    vwarn(szFmt, ap);
    va_end(ap);
}

void
errx(int nEval, const char *szFmt, ...)
{
    va_list ap;

    va_start(ap, szFmt);
    verrx(nEval, szFmt, ap);
    va_end(ap);
}

void
err(int nEval, const char *szFmt, ...)
{
    va_list ap;

    va_start(ap, szFmt);
    verr(nEval, szFmt, ap);
    va_end(ap);
}
