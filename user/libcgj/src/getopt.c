/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Clean-room getopt + getopt_long (POSIX/glibc-shaped soft subset).
 * Supports leading '+' / '-' / ':', optional "::" args, optind==0 reset.
 */
#include <getopt.h>
#include <string.h>
#include <unistd.h>

char *optarg;
int   optind = 1;
int   opterr = 1;
int   optopt;

static int g_nPos = 1; /* index within current argv[optind] after '-' */

static void
opt_reset_if_needed(void)
{
    /* glibc: optind == 0 forces a full reset. */
    if (optind == 0) {
        optind = 1;
        g_nPos = 1;
        optarg = NULL;
        optopt = 0;
    }
}

/* Skip leading mode chars in optstring: '+', '-', then optional ':'. */
static const char *
optstring_body(const char *szOptstring, int *pfColon, int *pfPlus, int *pfDash)
{
    const char *p = szOptstring;

    *pfColon = 0;
    *pfPlus = 0;
    *pfDash = 0;
    if (p == NULL) {
        return "";
    }
    if (*p == '+') {
        *pfPlus = 1;
        p++;
    } else if (*p == '-') {
        *pfDash = 1;
        p++;
    }
    if (*p == ':') {
        *pfColon = 1;
        p++;
    }
    return p;
}

static int
opt_err_colon(const char *szOptstring)
{
    int fColon = 0;
    int fPlus = 0;
    int fDash = 0;

    (void)optstring_body(szOptstring, &fColon, &fPlus, &fDash);
    return fColon;
}

int
getopt(int argc, char *const argv[], const char *szOptstring)
{
    const char *szBody;
    const char *p;
    int c;
    int fArg;
    int fOptArg;
    int fColon = 0;
    int fPlus = 0;
    int fDash = 0;

    opt_reset_if_needed();
    if (szOptstring == NULL || argv == NULL) {
        return -1;
    }
    szBody = optstring_body(szOptstring, &fColon, &fPlus, &fDash);
    (void)fPlus; /* default already stops at first non-option */

    if (optind >= argc || argv[optind] == NULL) {
        return -1;
    }

    /* Continue a clustered short-option argv element. */
    if (g_nPos > 1 && argv[optind] != NULL && argv[optind][0] == '-' &&
        argv[optind][1] != '\0' && argv[optind][1] != '-') {
        /* fall through using g_nPos */
    } else {
        if (argv[optind][0] != '-' || argv[optind][1] == '\0') {
            if (fDash) {
                /* Soft GNU: return non-options as 1 with optarg set. */
                optarg = argv[optind];
                optind++;
                g_nPos = 1;
                return 1;
            }
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
        g_nPos = 1;
    }

    c = (unsigned char)argv[optind][g_nPos];
    g_nPos++;
    optopt = c;
    p = strchr(szBody, c);
    if (p == NULL) {
        if (opterr && !fColon) {
            (void)write(2, "libcgj: illegal option\n", 23);
        }
        if (argv[optind][g_nPos] == '\0') {
            optind++;
            g_nPos = 1;
        }
        return fColon ? ':' : '?';
    }
    fArg = (p[1] == ':');
    fOptArg = (fArg && p[2] == ':');
    if (fArg) {
        if (argv[optind][g_nPos] != '\0') {
            optarg = &argv[optind][g_nPos];
            optind++;
            g_nPos = 1;
        } else if (fOptArg) {
            /* Optional argument absent. */
            optarg = NULL;
            optind++;
            g_nPos = 1;
        } else if (optind + 1 < argc) {
            optind++;
            optarg = argv[optind];
            optind++;
            g_nPos = 1;
        } else {
            if (opterr && !fColon) {
                (void)write(2, "libcgj: option requires an argument\n", 36);
            }
            optarg = NULL;
            if (argv[optind][g_nPos] == '\0') {
                optind++;
                g_nPos = 1;
            }
            /* Missing required arg: ':' if optstring mode, else '?'. */
            return fColon ? ':' : '?';
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
    if (nName == 0) {
        return -1;
    }
    for (i = 0; pLongopts[i].name != NULL; i++) {
        size_t nOpt = strlen(pLongopts[i].name);

        if (nOpt == nName && strncmp(pLongopts[i].name, szArg, nName) == 0) {
            nHit = i;
            nHits = 1;
            /* Exact match wins immediately. */
            break;
        }
        if (nOpt > nName && strncmp(pLongopts[i].name, szArg, nName) == 0) {
            if (nHits == 0) {
                nHit = i;
            }
            nHits++;
        }
    }
    /* If exact matched above, nHits==1 and nHit set. Re-scan prefixes only
     * when no exact hit. */
    if (nHits == 1 && nHit >= 0 &&
        strlen(pLongopts[nHit].name) == nName) {
        if (pIdx != NULL) {
            *pIdx = nHit;
        }
        *ppEq = pEq;
        return nHit;
    }
    /* Prefix-only path: recount unique prefixes if exact not taken. */
    if (nHits != 1) {
        nHit = -1;
        nHits = 0;
        for (i = 0; pLongopts[i].name != NULL; i++) {
            size_t nOpt = strlen(pLongopts[i].name);

            if (nOpt >= nName &&
                strncmp(pLongopts[i].name, szArg, nName) == 0) {
                if (nOpt == nName) {
                    nHit = i;
                    nHits = 1;
                    break;
                }
                if (nHits == 0) {
                    nHit = i;
                }
                nHits++;
            }
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
    int fColon;
    const struct option *pO;

    opt_reset_if_needed();
    fColon = opt_err_colon(szOptstring);

    if (argv == NULL || optind >= argc || argv[optind] == NULL) {
        return -1;
    }
    sz = argv[optind];
    if (sz[0] != '-' || sz[1] == '\0') {
        /* Let getopt handle dash-mode non-options. */
        return getopt(argc, argv, szOptstring != NULL ? szOptstring : "");
    }
    if (strcmp(sz, "--") == 0) {
        optind++;
        g_nPos = 1;
        return -1;
    }

    /* Long form: --name or --name=arg (and -name if long_only). */
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
                            if (opterr && !fColon) {
                                (void)write(2,
                                            "libcgj: option requires an argument\n",
                                            36);
                            }
                            optind++;
                            g_nPos = 1;
                            optopt = pO->val;
                            return fColon ? ':' : '?';
                        }
                    }
                    /* optional_argument with no '=' and no next argv: NULL */
                } else if (pEq != NULL) {
                    if (opterr && !fColon) {
                        (void)write(2,
                                    "libcgj: option doesn't allow an argument\n",
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
                if (opterr && !fColon) {
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
