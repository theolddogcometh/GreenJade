/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * rand/random, div family, reallocarray/memalign, get_current_dir_name.
 * Soft fill: LCG random; zero-divisor soft guards.
 */
#include <errno.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/* ISO C rand — linear congruential (glibc-shaped constants) */
static unsigned long g_uRand = 1UL;

void
srand(unsigned int uSeed)
{
    g_uRand = (unsigned long)uSeed;
}

int
rand(void)
{
    g_uRand = g_uRand * 1103515245UL + 12345UL;
    return (int)((g_uRand / 65536UL) % 32768UL);
}

int
rand_r(unsigned int *pSeed)
{
    unsigned long u;

    if (pSeed == NULL) {
        return 0;
    }
    u = (unsigned long)(*pSeed);
    u = u * 1103515245UL + 12345UL;
    *pSeed = (unsigned int)u;
    return (int)((u / 65536UL) % 32768UL);
}

/* BSD random(3) — same LCG for soft fill (not full 31-bit poly). */
static unsigned long g_uRandom = 1UL;

void
srandom(unsigned int uSeed)
{
    g_uRandom = uSeed ? (unsigned long)uSeed : 1UL;
}

long
random(void)
{
    g_uRandom = g_uRandom * 1103515245UL + 12345UL;
    return (long)((g_uRandom >> 16) & 0x7fffffffUL);
}

struct random_data {
    int32_t *fptr;
    int32_t *rptr;
    int32_t *state;
    int      rand_type;
    int      rand_deg;
    int      rand_sep;
    int32_t *end_ptr;
};

static char *g_pStateBuf;

int
srandom_r(unsigned int uSeed, struct random_data *pBuf)
{
    if (pBuf == NULL || pBuf->state == NULL) {
        errno = EINVAL;
        return -1;
    }
    pBuf->state[0] = (int32_t)(uSeed ? uSeed : 1u);
    pBuf->fptr = pBuf->state;
    pBuf->rptr = pBuf->state;
    return 0;
}

int
random_r(struct random_data *pBuf, int32_t *pResult)
{
    unsigned long u;

    if (pBuf == NULL || pBuf->state == NULL || pResult == NULL) {
        errno = EINVAL;
        return -1;
    }
    u = (unsigned long)(uint32_t)pBuf->state[0];
    u = u * 1103515245UL + 12345UL;
    pBuf->state[0] = (int32_t)u;
    *pResult = (int32_t)((u >> 16) & 0x7fffffffUL);
    return 0;
}

char *
initstate(unsigned int uSeed, char *szState, size_t n)
{
    char *pOld = g_pStateBuf;

    if (szState == NULL || n < sizeof(int32_t)) {
        errno = EINVAL;
        return NULL;
    }
    g_pStateBuf = szState;
    (void)memset(szState, 0, n);
    *(int32_t *)(void *)szState = (int32_t)(uSeed ? uSeed : 1u);
    g_uRandom = uSeed ? (unsigned long)uSeed : 1UL;
    return pOld != NULL ? pOld : szState;
}

char *
setstate(char *szState)
{
    char *pOld = g_pStateBuf;

    if (szState == NULL) {
        errno = EINVAL;
        return NULL;
    }
    g_pStateBuf = szState;
    g_uRandom = (unsigned long)(uint32_t)(*(int32_t *)(void *)szState);
    return pOld;
}

int
initstate_r(unsigned int uSeed, char *szState, size_t n,
            struct random_data *pBuf)
{
    if (szState == NULL || n < sizeof(int32_t) || pBuf == NULL) {
        errno = EINVAL;
        return -1;
    }
    (void)memset(szState, 0, n);
    pBuf->state = (int32_t *)(void *)szState;
    pBuf->fptr = pBuf->state;
    pBuf->rptr = pBuf->state;
    pBuf->end_ptr = (int32_t *)(void *)(szState + n);
    pBuf->rand_type = 0;
    pBuf->rand_deg = 0;
    pBuf->rand_sep = 0;
    pBuf->state[0] = (int32_t)(uSeed ? uSeed : 1u);
    return 0;
}

int
setstate_r(char *szState, struct random_data *pBuf)
{
    if (szState == NULL || pBuf == NULL) {
        errno = EINVAL;
        return -1;
    }
    pBuf->state = (int32_t *)(void *)szState;
    pBuf->fptr = pBuf->state;
    pBuf->rptr = pBuf->state;
    return 0;
}

div_t
div(int nNumer, int nDenom)
{
    div_t r;

    if (nDenom == 0) {
        r.quot = 0;
        r.rem = nNumer;
        return r;
    }
    /* Soft: avoid INT_MIN / -1 overflow trap on two's complement. */
    if (nNumer == (int)(((unsigned int)1) << (sizeof(int) * 8 - 1)) &&
        nDenom == -1) {
        r.quot = nNumer;
        r.rem = 0;
        return r;
    }
    r.quot = nNumer / nDenom;
    r.rem = nNumer % nDenom;
    return r;
}

ldiv_t
ldiv(long nNumer, long nDenom)
{
    ldiv_t r;

    if (nDenom == 0) {
        r.quot = 0;
        r.rem = nNumer;
        return r;
    }
    if (nNumer == (long)(((unsigned long)1) << (sizeof(long) * 8 - 1)) &&
        nDenom == -1L) {
        r.quot = nNumer;
        r.rem = 0;
        return r;
    }
    r.quot = nNumer / nDenom;
    r.rem = nNumer % nDenom;
    return r;
}

lldiv_t
lldiv(long long nNumer, long long nDenom)
{
    lldiv_t r;

    if (nDenom == 0) {
        r.quot = 0;
        r.rem = nNumer;
        return r;
    }
    if (nNumer == (long long)(((unsigned long long)1) << 63) &&
        nDenom == -1LL) {
        r.quot = nNumer;
        r.rem = 0;
        return r;
    }
    r.quot = nNumer / nDenom;
    r.rem = nNumer % nDenom;
    return r;
}

void *
reallocarray(void *p, size_t n, size_t cb)
{
    size_t total;

    if (n != 0 && cb > (size_t)-1 / n) {
        errno = ENOMEM;
        return NULL;
    }
    total = n * cb;
    return realloc(p, total);
}

void *
memalign(size_t uAlign, size_t cb)
{
    if (uAlign == 0 || (uAlign & (uAlign - 1)) != 0) {
        errno = EINVAL;
        return NULL;
    }
    return aligned_alloc(uAlign, cb);
}

void *
valloc(size_t cb)
{
    size_t uPage = (size_t)getpagesize();

    if (uPage == 0) {
        uPage = 4096;
    }
    return memalign(uPage, cb);
}

char *
get_current_dir_name(void)
{
    size_t cb = 256;
    char *p;

    for (;;) {
        p = (char *)malloc(cb);
        if (p == NULL) {
            return NULL;
        }
        if (getcwd(p, cb) != NULL) {
            return p;
        }
        {
            int nErr = errno;

            free(p);
            if (nErr != ERANGE) {
                errno = nErr;
                return NULL;
            }
        }
        if (cb > (size_t)-1 / 2) {
            errno = ENOMEM;
            return NULL;
        }
        cb *= 2;
        if (cb > 1024u * 1024u) {
            errno = ENAMETOOLONG;
            return NULL;
        }
    }
}
