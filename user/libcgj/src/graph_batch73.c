/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch73: Ed25519 (RFC 8032) pure twisted-Edwards
 * signature scheme. Clean-room from the public RFC only; freestanding
 * integer arithmetic (no SSE / no FP). Dual MIT OR Apache-2.0.
 *
 * Does NOT redefine x25519 (batch64 owns Curve25519 ECDH).
 * SHA-512 is extern from batch31 (SHA512_Init / Update / Final).
 *
 * Surface:
 *   ed25519_public_key / __ed25519_public_key
 *   ed25519_sign        / __ed25519_sign
 *   ed25519_verify      / __ed25519_verify   (0 ok / -1 fail)
 *   __libcgj_batch73_marker = "libcgj-batch73"
 *
 * Field: GF(2^255-19) as five 51-bit limbs (same layout as batch64).
 * Group: extended twisted Edwards (X:Y:Z:T) with XY = ZT, a = -1.
 * Curve: -x^2 + y^2 = 1 + d x^2 y^2, d = -121665/121666.
 *
 * Soft deepen (no API break / no multi-def):
 *   Null contract: sk/pk/msg/sig NULL → public_key/sign no-op; verify -1.
 *   Does not redefine x25519 (batch64). SHA-512 from batch31 (extern).
 */
#include <stddef.h>
#include <stdint.h>

/* SHA-512 from batch31 — digest 64, context ~224B */
void SHA512_Init(void *pCtx);
void SHA512_Update(void *pCtx, const void *pData, size_t cb);
void SHA512_Final(unsigned char aDig[64], void *pCtx);

const char __libcgj_batch73_marker[] = "libcgj-batch73";

#define B73_SHA512_CTX 256u
#define B73_MASK51     0x7ffffffffffffull

/* --------------------------------------------------------------------------
 * Tiny freestanding memory helpers (-fno-builtin safe)
 * -------------------------------------------------------------------------- */

static void
b73_bzero(void *pDst, size_t cb)
{
	unsigned char *p = (unsigned char *)pDst;
	size_t i;

	for (i = 0; i < cb; i++) {
		p[i] = 0;
	}
}

static void
b73_bcopy(void *pDst, const void *pSrc, size_t cb)
{
	unsigned char *pD = (unsigned char *)pDst;
	const unsigned char *pS = (const unsigned char *)pSrc;
	size_t i;

	for (i = 0; i < cb; i++) {
		pD[i] = pS[i];
	}
}

static int
b73_bcmp(const void *pA, const void *pB, size_t cb)
{
	const unsigned char *pX = (const unsigned char *)pA;
	const unsigned char *pY = (const unsigned char *)pB;
	unsigned uDiff = 0;
	size_t i;

	for (i = 0; i < cb; i++) {
		uDiff |= (unsigned)(pX[i] ^ pY[i]);
	}
	return (int)uDiff;
}

/* --------------------------------------------------------------------------
 * Field arithmetic over GF(2^255 - 19) — five 51-bit limbs
 * -------------------------------------------------------------------------- */

typedef struct {
	uint64_t aL[5];
} b73_fe;

static void
b73_fe_0(b73_fe *pH)
{
	pH->aL[0] = 0;
	pH->aL[1] = 0;
	pH->aL[2] = 0;
	pH->aL[3] = 0;
	pH->aL[4] = 0;
}

static void
b73_fe_1(b73_fe *pH)
{
	pH->aL[0] = 1;
	pH->aL[1] = 0;
	pH->aL[2] = 0;
	pH->aL[3] = 0;
	pH->aL[4] = 0;
}

static void
b73_fe_copy(b73_fe *pH, const b73_fe *pF)
{
	pH->aL[0] = pF->aL[0];
	pH->aL[1] = pF->aL[1];
	pH->aL[2] = pF->aL[2];
	pH->aL[3] = pF->aL[3];
	pH->aL[4] = pF->aL[4];
}

/* Constant-time conditional move: if uB==1 pF←pG; if uB==0 no-op. */
static void
b73_fe_cmov(b73_fe *pF, const b73_fe *pG, unsigned uB)
{
	uint64_t uMask = (uint64_t)0 - (uint64_t)uB;
	unsigned i;

	for (i = 0; i < 5u; i++) {
		pF->aL[i] ^= uMask & (pF->aL[i] ^ pG->aL[i]);
	}
}

static void
b73_fe_frombytes(b73_fe *pH, const unsigned char aS[32])
{
	uint64_t u0, u1, u2, u3;

	u0 = (uint64_t)aS[0] | ((uint64_t)aS[1] << 8) | ((uint64_t)aS[2] << 16) |
	     ((uint64_t)aS[3] << 24) | ((uint64_t)aS[4] << 32) |
	     ((uint64_t)aS[5] << 40) | ((uint64_t)aS[6] << 48) |
	     ((uint64_t)aS[7] << 56);
	u1 = (uint64_t)aS[8] | ((uint64_t)aS[9] << 8) | ((uint64_t)aS[10] << 16) |
	     ((uint64_t)aS[11] << 24) | ((uint64_t)aS[12] << 32) |
	     ((uint64_t)aS[13] << 40) | ((uint64_t)aS[14] << 48) |
	     ((uint64_t)aS[15] << 56);
	u2 = (uint64_t)aS[16] | ((uint64_t)aS[17] << 8) |
	     ((uint64_t)aS[18] << 16) | ((uint64_t)aS[19] << 24) |
	     ((uint64_t)aS[20] << 32) | ((uint64_t)aS[21] << 40) |
	     ((uint64_t)aS[22] << 48) | ((uint64_t)aS[23] << 56);
	u3 = (uint64_t)aS[24] | ((uint64_t)aS[25] << 8) |
	     ((uint64_t)aS[26] << 16) | ((uint64_t)aS[27] << 24) |
	     ((uint64_t)aS[28] << 32) | ((uint64_t)aS[29] << 40) |
	     ((uint64_t)aS[30] << 48) | ((uint64_t)aS[31] << 56);

	/* Ignore top bit (sign for Ed25519 point encoding). */
	u3 &= 0x7fffffffffffffffull;

	pH->aL[0] = u0 & B73_MASK51;
	pH->aL[1] = ((u0 >> 51) | (u1 << 13)) & B73_MASK51;
	pH->aL[2] = ((u1 >> 38) | (u2 << 26)) & B73_MASK51;
	pH->aL[3] = ((u2 >> 25) | (u3 << 39)) & B73_MASK51;
	pH->aL[4] = (u3 >> 12) & B73_MASK51;
}

