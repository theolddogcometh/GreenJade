/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * wcwidth / wcswidth — C/POSIX column widths (ASCII + simple Unicode).
 * Integer-only; no SSE doubles.
 *
 * greppable: CGJ_WCWIDTH_SOFT_WEOF
 * greppable: CGJ_WCWIDTH_SOFT_ZW
 * greppable: CGJ_WCWIDTH_SOFT_WIDE
 *
 * Soft deepen: WEOF → -1, extra zero-width / format ranges, soft East
 * Asian wide table expansion for common bring-up planes.
 */
#include <wchar.h>

int
wcwidth(wchar_t wc)
{
    /* greppable: CGJ_WCWIDTH_SOFT_WEOF */
    if (wc == (wchar_t)WEOF || wc == 0) {
        /* NUL width 0; WEOF not a printable column */
        if (wc == 0) {
            return 0;
        }
        return -1;
    }
    /* C0/C1 controls */
    if (wc < 32 || (wc >= 0x7f && wc < 0xa0)) {
        return -1;
    }
    /* ASCII printables */
    if (wc < 0x7f) {
        return 1;
    }
    /* greppable: CGJ_WCWIDTH_SOFT_ZW */
    /* Combining marks + common zero-width / format */
    if ((wc >= 0x0300 && wc <= 0x036f) || (wc >= 0x1ab0 && wc <= 0x1aff) ||
        (wc >= 0x1dc0 && wc <= 0x1dff) || (wc >= 0x20d0 && wc <= 0x20ff) ||
        (wc >= 0xfe20 && wc <= 0xfe2f) ||
        (wc >= 0x200b && wc <= 0x200f) || /* ZWSP..RLM */
        (wc >= 0x202a && wc <= 0x202e) || /* bidi format */
        (wc >= 0x2060 && wc <= 0x2064) || /* WJ..invisible plus */
        (wc >= 0x2066 && wc <= 0x2069) || /* bidi isolates */
        wc == 0xfeff ||                   /* BOM / ZWNBSP */
        (wc >= 0xfe00 && wc <= 0xfe0f) || /* variation selectors */
        (wc >= 0xe0100 && wc <= 0xe01ef)) {
        return 0;
    }
    /* greppable: CGJ_WCWIDTH_SOFT_WIDE */
    /* Wide East Asian (bring-up ranges) */
    if ((wc >= 0x1100 && wc <= 0x115f) || (wc >= 0x2329 && wc <= 0x232a) ||
        (wc >= 0x2e80 && wc <= 0xa4cf) || (wc >= 0xac00 && wc <= 0xd7a3) ||
        (wc >= 0xf900 && wc <= 0xfaff) || (wc >= 0xfe10 && wc <= 0xfe19) ||
        (wc >= 0xfe30 && wc <= 0xfe6f) || (wc >= 0xff00 && wc <= 0xff60) ||
        (wc >= 0xffe0 && wc <= 0xffe6) ||
        (wc >= 0x1f300 && wc <= 0x1f9ff) || /* emoji soft wide */
        (wc >= 0x20000 && wc <= 0x2fffd) ||
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
        int nW = wcwidth(sz[i]);

        if (nW < 0) {
            return -1;
        }
        /* Soft saturate toward INT_MAX-ish without pulling limits.h. */
        if (nTotal > 0x7fffffff - nW) {
            return -1;
        }
        nTotal += nW;
    }
    return nTotal;
}
