/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch85: PCLMULQDQ-accelerated GHASH (NIST SP 800-38D).
 * Userspace SSE2 + CLMUL hot path (CPUID.1 ECX bit 1); integer bit-serial
 * fallback. Does NOT redefine ghash (batch60). Clean-room from public NIST /
 * Intel CLMUL docs.
 *
 * Surface:
 *   ghash_pclmul(const unsigned char H[16], const unsigned char *data,
 *                size_t len, unsigned char out[16]);
 *   int gj_cpu_has_pclmul(void);  // CPUID.1 ECX bit 1 (PCLMULQDQ)
 *   __ghash_pclmul / __gj_cpu_has_pclmul
 *   __libcgj_batch85_marker = "libcgj-batch85"
 */

#include <stddef.h>
#include <stdint.h>

#if defined(__x86_64__) || defined(__i386__) || defined(_M_X64) || defined(_M_IX86)
#include <emmintrin.h>
#include <wmmintrin.h>
#endif

const char __libcgj_batch85_marker[] = "libcgj-batch85";

/* ---- freestanding helpers ---------------------------------------------- */

static void
b85_memzero(void *pDst, size_t cb)
{
    unsigned char *p = (unsigned char *)pDst;
    size_t i;

    for (i = 0; i < cb; i++) {
        p[i] = 0;
    }
}

static uint64_t
b85_load_be64(const unsigned char *p)
{
    return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) |
           ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) |
           ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) |
           ((uint64_t)p[6] << 8) | (uint64_t)p[7];
}

static void
b85_store_be64(unsigned char *p, uint64_t u)
{
    p[0] = (unsigned char)(u >> 56);
    p[1] = (unsigned char)(u >> 48);
    p[2] = (unsigned char)(u >> 40);
    p[3] = (unsigned char)(u >> 32);
    p[4] = (unsigned char)(u >> 24);
    p[5] = (unsigned char)(u >> 16);
    p[6] = (unsigned char)(u >> 8);
    p[7] = (unsigned char)u;
}

/* ========================================================================
 * CPUID: PCLMULQDQ = CPUID.1:ECX[1]
 * ======================================================================== */

int
gj_cpu_has_pclmul(void)
{
    /* -1 unknown, 0 no, 1 yes */
    static int g_nPclmul = -1;
#if defined(__x86_64__) || defined(__i386__)
    unsigned int uEax;
    unsigned int uEbx;
    unsigned int uEcx;
    unsigned int uEdx;
#endif

    if (g_nPclmul >= 0) {
        return g_nPclmul;
    }

#if defined(__x86_64__) || defined(__i386__)
    uEax = 0;
    uEbx = 0;
    uEcx = 0;
    uEdx = 0;
    /*
     * Leaf 1 feature flags. ECX bit 1 = PCLMULQDQ (Intel SDM Vol 2).
     * "=c" must not overlap the input "c"(0) constraint on older GCCs —
     * use a separate output binding via early-clobber-safe form.
     */
    __asm__ volatile("cpuid"
                     : "=a"(uEax), "=b"(uEbx), "=c"(uEcx), "=d"(uEdx)
                     : "a"(1u), "c"(0u)
                     : "cc");
    g_nPclmul = (int)((uEcx >> 1) & 1u);
    (void)uEax;
    (void)uEbx;
    (void)uEdx;
#else
    g_nPclmul = 0;
#endif
    return g_nPclmul;
}

int __gj_cpu_has_pclmul(void) __attribute__((alias("gj_cpu_has_pclmul")));

/* ========================================================================
 * Bit-serial GHASH multiply (same field / bit order as batch60)
 * Irreducible: x^128 + x^7 + x^2 + x + 1  (R = 0xe1 || 0^120)
 * Words are load_be64 of the 16-byte NIST block (leftmost bit = bit 63 of
 * the high word). CLMUL path bit-reverses into bit-i = x^i order.
 * ======================================================================== */

static void
b85_gf_mul_bitserial(uint64_t *pZh, uint64_t *pZl, uint64_t uXh, uint64_t uXl,
                     uint64_t uYh, uint64_t uYl)
{
    uint64_t uZh = 0;
    uint64_t uZl = 0;
    uint64_t uVh = uXh;
    uint64_t uVl = uXl;
    unsigned iBit;

    /* NIST: Z←0, V←X; for i=0..127 if Y_i then Z←Z⊕V; rightshift V (⊕R). */
    for (iBit = 0; iBit < 128u; iBit++) {
        uint64_t uYbit;

        if (iBit < 64u) {
            uYbit = (uYh >> (63u - iBit)) & 1ull;
        } else {
            uYbit = (uYl >> (127u - iBit)) & 1ull;
        }
        if (uYbit != 0ull) {
            uZh ^= uVh;
            uZl ^= uVl;
        }
        {
            uint64_t uLsb = uVl & 1ull;

            uVl = (uVl >> 1) | (uVh << 63);
            uVh = uVh >> 1;
            if (uLsb != 0ull) {
                uVh ^= 0xe100000000000000ull;
            }
        }
    }
    *pZh = uZh;
    *pZl = uZl;
}

