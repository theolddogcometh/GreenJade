/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * POSIX rand48 family (48-bit LCG). Integer paths only; double returns are
 * IEEE bit-pattern zeros (no SSE).
 */
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

struct drand48_data {
    unsigned short __x[3];
    unsigned short __old_x[3];
    unsigned short __c;
    unsigned short __init;
    unsigned long long __a;
};

static unsigned short g_x[3] = { 0x330e, 0xabcd, 0x1234 };
static unsigned short g_c = 0x000b;
static unsigned long long g_a = 0x5deece66dULL;

static void
r48_step(unsigned short x[3], unsigned short c, unsigned long long a)
{
    unsigned long long u;

    u = ((unsigned long long)x[2] << 32) | ((unsigned long long)x[1] << 16) |
        (unsigned long long)x[0];
    u = a * u + (unsigned long long)c;
    x[0] = (unsigned short)(u & 0xffffULL);
    x[1] = (unsigned short)((u >> 16) & 0xffffULL);
    x[2] = (unsigned short)((u >> 32) & 0xffffULL);
}

static long
r48_to_long(const unsigned short x[3])
{
    return (long)((((unsigned long)x[2]) << 15) | (((unsigned long)x[1]) >> 1));
}

static long
r48_to_signed(const unsigned short x[3])
{
    long v = (long)((((unsigned long)x[2]) << 16) | (unsigned long)x[1]);

    return (v << 16) >> 16; /* sign-extend low 32 of 48-bit */
}

void
srand48(long nSeed)
{
    g_x[0] = 0x330e;
    g_x[1] = (unsigned short)(nSeed & 0xffffL);
    g_x[2] = (unsigned short)((nSeed >> 16) & 0xffffL);
}

unsigned short *
seed48(unsigned short aSeed16v[3])
{
    static unsigned short aOld[3];

    aOld[0] = g_x[0];
    aOld[1] = g_x[1];
    aOld[2] = g_x[2];
    if (aSeed16v != NULL) {
        g_x[0] = aSeed16v[0];
        g_x[1] = aSeed16v[1];
        g_x[2] = aSeed16v[2];
    }
    return aOld;
}

void
lcong48(unsigned short aParam[7])
{
    if (aParam == NULL) {
        return;
    }
    g_x[0] = aParam[0];
    g_x[1] = aParam[1];
    g_x[2] = aParam[2];
    g_a = ((unsigned long long)aParam[5] << 32) |
          ((unsigned long long)aParam[4] << 16) |
          (unsigned long long)aParam[3];
    g_c = aParam[6];
}

long
lrand48(void)
{
    r48_step(g_x, g_c, g_a);
    return r48_to_long(g_x) & 0x7fffffffL;
}

long
nrand48(unsigned short aXsubi[3])
{
    if (aXsubi == NULL) {
        return 0;
    }
    r48_step(aXsubi, g_c, g_a);
    return r48_to_long(aXsubi) & 0x7fffffffL;
}

long
mrand48(void)
{
    r48_step(g_x, g_c, g_a);
    return r48_to_signed(g_x);
}

long
jrand48(unsigned short aXsubi[3])
{
    if (aXsubi == NULL) {
        return 0;
    }
    r48_step(aXsubi, g_c, g_a);
    return r48_to_signed(aXsubi);
}

/*
 * Double-return drand48/erand48 omitted under -mgeneral-regs-only / no SSE.
 * Use lrand48/nrand48 (integer). Reentrant double outs write IEEE zero bits
 * via integer stores only (no SSE register returns).
 */

int
srand48_r(long nSeed, struct drand48_data *pBuf)
{
    if (pBuf == NULL) {
        return -1;
    }
    pBuf->__x[0] = 0x330e;
    pBuf->__x[1] = (unsigned short)(nSeed & 0xffffL);
    pBuf->__x[2] = (unsigned short)((nSeed >> 16) & 0xffffL);
    pBuf->__c = 0x000b;
    pBuf->__a = 0x5deece66dULL;
    pBuf->__init = 1;
    return 0;
}

