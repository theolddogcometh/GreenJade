/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * backtrace bring-up: walk frame pointers when available; else 0 frames.
 */
#include <execinfo.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int
backtrace(void **ppBuffer, int nSize)
{
    int n = 0;
    uintptr_t *pFp;
    uintptr_t uSp;
    uintptr_t uPrev = 0;

    if (ppBuffer == NULL || nSize <= 0) {
        return 0;
    }
    /* x86_64: rbp chain (best-effort; -fomit-frame-pointer may leave junk) */
    __asm__ volatile("movq %%rbp, %0" : "=r"(pFp));
    __asm__ volatile("movq %%rsp, %0" : "=r"(uSp));
    while (pFp != NULL && n < nSize) {
        uintptr_t uFp = (uintptr_t)pFp;
        uintptr_t uRet;
        uintptr_t uNext;

        /* Frame must sit above current sp, be aligned, and not runaway. */
        if (uFp < uSp || (uFp & 7u) != 0 || uFp - uSp > (1u << 20)) {
            break;
        }
        if (uPrev != 0 && uFp <= uPrev) {
            break; /* must strictly unwind up the stack */
        }
        uRet = pFp[1];
        uNext = pFp[0];
        if (uRet < 0x1000u) {
            break;
        }
        ppBuffer[n++] = (void *)uRet;
        uPrev = uFp;
        pFp = (uintptr_t *)uNext;
    }
    return n;
}

char **
backtrace_symbols(void *const *ppBuffer, int nSize)
{
    char **pp;
    int i;
    size_t cb;

    if (ppBuffer == NULL || nSize <= 0) {
        return NULL;
    }
    cb = (size_t)nSize * sizeof(char *);
    pp = (char **)malloc(cb + (size_t)nSize * 32u);
    if (pp == NULL) {
        return NULL;
    }
    {
        char *pStr = (char *)(pp + nSize);

        for (i = 0; i < nSize; i++) {
            char aTmp[32];
            uintptr_t u = (uintptr_t)ppBuffer[i];
            int n = 0;
            int k;
            char hex[] = "0123456789abcdef";

            aTmp[n++] = '0';
            aTmp[n++] = 'x';
            for (k = (int)(sizeof(uintptr_t) * 2) - 1; k >= 0; k--) {
                aTmp[n++] = hex[(u >> (k * 4)) & 0xfu];
            }
            aTmp[n] = '\0';
            pp[i] = pStr;
            memcpy(pStr, aTmp, (size_t)n + 1);
            pStr += 32;
        }
    }
    return pp;
}

void
backtrace_symbols_fd(void *const *ppBuffer, int nSize, int nFd)
{
    int i;

    if (ppBuffer == NULL || nSize <= 0 || nFd < 0) {
        return;
    }
    for (i = 0; i < nSize; i++) {
        char aLine[40];
        uintptr_t u = (uintptr_t)ppBuffer[i];
        int n = 0;
        int k;
        char hex[] = "0123456789abcdef";

        aLine[n++] = '0';
        aLine[n++] = 'x';
        for (k = (int)(sizeof(uintptr_t) * 2) - 1; k >= 0; k--) {
            aLine[n++] = hex[(u >> (k * 4)) & 0xfu];
        }
        aLine[n++] = '\n';
        (void)write(nFd, aLine, (size_t)n);
    }
}