static void
b73_fe_reduce(b73_fe *pH)
{
	uint64_t u0 = pH->aL[0];
	uint64_t u1 = pH->aL[1];
	uint64_t u2 = pH->aL[2];
	uint64_t u3 = pH->aL[3];
	uint64_t u4 = pH->aL[4];
	uint64_t uQ;
	int i;

	for (i = 0; i < 2; i++) {
		u1 += u0 >> 51;
		u0 &= B73_MASK51;
		u2 += u1 >> 51;
		u1 &= B73_MASK51;
		u3 += u2 >> 51;
		u2 &= B73_MASK51;
		u4 += u3 >> 51;
		u3 &= B73_MASK51;
		u0 += 19ull * (u4 >> 51);
		u4 &= B73_MASK51;
	}

	uQ = (u0 + 19ull) >> 51;
	uQ = (u1 + uQ) >> 51;
	uQ = (u2 + uQ) >> 51;
	uQ = (u3 + uQ) >> 51;
	uQ = (u4 + uQ) >> 51;

	u0 += 19ull * uQ;
	u1 += u0 >> 51;
	u0 &= B73_MASK51;
	u2 += u1 >> 51;
	u1 &= B73_MASK51;
	u3 += u2 >> 51;
	u2 &= B73_MASK51;
	u4 += u3 >> 51;
	u3 &= B73_MASK51;
	u4 &= B73_MASK51;

	pH->aL[0] = u0;
	pH->aL[1] = u1;
	pH->aL[2] = u2;
	pH->aL[3] = u3;
	pH->aL[4] = u4;
}

static void
b73_fe_tobytes(unsigned char aS[32], const b73_fe *pH)
{
	b73_fe t;
	uint64_t u0, u1, u2, u3, u4;

	b73_fe_copy(&t, pH);
	b73_fe_reduce(&t);
	u0 = t.aL[0];
	u1 = t.aL[1];
	u2 = t.aL[2];
	u3 = t.aL[3];
	u4 = t.aL[4];

	aS[0] = (unsigned char)u0;
	aS[1] = (unsigned char)(u0 >> 8);
	aS[2] = (unsigned char)(u0 >> 16);
	aS[3] = (unsigned char)(u0 >> 24);
	aS[4] = (unsigned char)(u0 >> 32);
	aS[5] = (unsigned char)(u0 >> 40);
	aS[6] = (unsigned char)((u0 >> 48) | (u1 << 3));
	aS[7] = (unsigned char)(u1 >> 5);
	aS[8] = (unsigned char)(u1 >> 13);
	aS[9] = (unsigned char)(u1 >> 21);
	aS[10] = (unsigned char)(u1 >> 29);
	aS[11] = (unsigned char)(u1 >> 37);
	aS[12] = (unsigned char)((u1 >> 45) | (u2 << 6));
	aS[13] = (unsigned char)(u2 >> 2);
	aS[14] = (unsigned char)(u2 >> 10);
	aS[15] = (unsigned char)(u2 >> 18);
	aS[16] = (unsigned char)(u2 >> 26);
	aS[17] = (unsigned char)(u2 >> 34);
	aS[18] = (unsigned char)(u2 >> 42);
	aS[19] = (unsigned char)((u2 >> 50) | (u3 << 1));
	aS[20] = (unsigned char)(u3 >> 7);
	aS[21] = (unsigned char)(u3 >> 15);
	aS[22] = (unsigned char)(u3 >> 23);
	aS[23] = (unsigned char)(u3 >> 31);
	aS[24] = (unsigned char)(u3 >> 39);
	aS[25] = (unsigned char)((u3 >> 47) | (u4 << 4));
	aS[26] = (unsigned char)(u4 >> 4);
	aS[27] = (unsigned char)(u4 >> 12);
	aS[28] = (unsigned char)(u4 >> 20);
	aS[29] = (unsigned char)(u4 >> 28);
	aS[30] = (unsigned char)(u4 >> 36);
	aS[31] = (unsigned char)(u4 >> 44);
}

static void
b73_fe_add(b73_fe *pH, const b73_fe *pF, const b73_fe *pG)
{
	pH->aL[0] = pF->aL[0] + pG->aL[0];
	pH->aL[1] = pF->aL[1] + pG->aL[1];
	pH->aL[2] = pF->aL[2] + pG->aL[2];
	pH->aL[3] = pF->aL[3] + pG->aL[3];
	pH->aL[4] = pF->aL[4] + pG->aL[4];
}