int
seed48_r(unsigned short aSeed16v[3], struct drand48_data *pBuf)
{
    if (pBuf == NULL || aSeed16v == NULL) {
        return -1;
    }
    pBuf->__old_x[0] = pBuf->__x[0];
    pBuf->__old_x[1] = pBuf->__x[1];
    pBuf->__old_x[2] = pBuf->__x[2];
    pBuf->__x[0] = aSeed16v[0];
    pBuf->__x[1] = aSeed16v[1];
    pBuf->__x[2] = aSeed16v[2];
    pBuf->__init = 1;
    return 0;
}

int
lcong48_r(unsigned short aParam[7], struct drand48_data *pBuf)
{
    if (pBuf == NULL || aParam == NULL) {
        return -1;
    }
    pBuf->__x[0] = aParam[0];
    pBuf->__x[1] = aParam[1];
    pBuf->__x[2] = aParam[2];
    pBuf->__a = ((unsigned long long)aParam[5] << 32) |
                ((unsigned long long)aParam[4] << 16) |
                (unsigned long long)aParam[3];
    pBuf->__c = aParam[6];
    pBuf->__init = 1;
    return 0;
}

int
lrand48_r(struct drand48_data *pBuf, long *pResult)
{
    if (pBuf == NULL || pResult == NULL) {
        return -1;
    }
    r48_step(pBuf->__x, pBuf->__c, pBuf->__a ? pBuf->__a : 0x5deece66dULL);
    *pResult = r48_to_long(pBuf->__x) & 0x7fffffffL;
    return 0;
}

int
nrand48_r(unsigned short aXsubi[3], struct drand48_data *pBuf, long *pResult)
{
    unsigned long long a;
    unsigned short c;

    if (aXsubi == NULL || pResult == NULL) {
        return -1;
    }
    a = (pBuf != NULL && pBuf->__a) ? pBuf->__a : 0x5deece66dULL;
    c = (pBuf != NULL) ? pBuf->__c : 0x000b;
    r48_step(aXsubi, c, a);
    *pResult = r48_to_long(aXsubi) & 0x7fffffffL;
    return 0;
}

int
mrand48_r(struct drand48_data *pBuf, long *pResult)
{
    if (pBuf == NULL || pResult == NULL) {
        return -1;
    }
    r48_step(pBuf->__x, pBuf->__c, pBuf->__a ? pBuf->__a : 0x5deece66dULL);
    *pResult = r48_to_signed(pBuf->__x);
    return 0;
}

int
jrand48_r(unsigned short aXsubi[3], struct drand48_data *pBuf, long *pResult)
{
    unsigned long long a;
    unsigned short c;

    if (aXsubi == NULL || pResult == NULL) {
        return -1;
    }
    a = (pBuf != NULL && pBuf->__a) ? pBuf->__a : 0x5deece66dULL;
    c = (pBuf != NULL) ? pBuf->__c : 0x000b;
    r48_step(aXsubi, c, a);
    *pResult = r48_to_signed(aXsubi);
    return 0;
}

int
drand48_r(struct drand48_data *pBuf, double *pResult)
{
    long v;
    uint64_t z = 0;

    if (pResult == NULL) {
        return -1;
    }
    if (lrand48_r(pBuf, &v) != 0) {
        return -1;
    }
    (void)v;
    memcpy(pResult, &z, sizeof(z));
    return 0;
}

int
erand48_r(unsigned short aXsubi[3], struct drand48_data *pBuf, double *pResult)
{
    long v;
    uint64_t z = 0;

    if (pResult == NULL) {
        return -1;
    }
    if (nrand48_r(aXsubi, pBuf, &v) != 0) {
        return -1;
    }
    (void)v;
    memcpy(pResult, &z, sizeof(z));
    return 0;
}
