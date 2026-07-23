/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * GNU error.h family — error / error_at_line.
 */
#include <error.h>
#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

unsigned int error_message_count = 0;
int error_one_per_line = 0;
void (*error_print_progname)(void) = NULL;

extern char *program_invocation_name;

static void
print_prog(void)
{
    if (error_print_progname != NULL) {
        error_print_progname();
        return;
    }
    if (program_invocation_name != NULL && program_invocation_name[0] != '\0') {
        (void)fputs(program_invocation_name, stderr);
        (void)fputs(": ", stderr);
    }
}

void
error(int nStatus, int nErrnum, const char *szFmt, ...)
{
    va_list ap;

    print_prog();
    if (szFmt != NULL) {
        va_start(ap, szFmt);
        (void)vfprintf(stderr, szFmt, ap);
        va_end(ap);
    }
    if (nErrnum != 0) {
        (void)fputs(": ", stderr);
        (void)fputs(strerror(nErrnum), stderr);
    }
    (void)fputc('\n', stderr);
    error_message_count++;
    if (nStatus != 0) {
        exit(nStatus);
    }
}

void
error_at_line(int nStatus, int nErrnum, const char *szFile, unsigned nLine,
              const char *szFmt, ...)
{
    static const char *szLastFile;
    static unsigned nLastLine;
    va_list ap;

    if (error_one_per_line && szFile != NULL && szLastFile != NULL &&
        strcmp(szFile, szLastFile) == 0 && nLine == nLastLine) {
        return;
    }
    szLastFile = szFile;
    nLastLine = nLine;

    print_prog();
    if (szFile != NULL) {
        (void)fprintf(stderr, "%s:%u: ", szFile, nLine);
    }
    if (szFmt != NULL) {
        va_start(ap, szFmt);
        (void)vfprintf(stderr, szFmt, ap);
        va_end(ap);
    }
    if (nErrnum != 0) {
        (void)fputs(": ", stderr);
        (void)fputs(strerror(nErrnum), stderr);
    }
    (void)fputc('\n', stderr);
    error_message_count++;
    if (nStatus != 0) {
        exit(nStatus);
    }
}
