/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
#include <gj/types.h>

void *
memset(void *pDst, int nFill, size_t cbCount)
{
    u8 *pOut = (u8 *)pDst;

    while (cbCount--) {
        *pOut++ = (u8)nFill;
    }
    return pDst;
}

void *
memcpy(void *pDst, const void *pSrc, size_t cbCount)
{
    u8 *pOut = (u8 *)pDst;
    const u8 *pIn = (const u8 *)pSrc;

    while (cbCount--) {
        *pOut++ = *pIn++;
    }
    return pDst;
}

int
memcmp(const void *pA, const void *pB, size_t cbCount)
{
    const u8 *pLeft = (const u8 *)pA;
    const u8 *pRight = (const u8 *)pB;

    while (cbCount--) {
        if (*pLeft != *pRight) {
            return (int)*pLeft - (int)*pRight;
        }
        pLeft++;
        pRight++;
    }
    return 0;
}

size_t
strlen(const char *szText)
{
    size_t cChars = 0;

    while (szText[cChars]) {
        cChars++;
    }
    return cChars;
}
