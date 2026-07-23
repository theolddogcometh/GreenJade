/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch35: RFC 4122 UUID family, basename_r/dirname_r,
 * NetBSD-style strtoi/strtou, memcasecmp, Solaris gethrtime/gethrvtime.
 * Integer/pointer only (no SSE doubles). Clean-room public ABI.
 */
#include <errno.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

ssize_t getrandom(void *pBuf, size_t cb, unsigned uFlags);
int clock_gettime(clockid_t clk, struct timespec *pTs);

#ifndef CLOCK_MONOTONIC
#define CLOCK_MONOTONIC 1
#endif
#ifndef CLOCK_PROCESS_CPUTIME_ID
#define CLOCK_PROCESS_CPUTIME_ID 2
#endif
/* Not all freestanding errno sets define ECANCELED (Linux 125). */
#ifndef ECANCELED
#define ECANCELED 125
#endif

const char __libcgj_batch35_marker[] = "libcgj-batch35";

/* ---- UUID (opaque 16-byte) ---------------------------------------------- */

typedef struct {
    unsigned char aBytes[16];
} uuid_t_b35;

/* Public shape matches common BSD uuid_t as 16 raw bytes via void* APIs. */

void
uuid_create_nil(void *pUuid, uint32_t *pStatus)
{
    if (pUuid != NULL) {
        memset(pUuid, 0, 16u);
    }
    if (pStatus != NULL) {
        *pStatus = 0;
    }
}

void
uuid_create(void *pUuid, uint32_t *pStatus)
{
    unsigned char *p;
    ssize_t n;

    if (pUuid == NULL) {
        if (pStatus != NULL) {
            *pStatus = (uint32_t)EINVAL;
        }
        return;
    }
    p = (unsigned char *)pUuid;
    n = getrandom(p, 16u, 0);
    if (n != 16) {
        /* Fallback: mix times into a weak unique-ish id (bring-up). */
        struct timespec ts;
        uint64_t u;
        unsigned i;

        memset(&ts, 0, sizeof(ts));
        (void)clock_gettime(CLOCK_MONOTONIC, &ts);
        u = ((uint64_t)ts.tv_sec << 32) ^ (uint64_t)ts.tv_nsec;
        for (i = 0; i < 16u; i++) {
            u = u * 6364136223846793005ull + 1ull;
            p[i] = (unsigned char)(u >> 56);
        }
    }
    /* RFC 4122 version 4 + variant 10xx */
    p[6] = (unsigned char)((p[6] & 0x0fu) | 0x40u);
    p[8] = (unsigned char)((p[8] & 0x3fu) | 0x80u);
    if (pStatus != NULL) {
        *pStatus = 0;
    }
}

int
uuid_is_nil(const void *pUuid, uint32_t *pStatus)
{
    const unsigned char *p;
    unsigned i;

    if (pStatus != NULL) {
        *pStatus = 0;
    }
    if (pUuid == NULL) {
        return 1;
    }
    p = (const unsigned char *)pUuid;
    for (i = 0; i < 16u; i++) {
        if (p[i] != 0u) {
            return 0;
        }
    }
    return 1;
}

int
uuid_equal(const void *pA, const void *pB, uint32_t *pStatus)
{
    if (pStatus != NULL) {
        *pStatus = 0;
    }
    if (pA == NULL || pB == NULL) {
        return pA == pB;
    }
    return memcmp(pA, pB, 16u) == 0;
}

int32_t
uuid_compare(const void *pA, const void *pB, uint32_t *pStatus)
{
    int n;

    if (pStatus != NULL) {
        *pStatus = 0;
    }
    if (pA == NULL && pB == NULL) {
        return 0;
    }
    if (pA == NULL) {
        return -1;
    }
    if (pB == NULL) {
        return 1;
    }
    n = memcmp(pA, pB, 16u);
    if (n < 0) {
        return -1;
    }
    if (n > 0) {
        return 1;
    }
    return 0;
}

uint16_t
uuid_hash(const void *pUuid, uint32_t *pStatus)
{
    const unsigned char *p;
    uint16_t h = 0;
    unsigned i;

    if (pStatus != NULL) {
        *pStatus = 0;
    }
    if (pUuid == NULL) {
        return 0;
    }
    p = (const unsigned char *)pUuid;
    for (i = 0; i < 16u; i++) {
        h = (uint16_t)((h * 33u) + p[i]);
    }
    return h;
}

