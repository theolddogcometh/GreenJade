/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * err/warn family — print to stderr and optionally exit.
 *
 * greppable: CGJ_ERR_SOFT_PROGNAME
 * greppable: CGJ_ERR_SOFT_FFLUSH
 *
 * Soft deepen: BSD-shaped program_invocation_short_name prefix, capture
 * errno before formatting, fflush stderr after each message. errc/warnc
 * live in graph batch surface — not redefined here.
 */
#include <err.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char *program_invocation_short_name;

static void
err_soft_print_prog(void)
{
    /* greppable: CGJ_ERR_SOFT_PROGNAME */
    if (program_invocation_short_name != NULL &&
        program_invocation_short_name[0] != '\0') {
        (void)fputs(program_invocation_short_name, stderr);
        (void)fputs(": ", stderr);
    }
}

static void
err_soft_flush(void)
{
    /* greppable: CGJ_ERR_SOFT_FFLUSH */
    (void)fflush(stderr);
}

void
vwarnx(const char *szFmt, va_list ap)
{
    err_soft_print_prog();
    if (szFmt != NULL) {
        (void)vfprintf(stderr, szFmt, ap);
    }
    (void)fputc('\n', stderr);
    err_soft_flush();
}

void
vwarn(const char *szFmt, va_list ap)
{
    int nSaved = errno;

    err_soft_print_prog();
    if (szFmt != NULL) {
        (void)vfprintf(stderr, szFmt, ap);
        (void)fputs(": ", stderr);
    }
    (void)fputs(strerror(nSaved), stderr);
    (void)fputc('\n', stderr);
    err_soft_flush();
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
