/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Minimal fnmatch: *, ?, and literal match (+ optional CASEFOLD).
 */
#include <fnmatch.h>
#include <stddef.h>

static int
eqc(int a, int b, int fFold)
{
    if (fFold) {
        if (a >= 'A' && a <= 'Z') {
            a = a - 'A' + 'a';
        }
        if (b >= 'A' && b <= 'Z') {
            b = b - 'A' + 'a';
        }
    }
    return a == b;
}

int
fnmatch(const char *szPattern, const char *szString, int nFlags)
{
    const char *p;
    const char *s;
    int fFold = (nFlags & FNM_CASEFOLD) != 0;
    int fPath = (nFlags & FNM_PATHNAME) != 0;

    if (szPattern == NULL || szString == NULL) {
        return FNM_NOMATCH;
    }
    p = szPattern;
    s = szString;
    while (*p != '\0') {
        if (*p == '*') {
            while (*p == '*') {
                p++;
            }
            if (*p == '\0') {
                if (fPath) {
                    while (*s != '\0') {
                        if (*s == '/') {
                            return FNM_NOMATCH;
                        }
                        s++;
                    }
                }
                return 0;
            }
            while (*s != '\0') {
                if (fPath && *s == '/') {
                    return FNM_NOMATCH;
                }
                if (fnmatch(p, s, nFlags) == 0) {
                    return 0;
                }
                s++;
            }
            return FNM_NOMATCH;
        }
        if (*p == '?') {
            if (*s == '\0') {
                return FNM_NOMATCH;
            }
            if (fPath && *s == '/') {
                return FNM_NOMATCH;
            }
            p++;
            s++;
            continue;
        }
        if (*s == '\0' || !eqc((unsigned char)*p, (unsigned char)*s, fFold)) {
            return FNM_NOMATCH;
        }
        p++;
        s++;
    }
    return (*s == '\0') ? 0 : FNM_NOMATCH;
}
