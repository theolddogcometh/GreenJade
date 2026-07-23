/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * __assert_fail, stack-protector, program_invocation_name.
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *program_invocation_name = (char *)"";
char *program_invocation_short_name = (char *)"";
char *__progname = (char *)"";
char *__progname_full = (char *)"";

/* SSP guard (non-zero random-ish); set early if desired */
uintptr_t __stack_chk_guard = 0xdeadbeefcafebabeULL;

/* glibc single-threaded optimization flag (0 = multi-thread safe path) */
int __libc_single_threaded = 0;

void
_libcgj_set_progname(char *szArg0)
{
    char *p;

    if (szArg0 == NULL || szArg0[0] == '\0') {
        return;
    }
    program_invocation_name = szArg0;
    __progname_full = szArg0;
    p = strrchr(szArg0, '/');
    if (p != NULL && p[1] != '\0') {
        program_invocation_short_name = p + 1;
        __progname = p + 1;
    } else {
        program_invocation_short_name = szArg0;
        __progname = szArg0;
    }
}

void
__stack_chk_fail(void)
{
    static const char sz[] = "libcgj: stack smashing detected\n";

    (void)write(2, sz, sizeof(sz) - 1);
    abort();
}

void
__assert_fail(const char *szExpr, const char *szFile, unsigned nLine,
              const char *szFunc)
{
    char aBuf[256];
    size_t n = 0;
    const char *p;

    (void)szFunc;
    p = "libcgj: assertion failed: ";
    while (*p && n + 1 < sizeof(aBuf)) {
        aBuf[n++] = *p++;
    }
    if (szExpr != NULL) {
        while (*szExpr && n + 1 < sizeof(aBuf)) {
            aBuf[n++] = *szExpr++;
        }
    }
    if (n + 1 < sizeof(aBuf)) {
        aBuf[n++] = '\n';
    }
    (void)szFile;
    (void)nLine;
    (void)write(2, aBuf, n);
    abort();
}
