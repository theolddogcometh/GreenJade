/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * POSIX rand48 family (48-bit LCG). Integer paths primary; reentrant double
 * outs write IEEE unit bits via integer stores (safe under -mgeneral-regs-only
 * product paths). Non-reentrant drand48/erand48 live in graph_batch88.
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

/*
 * Map 48-bit residue to IEEE754 binary64 bits for value in [0.0, 1.0).
 * Uses only integer arithmetic — no SSE required for the conversion itself.
 * Formula: (x / 2^48) as double = (x | 0x3FF0000000000000) - 1.0 but we
 * construct the fraction bits carefully:
 *   value = mantissa / 2^52 with exponent for 2^-1..2^0 range.
 * Soft: pack (1023-1)<<52 | (top 52 of 48-bit zero-padded) is wrong for full
 * range. Standard approach:  u * 0x1p-48  via bit construction:
 *   bits = (u << 4)  as fraction of 2^-48..; use ldexp-free form:
 *   IEEE for k/2^48 = exponent 1023-48 when k has 48 significant bits, etc.
 *
 * Simpler soft (matches common freestanding libc):
 *   bits = 0x3FF0000000000000ULL | (u << 4) then subtract 1.0 in integer
 *   domain by clearing the hidden bit path — actually the clean integer
 *   method is:
 *     bits = ((uint64_t)u << 4);  // top 48 of 52 fraction
 *     bits |= (0x3FEuLL << 52);   // exponent for [0.5, 1) when msb set…
 *
 * Portable soft used here: write the exact bits of (u / 2^48) by placing
 * u into the 48 MSBs of the 52-bit mantissa with unbiased exponent -1 when
 * u >= 2^47, else normalize.
 */
static uint64_t
r48_to_unit_bits(const unsigned short x[3])
{
    uint64_t u;
    int nExp;
    uint64_t m;

    u = ((uint64_t)x[2] << 32) | ((uint64_t)x[1] << 16) | (uint64_t)x[0];
    if (u == 0) {
        return 0; /* +0.0 */
    }
    /* Normalize so top bit of mantissa is at bit 52 (hidden bit position). */
    nExp = 0;
    m = u;
    /* u is < 2^48; shift left until bit 47 set, then align to bit 52. */
    while ((m & (1ULL << 47)) == 0) {
        m <<= 1;
        nExp++;
    }
    /* m has bit 47 set; move to bit 52 (shift +5), drop hidden bit. */
    m <<= 5;
    m &= (1ULL << 52) - 1; /* drop hidden 1 at bit 52 */
    /* unbiased exponent: value = 2^(47-nExp-52+bias stuff)...
     * u/2^48 = m_full / 2^(48 - (shift amount))
     * After while, m = u << nExp with bit 47 set, so
     * u/2^48 = (m / 2^47) / 2^(48-47-nExp wait):
     * u = m >> nExp, u/2^48 = m / 2^(48+nExp)
     * With hidden bit at 2^47 of m before <<5:
     * m' = m << 5 has hidden at bit 52; significand = 1.frac
     * value = 2^(47 - nExp - 48) * (1.frac) = 2^(-1 - nExp) * (1.frac)
     * biased exp = 1023 + (-1 - nExp) = 1022 - nExp
     */
    {
        uint64_t e = (uint64_t)(1022 - nExp);

        return (e << 52) | m;
    }
}

void
srand48(long nSeed)
{
    g_x[0] = 0x330e;
    g_x[1] = (unsigned short)(nSeed & 0xffffL);
    g_x[2] = (unsigned short)((nSeed >> 16) & 0xffffL);
    g_c = 0x000b;
    g_a = 0x5deece66dULL;
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
    if (pBuf->__a == 0) {
        pBuf->__a = 0x5deece66dULL;
    }
    if (pBuf->__c == 0 && pBuf->__init == 0) {
        pBuf->__c = 0x000b;
    }
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
    unsigned long long a;
    unsigned short c;

    if (pBuf == NULL || pResult == NULL) {
        return -1;
    }
    a = pBuf->__a ? pBuf->__a : 0x5deece66dULL;
    c = pBuf->__init ? pBuf->__c : 0x000b;
    if (!pBuf->__init) {
        pBuf->__c = 0x000b;
        pBuf->__a = 0x5deece66dULL;
        pBuf->__init = 1;
    }
    r48_step(pBuf->__x, c, a);
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
    unsigned long long a;
    unsigned short c;

    if (pBuf == NULL || pResult == NULL) {
        return -1;
    }
    a = pBuf->__a ? pBuf->__a : 0x5deece66dULL;
    c = pBuf->__c ? pBuf->__c : 0x000b;
    r48_step(pBuf->__x, c, a);
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
    uint64_t bits;

    if (pResult == NULL) {
        return -1;
    }
    if (lrand48_r(pBuf, &v) != 0) {
        return -1;
    }
    (void)v;
    bits = r48_to_unit_bits(pBuf->__x);
    memcpy(pResult, &bits, sizeof(bits));
    return 0;
}

int
erand48_r(unsigned short aXsubi[3], struct drand48_data *pBuf, double *pResult)
{
    long v;
    uint64_t bits;

    if (pResult == NULL || aXsubi == NULL) {
        return -1;
    }
    if (nrand48_r(aXsubi, pBuf, &v) != 0) {
        return -1;
    }
    (void)v;
    bits = r48_to_unit_bits(aXsubi);
    memcpy(pResult, &bits, sizeof(bits));
    return 0;
}