static void
b73_fe_sub(b73_fe *pH, const b73_fe *pF, const b73_fe *pG)
{
	uint64_t u0, u1, u2, u3, u4;

	/* Add 2p so limbs stay non-negative before reduction. */
	u0 = (pF->aL[0] + 0xfffffffffffdaull) - pG->aL[0];
	u1 = (pF->aL[1] + 0xffffffffffffeull) - pG->aL[1];
	u2 = (pF->aL[2] + 0xffffffffffffeull) - pG->aL[2];
	u3 = (pF->aL[3] + 0xffffffffffffeull) - pG->aL[3];
	u4 = (pF->aL[4] + 0xffffffffffffeull) - pG->aL[4];

	u1 += u0 >> 51;
	u0 &= B73_MASK51;
	u2 += u1 >> 51;
	u1 &= B73_MASK51;
	u3 += u2 >> 51;
	u2 &= B73_MASK51;
	u4 += u3 >> 51;
	u3 &= B73_MASK51;
	u0 += 19ull * (u4 >> 51);
	u4 &= B73_MASK51;
	u1 += u0 >> 51;
	u0 &= B73_MASK51;

	pH->aL[0] = u0;
	pH->aL[1] = u1;
	pH->aL[2] = u2;
	pH->aL[3] = u3;
	pH->aL[4] = u4;
}

static void
b73_fe_neg(b73_fe *pH, const b73_fe *pF)
{
	b73_fe z;

	b73_fe_0(&z);
	b73_fe_sub(pH, &z, pF);
}

static void
b73_fe_mul(b73_fe *pH, const b73_fe *pF, const b73_fe *pG)
{
	uint64_t uF0 = pF->aL[0], uF1 = pF->aL[1], uF2 = pF->aL[2];
	uint64_t uF3 = pF->aL[3], uF4 = pF->aL[4];
	uint64_t uG0 = pG->aL[0], uG1 = pG->aL[1], uG2 = pG->aL[2];
	uint64_t uG3 = pG->aL[3], uG4 = pG->aL[4];
	uint64_t uF1_19 = uF1 * 19ull;
	uint64_t uF2_19 = uF2 * 19ull;
	uint64_t uF3_19 = uF3 * 19ull;
	uint64_t uF4_19 = uF4 * 19ull;
	__uint128_t uR0, uR1, uR2, uR3, uR4;
	uint64_t uT0, uT1, uT2, uT3, uT4;
	uint64_t uC;

	uR0 = (__uint128_t)uF0 * uG0 + (__uint128_t)uF1_19 * uG4 +
	      (__uint128_t)uF2_19 * uG3 + (__uint128_t)uF3_19 * uG2 +
	      (__uint128_t)uF4_19 * uG1;
	uR1 = (__uint128_t)uF0 * uG1 + (__uint128_t)uF1 * uG0 +
	      (__uint128_t)uF2_19 * uG4 + (__uint128_t)uF3_19 * uG3 +
	      (__uint128_t)uF4_19 * uG2;
	uR2 = (__uint128_t)uF0 * uG2 + (__uint128_t)uF1 * uG1 +
	      (__uint128_t)uF2 * uG0 + (__uint128_t)uF3_19 * uG4 +
	      (__uint128_t)uF4_19 * uG3;
	uR3 = (__uint128_t)uF0 * uG3 + (__uint128_t)uF1 * uG2 +
	      (__uint128_t)uF2 * uG1 + (__uint128_t)uF3 * uG0 +
	      (__uint128_t)uF4_19 * uG4;
	uR4 = (__uint128_t)uF0 * uG4 + (__uint128_t)uF1 * uG3 +
	      (__uint128_t)uF2 * uG2 + (__uint128_t)uF3 * uG1 +
	      (__uint128_t)uF4 * uG0;

	uT0 = (uint64_t)uR0 & B73_MASK51;
	uC = (uint64_t)(uR0 >> 51);
	uR1 += uC;
	uT1 = (uint64_t)uR1 & B73_MASK51;
	uC = (uint64_t)(uR1 >> 51);
	uR2 += uC;
	uT2 = (uint64_t)uR2 & B73_MASK51;
	uC = (uint64_t)(uR2 >> 51);
	uR3 += uC;
	uT3 = (uint64_t)uR3 & B73_MASK51;
	uC = (uint64_t)(uR3 >> 51);
	uR4 += uC;
	uT4 = (uint64_t)uR4 & B73_MASK51;
	uC = (uint64_t)(uR4 >> 51);
	uT0 += uC * 19ull;
	uT1 += uT0 >> 51;
	uT0 &= B73_MASK51;
	uT2 += uT1 >> 51;
	uT1 &= B73_MASK51;

	pH->aL[0] = uT0;
	pH->aL[1] = uT1;
	pH->aL[2] = uT2;
	pH->aL[3] = uT3;
	pH->aL[4] = uT4;
}

static void
b73_fe_sq(b73_fe *pH, const b73_fe *pF)
{
	b73_fe_mul(pH, pF, pF);
}

static void
b73_fe_sq2(b73_fe *pH, const b73_fe *pF)
{
	b73_fe_sq(pH, pF);
	b73_fe_add(pH, pH, pH);
}

