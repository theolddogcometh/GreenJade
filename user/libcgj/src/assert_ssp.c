/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * __assert_fail, stack-protector, program_invocation_name.
 *
 * greppable: CGJ_ASSERT_SOFT_FILE_LINE
 * greppable: CGJ_SSP_SOFT_GUARD
 *
 * Soft deepen: write file:line:function into assert diagnostics without
 * stdio; keep freestanding write(2) path. __stack_chk_fail_local lives in
 * graph batch surface — not redefined here.
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
/* greppable: CGJ_SSP_SOFT_GUARD */
uintptr_t __stack_chk_guard = 0xdeadbeefcafebabeULL;

/* glibc single-threaded optimization flag (0 = multi-thread safe path) */
int __libc_single_threaded = 0;

void
_libcgj_set_progname(char *szArg0)
{
    char *pSlash;

    if (szArg0 == NULL || szArg0[0] == '\0') {
        return;
    }
    program_invocation_name = szArg0;
    __progname_full = szArg0;
    pSlash = strrchr(szArg0, '/');
    if (pSlash != NULL && pSlash[1] != '\0') {
        program_invocation_short_name = pSlash + 1;
        __progname = pSlash + 1;
    } else {
        program_invocation_short_name = szArg0;
        __progname = szArg0;
    }
}

/*
 * Append szSrc into aBuf, capped so a NUL can always be stored.
 * Returns new length (excluding NUL).
 */
static size_t
assert_soft_append(char *aBuf, size_t cbCap, size_t nLen, const char *szSrc)
{
    if (aBuf == NULL || cbCap == 0) {
        return nLen;
    }
    if (szSrc == NULL) {
        return nLen;
    }
    while (*szSrc != '\0' && nLen + 1u < cbCap) {
        aBuf[nLen++] = *szSrc++;
    }
    return nLen;
}

/* Soft decimal for unsigned line numbers (no stdio). */
static size_t
assert_soft_append_u(char *aBuf, size_t cbCap, size_t nLen, unsigned uVal)
{
    char  aDig[16];
    int   nDig = 0;
    unsigned u = uVal;

    if (u == 0u) {
        aDig[nDig++] = '0';
    } else {
        while (u > 0u && nDig < (int)sizeof(aDig)) {
            aDig[nDig++] = (char)('0' + (u % 10u));
            u /= 10u;
        }
    }
    while (nDig > 0 && nLen + 1u < cbCap) {
        aBuf[nLen++] = aDig[--nDig];
    }
    return nLen;
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
    char   aBuf[384];
    size_t nLen = 0;

    /* greppable: CGJ_ASSERT_SOFT_FILE_LINE */
    nLen = assert_soft_append(aBuf, sizeof(aBuf), nLen,
                              "libcgj: assertion failed: ");
    if (szExpr != NULL) {
        nLen = assert_soft_append(aBuf, sizeof(aBuf), nLen, szExpr);
    }
    if (szFile != NULL) {
        nLen = assert_soft_append(aBuf, sizeof(aBuf), nLen, " (");
        nLen = assert_soft_append(aBuf, sizeof(aBuf), nLen, szFile);
        nLen = assert_soft_append(aBuf, sizeof(aBuf), nLen, ":");
        nLen = assert_soft_append_u(aBuf, sizeof(aBuf), nLen, nLine);
        if (szFunc != NULL && szFunc[0] != '\0') {
            nLen = assert_soft_append(aBuf, sizeof(aBuf), nLen, " in ");
            nLen = assert_soft_append(aBuf, sizeof(aBuf), nLen, szFunc);
        }
        nLen = assert_soft_append(aBuf, sizeof(aBuf), nLen, ")");
    }
    if (nLen + 1u < sizeof(aBuf)) {
        aBuf[nLen++] = '\n';
    }
    (void)write(2, aBuf, nLen);
    abort();
}