static int
b35_hex_nibble(int ch)
{
    if (ch >= '0' && ch <= '9') {
        return ch - '0';
    }
    if (ch >= 'a' && ch <= 'f') {
        return ch - 'a' + 10;
    }
    if (ch >= 'A' && ch <= 'F') {
        return ch - 'A' + 10;
    }
    return -1;
}

void
uuid_to_string(const void *pUuid, char **ppStr, uint32_t *pStatus)
{
    static const char *szDigits = "0123456789abcdef";
    static char aBuf[40];
    const unsigned char *p;
    unsigned i;
    unsigned j = 0;
    /* 8-4-4-4-12 */
    static const unsigned aDashAfter[] = {4, 6, 8, 10};

    if (ppStr == NULL) {
        if (pStatus != NULL) {
            *pStatus = (uint32_t)EINVAL;
        }
        return;
    }
    if (pUuid == NULL) {
        *ppStr = NULL;
        if (pStatus != NULL) {
            *pStatus = (uint32_t)EINVAL;
        }
        return;
    }
    p = (const unsigned char *)pUuid;
    for (i = 0; i < 16u; i++) {
        unsigned k;
        for (k = 0; k < 4u; k++) {
            if (aDashAfter[k] == i) {
                aBuf[j++] = '-';
            }
        }
        aBuf[j++] = szDigits[(p[i] >> 4) & 0xfu];
        aBuf[j++] = szDigits[p[i] & 0xfu];
    }
    aBuf[j] = '\0';
    *ppStr = aBuf;
    if (pStatus != NULL) {
        *pStatus = 0;
    }
}

void
uuid_from_string(const char *sz, void *pUuid, uint32_t *pStatus)
{
    unsigned char *p;
    unsigned i = 0;
    unsigned nNib = 0;
    unsigned char b = 0;

    if (pUuid == NULL || sz == NULL) {
        if (pStatus != NULL) {
            *pStatus = (uint32_t)EINVAL;
        }
        return;
    }
    p = (unsigned char *)pUuid;
    memset(p, 0, 16u);
    while (*sz != '\0' && i < 16u) {
        int n;

        if (*sz == '-') {
            sz++;
            continue;
        }
        n = b35_hex_nibble((unsigned char)*sz);
        if (n < 0) {
            if (pStatus != NULL) {
                *pStatus = (uint32_t)EINVAL;
            }
            return;
        }
        if ((nNib & 1u) == 0u) {
            b = (unsigned char)(n << 4);
        } else {
            b = (unsigned char)(b | (unsigned char)n);
            p[i++] = b;
        }
        nNib++;
        sz++;
    }
    if (i != 16u) {
        if (pStatus != NULL) {
            *pStatus = (uint32_t)EINVAL;
        }
        return;
    }
    if (pStatus != NULL) {
        *pStatus = 0;
    }
}

void
uuid_enc_le(void *pBuf, const void *pUuid)
{
    if (pBuf != NULL && pUuid != NULL) {
        memcpy(pBuf, pUuid, 16u);
    }
}

void
uuid_dec_le(void *pUuid, const void *pBuf)
{
    if (pBuf != NULL && pUuid != NULL) {
        memcpy(pUuid, pBuf, 16u);
    }
}

void
uuid_enc_be(void *pBuf, const void *pUuid)
{
    const unsigned char *p;
    unsigned char *q;
    unsigned i;

    if (pBuf == NULL || pUuid == NULL) {
        return;
    }
    p = (const unsigned char *)pUuid;
    q = (unsigned char *)pBuf;
    /* time-low, time-mid, time-hi as big-endian words */
    q[0] = p[3];
    q[1] = p[2];
    q[2] = p[1];
    q[3] = p[0];
    q[4] = p[5];
    q[5] = p[4];
    q[6] = p[7];
    q[7] = p[6];
    for (i = 8; i < 16u; i++) {
        q[i] = p[i];
    }
}

void
uuid_dec_be(void *pUuid, const void *pBuf)
{
    /* reverse of enc_be */
    uuid_enc_be(pUuid, pBuf);
}

/* libuuid-shaped aliases live in graph_batch36.c (unique; no multi-def). */

/* ---- basename_r / dirname_r --------------------------------------------- */