/* Invert via Fermat: a^(p-2) = a^(2^255-21). */
static void
b73_fe_invert(b73_fe *pOut, const b73_fe *pZ)
{
	b73_fe t0, t1, t2, t3;
	int i;

	b73_fe_sq(&t0, pZ);
	b73_fe_sq(&t1, &t0);
	b73_fe_sq(&t1, &t1);
	b73_fe_mul(&t1, pZ, &t1);
	b73_fe_mul(&t0, &t0, &t1);
	b73_fe_sq(&t2, &t0);
	b73_fe_mul(&t1, &t1, &t2);
	b73_fe_sq(&t2, &t1);
	for (i = 1; i < 5; i++) {
		b73_fe_sq(&t2, &t2);
	}
	b73_fe_mul(&t1, &t2, &t1);
	b73_fe_sq(&t2, &t1);
	for (i = 1; i < 10; i++) {
		b73_fe_sq(&t2, &t2);
	}
	b73_fe_mul(&t2, &t2, &t1);
	b73_fe_sq(&t3, &t2);
	for (i = 1; i < 20; i++) {
		b73_fe_sq(&t3, &t3);
	}
	b73_fe_mul(&t2, &t3, &t2);
	b73_fe_sq(&t2, &t2);
	for (i = 1; i < 10; i++) {
		b73_fe_sq(&t2, &t2);
	}
	b73_fe_mul(&t1, &t2, &t1);
	b73_fe_sq(&t2, &t1);
	for (i = 1; i < 50; i++) {
		b73_fe_sq(&t2, &t2);
	}
	b73_fe_mul(&t2, &t2, &t1);
	b73_fe_sq(&t3, &t2);
	for (i = 1; i < 100; i++) {
		b73_fe_sq(&t3, &t3);
	}
	b73_fe_mul(&t2, &t3, &t2);
	b73_fe_sq(&t2, &t2);
	for (i = 1; i < 50; i++) {
		b73_fe_sq(&t2, &t2);
	}
	b73_fe_mul(&t1, &t2, &t1);
	b73_fe_sq(&t1, &t1);
	for (i = 1; i < 5; i++) {
		b73_fe_sq(&t1, &t1);
	}
	b73_fe_mul(pOut, &t1, &t0);
}

/* z^((p-5)/8) used for square-root recovery (RFC 8032 §5.1.3). */
static void
b73_fe_pow22523(b73_fe *pOut, const b73_fe *pZ)
{
	b73_fe t0, t1, t2;
	int i;

	b73_fe_sq(&t0, pZ);
	b73_fe_sq(&t1, &t0);
	b73_fe_sq(&t1, &t1);
	b73_fe_mul(&t1, pZ, &t1);
	b73_fe_mul(&t0, &t0, &t1);
	b73_fe_sq(&t0, &t0);
	b73_fe_mul(&t0, &t1, &t0);
	b73_fe_sq(&t1, &t0);
	for (i = 1; i < 5; i++) {
		b73_fe_sq(&t1, &t1);
	}
	b73_fe_mul(&t0, &t1, &t0);
	b73_fe_sq(&t1, &t0);
	for (i = 1; i < 10; i++) {
		b73_fe_sq(&t1, &t1);
	}
	b73_fe_mul(&t1, &t1, &t0);
	b73_fe_sq(&t2, &t1);
	for (i = 1; i < 20; i++) {
		b73_fe_sq(&t2, &t2);
	}
	b73_fe_mul(&t1, &t2, &t1);
	b73_fe_sq(&t1, &t1);
	for (i = 1; i < 10; i++) {
		b73_fe_sq(&t1, &t1);
	}
	b73_fe_mul(&t0, &t1, &t0);
	b73_fe_sq(&t1, &t0);
	for (i = 1; i < 50; i++) {
		b73_fe_sq(&t1, &t1);
	}
	b73_fe_mul(&t1, &t1, &t0);
	b73_fe_sq(&t2, &t1);
	for (i = 1; i < 100; i++) {
		b73_fe_sq(&t2, &t2);
	}
	b73_fe_mul(&t1, &t2, &t1);
	b73_fe_sq(&t1, &t1);
	for (i = 1; i < 50; i++) {
		b73_fe_sq(&t1, &t1);
	}
	b73_fe_mul(&t0, &t1, &t0);
	b73_fe_sq(&t0, &t0);
	b73_fe_sq(&t0, &t0);
	b73_fe_mul(pOut, &t0, pZ);
}

/* Low bit of canonical encoding (parity / "negative" in RFC 8032). */
static int
b73_fe_isnegative(const b73_fe *pF)
{
	unsigned char aS[32];

	b73_fe_tobytes(aS, pF);
	return (int)(aS[0] & 1u);
}

static int
b73_fe_isnonzero(const b73_fe *pF)
{
	unsigned char aS[32];
	unsigned uA = 0;
	unsigned i;

	b73_fe_tobytes(aS, pF);
	for (i = 0; i < 32u; i++) {
		uA |= aS[i];
	}
	return uA != 0u ? 1 : 0;
}

/* Curve constant d = -121665/121666 (51-bit limbs). */
static const b73_fe s_b73_d = {
    { 929955233495203ull, 466365720129213ull, 1662059464998953ull,
      2033849074728123ull, 1442794654840575ull }
};

/* 2*d */
static const b73_fe s_b73_d2 = {
    { 1859910466990425ull, 932731440258426ull, 1072319116312658ull,
      1815898335770999ull, 633789495995903ull }
};

/* sqrt(-1) mod p = 2^((p-1)/4) */
static const b73_fe s_b73_sqrtm1 = {
    { 1718705420411056ull, 234908883556509ull, 2233514472574048ull,
      2117202627021982ull, 765476049583133ull }
};

/* --------------------------------------------------------------------------
 * Group elements — extended twisted Edwards (X:Y:Z:T), XY = ZT
 * -------------------------------------------------------------------------- */

typedef struct {
	b73_fe X;
	b73_fe Y;
	b73_fe Z;
	b73_fe T;
} b73_ge;

/* Base point B (RFC 8032) affine X,Y as 51-bit limbs; Z=1, T=X*Y at load. */
static const b73_fe s_b73_base_x = {
    { 1738742601995546ull, 1146398526822698ull, 2070867633025821ull,
      562264141797630ull, 587772402128613ull }
};
static const b73_fe s_b73_base_y = {
    { 1801439850948184ull, 1351079888211148ull, 450359962737049ull,
      900719925474099ull, 1801439850948198ull }
};

