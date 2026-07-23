/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room getopt + getopt_long (POSIX/glibc-shaped subset).
 */
#include <getopt.h>
#include <string.h>
#include <unistd.h>

char *optarg;
int   optind = 1;
int   opterr = 1;
int   optopt;

static int g_nPos = 1; /* index within current argv[optind] after '-' */

int
getopt(int argc, char *const argv[], const char *szOptstring)
{
    const char *p;
    int c;
    int fArg;

    if (szOptstring == NULL || argv == NULL) {
        return -1;
    }
    if (optind >= argc || argv[optind] == NULL) {
        return -1;
    }
    if (argv[optind][0] != '-' || argv[optind][1] == '\0') {
        return -1;
    }
    if (strcmp(argv[optind], "--") == 0) {
        optind++;
        g_nPos = 1;
        return -1;
    }
    /* Long options are not handled here; getopt_long peels them first. */
    if (argv[optind][1] == '-') {
        return -1;
    }
    c = (unsigned char)argv[optind][g_nPos];
    g_nPos++;
    optopt = c;
    p = strchr(szOptstring, c);
    if (p == NULL) {
        if (opterr && szOptstring[0] != ':') {
            (void)write(2, "libcgj: illegal option\n", 23);
        }
        if (argv[optind][g_nPos] == '\0') {
            optind++;
            g_nPos = 1;
        }
        return (szOptstring[0] == ':') ? ':' : '?';
    }
    fArg = (p[1] == ':');
    if (fArg) {
        if (argv[optind][g_nPos] != '\0') {
            optarg = &argv[optind][g_nPos];
            optind++;
            g_nPos = 1;
        } else if (optind + 1 < argc) {
            optind++;
            optarg = argv[optind];
            optind++;
            g_nPos = 1;
        } else {
            if (opterr && szOptstring[0] != ':') {
                (void)write(2, "libcgj: option requires an argument\n", 36);
            }
            optarg = NULL;
            if (argv[optind][g_nPos] == '\0') {
                optind++;
                g_nPos = 1;
            }
            return (szOptstring[0] == ':') ? ':' : '?';
        }
    } else {
        optarg = NULL;
        if (argv[optind][g_nPos] == '\0') {
            optind++;
            g_nPos = 1;
        }
    }
    return c;
}

static int
match_long(const char *szArg, const struct option *pLongopts, int *pIdx,
           const char **ppEq)
{
    size_t nName;
    int i;
    int nHit = -1;
    int nHits = 0;
    const char *pEq;

    *ppEq = NULL;
    if (szArg == NULL || pLongopts == NULL) {
        return -1;
    }
    pEq = strchr(szArg, '=');
    nName = (pEq != NULL) ? (size_t)(pEq - szArg) : strlen(szArg);
    for (i = 0; pLongopts[i].name != NULL; i++) {
        size_t nOpt = strlen(pLongopts[i].name);

        if (nOpt == nName && strncmp(pLongopts[i].name, szArg, nName) == 0) {
            nHit = i;
            nHits++;
        } else if (nOpt > nName &&
                   strncmp(pLongopts[i].name, szArg, nName) == 0) {
            /* unique prefix match */
            if (nHits == 0) {
                nHit = i;
            }
            nHits++;
        }
    }
    if (nHits != 1) {
        return -1;
    }
    if (pIdx != NULL) {
        *pIdx = nHit;
    }
    *ppEq = pEq;
    return nHit;
}

static int
getopt_long_inner(int argc, char *const argv[], const char *szOptstring,
                  const struct option *pLongopts, int *pLongindex, int fOnly)
{
    const char *sz;
    const char *pEq;
    int nIdx;
    int n;
    const struct option *pO;

    (void)fOnly;
    if (argv == NULL || optind >= argc || argv[optind] == NULL) {
        return -1;
    }
    sz = argv[optind];
    if (sz[0] != '-' || sz[1] == '\0') {
        return -1;
    }
    if (strcmp(sz, "--") == 0) {
        optind++;
        g_nPos = 1;
        return -1;
    }

    /* Long form: --name or --name=arg (and -name if long_only) */
    if (sz[1] == '-' || fOnly) {
        const char *szName = (sz[1] == '-') ? sz + 2 : sz + 1;

        if (sz[1] == '-' && sz[2] == '\0') {
            optind++;
            g_nPos = 1;
            return -1;
        }
        if (pLongopts == NULL) {
            if (sz[1] == '-') {
                optopt = 0;
                optind++;
                g_nPos = 1;
                return '?';
            }
            /* fall through to short getopt when long_only and not -- */
        } else {
            n = match_long(szName, pLongopts, &nIdx, &pEq);
            if (n >= 0) {
                pO = &pLongopts[nIdx];
                if (pLongindex != NULL) {
                    *pLongindex = nIdx;
                }
                optarg = NULL;
                if (pO->has_arg == required_argument ||
                    pO->has_arg == optional_argument) {
                    if (pEq != NULL) {
                        optarg = (char *)(pEq + 1);
                    } else if (pO->has_arg == required_argument) {
                        if (optind + 1 < argc) {
                            optind++;
                            optarg = argv[optind];
                        } else {
                            if (opterr &&
                                (szOptstring == NULL || szOptstring[0] != ':')) {
                                (void)write(2,
                                            "libcgj: option requires an argument\n",
                                            36);
                            }
                            optind++;
                            g_nPos = 1;
                            optopt = pO->val;
                            return (szOptstring != NULL && szOptstring[0] == ':')
                                       ? ':'
                                       : '?';
                        }
                    }
                } else if (pEq != NULL) {
                    /* argument given to no_argument option */
                    if (opterr) {
                        (void)write(2, "libcgj: option doesn't allow an argument\n",
                                    41);
                    }
                    optind++;
                    g_nPos = 1;
                    optopt = pO->val;
                    return '?';
                }
                optind++;
                g_nPos = 1;
                if (pO->flag != NULL) {
                    *pO->flag = pO->val;
                    return 0;
                }
                return pO->val;
            }
            if (sz[1] == '-') {
                /* unknown long option */
                if (opterr && (szOptstring == NULL || szOptstring[0] != ':')) {
                    (void)write(2, "libcgj: unrecognized option\n", 28);
                }
                optind++;
                g_nPos = 1;
                optopt = 0;
                return '?';
            }
            /* long_only: fall through to short options */
        }
    }

    return getopt(argc, argv, szOptstring != NULL ? szOptstring : "");
}

int
getopt_long(int argc, char *const argv[], const char *szOptstring,
            const struct option *pLongopts, int *pLongindex)
{
    return getopt_long_inner(argc, argv, szOptstring, pLongopts, pLongindex, 0);
}

int
getopt_long_only(int argc, char *const argv[], const char *szOptstring,
                 const struct option *pLongopts, int *pLongindex)
{
    return getopt_long_inner(argc, argv, szOptstring, pLongopts, pLongindex, 1);
}
