/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

char *
setlocale(int nCategory, const char *szLocale)
{
    (void)nCategory;
    /* Bring-up: C locale only */
    if (szLocale == NULL) {
        return (char *)"C";
    }
    if (szLocale[0] == '\0' ||
        (szLocale[0] == 'C' && szLocale[1] == '\0') ||
        (szLocale[0] == 'P' /* POSIX */)) {
        return (char *)"C";
    }
    return (char *)"C";
}

extern void _libcgj_run_atexit(int nCode);

void
exit(int nCode)
{
    _libcgj_run_atexit(nCode);
    _exit(nCode);
}

void
abort(void)
{
    const char sz[] = "libcgj: abort\n";

    (void)write(2, sz, sizeof(sz) - 1);
    _exit(127);
}

int
abs(int n)
{
    return (n < 0) ? -n : n;
}

long
labs(long n)
{
    return (n < 0) ? -n : n;
}

long long
llabs(long long n)
{
    return (n < 0) ? -n : n;
}

int
atoi(const char *sz)
{
    return (int)strtol(sz, NULL, 10);
}

long
atol(const char *sz)
{
    return strtol(sz, NULL, 10);
}

long long
atoll(const char *sz)
{
    return strtoll(sz, NULL, 10);
}

static int
digit_val(int c, int nBase)
{
    if (c >= '0' && c <= '9') {
        c -= '0';
    } else if (c >= 'a' && c <= 'z') {
        c = c - 'a' + 10;
    } else if (c >= 'A' && c <= 'Z') {
        c = c - 'A' + 10;
    } else {
        return -1;
    }
    if (c >= nBase) {
        return -1;
    }
    return c;
}

/*
 * Shared prefix parse for strto*: skip space/sign, resolve base, leave
 * *pp at first digit candidate. Returns 0 ok, -1 bad base (errno set).
 */
static int
strto_prefix(const char *sz, char **ppEnd, int *pnBase, int *pfNeg,
             const char **pp, int fAllowNeg)
{
    int nBase;
    const char *p;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        *pp = NULL;
        return -1;
    }
    p = sz;
    while (isspace((unsigned char)*p)) {
        p++;
    }
    *pfNeg = 0;
    if (*p == '-') {
        if (fAllowNeg) {
            *pfNeg = 1;
        } else {
            *pfNeg = 1; /* strtoul: still accept '-', wrap as unsigned */
        }
        p++;
    } else if (*p == '+') {
        p++;
    }
    nBase = *pnBase;
    if (nBase == 0) {
        if (*p == '0' && (p[1] == 'x' || p[1] == 'X') &&
            digit_val((unsigned char)p[2], 16) >= 0) {
            nBase = 16;
            p += 2;
        } else if (*p == '0') {
            /* Octal: leave '0' for the digit loop so "0" counts as a digit. */
            nBase = 8;
        } else {
            nBase = 10;
        }
    } else if (nBase == 16 && *p == '0' && (p[1] == 'x' || p[1] == 'X') &&
               digit_val((unsigned char)p[2], 16) >= 0) {
        p += 2;
    }
    if (nBase < 2 || nBase > 36) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            *ppEnd = (char *)(uintptr_t)sz;
        }
        *pp = NULL;
        return -1;
    }
    *pnBase = nBase;
    *pp = p;
    return 0;
}

static void
strto_endptr(const char *sz, const char *p, int fAny, char **ppEnd)
{
    if (ppEnd == NULL) {
        return;
    }
    if (!fAny) {
        const char *p0 = sz;

        while (isspace((unsigned char)*p0)) {
            p0++;
        }
        *ppEnd = (char *)(uintptr_t)p0;
    } else {
        *ppEnd = (char *)(uintptr_t)p;
    }
}

long
strtol(const char *sz, char **ppEnd, int nBase)
{
    unsigned long u = 0;
    int fNeg = 0;
    int d;
    int fAny = 0;
    int fOver = 0;
    const char *p;
    unsigned long uLim;
    unsigned long uBase;

    if (strto_prefix(sz, ppEnd, &nBase, &fNeg, &p, 1) != 0) {
        return 0;
    }
    uBase = (unsigned long)nBase;
    /* Max magnitude: LONG_MAX, or LONG_MAX+1 when negative (LONG_MIN). */
    uLim = (unsigned long)LONG_MAX + (fNeg ? 1UL : 0UL);
    while ((d = digit_val((unsigned char)*p, nBase)) >= 0) {
        fAny = 1;
        if (!fOver) {
            if (u > uLim / uBase ||
                (u == uLim / uBase && (unsigned long)d > uLim % uBase)) {
                fOver = 1;
                u = uLim;
            } else {
                u = u * uBase + (unsigned long)d;
            }
        }
        p++;
    }
    strto_endptr(sz, p, fAny, ppEnd);
    if (fOver) {
        errno = ERANGE;
        return fNeg ? LONG_MIN : LONG_MAX;
    }
    if (fNeg) {
        if (u == (unsigned long)LONG_MAX + 1UL) {
            return LONG_MIN;
        }
        return -(long)u;
    }
    return (long)u;
}

unsigned long
strtoul(const char *sz, char **ppEnd, int nBase)
{
    unsigned long u = 0;
    int fNeg = 0;
    int d;
    int fAny = 0;
    int fOver = 0;
    const char *p;
    unsigned long uBase;

    if (strto_prefix(sz, ppEnd, &nBase, &fNeg, &p, 1) != 0) {
        return 0;
    }
    uBase = (unsigned long)nBase;
    while ((d = digit_val((unsigned char)*p, nBase)) >= 0) {
        fAny = 1;
        if (!fOver) {
            if (u > ULONG_MAX / uBase ||
                (u == ULONG_MAX / uBase &&
                 (unsigned long)d > ULONG_MAX % uBase)) {
                fOver = 1;
                u = ULONG_MAX;
            } else {
                u = u * uBase + (unsigned long)d;
            }
        }
        p++;
    }
    strto_endptr(sz, p, fAny, ppEnd);
    if (fOver) {
        errno = ERANGE;
        return ULONG_MAX;
    }
    /* POSIX: leading '-' yields negated unsigned magnitude. */
    return fNeg ? (0UL - u) : u;
}

long long
strtoll(const char *sz, char **ppEnd, int nBase)
{
    unsigned long long u = 0;
    int fNeg = 0;
    int d;
    int fAny = 0;
    int fOver = 0;
    const char *p;
    unsigned long long uLim;
    unsigned long long uBase;

    if (strto_prefix(sz, ppEnd, &nBase, &fNeg, &p, 1) != 0) {
        return 0;
    }
    uBase = (unsigned long long)nBase;
    uLim = (unsigned long long)LLONG_MAX + (fNeg ? 1ULL : 0ULL);
    while ((d = digit_val((unsigned char)*p, nBase)) >= 0) {
        fAny = 1;
        if (!fOver) {
            if (u > uLim / uBase ||
                (u == uLim / uBase &&
                 (unsigned long long)d > uLim % uBase)) {
                fOver = 1;
                u = uLim;
            } else {
                u = u * uBase + (unsigned long long)d;
            }
        }
        p++;
    }
    strto_endptr(sz, p, fAny, ppEnd);
    if (fOver) {
        errno = ERANGE;
        return fNeg ? LLONG_MIN : LLONG_MAX;
    }
    if (fNeg) {
        if (u == (unsigned long long)LLONG_MAX + 1ULL) {
            return LLONG_MIN;
        }
        return -(long long)u;
    }
    return (long long)u;
}

unsigned long long
strtoull(const char *sz, char **ppEnd, int nBase)
{
    unsigned long long u = 0;
    int fNeg = 0;
    int d;
    int fAny = 0;
    int fOver = 0;
    const char *p;
    unsigned long long uBase;

    if (strto_prefix(sz, ppEnd, &nBase, &fNeg, &p, 1) != 0) {
        return 0;
    }
    uBase = (unsigned long long)nBase;
    while ((d = digit_val((unsigned char)*p, nBase)) >= 0) {
        fAny = 1;
        if (!fOver) {
            if (u > ULLONG_MAX / uBase ||
                (u == ULLONG_MAX / uBase &&
                 (unsigned long long)d > ULLONG_MAX % uBase)) {
                fOver = 1;
                u = ULLONG_MAX;
            } else {
                u = u * uBase + (unsigned long long)d;
            }
        }
        p++;
    }
    strto_endptr(sz, p, fAny, ppEnd);
    if (fOver) {
        errno = ERANGE;
        return ULLONG_MAX;
    }
    return fNeg ? (0ULL - u) : u;
}