/* Identity (neutral): (0:1:1:0) */
static void
b73_ge_0(b73_ge *pH)
{
	b73_fe_0(&pH->X);
	b73_fe_1(&pH->Y);
	b73_fe_1(&pH->Z);
	b73_fe_0(&pH->T);
}

static void
b73_ge_copy(b73_ge *pH, const b73_ge *pF)
{
	b73_fe_copy(&pH->X, &pF->X);
	b73_fe_copy(&pH->Y, &pF->Y);
	b73_fe_copy(&pH->Z, &pF->Z);
	b73_fe_copy(&pH->T, &pF->T);
}

/* Encode affine y with sign bit of x (RFC 8032). */
static void
b73_ge_tobytes(unsigned char aS[32], const b73_ge *pH)
{
	b73_fe uRecip, uX, uY;

	b73_fe_invert(&uRecip, &pH->Z);
	b73_fe_mul(&uX, &pH->X, &uRecip);
	b73_fe_mul(&uY, &pH->Y, &uRecip);
	b73_fe_tobytes(aS, &uY);
	aS[31] ^= (unsigned char)(b73_fe_isnegative(&uX) << 7);
}

/*
 * Point addition in extended coords (Hisil–Wong–Carter–Dawson):
 *   A=(Y1-X1)(Y2-X2), B=(Y1+X1)(Y2+X2), C=T1*2d*T2, D=Z1*2*Z2
 *   E=B-A, F=D-C, G=D+C, H=B+A
 *   X3=E*F, Y3=G*H, T3=E*H, Z3=F*G
 */
static void
b73_ge_add(b73_ge *pR, const b73_ge *pP, const b73_ge *pQ)
{
	b73_fe aA, aB, aC, aD, aE, aF, aG, aH, aT;

	b73_fe_sub(&aA, &pP->Y, &pP->X);
	b73_fe_sub(&aT, &pQ->Y, &pQ->X);
	b73_fe_mul(&aA, &aA, &aT);
	b73_fe_add(&aB, &pP->Y, &pP->X);
	b73_fe_add(&aT, &pQ->Y, &pQ->X);
	b73_fe_mul(&aB, &aB, &aT);
	b73_fe_mul(&aC, &pP->T, &s_b73_d2);
	b73_fe_mul(&aC, &aC, &pQ->T);
	b73_fe_mul(&aD, &pP->Z, &pQ->Z);
	b73_fe_add(&aD, &aD, &aD);
	b73_fe_sub(&aE, &aB, &aA);
	b73_fe_sub(&aF, &aD, &aC);
	b73_fe_add(&aG, &aD, &aC);
	b73_fe_add(&aH, &aB, &aA);
	b73_fe_mul(&pR->X, &aE, &aF);
	b73_fe_mul(&pR->Y, &aG, &aH);
	b73_fe_mul(&pR->T, &aE, &aH);
	b73_fe_mul(&pR->Z, &aF, &aG);
}

/*
 * Doubling (EFD dbl-2008-hwcd, a = -1):
 *   A=X^2 B=Y^2 C=2Z^2 D=-A E=(X+Y)^2-A-B G=D+B F=G-C H=D-B
 */
static void
b73_ge_dbl(b73_ge *pR, const b73_ge *pP)
{
	b73_fe aA, aB, aC, aD, aE, aF, aG, aH, aT;

	b73_fe_sq(&aA, &pP->X);
	b73_fe_sq(&aB, &pP->Y);
	b73_fe_sq2(&aC, &pP->Z);
	b73_fe_neg(&aD, &aA);
	b73_fe_add(&aT, &pP->X, &pP->Y);
	b73_fe_sq(&aE, &aT);
	b73_fe_sub(&aE, &aE, &aA);
	b73_fe_sub(&aE, &aE, &aB);
	b73_fe_add(&aG, &aD, &aB);
	b73_fe_sub(&aF, &aG, &aC);
	b73_fe_sub(&aH, &aD, &aB);
	b73_fe_mul(&pR->X, &aE, &aF);
	b73_fe_mul(&pR->Y, &aG, &aH);
	b73_fe_mul(&pR->T, &aE, &aH);
	b73_fe_mul(&pR->Z, &aF, &aG);
}

/* Constant-time conditional move of whole group element. */
static void
b73_ge_cmov(b73_ge *pF, const b73_ge *pG, unsigned uB)
{
	b73_fe_cmov(&pF->X, &pG->X, uB);
	b73_fe_cmov(&pF->Y, &pG->Y, uB);
	b73_fe_cmov(&pF->Z, &pG->Z, uB);
	b73_fe_cmov(&pF->T, &pG->T, uB);
}

