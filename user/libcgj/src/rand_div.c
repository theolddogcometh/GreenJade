/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * rand/random, div family, reallocarray/memalign, get_current_dir_name.
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
    g_uRand = uSeed;
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
    u = *pSeed;
    u = u * 1103515245UL + 12345UL;
    *pSeed = (unsigned int)u;
    return (int)((u / 65536UL) % 32768UL);
}

/* BSD random(3) — same LCG for bring-up */
static unsigned long g_uRandom = 1UL;

void
srandom(unsigned int uSeed)
{
    g_uRandom = uSeed ? uSeed : 1UL;
}

long
random(void)
{
    g_uRandom = g_uRandom * 1103515245UL + 12345UL;
    return (long)((g_uRandom >> 16) & 0x7fffffffUL);
}

/* BSD random_r / initstate family (bring-up LCG; not full 31-bit poly). */
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
    g_uRandom = uSeed ? uSeed : 1UL;
    return pOld;
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

    r.quot = nNumer / nDenom;
    r.rem = nNumer % nDenom;
    return r;
}

ldiv_t
ldiv(long nNumer, long nDenom)
{
    ldiv_t r;

    r.quot = nNumer / nDenom;
    r.rem = nNumer % nDenom;
    return r;
}

lldiv_t
lldiv(long long nNumer, long long nDenom)
{
    lldiv_t r;

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
    return aligned_alloc(uAlign, cb);
}

void *
valloc(size_t cb)
{
    return memalign((size_t)getpagesize(), cb);
}

char *
get_current_dir_name(void)
{
    char aBuf[4096];
    char *p;

    if (getcwd(aBuf, sizeof(aBuf)) == NULL) {
        return NULL;
    }
    p = (char *)malloc(strlen(aBuf) + 1);
    if (p == NULL) {
        return NULL;
    }
    memcpy(p, aBuf, strlen(aBuf) + 1);
    return p;
}
