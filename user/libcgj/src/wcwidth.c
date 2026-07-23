/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * wcwidth / wcswidth — C/POSIX column widths (ASCII + simple Unicode).
 * Integer-only; no SSE doubles.
 */
#include <wchar.h>

int
wcwidth(wchar_t wc)
{
    if (wc == 0) {
        return 0;
    }
    /* C0/C1 controls */
    if (wc < 32 || (wc >= 0x7f && wc < 0xa0)) {
        return -1;
    }
    /* ASCII printables */
    if (wc < 0x7f) {
        return 1;
    }
    /* Combining marks (common block) */
    if ((wc >= 0x0300 && wc <= 0x036f) || (wc >= 0x1ab0 && wc <= 0x1aff) ||
        (wc >= 0x1dc0 && wc <= 0x1dff) || (wc >= 0x20d0 && wc <= 0x20ff) ||
        (wc >= 0xfe20 && wc <= 0xfe2f)) {
        return 0;
    }
    /* Wide East Asian (bring-up ranges) */
    if ((wc >= 0x1100 && wc <= 0x115f) || (wc >= 0x2329 && wc <= 0x232a) ||
        (wc >= 0x2e80 && wc <= 0xa4cf) || (wc >= 0xac00 && wc <= 0xd7a3) ||
        (wc >= 0xf900 && wc <= 0xfaff) || (wc >= 0xfe10 && wc <= 0xfe19) ||
        (wc >= 0xfe30 && wc <= 0xfe6f) || (wc >= 0xff00 && wc <= 0xff60) ||
        (wc >= 0xffe0 && wc <= 0xffe6) || (wc >= 0x20000 && wc <= 0x2fffd) ||
        (wc >= 0x30000 && wc <= 0x3fffd)) {
        return 2;
    }
    return 1;
}

int
wcswidth(const wchar_t *sz, size_t n)
{
    int nTotal = 0;
    size_t i;

    if (sz == NULL) {
        return -1;
    }
    for (i = 0; i < n && sz[i] != 0; i++) {
        int w = wcwidth(sz[i]);

        if (w < 0) {
            return -1;
        }
        nTotal += w;
    }
    return nTotal;
}