/* Decompress point (RFC 8032 §5.1.3). Returns 0 ok, -1 fail. */
static int
b73_ge_frombytes(b73_ge *pH, const unsigned char aS[32])
{
	b73_fe u, v, v3, vxx, check, one;
	b73_fe uY, uX;
	unsigned char aTmp[32];
	unsigned char aChk[32];
	int nSign;

	b73_bcopy(aTmp, aS, 32);
	nSign = (int)((aTmp[31] >> 7) & 1u);
	aTmp[31] &= 0x7fu;
	b73_fe_frombytes(&uY, aTmp);

	/* Reject non-canonical y (not reduced mod p). */
	b73_fe_tobytes(aChk, &uY);
	if (b73_bcmp(aChk, aTmp, 32) != 0) {
		return -1;
	}

	/* u = y^2 - 1 ; v = d y^2 + 1 */
	b73_fe_1(&one);
	b73_fe_sq(&u, &uY);
	b73_fe_sub(&u, &u, &one);
	b73_fe_sq(&v, &uY);
	b73_fe_mul(&v, &v, &s_b73_d);
	b73_fe_add(&v, &v, &one);

	/* x = (u/v)^((p+3)/8) via u v^3 (u v^7)^((p-5)/8) */
	b73_fe_sq(&v3, &v);
	b73_fe_mul(&v3, &v3, &v);
	b73_fe_sq(&uX, &v3);
	b73_fe_mul(&uX, &uX, &v);
	b73_fe_mul(&uX, &uX, &u);
	b73_fe_pow22523(&uX, &uX);
	b73_fe_mul(&uX, &uX, &v3);
	b73_fe_mul(&uX, &uX, &u);

	b73_fe_sq(&vxx, &uX);
	b73_fe_mul(&vxx, &vxx, &v);
	b73_fe_sub(&check, &vxx, &u);
	if (b73_fe_isnonzero(&check)) {
		b73_fe_add(&check, &vxx, &u);
		if (b73_fe_isnonzero(&check)) {
			return -1;
		}
		b73_fe_mul(&uX, &uX, &s_b73_sqrtm1);
	}

	if (b73_fe_isnegative(&uX) != nSign) {
		b73_fe_neg(&uX, &uX);
	}

	/* x=0 with sign bit set is invalid */
	if (nSign != 0 && !b73_fe_isnonzero(&uX)) {
		return -1;
	}

	b73_fe_copy(&pH->X, &uX);
	b73_fe_copy(&pH->Y, &uY);
	b73_fe_1(&pH->Z);
	b73_fe_mul(&pH->T, &uX, &uY);
	return 0;
}

/* Load base point with correct T = X*Y. */
static void
b73_ge_base(b73_ge *pB)
{
	b73_fe_copy(&pB->X, &s_b73_base_x);
	b73_fe_copy(&pB->Y, &s_b73_base_y);
	b73_fe_1(&pB->Z);
	b73_fe_mul(&pB->T, &pB->X, &pB->Y);
}

/*
 * Variable-base scalar mult: Q = s * P.
 * Constant-time double-and-add over 256 bits (secret-safe).
 * s is 32-byte little-endian.
 */
static void
b73_ge_scalarmult(b73_ge *pQ, const unsigned char aS[32], const b73_ge *pP)
{
	b73_ge aR, aT, aP;
	int iBit;
	unsigned uBit;

	b73_ge_0(&aR);
	b73_ge_copy(&aP, pP);

	for (iBit = 255; iBit >= 0; iBit--) {
		b73_ge_dbl(&aR, &aR);
		uBit = (unsigned)(aS[iBit >> 3] >> (iBit & 7)) & 1u;
		b73_ge_add(&aT, &aR, &aP);
		b73_ge_cmov(&aR, &aT, uBit);
	}
	b73_ge_copy(pQ, &aR);
}

/* Fixed-base: Q = s * B */
static void
b73_ge_scalarmult_base(b73_ge *pQ, const unsigned char aS[32])
{
	b73_ge aB;

	b73_ge_base(&aB);
	b73_ge_scalarmult(pQ, aS, &aB);
}

/* --------------------------------------------------------------------------
 * Scalar arithmetic modulo group order L
 * L = 2^252 + 27742317777372353535851937790883648493
 * -------------------------------------------------------------------------- */

/* L as little-endian 32 bytes (RFC 8032). */
static const unsigned char s_b73_L[32] = {
    0xed, 0xd3, 0xf5, 0x5c, 0x1a, 0x63, 0x12, 0x58, 0xd6, 0x9c, 0xf7,
    0xa2, 0xde, 0xf9, 0xde, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10
};

/* Left-shift 64-byte LE integer by 1. */
static void
b73_mp_shl1(unsigned char *pA, size_t cb)
{
	unsigned uCarry = 0;
	size_t i;

	for (i = 0; i < cb; i++) {
		unsigned uV = (unsigned)pA[i] << 1;

		pA[i] = (unsigned char)((uV & 0xffu) | uCarry);
		uCarry = uV >> 8;
	}
}

/* Compare 64-byte LE aR with L (zero-extended). Returns 1 if aR >= L. */
static int
b73_mp_ge_l(const unsigned char aR[64])
{
	int i;

	for (i = 63; i >= 32; i--) {
		if (aR[i] != 0) {
			return 1;
		}
	}
	for (i = 31; i >= 0; i--) {
		if (aR[i] > s_b73_L[i]) {
			return 1;
		}
		if (aR[i] < s_b73_L[i]) {
			return 0;
		}
	}
	return 1;
}

/* aR (64-byte LE) -= L */
static void
b73_mp_sub_l(unsigned char aR[64])
{
	unsigned uBorrow = 0;
	int i;

	for (i = 0; i < 32; i++) {
		unsigned uV = (unsigned)aR[i] - (unsigned)s_b73_L[i] - uBorrow;

		aR[i] = (unsigned char)uV;
		uBorrow = (uV >> 8) & 1u;
	}
	for (; i < 64; i++) {
		unsigned uV = (unsigned)aR[i] - uBorrow;

		aR[i] = (unsigned char)uV;
		uBorrow = (uV >> 8) & 1u;
	}
}

/*
 * Reduce 512-bit little-endian integer mod L → 32-byte scalar.
 * Binary long-division remainder (correct; not constant-time — used
 * after hashing public transcript data or already-clamped secrets).
 */