/*
 * Reverse bits of a 64-bit word (maps NIST left-to-right bits onto CLMUL
 * polynomial significance bit i = x^i, and back).
 */
static uint64_t
b85_bitrev64(uint64_t u)
{
    u = ((u >> 1) & 0x5555555555555555ull) | ((u & 0x5555555555555555ull) << 1);
    u = ((u >> 2) & 0x3333333333333333ull) | ((u & 0x3333333333333333ull) << 2);
    u = ((u >> 4) & 0x0f0f0f0f0f0f0f0full) | ((u & 0x0f0f0f0f0f0f0f0full) << 4);
    u = ((u >> 8) & 0x00ff00ff00ff00ffull) | ((u & 0x00ff00ff00ff00ffull) << 8);
    u = ((u >> 16) & 0x0000ffff0000ffffull) | ((u & 0x0000ffff0000ffffull) << 16);
    u = (u >> 32) | (u << 32);
    return u;
}

/*
 * Reduce 256-bit product (bit i = x^i) mod P = x^128 + x^7 + x^2 + x + 1.
 * u0..u3 are limbs [63:0] .. [255:192]. Result in *pZh:*pZl (high:low 64)
 * still in CLMUL bit order (not yet bit-reversed back to NIST words).
 */
static void
b85_gf_reduce256(uint64_t *pZh, uint64_t *pZl, uint64_t u0, uint64_t u1,
                 uint64_t u2, uint64_t u3)
{
    uint64_t uL0;
    uint64_t uL1;
    uint64_t uL2;
    uint64_t uE;
    uint64_t uF;

    /*
     * product = lo128 + hi128 * x^128
     * x^128 ≡ 1 + x + x^2 + x^7  (mod P)
     * Fold hi * (1+x+x^2+x^7) into lo; then fold ~7 overflow bits once more.
     */
    uL0 = u2;
    uL1 = u3;
    uL2 = 0;

    /* * x */
    uL0 ^= u2 << 1;
    uL1 ^= (u3 << 1) | (u2 >> 63);
    uL2 ^= u3 >> 63;

    /* * x^2 */
    uL0 ^= u2 << 2;
    uL1 ^= (u3 << 2) | (u2 >> 62);
    uL2 ^= u3 >> 62;

    /* * x^7 */
    uL0 ^= u2 << 7;
    uL1 ^= (u3 << 7) | (u2 >> 57);
    uL2 ^= u3 >> 57;

    u0 ^= uL0;
    u1 ^= uL1;

    /* uL2 holds degrees 128..134 of the intermediate; fold again. */
    uE = uL2;
    uF = uE ^ (uE << 1) ^ (uE << 2) ^ (uE << 7);
    u0 ^= uF;

    *pZl = u0;
    *pZh = u1;
}

#if defined(__x86_64__) || defined(__i386__) || defined(_M_X64) || defined(_M_IX86)

/*
 * 128×128 carry-less multiply via PCLMULQDQ → 256-bit product limbs.
 * Operands are in CLMUL order (bit i = coefficient of x^i).
 */
__attribute__((target("pclmul,sse2")))
static void
b85_clmul_product(uint64_t *p0, uint64_t *p1, uint64_t *p2, uint64_t *p3,
                  uint64_t uAh, uint64_t uAl, uint64_t uBh, uint64_t uBl)
{
    __m128i xA = _mm_set_epi64x((long long)uAh, (long long)uAl);
    __m128i xB = _mm_set_epi64x((long long)uBh, (long long)uBl);
    __m128i xT00 = _mm_clmulepi64_si128(xA, xB, 0x00); /* Al * Bl */
    __m128i xT11 = _mm_clmulepi64_si128(xA, xB, 0x11); /* Ah * Bh */
    __m128i xT01 = _mm_clmulepi64_si128(xA, xB, 0x01); /* Al * Bh */
    __m128i xT10 = _mm_clmulepi64_si128(xA, xB, 0x10); /* Ah * Bl */
    __m128i xMid = _mm_xor_si128(xT01, xT10);
    uint64_t uT00l = (uint64_t)_mm_cvtsi128_si64(xT00);
    uint64_t uT00h =
        (uint64_t)_mm_cvtsi128_si64(_mm_srli_si128(xT00, 8));
    uint64_t uT11l = (uint64_t)_mm_cvtsi128_si64(xT11);
    uint64_t uT11h =
        (uint64_t)_mm_cvtsi128_si64(_mm_srli_si128(xT11, 8));
    uint64_t uMidl = (uint64_t)_mm_cvtsi128_si64(xMid);
    uint64_t uMidh =
        (uint64_t)_mm_cvtsi128_si64(_mm_srli_si128(xMid, 8));

    /* product = T00 + (mid << 64) + (T11 << 128)  in GF(2) */
    *p0 = uT00l;
    *p1 = uT00h ^ uMidl;
    *p2 = uT11l ^ uMidh;
    *p3 = uT11h;
}

