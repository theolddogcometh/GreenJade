/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Soft fnmatch: *, ?, [...], escape, PATHNAME / PERIOD / NOESCAPE / CASEFOLD.
 * Clean-room pure C (no locale ctype tables).
 */
#include <fnmatch.h>
#include <stddef.h>

static int
eqc(int nA, int nB, int fFold)
{
    if (fFold) {
        if (nA >= 'A' && nA <= 'Z') {
            nA = nA - 'A' + 'a';
        }
        if (nB >= 'A' && nB <= 'Z') {
            nB = nB - 'A' + 'a';
        }
    }
    return nA == nB;
}

/* Match one character against a [...] class; advances *pp past ']'. */
static int
match_bracket(const char **pp, int nCh, int fFold)
{
    const char *p = *pp;
    int fNeg = 0;
    int fMatched = 0;
    int nPrev = -1;
    int fHavePrev = 0;

    if (*p == '!' || *p == '^') {
        fNeg = 1;
        p++;
    }
    /* Empty or missing ']' → no match (soft fail closed). */
    if (*p == '\0') {
        *pp = p;
        return 0;
    }
    /* Leading ']' is literal. */
    do {
        if (*p == ']' && fHavePrev) {
            p++;
            break;
        }
        if (*p == '\0') {
            break;
        }
        if (*p == '-' && fHavePrev && p[1] != '\0' && p[1] != ']') {
            int nLo = nPrev;
            int nHi = (unsigned char)p[1];

            p += 2;
            if (fFold) {
                if (nLo >= 'A' && nLo <= 'Z') {
                    nLo = nLo - 'A' + 'a';
                }
                if (nHi >= 'A' && nHi <= 'Z') {
                    nHi = nHi - 'A' + 'a';
                }
                if (nCh >= 'A' && nCh <= 'Z') {
                    nCh = nCh - 'A' + 'a';
                }
            }
            if (nLo > nHi) {
                int nT = nLo;

                nLo = nHi;
                nHi = nT;
            }
            if (nCh >= nLo && nCh <= nHi) {
                fMatched = 1;
            }
            fHavePrev = 0;
            nPrev = -1;
            continue;
        }
        if (eqc((unsigned char)*p, nCh, fFold)) {
            fMatched = 1;
        }
        nPrev = (unsigned char)*p;
        fHavePrev = 1;
        p++;
    } while (*p != '\0');

    /* Consume trailing ']' if still present after loop edge cases. */
    if (*p == ']') {
        p++;
    }
    *pp = p;
    return fNeg ? !fMatched : fMatched;
}

int
fnmatch(const char *szPattern, const char *szString, int nFlags)
{
    const char *p;
    const char *s;
    int fFold = (nFlags & FNM_CASEFOLD) != 0;
    int fPath = (nFlags & FNM_PATHNAME) != 0;
    int fNoEsc = (nFlags & FNM_NOESCAPE) != 0;
    int fPeriod = (nFlags & FNM_PERIOD) != 0;

    if (szPattern == NULL || szString == NULL) {
        return FNM_NOMATCH;
    }
    p = szPattern;
    s = szString;

    /* Leading period: pattern must also start with '.' when FNM_PERIOD. */
    if (fPeriod && *s == '.' && *p != '.' && *p != '\0') {
        /* Also after '/' under PATHNAME — handled in the loop below. */
        if (s == szString) {
            return FNM_NOMATCH;
        }
    }

    while (*p != '\0') {
        if (*p == '\\' && !fNoEsc) {
            p++;
            if (*p == '\0') {
                return FNM_NOMATCH;
            }
            if (*s == '\0' || !eqc((unsigned char)*p, (unsigned char)*s, fFold)) {
                return FNM_NOMATCH;
            }
            p++;
            s++;
            continue;
        }
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
                if (fPeriod && *s == '.' &&
                    (s == szString || (fPath && s > szString && s[-1] == '/'))) {
                    /* Soft: do not let '*' eat a leading period segment. */
                    if (*p != '.') {
                        return FNM_NOMATCH;
                    }
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
            if (fPeriod && *s == '.' &&
                (s == szString || (fPath && s > szString && s[-1] == '/'))) {
                return FNM_NOMATCH;
            }
            p++;
            s++;
            continue;
        }
        if (*p == '[') {
            const char *pCls = p + 1;

            if (*s == '\0') {
                return FNM_NOMATCH;
            }
            if (fPath && *s == '/') {
                return FNM_NOMATCH;
            }
            if (fPeriod && *s == '.' &&
                (s == szString || (fPath && s > szString && s[-1] == '/'))) {
                return FNM_NOMATCH;
            }
            if (!match_bracket(&pCls, (unsigned char)*s, fFold)) {
                return FNM_NOMATCH;
            }
            p = pCls;
            s++;
            continue;
        }
        if (*s == '\0' || !eqc((unsigned char)*p, (unsigned char)*s, fFold)) {
            return FNM_NOMATCH;
        }
        /* Crossing into a new path segment under FNM_PERIOD. */
        if (fPath && *s == '/' && s[1] == '.' && p[1] != '.' && p[1] != '\0') {
            /* period check applies to the next component on next iteration */
        }
        p++;
        s++;
    }
    return (*s == '\0') ? 0 : FNM_NOMATCH;
}