static void
b73_sc_reduce(unsigned char aOut[32], const unsigned char aIn[64])
{
	unsigned char aR[64];
	int iBit;

	b73_bzero(aR, 64);
	for (iBit = 511; iBit >= 0; iBit--) {
		b73_mp_shl1(aR, 64);
		aR[0] = (unsigned char)(aR[0] |
		    ((aIn[iBit >> 3] >> (iBit & 7)) & 1u));
		if (b73_mp_ge_l(aR)) {
			b73_mp_sub_l(aR);
		}
	}
	b73_bcopy(aOut, aR, 32);
	b73_bzero(aR, 64);
}

/* Return 1 if 32-byte LE scalar s is in [0, L). */
static int
b73_sc_valid(const unsigned char aS[32])
{
	int i;

	for (i = 31; i >= 0; i--) {
		if (aS[i] > s_b73_L[i]) {
			return 0;
		}
		if (aS[i] < s_b73_L[i]) {
			return 1;
		}
	}
	return 0; /* s == L → invalid */
}

/* Schoolbook 256×256 → 512, then aC added, reduce mod L: out = a*b+c mod L */
static void
b73_sc_muladd(unsigned char aOut[32], const unsigned char aA[32],
              const unsigned char aB[32], const unsigned char aC[32])
{
	uint32_t aAl[8], aBl[8], aCl[8];
	uint32_t aProd[16];
	unsigned char aWide[64];
	unsigned i, j;
	uint64_t uCarry;

	for (i = 0; i < 8u; i++) {
		aAl[i] = (uint32_t)aA[4u * i] | ((uint32_t)aA[4u * i + 1u] << 8) |
		         ((uint32_t)aA[4u * i + 2u] << 16) |
		         ((uint32_t)aA[4u * i + 3u] << 24);
		aBl[i] = (uint32_t)aB[4u * i] | ((uint32_t)aB[4u * i + 1u] << 8) |
		         ((uint32_t)aB[4u * i + 2u] << 16) |
		         ((uint32_t)aB[4u * i + 3u] << 24);
		aCl[i] = (uint32_t)aC[4u * i] | ((uint32_t)aC[4u * i + 1u] << 8) |
		         ((uint32_t)aC[4u * i + 2u] << 16) |
		         ((uint32_t)aC[4u * i + 3u] << 24);
	}

	for (i = 0; i < 16u; i++) {
		aProd[i] = 0;
	}

	for (i = 0; i < 8u; i++) {
		uCarry = 0;
		for (j = 0; j < 8u; j++) {
			uint64_t uT = (uint64_t)aProd[i + j] +
			              (uint64_t)aAl[i] * (uint64_t)aBl[j] + uCarry;

			aProd[i + j] = (uint32_t)uT;
			uCarry = uT >> 32;
		}
		{
			uint64_t uT = (uint64_t)aProd[i + 8u] + uCarry;

			aProd[i + 8u] = (uint32_t)uT;
			/* 256×256 fits in 512 bits; high carry is always 0. */
		}
	}

	/* + c (256-bit) */
	uCarry = 0;
	for (i = 0; i < 8u; i++) {
		uint64_t uT = (uint64_t)aProd[i] + (uint64_t)aCl[i] + uCarry;

		aProd[i] = (uint32_t)uT;
		uCarry = uT >> 32;
	}
	for (; i < 16u && uCarry != 0; i++) {
		uint64_t uT = (uint64_t)aProd[i] + uCarry;

		aProd[i] = (uint32_t)uT;
		uCarry = uT >> 32;
	}

	for (i = 0; i < 16u; i++) {
		aWide[4u * i] = (unsigned char)aProd[i];
		aWide[4u * i + 1u] = (unsigned char)(aProd[i] >> 8);
		aWide[4u * i + 2u] = (unsigned char)(aProd[i] >> 16);
		aWide[4u * i + 3u] = (unsigned char)(aProd[i] >> 24);
	}
	b73_sc_reduce(aOut, aWide);
	b73_bzero(aWide, sizeof(aWide));
	b73_bzero(aProd, sizeof(aProd));
}

/* --------------------------------------------------------------------------
 * SHA-512 helper
 * -------------------------------------------------------------------------- */

static void
b73_sha512(unsigned char aOut[64], const unsigned char *pData, size_t cb,
           const unsigned char *pData2, size_t cb2,
           const unsigned char *pData3, size_t cb3)
{
	unsigned char aCtx[B73_SHA512_CTX];

	b73_bzero(aCtx, sizeof(aCtx));
	SHA512_Init(aCtx);
	if (pData != NULL && cb > 0u) {
		SHA512_Update(aCtx, pData, cb);
	}
	if (pData2 != NULL && cb2 > 0u) {
		SHA512_Update(aCtx, pData2, cb2);
	}
	if (pData3 != NULL && cb3 > 0u) {
		SHA512_Update(aCtx, pData3, cb3);
	}
	SHA512_Final(aOut, aCtx);
	b73_bzero(aCtx, sizeof(aCtx));
}

/* Clamp scalar half of SHA-512(sk) per RFC 8032 §5.1.5. */
static void
b73_clamp(unsigned char aA[32])
{
	aA[0] &= 248u;
	aA[31] &= 63u;
	aA[31] |= 64u;
}

/* Expand secret: a[32] clamped scalar, prefix[32] nonce key. */
static void
b73_expand_sk(unsigned char aA[32], unsigned char aPrefix[32],
              const unsigned char aSk[32])
{
	unsigned char aH[64];

	b73_sha512(aH, aSk, 32u, NULL, 0, NULL, 0);
	b73_bcopy(aA, aH, 32);
	b73_clamp(aA);
	b73_bcopy(aPrefix, aH + 32, 32);
	b73_bzero(aH, sizeof(aH));
}