char *
basename_r(const char *szPath, char *pBuf, size_t cb)
{
    size_t n;
    size_t i;
    size_t start;
    size_t end;
    size_t len;

    if (pBuf == NULL || cb == 0) {
        errno = EINVAL;
        return NULL;
    }
    if (szPath == NULL || szPath[0] == '\0') {
        if (cb < 2u) {
            errno = ENAMETOOLONG;
            return NULL;
        }
        pBuf[0] = '.';
        pBuf[1] = '\0';
        return pBuf;
    }
    n = strlen(szPath);
    end = n;
    while (end > 1u && szPath[end - 1u] == '/') {
        end--;
    }
    if (end == 1u && szPath[0] == '/') {
        if (cb < 2u) {
            errno = ENAMETOOLONG;
            return NULL;
        }
        pBuf[0] = '/';
        pBuf[1] = '\0';
        return pBuf;
    }
    start = 0;
    for (i = 0; i < end; i++) {
        if (szPath[i] == '/') {
            start = i + 1u;
        }
    }
    len = end - start;
    if (len + 1u > cb) {
        errno = ENAMETOOLONG;
        return NULL;
    }
    memcpy(pBuf, szPath + start, len);
    pBuf[len] = '\0';
    return pBuf;
}

char *
dirname_r(const char *szPath, char *pBuf, size_t cb)
{
    size_t n;
    size_t end;
    size_t i;
    size_t last = 0;
    int fSlash = 0;

    if (pBuf == NULL || cb == 0) {
        errno = EINVAL;
        return NULL;
    }
    if (szPath == NULL || szPath[0] == '\0') {
        if (cb < 2u) {
            errno = ENAMETOOLONG;
            return NULL;
        }
        pBuf[0] = '.';
        pBuf[1] = '\0';
        return pBuf;
    }
    n = strlen(szPath);
    end = n;
    while (end > 1u && szPath[end - 1u] == '/') {
        end--;
    }
    for (i = 0; i < end; i++) {
        if (szPath[i] == '/') {
            last = i;
            fSlash = 1;
        }
    }
    if (!fSlash) {
        if (cb < 2u) {
            errno = ENAMETOOLONG;
            return NULL;
        }
        pBuf[0] = '.';
        pBuf[1] = '\0';
        return pBuf;
    }
    if (last == 0) {
        if (cb < 2u) {
            errno = ENAMETOOLONG;
            return NULL;
        }
        pBuf[0] = '/';
        pBuf[1] = '\0';
        return pBuf;
    }
    if (last + 1u > cb) {
        errno = ENAMETOOLONG;
        return NULL;
    }
    memcpy(pBuf, szPath, last);
    pBuf[last] = '\0';
    return pBuf;
}

/* ---- strtoi / strtou (NetBSD-shaped, long long) ------------------------- */

long long
strtoi(const char *sz, char **ppEnd, int nBase, long long nLo, long long nHi,
       int *pStatus)
{
    char *pEnd = NULL;
    long long n;
    int nSt = 0;
    extern long long strtoll(const char *, char **, int);

    if (sz == NULL) {
        nSt = EINVAL;
        n = 0;
        goto out;
    }
    errno = 0;
    n = strtoll(sz, &pEnd, nBase);
    if (ppEnd != NULL) {
        *ppEnd = pEnd;
    }
    if (pEnd == sz) {
        nSt = ECANCELED;
        n = 0;
    } else if (errno == ERANGE) {
        nSt = ERANGE;
    } else if (n < nLo) {
        n = nLo;
        nSt = ERANGE;
    } else if (n > nHi) {
        n = nHi;
        nSt = ERANGE;
    } else if (pEnd != NULL && *pEnd != '\0') {
        nSt = ENOTSUP;
    }
out:
    if (pStatus != NULL) {
        *pStatus = nSt;
    }
    return n;
}

unsigned long long
strtou(const char *sz, char **ppEnd, int nBase, unsigned long long nLo,
       unsigned long long nHi, int *pStatus)
{
    char *pEnd = NULL;
    unsigned long long n;
    int nSt = 0;
    extern unsigned long long strtoull(const char *, char **, int);

    if (sz == NULL) {
        nSt = EINVAL;
        n = 0;
        goto out;
    }
    errno = 0;
    n = strtoull(sz, &pEnd, nBase);
    if (ppEnd != NULL) {
        *ppEnd = pEnd;
    }
    if (pEnd == sz) {
        nSt = ECANCELED;
        n = 0;
    } else if (errno == ERANGE) {
        nSt = ERANGE;
    } else if (n < nLo) {
        n = nLo;
        nSt = ERANGE;
    } else if (n > nHi) {
        n = nHi;
        nSt = ERANGE;
    } else if (pEnd != NULL && *pEnd != '\0') {
        nSt = ENOTSUP;
    }
out:
    if (pStatus != NULL) {
        *pStatus = nSt;
    }
    return n;
}