/*
 * Decimal + optional exponent strtod for OpenSSH convtime (1.5m, 2d).
 * Not a full C99 hex-float / locale numeric. No libm.
 */
double
strtod(const char *sz, char **ppEnd)
{
    const char *p;
    int fNeg = 0;
    int fAny = 0;
    int fExpNeg = 0;
    int nExpVal = 0;
    int nExp;
    int i;
    double v = 0.0;
    double frac = 0.1;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0.0;
    }
    p = sz;
    while (isspace((unsigned char)*p)) {
        p++;
    }
    if (*p == '-') {
        fNeg = 1;
        p++;
    } else if (*p == '+') {
        p++;
    }
    /* C99 inf / infinity / nan (OpenSSH HAVE_DECL_INFINITY). */
    if ((p[0] == 'i' || p[0] == 'I') && (p[1] == 'n' || p[1] == 'N') &&
        (p[2] == 'f' || p[2] == 'F')) {
        p += 3;
        if ((p[0] == 'i' || p[0] == 'I') && (p[1] == 'n' || p[1] == 'N') &&
            (p[2] == 'f' || p[2] == 'F') && (p[3] == 'i' || p[3] == 'I') &&
            (p[4] == 'n' || p[4] == 'N') && (p[5] == 'i' || p[5] == 'I') &&
            (p[6] == 't' || p[6] == 'T') && (p[7] == 'y' || p[7] == 'Y')) {
            p += 8;
        }
        if (ppEnd != NULL) {
            *ppEnd = (char *)(uintptr_t)p;
        }
        return fNeg ? -__builtin_huge_val() : __builtin_huge_val();
    }
    if ((p[0] == 'n' || p[0] == 'N') && (p[1] == 'a' || p[1] == 'A') &&
        (p[2] == 'n' || p[2] == 'N')) {
        p += 3;
        if (*p == '(') {
            p++;
            while (*p != '\0' && *p != ')') {
                p++;
            }
            if (*p == ')') {
                p++;
            }
        }
        if (ppEnd != NULL) {
            *ppEnd = (char *)(uintptr_t)p;
        }
        return fNeg ? -__builtin_nan("") : __builtin_nan("");
    }
    /* C99 hex float 0x1.fp+3 — OpenSSH does not need it; openssl-libcgj may. */
    if (p[0] == '0' && (p[1] == 'x' || p[1] == 'X')) {
        const char *pHex = p + 2;
        unsigned long long uMant = 0ULL;
        int nHex = 0;
        int nFrac = 0;
        int fDot = 0;
        int nBinExp = 0;
        int fExpNeg = 0;
        int fHexAny = 0;
        double dHex;

        while (*pHex != '\0') {
            int nDig = -1;

            if (*pHex == '.' && !fDot) {
                fDot = 1;
                pHex++;
                continue;
            }
            if (*pHex >= '0' && *pHex <= '9') {
                nDig = *pHex - '0';
            } else if (*pHex >= 'a' && *pHex <= 'f') {
                nDig = 10 + (*pHex - 'a');
            } else if (*pHex >= 'A' && *pHex <= 'F') {
                nDig = 10 + (*pHex - 'A');
            }
            if (nDig < 0) {
                break;
            }
            fHexAny = 1;
            if (nHex < 16) {
                uMant = (uMant << 4) | (unsigned long long)nDig;
                nHex++;
                if (fDot) {
                    nFrac += 4;
                }
            } else if (fDot) {
                nFrac += 4;
            } else {
                nBinExp += 4;
            }
            pHex++;
        }
        if (fHexAny && (*pHex == 'p' || *pHex == 'P')) {
            const char *pExp = pHex + 1;
            int nExpVal = 0;

            if (*pExp == '-' || *pExp == '+') {
                if (*pExp == '-') {
                    fExpNeg = 1;
                }
                pExp++;
            }
            if (*pExp >= '0' && *pExp <= '9') {
                pHex = pExp;
                while (*pHex >= '0' && *pHex <= '9') {
                    if (nExpVal < 10000) {
                        nExpVal = nExpVal * 10 + (*pHex - '0');
                    }
                    pHex++;
                }
                nBinExp += fExpNeg ? -nExpVal : nExpVal;
                p = pHex;
                fAny = 1;
                dHex = ldexp((double)uMant, nBinExp - nFrac);
                if (ppEnd != NULL) {
                    *ppEnd = (char *)(uintptr_t)p;
                }
                if (dHex > 1.7976931348623157e+308) {
                    errno = ERANGE;
                    dHex = __builtin_huge_val();
                }
                return fNeg ? -dHex : dHex;
            }
        }
        /* No hex digits or missing p-exp: parse as decimal 0, end at 'x'. */
        if (!fHexAny) {
            p++;
            fAny = 1;
            v = 0.0;
            if (ppEnd != NULL) {
                *ppEnd = (char *)(uintptr_t)p;
            }
            return fNeg ? -v : v;
        }
    }
    while (*p >= '0' && *p <= '9') {
        fAny = 1;
        v = v * 10.0 + (double)(*p - '0');
        p++;
    }
    if (*p == '.') {
        p++;
        while (*p >= '0' && *p <= '9') {
            fAny = 1;
            v += frac * (double)(*p - '0');
            frac *= 0.1;
            p++;
        }
    }
    if (fAny && (*p == 'e' || *p == 'E')) {
        const char *pExp = p + 1;

        fExpNeg = 0;
        if (*pExp == '-' || *pExp == '+') {
            if (*pExp == '-') {
                fExpNeg = 1;
            }
            pExp++;
        }
        if (*pExp >= '0' && *pExp <= '9') {
            p = pExp;
            while (*p >= '0' && *p <= '9') {
                if (nExpVal < 10000) {
                    nExpVal = nExpVal * 10 + (*p - '0');
                }
                p++;
            }
            nExp = fExpNeg ? -nExpVal : nExpVal;
            if (nExp > 0) {
                for (i = 0; i < nExp; i++) {
                    v *= 10.0;
                }
            } else {
                for (i = 0; i < -nExp; i++) {
                    v *= 0.1;
                }
            }
        }
    }
    if (!fAny) {
        if (ppEnd != NULL) {
            *ppEnd = (char *)(uintptr_t)sz;
        }
        return 0.0;
    }
    if (ppEnd != NULL) {
        *ppEnd = (char *)(uintptr_t)p;
    }
    if (v > 1.7976931348623157e+308) {
        errno = ERANGE;
        v = __builtin_huge_val();
    }
    return fNeg ? -v : v;
}

float
strtof(const char *sz, char **ppEnd)
{
    return (float)strtod(sz, ppEnd);
}

long double
strtold(const char *sz, char **ppEnd)
{
    return (long double)strtod(sz, ppEnd);
}

double
atof(const char *sz)
{
    return strtod(sz, NULL);
}

double
__isoc99_strtod(const char *sz, char **ppEnd)
{
    return strtod(sz, ppEnd);
}

float
__isoc99_strtof(const char *sz, char **ppEnd)
{
    return strtof(sz, ppEnd);
}

long double
__isoc99_strtold(const char *sz, char **ppEnd)
{
    return strtold(sz, ppEnd);
}

double
__isoc23_strtod(const char *sz, char **ppEnd)
{
    return strtod(sz, ppEnd);
}

float
__isoc23_strtof(const char *sz, char **ppEnd)
{
    return strtof(sz, ppEnd);
}

long double
__isoc23_strtold(const char *sz, char **ppEnd)
{
    return strtold(sz, ppEnd);
}

/* Process environment (glibc-shaped). Seeded from aux stack by __libc_start_main. */
char **environ;

#define GJ_ENV_MAX 256

static char *g_aEnvOwn[GJ_ENV_MAX];
static int   g_fEnvOwn; /* 1 when environ points at g_aEnvOwn */

static int
env_name_eq(const char *szEnt, const char *szName)
{
    size_t i;

    if (szEnt == NULL || szName == NULL) {
        return 0;
    }
    for (i = 0; szName[i] != '\0'; i++) {
        if (szEnt[i] != szName[i]) {
            return 0;
        }
    }
    return szEnt[i] == '=';
}

void
_libcgj_env_init(char **envp)
{
    int i;

    if (envp == NULL) {
        environ = NULL;
        g_fEnvOwn = 0;
        return;
    }
    /* Prefer stack envp when present; own table used after first setenv. */
    environ = envp;
    g_fEnvOwn = 0;
    for (i = 0; i < GJ_ENV_MAX; i++) {
        g_aEnvOwn[i] = NULL;
    }
}