/* --------------------------------------------------------------------------
 * Public API
 * -------------------------------------------------------------------------- */

void
ed25519_public_key(const unsigned char aSk[32], unsigned char aPk[32])
{
	unsigned char aA[32];
	unsigned char aPrefix[32];
	b73_ge aP;

	if (aSk == NULL || aPk == NULL) {
		return;
	}
	b73_expand_sk(aA, aPrefix, aSk);
	b73_ge_scalarmult_base(&aP, aA);
	b73_ge_tobytes(aPk, &aP);
	b73_bzero(aA, sizeof(aA));
	b73_bzero(aPrefix, sizeof(aPrefix));
	b73_bzero(&aP, sizeof(aP));
}

void
ed25519_sign(const unsigned char aSk[32], const unsigned char *pMsg,
             size_t cbMsg, unsigned char aSig[64])
{
	unsigned char aA[32];
	unsigned char aPrefix[32];
	unsigned char aPk[32];
	unsigned char aR[32];
	unsigned char aS[32];
	unsigned char aK[32];
	unsigned char aHr[64];
	unsigned char aHk[64];
	b73_ge aRP, aAP;

	if (aSk == NULL || aSig == NULL) {
		return;
	}
	if (pMsg == NULL && cbMsg != 0u) {
		return;
	}

	b73_expand_sk(aA, aPrefix, aSk);
	b73_ge_scalarmult_base(&aAP, aA);
	b73_ge_tobytes(aPk, &aAP);

	/* r = H(prefix || M) mod L */
	b73_sha512(aHr, aPrefix, 32u, pMsg, cbMsg, NULL, 0);
	b73_sc_reduce(aR, aHr);

	/* R = r·B */
	b73_ge_scalarmult_base(&aRP, aR);
	b73_ge_tobytes(aSig, &aRP);

	/* k = H(R || A || M) mod L */
	b73_sha512(aHk, aSig, 32u, aPk, 32u, pMsg, cbMsg);
	b73_sc_reduce(aK, aHk);

	/* S = (r + k·a) mod L */
	b73_sc_muladd(aS, aK, aA, aR);
	b73_bcopy(aSig + 32, aS, 32);

	b73_bzero(aA, sizeof(aA));
	b73_bzero(aPrefix, sizeof(aPrefix));
	b73_bzero(aR, sizeof(aR));
	b73_bzero(aS, sizeof(aS));
	b73_bzero(aK, sizeof(aK));
	b73_bzero(aHr, sizeof(aHr));
	b73_bzero(aHk, sizeof(aHk));
	b73_bzero(aPk, sizeof(aPk));
	b73_bzero(&aRP, sizeof(aRP));
	b73_bzero(&aAP, sizeof(aAP));
}

int
ed25519_verify(const unsigned char aPk[32], const unsigned char *pMsg,
               size_t cbMsg, const unsigned char aSig[64])
{
	unsigned char aRenc[32];
	unsigned char aS[32];
	unsigned char aK[32];
	unsigned char aHk[64];
	unsigned char aCheck[32];
	b73_ge aA, aR, aSB, aKA, aNeg, aSum;

	if (aPk == NULL || aSig == NULL) {
		return -1;
	}
	if (pMsg == NULL && cbMsg != 0u) {
		return -1;
	}

	b73_bcopy(aRenc, aSig, 32);
	b73_bcopy(aS, aSig + 32, 32);

	if (!b73_sc_valid(aS)) {
		return -1;
	}
	if (b73_ge_frombytes(&aA, aPk) != 0) {
		return -1;
	}
	if (b73_ge_frombytes(&aR, aRenc) != 0) {
		return -1;
	}
	/* R on curve was accepted; encoded form must be canonical. */
	b73_ge_tobytes(aCheck, &aR);
	if (b73_bcmp(aCheck, aRenc, 32) != 0) {
		return -1;
	}

	/* k = H(R || A || M) mod L */
	b73_sha512(aHk, aRenc, 32u, aPk, 32u, pMsg, cbMsg);
	b73_sc_reduce(aK, aHk);

	/* Check [S]B = R + [k]A  ⇔  encode([S]B − [k]A) == R */
	b73_ge_scalarmult_base(&aSB, aS);
	b73_ge_scalarmult(&aKA, aK, &aA);
	b73_ge_copy(&aNeg, &aKA);
	b73_fe_neg(&aNeg.X, &aNeg.X);
	b73_fe_neg(&aNeg.T, &aNeg.T);
	b73_ge_add(&aSum, &aSB, &aNeg);
	b73_ge_tobytes(aCheck, &aSum);

	if (b73_bcmp(aCheck, aRenc, 32) != 0) {
		b73_bzero(aK, sizeof(aK));
		b73_bzero(aHk, sizeof(aHk));
		b73_bzero(aS, sizeof(aS));
		return -1;
	}

	b73_bzero(aK, sizeof(aK));
	b73_bzero(aHk, sizeof(aHk));
	b73_bzero(aS, sizeof(aS));
	return 0;
}

void __ed25519_public_key(const unsigned char aSk[32], unsigned char aPk[32])
    __attribute__((alias("ed25519_public_key")));

void __ed25519_sign(const unsigned char aSk[32], const unsigned char *pMsg,
                    size_t cbMsg, unsigned char aSig[64])
    __attribute__((alias("ed25519_sign")));

int __ed25519_verify(const unsigned char aPk[32], const unsigned char *pMsg,
                     size_t cbMsg, const unsigned char aSig[64])
    __attribute__((alias("ed25519_verify")));