/* ---- memcasecmp --------------------------------------------------------- */

int
memcasecmp(const void *pA, const void *pB, size_t cb)
{
    const unsigned char *a = (const unsigned char *)pA;
    const unsigned char *b = (const unsigned char *)pB;
    size_t i;

    if (a == NULL || b == NULL) {
        if (a == b) {
            return 0;
        }
        return a == NULL ? -1 : 1;
    }
    for (i = 0; i < cb; i++) {
        unsigned char ca = a[i];
        unsigned char cbv = b[i];

        if (ca >= 'A' && ca <= 'Z') {
            ca = (unsigned char)(ca - 'A' + 'a');
        }
        if (cbv >= 'A' && cbv <= 'Z') {
            cbv = (unsigned char)(cbv - 'A' + 'a');
        }
        if (ca != cbv) {
            return (int)ca - (int)cbv;
        }
    }
    return 0;
}

/* ---- Solaris gethrtime / gethrvtime ------------------------------------- */

typedef long long hrtime_t;

hrtime_t
gethrtime(void)
{
    struct timespec ts;

    memset(&ts, 0, sizeof(ts));
    if (clock_gettime(CLOCK_MONOTONIC, &ts) != 0) {
        return 0;
    }
    return (hrtime_t)ts.tv_sec * 1000000000ll + (hrtime_t)ts.tv_nsec;
}

hrtime_t
gethrvtime(void)
{
    struct timespec ts;

    memset(&ts, 0, sizeof(ts));
    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &ts) != 0) {
        /* Fall back to monotonic if CPUtime unavailable. */
        return gethrtime();
    }
    return (hrtime_t)ts.tv_sec * 1000000000ll + (hrtime_t)ts.tv_nsec;
}

/* underscored aliases */
void __uuid_create(void *pUuid, uint32_t *pStatus)
    __attribute__((alias("uuid_create")));
void __uuid_to_string(const void *pUuid, char **ppStr, uint32_t *pStatus)
    __attribute__((alias("uuid_to_string")));
void __uuid_from_string(const char *sz, void *pUuid, uint32_t *pStatus)
    __attribute__((alias("uuid_from_string")));
char *__basename_r(const char *szPath, char *pBuf, size_t cb)
    __attribute__((alias("basename_r")));
char *__dirname_r(const char *szPath, char *pBuf, size_t cb)
    __attribute__((alias("dirname_r")));
long long __strtoi(const char *sz, char **ppEnd, int nBase, long long nLo,
                   long long nHi, int *pStatus)
    __attribute__((alias("strtoi")));
unsigned long long __strtou(const char *sz, char **ppEnd, int nBase,
                            unsigned long long nLo, unsigned long long nHi,
                            int *pStatus) __attribute__((alias("strtou")));
int __memcasecmp(const void *pA, const void *pB, size_t cb)
    __attribute__((alias("memcasecmp")));
hrtime_t __gethrtime(void) __attribute__((alias("gethrtime")));
hrtime_t __gethrvtime(void) __attribute__((alias("gethrvtime")));

/* ---- soft deepen: gethrvtime_delta + uuid nil probe (unique) ------------ */

hrtime_t
gethrvtime_delta(hrtime_t t0)
{
    hrtime_t t1 = gethrvtime();

    if (t1 < t0) {
        return 0;
    }
    return t1 - t0;
}

hrtime_t __gethrvtime_delta(hrtime_t t0)
    __attribute__((alias("gethrvtime_delta")));

/* uuid_create_nil already exists; add boolean-shaped is-nil without status. */
int
uuid_is_nil_np(const void *pUuid)
{
    uint32_t uSt = 0;

    return uuid_is_nil(pUuid, &uSt);
}

int __uuid_is_nil_np(const void *pUuid)
    __attribute__((alias("uuid_is_nil_np")));