static int
env_ensure_own(void)
{
    int i;
    int n = 0;

    if (g_fEnvOwn) {
        return 0;
    }
    if (environ != NULL) {
        for (i = 0; environ[i] != NULL && n < GJ_ENV_MAX - 1; i++) {
            size_t cb = strlen(environ[i]) + 1;
            char *p = (char *)malloc(cb);

            if (p == NULL) {
                while (n > 0) {
                    n--;
                    free(g_aEnvOwn[n]);
                    g_aEnvOwn[n] = NULL;
                }
                errno = ENOMEM;
                return -1;
            }
            memcpy(p, environ[i], cb);
            g_aEnvOwn[n++] = p;
        }
    }
    g_aEnvOwn[n] = NULL;
    environ = g_aEnvOwn;
    g_fEnvOwn = 1;
    return 0;
}

char *
getenv(const char *szName)
{
    int i;

    if (szName == NULL || szName[0] == '\0' || environ == NULL) {
        return NULL;
    }
    for (i = 0; environ[i] != NULL; i++) {
        if (env_name_eq(environ[i], szName)) {
            return environ[i] + strlen(szName) + 1;
        }
    }
    return NULL;
}

/* glibc: like getenv but NULL when process is setuid/setgid (bring-up: always getenv). */
char *
secure_getenv(const char *szName)
{
    /* Full secure check needs AT_SECURE auxv; bring-up treats all as unprivileged. */
    return getenv(szName);
}

int
setenv(const char *szName, const char *szVal, int nOverwrite)
{
    size_t cbName;
    size_t cbVal;
    size_t cb;
    char *pEnt;
    int i;
    int nSlot = -1;

    if (szName == NULL || szName[0] == '\0' || strchr(szName, '=') != NULL) {
        errno = EINVAL;
        return -1;
    }
    if (szVal == NULL) {
        szVal = "";
    }
    if (env_ensure_own() != 0) {
        return -1;
    }
    cbName = strlen(szName);
    cbVal = strlen(szVal);
    for (i = 0; environ[i] != NULL; i++) {
        if (env_name_eq(environ[i], szName)) {
            if (!nOverwrite) {
                return 0;
            }
            nSlot = i;
            break;
        }
    }
    if (nSlot < 0) {
        for (i = 0; i < GJ_ENV_MAX - 1; i++) {
            if (environ[i] == NULL) {
                nSlot = i;
                break;
            }
        }
        if (nSlot < 0) {
            errno = ENOMEM;
            return -1;
        }
    }
    cb = cbName + 1 + cbVal + 1;
    pEnt = (char *)malloc(cb);
    if (pEnt == NULL) {
        errno = ENOMEM;
        return -1;
    }
    memcpy(pEnt, szName, cbName);
    pEnt[cbName] = '=';
    memcpy(pEnt + cbName + 1, szVal, cbVal + 1);
    {
        int fNew = (environ[nSlot] == NULL);

        if (environ[nSlot] != NULL) {
            free(environ[nSlot]);
        }
        environ[nSlot] = pEnt;
        if (fNew && nSlot + 1 < GJ_ENV_MAX) {
            environ[nSlot + 1] = NULL;
        }
    }
    return 0;
}

int
unsetenv(const char *szName)
{
    int i;
    int j;

    if (szName == NULL || szName[0] == '\0' || strchr(szName, '=') != NULL) {
        errno = EINVAL;
        return -1;
    }
    if (environ == NULL) {
        return 0;
    }
    if (env_ensure_own() != 0) {
        return -1;
    }
    for (i = 0; environ[i] != NULL; i++) {
        if (env_name_eq(environ[i], szName)) {
            free(environ[i]);
            for (j = i; environ[j] != NULL; j++) {
                environ[j] = environ[j + 1];
            }
            return 0;
        }
    }
    return 0;
}

int
clearenv(void)
{
    int i;

    if (environ == NULL) {
        return 0;
    }
    if (env_ensure_own() != 0) {
        return -1;
    }
    for (i = 0; environ[i] != NULL; i++) {
        free(environ[i]);
        environ[i] = NULL;
    }
    return 0;
}

int
putenv(char *szString)
{
    char *pEq;
    size_t cbName;
    char aName[128];
    int i;
    int nSlot = -1;

    if (szString == NULL) {
        errno = EINVAL;
        return -1;
    }
    pEq = strchr(szString, '=');
    if (pEq == NULL || pEq == szString) {
        errno = EINVAL;
        return -1;
    }
    cbName = (size_t)(pEq - szString);
    if (cbName >= sizeof(aName)) {
        errno = EINVAL;
        return -1;
    }
    memcpy(aName, szString, cbName);
    aName[cbName] = '\0';
    if (env_ensure_own() != 0) {
        return -1;
    }
    for (i = 0; environ[i] != NULL; i++) {
        if (env_name_eq(environ[i], aName)) {
            nSlot = i;
            break;
        }
    }
    if (nSlot < 0) {
        for (i = 0; i < GJ_ENV_MAX - 1; i++) {
            if (environ[i] == NULL) {
                nSlot = i;
                break;
            }
        }
        if (nSlot < 0) {
            errno = ENOMEM;
            return -1;
        }
    }
    /* glibc putenv: take ownership of caller string pointer (no free). */
    if (g_fEnvOwn && environ[nSlot] != NULL && environ[nSlot] != szString) {
        free(environ[nSlot]);
    }
    if (environ[nSlot] == NULL && nSlot + 1 < GJ_ENV_MAX) {
        environ[nSlot + 1] = NULL; /* extend table */
    }
    environ[nSlot] = szString;
    return 0;
}

static void
qsort_swap(unsigned char *pA, unsigned char *pB, size_t cb)
{
    size_t i;
    unsigned char t;

    for (i = 0; i < cb; i++) {
        t = pA[i];
        pA[i] = pB[i];
        pB[i] = t;
    }
}

void
qsort(void *pBase, size_t n, size_t cb,
      int (*pfnCmp)(const void *, const void *))
{
    size_t i;
    size_t j;
    size_t gap;
    unsigned char *p = (unsigned char *)pBase;

    if (pBase == NULL || pfnCmp == NULL || cb == 0 || n < 2) {
        return;
    }
    /*
     * Shellsort (Hibbard gaps): pure-C, in-place, O(n^{3/2}) soft deepen
     * of the prior insertion-only bring-up path. API unchanged.
     */
    for (gap = 1; gap < n / 3; gap = gap * 3 + 1) {
        /* grow */
    }
    while (gap > 0) {
        for (i = gap; i < n; i++) {
            j = i;
            while (j >= gap &&
                   pfnCmp(p + (j - gap) * cb, p + j * cb) > 0) {
                qsort_swap(p + (j - gap) * cb, p + j * cb, cb);
                j -= gap;
            }
        }
        gap /= 3;
    }
}

void *
bsearch(const void *pKey, const void *pBase, size_t n, size_t cb,
        int (*pfnCmp)(const void *, const void *))
{
    size_t lo = 0;
    size_t hi = n;
    const unsigned char *p = (const unsigned char *)pBase;

    if (pKey == NULL || pBase == NULL || pfnCmp == NULL || cb == 0) {
        return NULL;
    }
    while (lo < hi) {
        size_t mid = lo + (hi - lo) / 2;
        int c = pfnCmp(pKey, p + mid * cb);

        if (c == 0) {
            return (void *)(uintptr_t)(p + mid * cb);
        }
        if (c < 0) {
            hi = mid;
        } else {
            lo = mid + 1;
        }
    }
    return NULL;
}

