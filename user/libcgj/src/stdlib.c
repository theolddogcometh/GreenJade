/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 */
#include <ctype.h>
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
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

long
strtol(const char *sz, char **ppEnd, int nBase)
{
    long v = 0;
    int fNeg = 0;
    int d;
    const char *p;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0;
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
    if (nBase == 0) {
        if (*p == '0' && (p[1] == 'x' || p[1] == 'X')) {
            nBase = 16;
            p += 2;
        } else if (*p == '0') {
            nBase = 8;
            p++;
        } else {
            nBase = 10;
        }
    } else if (nBase == 16 && *p == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }
    if (nBase < 2 || nBase > 36) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            *ppEnd = (char *)(uintptr_t)sz;
        }
        return 0;
    }
    while ((d = digit_val((unsigned char)*p, nBase)) >= 0) {
        v = v * (long)nBase + d;
        p++;
    }
    if (ppEnd != NULL) {
        *ppEnd = (char *)(uintptr_t)p;
    }
    return fNeg ? -v : v;
}

unsigned long
strtoul(const char *sz, char **ppEnd, int nBase)
{
    unsigned long v = 0;
    int d;
    const char *p;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0;
    }
    p = sz;
    while (isspace((unsigned char)*p)) {
        p++;
    }
    if (*p == '+') {
        p++;
    }
    if (nBase == 0) {
        if (*p == '0' && (p[1] == 'x' || p[1] == 'X')) {
            nBase = 16;
            p += 2;
        } else if (*p == '0') {
            nBase = 8;
            p++;
        } else {
            nBase = 10;
        }
    } else if (nBase == 16 && *p == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }
    if (nBase < 2 || nBase > 36) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            *ppEnd = (char *)(uintptr_t)sz;
        }
        return 0;
    }
    while ((d = digit_val((unsigned char)*p, nBase)) >= 0) {
        v = v * (unsigned long)nBase + (unsigned long)d;
        p++;
    }
    if (ppEnd != NULL) {
        *ppEnd = (char *)(uintptr_t)p;
    }
    return v;
}

long long
strtoll(const char *sz, char **ppEnd, int nBase)
{
    long long v = 0;
    int fNeg = 0;
    int d;
    const char *p;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0;
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
    if (nBase == 0) {
        if (*p == '0' && (p[1] == 'x' || p[1] == 'X')) {
            nBase = 16;
            p += 2;
        } else if (*p == '0') {
            nBase = 8;
            p++;
        } else {
            nBase = 10;
        }
    } else if (nBase == 16 && *p == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }
    if (nBase < 2 || nBase > 36) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            *ppEnd = (char *)(uintptr_t)sz;
        }
        return 0;
    }
    while ((d = digit_val((unsigned char)*p, nBase)) >= 0) {
        v = v * (long long)nBase + d;
        p++;
    }
    if (ppEnd != NULL) {
        *ppEnd = (char *)(uintptr_t)p;
    }
    return fNeg ? -v : v;
}

unsigned long long
strtoull(const char *sz, char **ppEnd, int nBase)
{
    unsigned long long v = 0;
    int d;
    const char *p;

    if (sz == NULL) {
        if (ppEnd != NULL) {
            *ppEnd = NULL;
        }
        return 0;
    }
    p = sz;
    while (isspace((unsigned char)*p)) {
        p++;
    }
    if (*p == '+') {
        p++;
    }
    if (nBase == 0) {
        if (*p == '0' && (p[1] == 'x' || p[1] == 'X')) {
            nBase = 16;
            p += 2;
        } else if (*p == '0') {
            nBase = 8;
            p++;
        } else {
            nBase = 10;
        }
    } else if (nBase == 16 && *p == '0' && (p[1] == 'x' || p[1] == 'X')) {
        p += 2;
    }
    if (nBase < 2 || nBase > 36) {
        errno = EINVAL;
        if (ppEnd != NULL) {
            *ppEnd = (char *)(uintptr_t)sz;
        }
        return 0;
    }
    while ((d = digit_val((unsigned char)*p, nBase)) >= 0) {
        v = v * (unsigned long long)nBase + (unsigned long long)d;
        p++;
    }
    if (ppEnd != NULL) {
        *ppEnd = (char *)(uintptr_t)p;
    }
    return v;
}

/* Process environment (glibc-shaped). Seeded from aux stack by __libc_start_main. */
char **environ;

#define GJ_ENV_MAX 64

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
    size_t i, j;
    unsigned char *p = (unsigned char *)pBase;

    if (pBase == NULL || pfnCmp == NULL || cb == 0 || n < 2) {
        return;
    }
    /* Simple insertion sort (bring-up; correct, not fast) */
    for (i = 1; i < n; i++) {
        j = i;
        while (j > 0 && pfnCmp(p + (j - 1) * cb, p + j * cb) > 0) {
            qsort_swap(p + (j - 1) * cb, p + j * cb, cb);
            j--;
        }
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
    static char aTmp[512];
    char *pOut = szResolved != NULL ? szResolved : aTmp;
    size_t n;
    char aCwd[256];

    if (szPath == NULL) {
        errno = EINVAL;
        return NULL;
    }
    if (szPath[0] == '/') {
        n = strlen(szPath);
        if (n + 1 > sizeof(aTmp) && szResolved == NULL) {
            errno = ENAMETOOLONG;
            return NULL;
        }
        if (szResolved != NULL) {
            /* caller buffer — assume PATH_MAX-ish 4096 not enforced here */
        }
        memcpy(pOut, szPath, n + 1);
        return pOut;
    }
    if (getcwd(aCwd, sizeof(aCwd)) == NULL) {
        return NULL;
    }
    n = strlen(aCwd);
    if (n + 1 + strlen(szPath) + 1 > 512) {
        errno = ENAMETOOLONG;
        return NULL;
    }
    memcpy(pOut, aCwd, n);
    pOut[n] = '/';
    memcpy(pOut + n + 1, szPath, strlen(szPath) + 1);
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