/*
 * GF(2^128) mul matching batch60/NIST GHASH bit order.
 *
 * NIST numbers bits left-to-right as 0..127 in the 16-byte block. Under the
 * SP 800-38D multiply algorithm that is the dual of CLMUL's bit-i = x^i
 * convention, so we bit-reverse both operands into CLMUL order, multiply +
 * reduce with P(x)=x^128+x^7+x^2+x+1, then bit-reverse the 128-bit result
 * back to NIST words (high/low as load_be64).
 */
__attribute__((target("pclmul,sse2")))
static void
b85_gf_mul_pclmul(uint64_t *pZh, uint64_t *pZl, uint64_t uXh, uint64_t uXl,
                  uint64_t uYh, uint64_t uYl)
{
    uint64_t uA_lo = b85_bitrev64(uXh);
    uint64_t uA_hi = b85_bitrev64(uXl);
    uint64_t uB_lo = b85_bitrev64(uYh);
    uint64_t uB_hi = b85_bitrev64(uYl);
    uint64_t u0;
    uint64_t u1;
    uint64_t u2;
    uint64_t u3;
    uint64_t uRh;
    uint64_t uRl;

    b85_clmul_product(&u0, &u1, &u2, &u3, uA_hi, uA_lo, uB_hi, uB_lo);
    b85_gf_reduce256(&uRh, &uRl, u0, u1, u2, u3);
    *pZh = b85_bitrev64(uRl);
    *pZl = b85_bitrev64(uRh);
}

#else /* !x86 */

static void
b85_gf_mul_pclmul(uint64_t *pZh, uint64_t *pZl, uint64_t uXh, uint64_t uXl,
                  uint64_t uYh, uint64_t uYl)
{
    /* No CLMUL on this arch — bit-serial only. */
    b85_gf_mul_bitserial(pZh, pZl, uXh, uXl, uYh, uYl);
}

#endif

/* ========================================================================
 * One-shot GHASH (same semantics as batch60 ghash)
 * ======================================================================== */

static void
b85_ghash_core(const unsigned char aH[16], const unsigned char *pData,
               size_t cbData, unsigned char aOut[16], int fUsePclmul)
{
    uint64_t uYh = 0;
    uint64_t uYl = 0;
    uint64_t uHh;
    uint64_t uHl;
    unsigned char aBuf[16];
    size_t cbBuf = 0;
    size_t cbOff = 0;

    uHh = b85_load_be64(aH);
    uHl = b85_load_be64(aH + 8);

    if (pData != NULL && cbData != 0) {
        while (cbOff + 16u <= cbData) {
            uint64_t uXh;
            uint64_t uXl;

            uXh = uYh ^ b85_load_be64(pData + cbOff);
            uXl = uYl ^ b85_load_be64(pData + cbOff + 8);
            if (fUsePclmul) {
                b85_gf_mul_pclmul(&uYh, &uYl, uXh, uXl, uHh, uHl);
            } else {
                b85_gf_mul_bitserial(&uYh, &uYl, uXh, uXl, uHh, uHl);
            }
            cbOff += 16u;
        }
        if (cbOff < cbData) {
            size_t cbRem = cbData - cbOff;
            size_t i;

            b85_memzero(aBuf, 16);
            for (i = 0; i < cbRem; i++) {
                aBuf[i] = pData[cbOff + i];
            }
            cbBuf = 16u; /* mark: full padded block pending */
            {
                uint64_t uXh = uYh ^ b85_load_be64(aBuf);
                uint64_t uXl = uYl ^ b85_load_be64(aBuf + 8);

                if (fUsePclmul) {
                    b85_gf_mul_pclmul(&uYh, &uYl, uXh, uXl, uHh, uHl);
                } else {
                    b85_gf_mul_bitserial(&uYh, &uYl, uXh, uXl, uHh, uHl);
                }
            }
            (void)cbBuf;
        }
    }

    b85_store_be64(aOut, uYh);
    b85_store_be64(aOut + 8, uYl);
}

/*
 * ghash_pclmul — one-shot GHASH_H(data) with zero-pad of a short final block.
 * Uses PCLMUL when gj_cpu_has_pclmul(); otherwise bit-serial (same field as
 * batch60). Does not append GCM length blocks (caller builds full input).
 */
void
ghash_pclmul(const unsigned char aH[16], const unsigned char *pData,
             size_t cbData, unsigned char aOut[16])
{
    int fPclmul;

    if (aH == NULL || aOut == NULL) {
        return;
    }

    fPclmul = gj_cpu_has_pclmul();
    /*
     * PCLMUL path when CPUID.1:ECX[1] is set; otherwise integer bit-serial
     * (same field as batch60 ghash). Local fallback keeps this TU correct
     * freestanding without a link dependency on batch60.
     */
    b85_ghash_core(aH, pData, cbData, aOut, fPclmul);
}

void __ghash_pclmul(const unsigned char aH[16], const unsigned char *pData,
                    size_t cbData, unsigned char aOut[16])
    __attribute__((alias("ghash_pclmul")));