char *
realpath(const char *szPath, char *szResolved)
{
    static char aStatic[4096];
    char aOut[4096];
    char aRest[4096];
    char aLink[4096];
    char *pOut = (szResolved != NULL) ? szResolved : aStatic;
    size_t nOut = 0;
    const char *pWalk;
    int nHops = 0;

    if (szPath == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (szPath[0] == '\0') {
        errno = ENOENT;
        return NULL;
    }
    if (szPath[0] != '/') {
        if (getcwd(aOut, sizeof(aOut)) == NULL) {
            return NULL;
        }
        nOut = strlen(aOut);
        if (nOut + 1 + strlen(szPath) + 1 > sizeof(aRest)) {
            errno = ENAMETOOLONG;
            return NULL;
        }
        memcpy(aRest, aOut, nOut);
        aRest[nOut] = '/';
        memcpy(aRest + nOut + 1, szPath, strlen(szPath) + 1);
    } else if (strlen(szPath) + 1 > sizeof(aRest)) {
        errno = ENAMETOOLONG;
        return NULL;
    } else {
        memcpy(aRest, szPath, strlen(szPath) + 1);
    }

    aOut[0] = '\0';
    nOut = 0;
    pWalk = aRest;
    while (*pWalk != '\0') {
        char aComp[256];
        size_t nC = 0;
        struct stat st;

        while (*pWalk == '/') {
            pWalk++;
        }
        if (*pWalk == '\0') {
            break;
        }
        while (*pWalk != '\0' && *pWalk != '/' && nC + 1 < sizeof(aComp)) {
            aComp[nC++] = *pWalk++;
        }
        aComp[nC] = '\0';
        if (aComp[0] == '.' && aComp[1] == '\0') {
            continue;
        }
        if (aComp[0] == '.' && aComp[1] == '.' && aComp[2] == '\0') {
            while (nOut > 0 && aOut[nOut - 1] != '/') {
                nOut--;
            }
            if (nOut > 0) {
                nOut--;
            }
            aOut[nOut] = '\0';
            continue;
        }
        if (nOut + 1 + nC + 1 > sizeof(aOut)) {
            errno = ENAMETOOLONG;
            return NULL;
        }
        aOut[nOut++] = '/';
        memcpy(aOut + nOut, aComp, nC);
        nOut += nC;
        aOut[nOut] = '\0';
        if (lstat(aOut, &st) != 0) {
            return NULL;
        }
        if (S_ISLNK(st.st_mode)) {
            ssize_t nL;
            char aTail[4096];
            size_t nTail;

            if (nHops++ > 32) {
                errno = ELOOP;
                return NULL;
            }
            nL = readlink(aOut, aLink, sizeof(aLink) - 1);
            if (nL < 0) {
                return NULL;
            }
            aLink[nL] = '\0';
            nTail = strlen(pWalk);
            if (nTail >= sizeof(aTail)) {
                errno = ENAMETOOLONG;
                return NULL;
            }
            memcpy(aTail, pWalk, nTail + 1);
            if (aLink[0] == '/') {
                if ((size_t)nL + nTail + 1 > sizeof(aRest)) {
                    errno = ENAMETOOLONG;
                    return NULL;
                }
                memcpy(aRest, aLink, (size_t)nL);
                memcpy(aRest + nL, aTail, nTail + 1);
            } else {
                while (nOut > 0 && aOut[nOut - 1] != '/') {
                    nOut--;
                }
                aOut[nOut] = '\0';
                if (nOut + (size_t)nL + nTail + 1 > sizeof(aRest)) {
                    errno = ENAMETOOLONG;
                    return NULL;
                }
                memcpy(aRest, aOut, nOut);
                memcpy(aRest + nOut, aLink, (size_t)nL);
                memcpy(aRest + nOut + (size_t)nL, aTail, nTail + 1);
            }
            aOut[0] = '\0';
            nOut = 0;
            pWalk = aRest;
        }
    }
    if (nOut == 0) {
        aOut[0] = '/';
        aOut[1] = '\0';
        nOut = 1;
    }
    memcpy(pOut, aOut, nOut + 1);
    return pOut;
}


static int
template_fill(char *sz)
{
    static unsigned uSeq;
    char *p;
    size_t n = 0;
    unsigned u;
    unsigned v;
    const char *hex = "0123456789abcdefghijklmnopqrstuvwxyz";

    if (sz == NULL) {
        return -1;
    }
    p = sz + strlen(sz);
    while (p > sz && p[-1] == 'X') {
        p--;
        n++;
    }
    if (n < 6) {
        errno = EINVAL;
        return -1;
    }
    u = (unsigned)getpid() ^ (uSeq++ * 2654435761u);
    v = u;
    while (n > 0) {
        n--;
        p[n] = hex[v % 36u];
        v /= 36u;
        if (v == 0) {
            v = u + n + 1u;
        }
    }
    return 0;
}

int
mkstemp(char *szTemplate)
{
    int nFd;
    int i;

    if (szTemplate == NULL) {
        errno = EINVAL;
        return -1;
    }
    for (i = 0; i < 256; i++) {
        if (template_fill(szTemplate) != 0) {
            return -1;
        }
        nFd = open(szTemplate, O_RDWR | O_CREAT | O_EXCL, 0600);
        if (nFd >= 0) {
            return nFd;
        }
        if (errno != EEXIST) {
            return -1;
        }
    }
    errno = EEXIST;
    return -1;
}

char *
mkdtemp(char *szTemplate)
{
    int i;

    if (szTemplate == NULL) {
        errno = EINVAL;
        return NULL;
    }
    for (i = 0; i < 256; i++) {
        if (template_fill(szTemplate) != 0) {
            return NULL;
        }
        if (mkdir(szTemplate, 0700) == 0) {
            return szTemplate;
        }
        if (errno != EEXIST) {
            return NULL;
        }
    }
    errno = EEXIST;
    return NULL;
}

/* OpenBSD-shaped arc4random over getrandom (no RC4; name kept for ABI) */
void
arc4random_buf(void *pBuf, size_t cb)
{
    unsigned char *p;
    size_t off;
    ssize_t n;

    if (pBuf == NULL || cb == 0) {
        return;
    }
    p = (unsigned char *)pBuf;
    off = 0;
    while (off < cb) {
        n = getrandom(p + off, cb - off, 0);
        if (n < 0) {
            /* fall back: weak but progress (fork/pid mix) */
            unsigned u = (unsigned)getpid() ^ (unsigned)(uintptr_t)(p + off);
            size_t i;
            for (i = off; i < cb; i++) {
                u = u * 1664525u + 1013904223u;
                p[i] = (unsigned char)(u >> 16);
            }
            return;
        }
        if (n == 0) {
            break;
        }
        off += (size_t)n;
    }
}

uint32_t
arc4random(void)
{
    uint32_t u = 0;

    arc4random_buf(&u, sizeof(u));
    return u;
}

uint32_t
arc4random_uniform(uint32_t uUpper)
{
    uint32_t uMin;
    uint32_t u;

    if (uUpper < 2u) {
        return 0;
    }
    /* rejection sampling: min = 2^32 % upper, but use 2^32-biased form */
    uMin = (uint32_t)(-uUpper) % uUpper;
    do {
        u = arc4random();
    } while (u < uMin);
    return u % uUpper;
}

/*
 * IEEE-754 bit libm for OpenSSH DUT (strtod / convtime neighbours).
 * No libm.so. Freestanding: do not call __builtin_ldexp (would recurse).
 * Dual DoD B OPEN.
 */
typedef union {
    double d;
    uint64_t u64;
} gj_dbits;

typedef union {
    float f;
    uint32_t u32;
} gj_fbits;

double
copysign(double dX, double dY)
{
    gj_dbits ux;
    gj_dbits uy;

    ux.d = dX;
    uy.d = dY;
    ux.u64 = (ux.u64 & 0x7fffffffffffffffULL) |
             (uy.u64 & 0x8000000000000000ULL);
    return ux.d;
}

float
copysignf(float fX, float fY)
{
    gj_fbits ux;
    gj_fbits uy;

    ux.f = fX;
    uy.f = fY;
    ux.u32 = (ux.u32 & 0x7fffffffu) | (uy.u32 & 0x80000000u);
    return ux.f;
}

static double
gj_trunc(double dX)
{
    gj_dbits ub;
    uint64_t uBits;
    int nExp;
    uint64_t uMask;

    ub.d = dX;
    uBits = ub.u64;
    nExp = (int)((uBits >> 52) & 0x7ffULL) - 1023;
    if (nExp >= 52) {
        return dX;
    }
    if (nExp < 0) {
        ub.u64 = uBits & 0x8000000000000000ULL;
        return ub.d;
    }
    uMask = (1ULL << (52 - nExp)) - 1ULL;
    uBits &= ~uMask;
    ub.u64 = uBits;
    return ub.d;
}

double
ldexp(double dX, int nExp)
{
    gj_dbits ub;
    int nCur;
    int nNew;
    uint64_t uSign;
    uint64_t uFrac;

    ub.d = dX;
    uSign = ub.u64 & 0x8000000000000000ULL;
    nCur = (int)((ub.u64 >> 52) & 0x7ffULL);
    uFrac = ub.u64 & 0x000fffffffffffffULL;
    if (nCur == 0x7ff) {
        return dX;
    }
    if (nCur == 0) {
        if (uFrac == 0ULL) {
            return dX;
        }
        while ((uFrac & 0x0010000000000000ULL) == 0ULL) {
            uFrac <<= 1;
            nExp--;
        }
        uFrac &= 0x000fffffffffffffULL;
        nCur = 1;
    }
    if (nExp > 4000) {
        errno = ERANGE;
        ub.u64 = uSign | 0x7ff0000000000000ULL;
        return ub.d;
    }
    if (nExp < -4000) {
        errno = ERANGE;
        ub.u64 = uSign;
        return ub.d;
    }
    nNew = nCur + nExp;
    if (nNew >= 0x7ff) {
        errno = ERANGE;
        ub.u64 = uSign | 0x7ff0000000000000ULL;
        return ub.d;
    }
    if (nNew <= 0) {
        int nShift = 1 - nNew;

        if (nShift > 52) {
            errno = ERANGE;
            ub.u64 = uSign;
            return ub.d;
        }
        uFrac = (uFrac | 0x0010000000000000ULL) >> nShift;
        ub.u64 = uSign | uFrac;
        return ub.d;
    }
    ub.u64 = uSign | ((uint64_t)nNew << 52) | uFrac;
    return ub.d;
}

float
ldexpf(float fX, int nExp)
{
    return (float)ldexp((double)fX, nExp);
}

double
scalbn(double dX, int nExp)
{
    return ldexp(dX, nExp);
}

float
scalbnf(float fX, int nExp)
{
    return ldexpf(fX, nExp);
}

double
frexp(double dX, int *pExp)
{
    gj_dbits ub;
    int nCur;
    uint64_t uSign;
    uint64_t uFrac;

    ub.d = dX;
    uSign = ub.u64 & 0x8000000000000000ULL;
    nCur = (int)((ub.u64 >> 52) & 0x7ffULL);
    uFrac = ub.u64 & 0x000fffffffffffffULL;
    if (nCur == 0x7ff) {
        if (pExp != NULL) {
            *pExp = 0;
        }
        return dX;
    }
    if (nCur == 0) {
        if (uFrac == 0ULL) {
            if (pExp != NULL) {
                *pExp = 0;
            }
            return dX;
        }
        nCur = -1021;
        while ((uFrac & 0x0010000000000000ULL) == 0ULL) {
            uFrac <<= 1;
            nCur--;
        }
        uFrac &= 0x000fffffffffffffULL;
        if (pExp != NULL) {
            *pExp = nCur;
        }
        ub.u64 = uSign | (0x3feULL << 52) | uFrac;
        return ub.d;
    }
    if (pExp != NULL) {
        *pExp = nCur - 1022;
    }
    ub.u64 = uSign | (0x3feULL << 52) | uFrac;
    return ub.d;
}

float
frexpf(float fX, int *pExp)
{
    return (float)frexp((double)fX, pExp);
}

int
ilogb(double dX)
{
    gj_dbits ub;
    int nCur;
    uint64_t uFrac;
    int nExp;

    ub.d = dX;
    nCur = (int)((ub.u64 >> 52) & 0x7ffULL);
    uFrac = ub.u64 & 0x000fffffffffffffULL;
    if (nCur == 0x7ff) {
        errno = EDOM;
        if (uFrac != 0ULL) {
            return FP_ILOGBNAN;
        }
        return INT_MAX;
    }
    if (nCur == 0) {
        if (uFrac == 0ULL) {
            errno = EDOM;
            return FP_ILOGB0;
        }
        nExp = -1022;
        while ((uFrac & 0x0010000000000000ULL) == 0ULL) {
            uFrac <<= 1;
            nExp--;
        }
        return nExp;
    }
    return nCur - 1023;
}

double
modf(double dX, double *pInt)
{
    double dI = gj_trunc(dX);

    if (pInt != NULL) {
        *pInt = dI;
    }
    return copysign(dX - dI, dX);
}

float
modff(float fX, float *pInt)
{
    double dI = 0.0;
    double dF = modf((double)fX, &dI);

    if (pInt != NULL) {
        *pInt = (float)dI;
    }
    return (float)dF;
}

double
fmod(double dX, double dY)
{
    double dN;
    gj_dbits uy;

    uy.d = dY;
    if ((uy.u64 << 1) == 0ULL) {
        errno = EDOM;
        return __builtin_nan("");
    }
    dN = gj_trunc(dX / dY);
    return dX - dN * dY;
}

float
fmodf(float fX, float fY)
{
    return (float)fmod((double)fX, (double)fY);
}

double
round(double dX)
{
    return copysign(floor(fabs(dX) + 0.5), dX);
}

float
roundf(float fX)
{
    return (float)round((double)fX);
}

/*
 * log/exp/pow bring-up for OpenSSH DUT + openssl-libcgj (no libm.so).
 * IEEE bit classifies; range-reduced series. Dual DoD B OPEN.
 */
static const double GJ_LN2 = 0.693147180559945309417232121458176568;
static const double GJ_LOG2E = 1.442695040888963407359924681001892137;
static const double GJ_LN10 = 2.302585092994045684017991454684364214;
static const double GJ_SQRT_HALF = 0.707106781186547524400844362104849039;

static int
gj_isnan(double dX)
{
    gj_dbits ub;

    ub.d = dX;
    return ((ub.u64 & 0x7fffffffffffffffULL) > 0x7ff0000000000000ULL);
}

static int
gj_isinf(double dX)
{
    gj_dbits ub;

    ub.d = dX;
    return ((ub.u64 & 0x7fffffffffffffffULL) == 0x7ff0000000000000ULL);
}

static int
gj_iszero(double dX)
{
    gj_dbits ub;

    ub.d = dX;
    return (ub.u64 << 1) == 0ULL;
}

int
__isnan(double dX)
{
    return gj_isnan(dX);
}

int
__isinf(double dX)
{
    return gj_isinf(dX) ? (dX < 0.0 ? -1 : 1) : 0;
}

int
__finite(double dX)
{
    return !gj_isnan(dX) && !gj_isinf(dX);
}

int
__isnanf(float fX)
{
    return gj_isnan((double)fX);
}

int
__isinff(float fX)
{
    return __isinf((double)fX);
}

int
__finitef(float fX)
{
    return __finite((double)fX);
}

double
log(double dX)
{
    gj_dbits ub;
    int nExp;
    double dM;
    double dF;
    double dS;
    double dS2;
    double dLn;

    ub.d = dX;
    if (((ub.u64 >> 52) & 0x7ffULL) == 0x7ffULL) {
        if ((ub.u64 << 12) != 0ULL) {
            return dX;
        }
        if ((ub.u64 >> 63) != 0ULL) {
            errno = EDOM;
            return __builtin_nan("");
        }
        return dX;
    }
    if (gj_iszero(dX)) {
        errno = ERANGE;
        return -__builtin_huge_val();
    }
    if ((ub.u64 >> 63) != 0ULL) {
        errno = EDOM;
        return __builtin_nan("");
    }
    dM = frexp(dX, &nExp);
    if (dM < GJ_SQRT_HALF) {
        dM *= 2.0;
        nExp--;
    }
    dF = dM - 1.0;
    dS = dF / (2.0 + dF);
    dS2 = dS * dS;
    dLn = dS * (2.0 +
                dS2 * (2.0 / 3.0 +
                       dS2 * (2.0 / 5.0 +
                              dS2 * (2.0 / 7.0 +
                                     dS2 * (2.0 / 9.0 +
                                            dS2 * (2.0 / 11.0 +
                                                   dS2 * (2.0 / 13.0)))))));
    return dLn + (double)nExp * GJ_LN2;
}

double
log2(double dX)
{
    return log(dX) * GJ_LOG2E;
}

double
log10(double dX)
{
    return log(dX) / GJ_LN10;
}

double
logb(double dX)
{
    if (gj_iszero(dX)) {
        errno = EDOM;
        return -__builtin_huge_val();
    }
    if (gj_isnan(dX)) {
        return dX;
    }
    if (gj_isinf(dX)) {
        return __builtin_huge_val();
    }
    return (double)ilogb(dX);
}

double
exp(double dX)
{
    gj_dbits ub;
    double dT;
    double dR;
    double dP;
    int n;

    ub.d = dX;
    if (((ub.u64 >> 52) & 0x7ffULL) == 0x7ffULL) {
        if ((ub.u64 << 12) != 0ULL) {
            return dX;
        }
        if ((ub.u64 >> 63) != 0ULL) {
            return 0.0;
        }
        return dX;
    }
    if (dX > 709.782712893384) {
        errno = ERANGE;
        return __builtin_huge_val();
    }
    if (dX < -745.133219101941) {
        errno = ERANGE;
        return 0.0;
    }
    dT = dX * GJ_LOG2E;
    n = (int)dT;
    if (dT - (double)n >= 0.5) {
        n++;
    } else if (dT - (double)n <= -0.5) {
        n--;
    }
    dR = dX - (double)n * GJ_LN2;
    dP = 1.0 +
         dR * (1.0 +
               dR * (1.0 / 2.0 +
                     dR * (1.0 / 6.0 +
                           dR * (1.0 / 24.0 +
                                 dR * (1.0 / 120.0 +
                                       dR * (1.0 / 720.0 +
                                             dR * (1.0 / 5040.0 +
                                                   dR * (1.0 / 40320.0 +
                                                         dR * (1.0 / 362880.0)))))))));
    return ldexp(dP, n);
}

double
exp2(double dX)
{
    double dI;
    double dF;
    int n;

    if (gj_isnan(dX) || gj_isinf(dX)) {
        if (gj_isinf(dX) && dX < 0.0) {
            return 0.0;
        }
        return dX;
    }
    dI = gj_trunc(dX);
    dF = dX - dI;
    if (dI > 1023.0) {
        errno = ERANGE;
        return __builtin_huge_val();
    }
    if (dI < -1074.0) {
        errno = ERANGE;
        return 0.0;
    }
    n = (int)dI;
    return ldexp(exp(dF * GJ_LN2), n);
}

static double
gj_pow_uint(double dX, unsigned long long uY)
{
    double dR = 1.0;

    while (uY > 0ULL) {
        if ((uY & 1ULL) != 0ULL) {
            dR *= dX;
        }
        dX *= dX;
        uY >>= 1;
    }
    return dR;
}

double
pow(double dX, double dY)
{
    gj_dbits ux;
    long long nY;
    double dT;
    int fNeg = 0;
    int fOdd = 0;

    ux.d = dX;
    if (gj_isnan(dX) || gj_isnan(dY)) {
        if (dX == 1.0 || gj_iszero(dY)) {
            return 1.0;
        }
        return dX + dY;
    }
    if (gj_iszero(dY)) {
        return 1.0;
    }
    if (dX == 1.0) {
        return 1.0;
    }
    if (dX == -1.0 && gj_isinf(dY)) {
        return 1.0;
    }
    if (gj_isinf(dY)) {
        double dA = (dX < 0.0) ? -dX : dX;

        if (dA == 1.0) {
            return 1.0;
        }
        if (dY > 0.0) {
            return (dA > 1.0) ? __builtin_huge_val() : 0.0;
        }
        return (dA > 1.0) ? 0.0 : __builtin_huge_val();
    }
    if (gj_iszero(dX)) {
        if (dY > 0.0) {
            return dX;
        }
        errno = ERANGE;
        if ((ux.u64 >> 63) != 0ULL) {
            return -__builtin_huge_val();
        }
        return __builtin_huge_val();
    }
    if (gj_isinf(dX)) {
        if (dX > 0.0) {
            return (dY > 0.0) ? dX : 0.0;
        }
        if (dY > 0.0) {
            return dX;
        }
        return 0.0;
    }
    dT = gj_trunc(dY);
    if (dT == dY && dT >= (double)LLONG_MIN && dT <= (double)LLONG_MAX) {
        nY = (long long)dT;
        fOdd = (nY & 1LL) != 0LL;
        if (nY < 0LL) {
            if (nY == LLONG_MIN) {
                return exp(dY * log((dX < 0.0) ? -dX : dX));
            }
            nY = -nY;
            dX = 1.0 / dX;
        }
        if (nY <= 1024LL) {
            return gj_pow_uint(dX, (unsigned long long)nY);
        }
    }
    if (dX < 0.0) {
        if (dT != dY) {
            errno = EDOM;
            return __builtin_nan("");
        }
        fNeg = fOdd;
        dX = -dX;
    }
    dT = exp(dY * log(dX));
    return fNeg ? -dT : dT;
}

double
hypot(double dX, double dY)
{
    double dA;
    double dB;
    double dR;

    dA = fabs(dX);
    dB = fabs(dY);
    if (gj_isnan(dX) || gj_isnan(dY)) {
        if (gj_isinf(dX) || gj_isinf(dY)) {
            return __builtin_huge_val();
        }
        return dX + dY;
    }
    if (gj_isinf(dX) || gj_isinf(dY)) {
        return __builtin_huge_val();
    }
    if (dA < dB) {
        dR = dA;
        dA = dB;
        dB = dR;
    }
    if (dA == 0.0) {
        return 0.0;
    }
    dR = dB / dA;
    return dA * sqrt(1.0 + dR * dR);
}

double
cbrt(double dX)
{
    double dA;

    if (gj_iszero(dX) || gj_isinf(dX) || gj_isnan(dX)) {
        return dX;
    }
    dA = exp(log(fabs(dX)) * (1.0 / 3.0));
    return copysign(dA, dX);
}

double
rint(double dX)
{
    double dI;
    double dF;
    double dAf;

    if (gj_isnan(dX) || gj_isinf(dX)) {
        return dX;
    }
    dI = gj_trunc(dX);
    dF = dX - dI;
    dAf = (dF < 0.0) ? -dF : dF;
    if (dAf > 0.5) {
        return dI + copysign(1.0, dX);
    }
    if (dAf < 0.5) {
        return dI;
    }
    if (fabs(dI) < 9007199254740992.0) {
        long long nI = (long long)dI;

        if ((nI & 1LL) != 0LL) {
            return dI + copysign(1.0, dX);
        }
    }
    return dI;
}

double
nearbyint(double dX)
{
    return rint(dX);
}

double
nextafter(double dX, double dY)
{
    gj_dbits ux;
    gj_dbits uy;

    ux.d = dX;
    uy.d = dY;
    if (gj_isnan(dX) || gj_isnan(dY)) {
        return dX + dY;
    }
    if (dX == dY) {
        return dY;
    }
    if (gj_iszero(dX)) {
        ux.u64 = 1ULL;
        if ((uy.u64 >> 63) != 0ULL) {
            ux.u64 |= 0x8000000000000000ULL;
        }
        return ux.d;
    }
    if (dX < dY) {
        if ((ux.u64 >> 63) == 0ULL) {
            ux.u64++;
        } else {
            ux.u64--;
        }
    } else {
        if ((ux.u64 >> 63) == 0ULL) {
            ux.u64--;
        } else {
            ux.u64++;
        }
    }
    if (((ux.u64 >> 52) & 0x7ffULL) == 0x7ffULL && (ux.u64 << 12) == 0ULL) {
        errno = ERANGE;
    }
    return ux.d;
}

double
fmin(double dX, double dY)
{
    if (gj_isnan(dX)) {
        return dY;
    }
    if (gj_isnan(dY)) {
        return dX;
    }
    return (dX < dY) ? dX : dY;
}

double
fmax(double dX, double dY)
{
    if (gj_isnan(dX)) {
        return dY;
    }
    if (gj_isnan(dY)) {
        return dX;
    }
    return (dX > dY) ? dX : dY;
}

double
fdim(double dX, double dY)
{
    if (gj_isnan(dX) || gj_isnan(dY)) {
        return dX + dY;
    }
    if (dX > dY) {
        return dX - dY;
    }
    return 0.0;
}

double
remainder(double dX, double dY)
{
    double dN;
    gj_dbits uy;

    uy.d = dY;
    if (gj_isnan(dX) || gj_isnan(dY)) {
        return dX + dY;
    }
    if ((uy.u64 << 1) == 0ULL || gj_isinf(dX)) {
        errno = EDOM;
        return __builtin_nan("");
    }
    if (gj_isinf(dY)) {
        return dX;
    }
    dN = rint(dX / dY);
    return dX - dN * dY;
}

float
logf(float fX)
{
    return (float)log((double)fX);
}

float
log2f(float fX)
{
    return (float)log2((double)fX);
}

float
log10f(float fX)
{
    return (float)log10((double)fX);
}

float
logbf(float fX)
{
    return (float)logb((double)fX);
}

float
expf(float fX)
{
    return (float)exp((double)fX);
}

float
exp2f(float fX)
{
    return (float)exp2((double)fX);
}

float
powf(float fX, float fY)
{
    return (float)pow((double)fX, (double)fY);
}

float
hypotf(float fX, float fY)
{
    return (float)hypot((double)fX, (double)fY);
}

float
cbrtf(float fX)
{
    return (float)cbrt((double)fX);
}

float
rintf(float fX)
{
    return (float)rint((double)fX);
}

float
nearbyintf(float fX)
{
    return (float)nearbyint((double)fX);
}

float
nextafterf(float fX, float fY)
{
    gj_fbits ux;
    gj_fbits uy;

    ux.f = fX;
    uy.f = fY;
    if (gj_isnan((double)fX) || gj_isnan((double)fY)) {
        return fX + fY;
    }
    if (fX == fY) {
        return fY;
    }
    if ((ux.u32 << 1) == 0u) {
        ux.u32 = 1u;
        if ((uy.u32 >> 31) != 0u) {
            ux.u32 |= 0x80000000u;
        }
        return ux.f;
    }
    if (fX < fY) {
        if ((ux.u32 >> 31) == 0u) {
            ux.u32++;
        } else {
            ux.u32--;
        }
    } else {
        if ((ux.u32 >> 31) == 0u) {
            ux.u32--;
        } else {
            ux.u32++;
        }
    }
    if (((ux.u32 >> 23) & 0xffu) == 0xffu && (ux.u32 << 9) == 0u) {
        errno = ERANGE;
    }
    return ux.f;
}

float
fminf(float fX, float fY)
{
    return (float)fmin((double)fX, (double)fY);
}

float
fmaxf(float fX, float fY)
{
    return (float)fmax((double)fX, (double)fY);
}

float
fdimf(float fX, float fY)
{
    return (float)fdim((double)fX, (double)fY);
}

float
remainderf(float fX, float fY)
{
    return (float)remainder((double)fX, (double)fY);
}

double
log1p(double dX)
{
    if (gj_isnan(dX) || dX == 0.0) {
        return dX;
    }
    if (dX == -1.0) {
        errno = ERANGE;
        return -__builtin_huge_val();
    }
    if (dX < -1.0) {
        errno = EDOM;
        return __builtin_nan("");
    }
    if (dX > -1.0e-8 && dX < 1.0e-8) {
        return dX - (0.5 * dX * dX);
    }
    return log(1.0 + dX);
}

double
expm1(double dX)
{
    if (gj_isnan(dX) || dX == 0.0) {
        return dX;
    }
    if (gj_isinf(dX)) {
        return (dX > 0.0) ? dX : -1.0;
    }
    if (dX > -1.0e-8 && dX < 1.0e-8) {
        return dX + (0.5 * dX * dX);
    }
    return exp(dX) - 1.0;
}

float
log1pf(float fX)
{
    return (float)log1p((double)fX);
}

float
expm1f(float fX)
{
    return (float)expm1((double)fX);
}

/*
 * Trig bring-up for DUT libcrypto/OpenSSH (no libm.so).
 * Payne-Hanek-free 2-pi reduction + Taylor. Dual DoD B OPEN.
 */
static const double GJ_PI = 3.14159265358979323846264338327950288;
static const double GJ_PI_2 = 1.57079632679489661923132169163975144;
static const double GJ_TWO_PI = 6.28318530717958647692528676655900576;

static double
gj_reduce_pi(double dX)
{
    double dN;

    if (gj_isnan(dX) || gj_isinf(dX)) {
        return dX;
    }
    dN = gj_trunc(dX / GJ_TWO_PI);
    dX = dX - dN * GJ_TWO_PI;
    if (dX > GJ_PI) {
        dX -= GJ_TWO_PI;
    }
    if (dX < -GJ_PI) {
        dX += GJ_TWO_PI;
    }
    return dX;
}

static double
gj_sin_poly(double dX)
{
    double dX2 = dX * dX;
    double dS = dX;
    double dT = dX;

    dT *= dX2;
    dS -= dT / 6.0;
    dT *= dX2;
    dS += dT / 120.0;
    dT *= dX2;
    dS -= dT / 5040.0;
    dT *= dX2;
    dS += dT / 362880.0;
    dT *= dX2;
    dS -= dT / 39916800.0;
    dT *= dX2;
    dS += dT / 6227020800.0;
    return dS;
}

double
sin(double dX)
{
    if (gj_isnan(dX) || gj_isinf(dX)) {
        errno = EDOM;
        return __builtin_nan("");
    }
    return gj_sin_poly(gj_reduce_pi(dX));
}

double
cos(double dX)
{
    if (gj_isnan(dX) || gj_isinf(dX)) {
        errno = EDOM;
        return __builtin_nan("");
    }
    return gj_sin_poly(gj_reduce_pi(dX + GJ_PI_2));
}

double
tan(double dX)
{
    double dC;

    dC = cos(dX);
    if (dC == 0.0) {
        errno = ERANGE;
        return copysign(__builtin_huge_val(), sin(dX));
    }
    return sin(dX) / dC;
}

double
atan(double dX)
{
    double dA;
    double dA2;
    double dS;
    double dT;
    int fNeg = 0;
    int fRecip = 0;

    if (gj_isnan(dX)) {
        return dX;
    }
    if (gj_isinf(dX)) {
        return copysign(GJ_PI_2, dX);
    }
    if (dX < 0.0) {
        fNeg = 1;
        dX = -dX;
    }
    if (dX > 1.0) {
        fRecip = 1;
        dX = 1.0 / dX;
    }
    dA = dX;
    dA2 = dA * dA;
    dT = dA;
    dS = dA;
    dT *= dA2;
    dS -= dT / 3.0;
    dT *= dA2;
    dS += dT / 5.0;
    dT *= dA2;
    dS -= dT / 7.0;
    dT *= dA2;
    dS += dT / 9.0;
    dT *= dA2;
    dS -= dT / 11.0;
    dT *= dA2;
    dS += dT / 13.0;
    if (fRecip) {
        dS = GJ_PI_2 - dS;
    }
    return fNeg ? -dS : dS;
}

double
atan2(double dY, double dX)
{
    if (gj_isnan(dX) || gj_isnan(dY)) {
        return dX + dY;
    }
    if (dX > 0.0) {
        return atan(dY / dX);
    }
    if (dX < 0.0) {
        if (dY >= 0.0) {
            return atan(dY / dX) + GJ_PI;
        }
        return atan(dY / dX) - GJ_PI;
    }
    if (dY > 0.0) {
        return GJ_PI_2;
    }
    if (dY < 0.0) {
        return -GJ_PI_2;
    }
    return 0.0;
}

double
asin(double dX)
{
    if (gj_isnan(dX)) {
        return dX;
    }
    if (dX < -1.0 || dX > 1.0) {
        errno = EDOM;
        return __builtin_nan("");
    }
    if (dX == 1.0 || dX == -1.0) {
        return copysign(GJ_PI_2, dX);
    }
    return atan(dX / sqrt(1.0 - dX * dX));
}

double
acos(double dX)
{
    if (gj_isnan(dX)) {
        return dX;
    }
    if (dX < -1.0 || dX > 1.0) {
        errno = EDOM;
        return __builtin_nan("");
    }
    return GJ_PI_2 - asin(dX);
}

float
sinf(float fX)
{
    return (float)sin((double)fX);
}

float
cosf(float fX)
{
    return (float)cos((double)fX);
}

float
tanf(float fX)
{
    return (float)tan((double)fX);
}

float
atanf(float fX)
{
    return (float)atan((double)fX);
}

float
atan2f(float fY, float fX)
{
    return (float)atan2((double)fY, (double)fX);
}

float
asinf(float fX)
{
    return (float)asin((double)fX);
}

float
acosf(float fX)
{
    return (float)acos((double)fX);
}

int signgam;

long
lround(double dX)
{
    double dR = round(dX);

    if (dR > (double)LONG_MAX) {
        errno = ERANGE;
        return LONG_MAX;
    }
    if (dR < (double)LONG_MIN) {
        errno = ERANGE;
        return LONG_MIN;
    }
    return (long)dR;
}

long
lrint(double dX)
{
    double dR = rint(dX);

    if (dR > (double)LONG_MAX) {
        errno = ERANGE;
        return LONG_MAX;
    }
    if (dR < (double)LONG_MIN) {
        errno = ERANGE;
        return LONG_MIN;
    }
    return (long)dR;
}

long long
llround(double dX)
{
    double dR = round(dX);

    if (dR > (double)LLONG_MAX) {
        errno = ERANGE;
        return LLONG_MAX;
    }
    if (dR < (double)LLONG_MIN) {
        errno = ERANGE;
        return LLONG_MIN;
    }
    return (long long)dR;
}

long long
llrint(double dX)
{
    double dR = rint(dX);

    if (dR > (double)LLONG_MAX) {
        errno = ERANGE;
        return LLONG_MAX;
    }
    if (dR < (double)LLONG_MIN) {
        errno = ERANGE;
        return LLONG_MIN;
    }
    return (long long)dR;
}

long
lroundf(float fX)
{
    return lround((double)fX);
}

long
lrintf(float fX)
{
    return lrint((double)fX);
}

long long
llroundf(float fX)
{
    return llround((double)fX);
}

long long
llrintf(float fX)
{
    return llrint((double)fX);
}

double
fma(double dX, double dY, double dZ)
{
    /* One-rounding fma needs FMA hardware; bring-up fused-by-eval. */
    return (dX * dY) + dZ;
}

float
fmaf(float fX, float fY, float fZ)
{
    return (float)fma((double)fX, (double)fY, (double)fZ);
}

double
sinh(double dX)
{
    double dE;

    if (gj_isnan(dX) || gj_isinf(dX) || gj_iszero(dX)) {
        return dX;
    }
    if (dX > 700.0) {
        errno = ERANGE;
        return __builtin_huge_val();
    }
    if (dX < -700.0) {
        errno = ERANGE;
        return -__builtin_huge_val();
    }
    dE = exp(dX);
    return (dE - (1.0 / dE)) * 0.5;
}

double
cosh(double dX)
{
    double dE;
    double dA = (dX < 0.0) ? -dX : dX;

    if (gj_isnan(dX)) {
        return dX;
    }
    if (gj_isinf(dX)) {
        return __builtin_huge_val();
    }
    if (dA > 700.0) {
        errno = ERANGE;
        return __builtin_huge_val();
    }
    dE = exp(dA);
    return (dE + (1.0 / dE)) * 0.5;
}

double
tanh(double dX)
{
    if (gj_isnan(dX)) {
        return dX;
    }
    if (dX > 20.0) {
        return 1.0;
    }
    if (dX < -20.0) {
        return -1.0;
    }
    return sinh(dX) / cosh(dX);
}

double
asinh(double dX)
{
    double dA;

    if (gj_isnan(dX) || gj_isinf(dX) || gj_iszero(dX)) {
        return dX;
    }
    dA = (dX < 0.0) ? -dX : dX;
    if (dA > 1.0e8) {
        return copysign(log(dA) + GJ_LN2, dX);
    }
    return copysign(log(dA + sqrt(dA * dA + 1.0)), dX);
}

double
acosh(double dX)
{
    if (gj_isnan(dX)) {
        return dX;
    }
    if (dX < 1.0) {
        errno = EDOM;
        return __builtin_nan("");
    }
    if (gj_isinf(dX)) {
        return dX;
    }
    if (dX > 1.0e8) {
        return log(dX) + GJ_LN2;
    }
    return log(dX + sqrt(dX * dX - 1.0));
}

double
atanh(double dX)
{
    double dA;

    if (gj_isnan(dX) || gj_iszero(dX)) {
        return dX;
    }
    dA = (dX < 0.0) ? -dX : dX;
    if (dA > 1.0) {
        errno = EDOM;
        return __builtin_nan("");
    }
    if (dA == 1.0) {
        errno = ERANGE;
        return copysign(__builtin_huge_val(), dX);
    }
    return 0.5 * log((1.0 + dX) / (1.0 - dX));
}

float
sinhf(float fX)
{
    return (float)sinh((double)fX);
}

float
coshf(float fX)
{
    return (float)cosh((double)fX);
}

float
tanhf(float fX)
{
    return (float)tanh((double)fX);
}

float
asinhf(float fX)
{
    return (float)asinh((double)fX);
}

float
acoshf(float fX)
{
    return (float)acosh((double)fX);
}

float
atanhf(float fX)
{
    return (float)atanh((double)fX);
}

double
erf(double dX)
{
    double dA;
    double dT;
    double dY;
    int fNeg = 0;

    if (gj_isnan(dX)) {
        return dX;
    }
    if (dX < 0.0) {
        fNeg = 1;
        dX = -dX;
    }
    if (dX > 6.0) {
        return fNeg ? -1.0 : 1.0;
    }
    /* Abramowitz-Stegun 7.1.26 */
    dT = 1.0 / (1.0 + 0.3275911 * dX);
    dY = (((((1.061405429 * dT - 1.453152027) * dT) + 1.421413741) * dT -
           0.284496736) *
              dT +
          0.254829592) *
         dT;
    dA = 1.0 - dY * exp(-dX * dX);
    return fNeg ? -dA : dA;
}

double
erfc(double dX)
{
    return 1.0 - erf(dX);
}

double
tgamma(double dX)
{
    static const double aC[9] = {
        0.99999999999980993, 676.5203681218851, -1259.1392167224028,
        771.32342877765313, -176.61502916214059, 12.507343278686905,
        -0.13857109526572012, 9.9843695780195716e-6, 1.5056327351493116e-7
    };
    double dZ;
    double dAg = 0.99999999999980993;
    double dT;
    int i;

    if (gj_isnan(dX) || gj_isinf(dX)) {
        return dX;
    }
    if (dX == 0.0) {
        errno = ERANGE;
        return copysign(__builtin_huge_val(), dX);
    }
    if (dX < 0.5) {
        /* reflection: pi / (sin(pi x) gamma(1-x)) */
        dT = GJ_PI / (sin(GJ_PI * dX) * tgamma(1.0 - dX));
        return dT;
    }
    dZ = dX - 1.0;
    for (i = 1; i < 9; i++) {
        dAg += aC[i] / (dZ + (double)i);
    }
    dT = dZ + 7.5;
    return sqrt(2.0 * GJ_PI) * pow(dT, dZ + 0.5) * exp(-dT) * dAg;
}

double
lgamma(double dX)
{
    double dG;

    if (gj_isnan(dX) || gj_isinf(dX)) {
        signgam = 1;
        return dX;
    }
    dG = tgamma(dX);
    if (dG < 0.0) {
        signgam = -1;
        dG = -dG;
    } else {
        signgam = 1;
    }
    if (dG == 0.0) {
        errno = ERANGE;
        return -__builtin_huge_val();
    }
    return log(dG);
}

double
nan(const char *sz)
{
    (void)sz;
    return __builtin_nan("");
}

float
nanf(const char *sz)
{
    (void)sz;
    return __builtin_nanf("");
}

double
scalbln(double dX, long nExp)
{
    if (nExp > 4000L) {
        return scalbn(dX, 4000);
    }
    if (nExp < -4000L) {
        return scalbn(dX, -4000);
    }
    return scalbn(dX, (int)nExp);
}

double
nexttoward(double dX, long double dY)
{
    return nextafter(dX, (double)dY);
}

double
remquo(double dX, double dY, int *pQuo)
{
    double dN;
    double dR;
    long nQ;

    dR = remainder(dX, dY);
    if (pQuo != NULL) {
        if (dY != 0.0 && !gj_isnan(dX) && !gj_isnan(dY)) {
            dN = rint(dX / dY);
            if (dN > (double)LONG_MAX) {
                nQ = LONG_MAX;
            } else if (dN < (double)LONG_MIN) {
                nQ = LONG_MIN;
            } else {
                nQ = (long)dN;
            }
            *pQuo = (int)(nQ & 0x7fffffffL);
            if (nQ < 0) {
                *pQuo = -*pQuo;
            }
        } else {
            *pQuo = 0;
        }
    }
    return dR;
}

/*
 * trunc/floorf/ceilf: math.h already declares these. graph_batch92.c also
 * exports them; keep these weak so a full libcgj link stays one definition.
 * Dual DoD B OPEN.
 */
__attribute__((weak)) double
trunc(double dX)
{
    return gj_trunc(dX);
}

__attribute__((weak)) float
truncf(float fX)
{
    return (float)gj_trunc((double)fX);
}

__attribute__((weak)) float
floorf(float fX)
{
    return (float)floor((double)fX);
}

__attribute__((weak)) float
ceilf(float fX)
{
    return (float)ceil((double)fX);
}

int
finite(double dX)
{
    return __finite(dX);
}

int
finitef(float fX)
{
    return __finitef(fX);
}

void
sincos(double dX, double *pSin, double *pCos)
{
    if (pSin != NULL) {
        *pSin = sin(dX);
    }
    if (pCos != NULL) {
        *pCos = cos(dX);
    }
}

void
sincosf(float fX, float *pSin, float *pCos)
{
    if (pSin != NULL) {
        *pSin = sinf(fX);
    }
    if (pCos != NULL) {
        *pCos = cosf(fX);
    }
}

double
exp10(double dX)
{
    return exp(dX * GJ_LN10);
}

float
exp10f(float fX)
{
    return (float)exp10((double)fX);
}

double
drem(double dX, double dY)
{
    return remainder(dX, dY);
}

double
gamma(double dX)
{
    return lgamma(dX);
}
