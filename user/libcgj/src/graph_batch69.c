/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch69: Camellia block cipher (RFC 3713 / NTT).
 * Clean-room from the public RFC description; pure C, freestanding,
 * integer/pointer only (no SSE). Dual MIT OR Apache-2.0.
 *
 * Surface:
 *   Camellia_set_key / Camellia_encrypt / Camellia_decrypt
 *   __Camellia_set_key / __Camellia_encrypt / __Camellia_decrypt
 *   __libcgj_batch69_marker = "libcgj-batch69"
 *
 * Key schedule buffer k[] (uint32_t):
 *   Allocate at least CAMELLIA_TABLE_WORDS (69) words.
 *   k[0]     = grand rounds: 18 (128-bit key) or 24 (192/256-bit)
 *   k[1..]   = 64-bit subkeys as big-endian uint32 pairs (hi, lo),
 *              in encrypt order:
 *                kw1, kw2,
 *                k1..k6, ke1, ke2,
 *                k7..k12, ke3, ke4,
 *                [128: k13..k18, kw3, kw4]                    → 52 key words
 *                [192/256: k13..k18, ke5, ke6, k19..k24, kw3, kw4]
 *                                                              → 68 key words
 *   Total words used: 53 (128-bit) or 69 (192/256-bit).
 *
 * Camellia_set_key returns 0 on success, -1 on bad args / unsupported bits.
 * Supported key sizes: 128, 192, 256 bits (primary: Camellia-128).
 */
#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch69_marker[] = "libcgj-batch69";

/* Caller-facing schedule size (uint32_t words). */
#define CAMELLIA_TABLE_WORDS 69

/* Sigma constants (RFC 3713 §2.2), each as (hi32, lo32). */
#define B69_SIG1_H 0xA09E667Fu
#define B69_SIG1_L 0x3BCC908Bu
#define B69_SIG2_H 0xB67AE858u
#define B69_SIG2_L 0x4CAA73B2u
#define B69_SIG3_H 0xC6EF372Fu
#define B69_SIG3_L 0xE94F82BEu
#define B69_SIG4_H 0x54FF53A5u
#define B69_SIG4_L 0xF1D36F1Cu
#define B69_SIG5_H 0x10E527FAu
#define B69_SIG5_L 0xDE682D1Du
#define B69_SIG6_H 0xB05688C2u
#define B69_SIG6_L 0xB3E6C1FDu

/* SBOX1 from RFC 3713 §2.4.1 (decimal table). */
static const unsigned char s_b69_sbox1[256] = {
    112, 130, 44,  236, 179, 39,  192, 229, 228, 133, 87,  53,  234, 12,  174,
    65,  35,  239, 107, 147, 69,  25,  165, 33,  237, 14,  79,  78,  29,  101,
    146, 189, 134, 184, 175, 143, 124, 235, 31,  206, 62,  48,  220, 95,  94,
    197, 11,  26,  166, 225, 57,  202, 213, 71,  93,  61,  217, 1,   90,  214,
    81,  86,  108, 77,  139, 13,  154, 102, 251, 204, 176, 45,  116, 18,  43,
    32,  240, 177, 132, 153, 223, 76,  203, 194, 52,  126, 118, 5,   109, 183,
    169, 49,  209, 23,  4,   215, 20,  88,  58,  97,  222, 27,  17,  28,  50,
    15,  156, 22,  83,  24,  242, 34,  254, 68,  207, 178, 195, 181, 122, 145,
    36,  8,   232, 168, 96,  252, 105, 80,  170, 208, 160, 125, 161, 137, 98,
    151, 84,  91,  30,  149, 224, 255, 100, 210, 16,  196, 0,   72,  163, 247,
    117, 219, 138, 3,   230, 218, 9,   63,  221, 148, 135, 92,  131, 2,   205,
    74,  144, 51,  115, 103, 246, 243, 157, 127, 191, 226, 82,  155, 216, 38,
    200, 55,  198, 59,  129, 150, 111, 75,  19,  190, 99,  46,  233, 121, 167,
    140, 159, 110, 188, 142, 41,  245, 249, 182, 47,  253, 180, 89,  120, 152,
    6,   106, 231, 70,  113, 186, 212, 37,  171, 66,  136, 162, 141, 250, 114,
    7,   185, 85,  248, 238, 172, 10,  54,  73,  42,  104, 60,  56,  241, 164,
    64,  40,  211, 123, 187, 201, 67,  193, 21,  227, 173, 244, 119, 199, 128,
    158};

/* ---- helpers ----------------------------------------------------------- */

static uint32_t
b69_rol32(uint32_t u32X, unsigned uN)
{
	return (u32X << uN) | (u32X >> (32u - uN));
}

static unsigned char
b69_sbox1(unsigned char u8X)
{
	return s_b69_sbox1[u8X];
}

/* SBOX2[x] = SBOX1[x] <<< 1 */
static unsigned char
b69_sbox2(unsigned char u8X)
{
	unsigned char u8S = s_b69_sbox1[u8X];

	return (unsigned char)((u8S << 1) | (u8S >> 7));
}

/* SBOX3[x] = SBOX1[x] <<< 7 */
static unsigned char
b69_sbox3(unsigned char u8X)
{
	unsigned char u8S = s_b69_sbox1[u8X];

	return (unsigned char)((u8S << 7) | (u8S >> 1));
}

/* SBOX4[x] = SBOX1[x <<< 1] */
static unsigned char
b69_sbox4(unsigned char u8X)
{
	unsigned char u8R = (unsigned char)((u8X << 1) | (u8X >> 7));

	return s_b69_sbox1[u8R];
}

static uint64_t
b69_load_be64(const unsigned char *p)
{
	return ((uint64_t)p[0] << 56) | ((uint64_t)p[1] << 48) |
	       ((uint64_t)p[2] << 40) | ((uint64_t)p[3] << 32) |
	       ((uint64_t)p[4] << 24) | ((uint64_t)p[5] << 16) |
	       ((uint64_t)p[6] << 8) | (uint64_t)p[7];
}

static void
b69_store_be64(unsigned char *p, uint64_t u64X)
{
	p[0] = (unsigned char)(u64X >> 56);
	p[1] = (unsigned char)(u64X >> 48);
	p[2] = (unsigned char)(u64X >> 40);
	p[3] = (unsigned char)(u64X >> 32);
	p[4] = (unsigned char)(u64X >> 24);
	p[5] = (unsigned char)(u64X >> 16);
	p[6] = (unsigned char)(u64X >> 8);
	p[7] = (unsigned char)u64X;
}

/* Pack / unpack 64-bit subkey as two big-endian uint32 words. */
static void
b69_put_u64(uint32_t *pK, unsigned uIdx, uint64_t u64V)
{
	pK[uIdx] = (uint32_t)(u64V >> 32);
	pK[uIdx + 1u] = (uint32_t)u64V;
}

static uint64_t
b69_get_u64(const uint32_t *pK, unsigned uIdx)
{
	return ((uint64_t)pK[uIdx] << 32) | (uint64_t)pK[uIdx + 1u];
}

/* 128-bit left rotate of (hi||lo) by uN bits (0..127). */
static void
b69_rotl128(uint64_t *pHi, uint64_t *pLo, unsigned uN)
{
	uint64_t u64Hi = *pHi;
	uint64_t u64Lo = *pLo;
	uint64_t u64NHi, u64NLo;

	uN &= 127u;
	if (uN == 0u) {
		return;
	}
	if (uN >= 64u) {
		uint64_t u64T = u64Hi;

		u64Hi = u64Lo;
		u64Lo = u64T;
		uN -= 64u;
		if (uN == 0u) {
			*pHi = u64Hi;
			*pLo = u64Lo;
			return;
		}
	}
	u64NHi = (u64Hi << uN) | (u64Lo >> (64u - uN));
	u64NLo = (u64Lo << uN) | (u64Hi >> (64u - uN));
	*pHi = u64NHi;
	*pLo = u64NLo;
}

/* ---- F / FL / FLINV (RFC 3713 §2.4) ------------------------------------ */

static uint64_t
b69_F(uint64_t u64In, uint64_t u64Ke)
{
	uint64_t u64X = u64In ^ u64Ke;
	unsigned char u8T1, u8T2, u8T3, u8T4, u8T5, u8T6, u8T7, u8T8;
	unsigned char u8Y1, u8Y2, u8Y3, u8Y4, u8Y5, u8Y6, u8Y7, u8Y8;

	u8T1 = b69_sbox1((unsigned char)(u64X >> 56));
	u8T2 = b69_sbox2((unsigned char)(u64X >> 48));
	u8T3 = b69_sbox3((unsigned char)(u64X >> 40));
	u8T4 = b69_sbox4((unsigned char)(u64X >> 32));
	u8T5 = b69_sbox2((unsigned char)(u64X >> 24));
	u8T6 = b69_sbox3((unsigned char)(u64X >> 16));
	u8T7 = b69_sbox4((unsigned char)(u64X >> 8));
	u8T8 = b69_sbox1((unsigned char)u64X);

	u8Y1 = (unsigned char)(u8T1 ^ u8T3 ^ u8T4 ^ u8T6 ^ u8T7 ^ u8T8);
	u8Y2 = (unsigned char)(u8T1 ^ u8T2 ^ u8T4 ^ u8T5 ^ u8T7 ^ u8T8);
	u8Y3 = (unsigned char)(u8T1 ^ u8T2 ^ u8T3 ^ u8T5 ^ u8T6 ^ u8T8);
	u8Y4 = (unsigned char)(u8T2 ^ u8T3 ^ u8T4 ^ u8T5 ^ u8T6 ^ u8T7);
	u8Y5 = (unsigned char)(u8T1 ^ u8T2 ^ u8T6 ^ u8T7 ^ u8T8);
	u8Y6 = (unsigned char)(u8T2 ^ u8T3 ^ u8T5 ^ u8T7 ^ u8T8);
	u8Y7 = (unsigned char)(u8T3 ^ u8T4 ^ u8T5 ^ u8T6 ^ u8T8);
	u8Y8 = (unsigned char)(u8T1 ^ u8T4 ^ u8T5 ^ u8T6 ^ u8T7);

	return ((uint64_t)u8Y1 << 56) | ((uint64_t)u8Y2 << 48) |
	       ((uint64_t)u8Y3 << 40) | ((uint64_t)u8Y4 << 32) |
	       ((uint64_t)u8Y5 << 24) | ((uint64_t)u8Y6 << 16) |
	       ((uint64_t)u8Y7 << 8) | (uint64_t)u8Y8;
}

static uint64_t
b69_FL(uint64_t u64In, uint64_t u64Ke)
{
	uint32_t u32X1 = (uint32_t)(u64In >> 32);
	uint32_t u32X2 = (uint32_t)u64In;
	uint32_t u32K1 = (uint32_t)(u64Ke >> 32);
	uint32_t u32K2 = (uint32_t)u64Ke;

	u32X2 ^= b69_rol32(u32X1 & u32K1, 1);
	u32X1 ^= (u32X2 | u32K2);
	return ((uint64_t)u32X1 << 32) | (uint64_t)u32X2;
}

static uint64_t
b69_FLINV(uint64_t u64In, uint64_t u64Ke)
{
	uint32_t u32Y1 = (uint32_t)(u64In >> 32);
	uint32_t u32Y2 = (uint32_t)u64In;
	uint32_t u32K1 = (uint32_t)(u64Ke >> 32);
	uint32_t u32K2 = (uint32_t)u64Ke;

	u32Y1 ^= (u32Y2 | u32K2);
	u32Y2 ^= b69_rol32(u32Y1 & u32K1, 1);
	return ((uint64_t)u32Y1 << 32) | (uint64_t)u32Y2;
}

/* Half helpers on a 128-bit (hi||lo) value after rotate. */
static uint64_t
b69_half_hi(uint64_t u64A, uint64_t u64B, unsigned uRot)
{
	uint64_t u64Hi = u64A;
	uint64_t u64Lo = u64B;

	b69_rotl128(&u64Hi, &u64Lo, uRot);
	return u64Hi;
}

static uint64_t
b69_half_lo(uint64_t u64A, uint64_t u64B, unsigned uRot)
{
	uint64_t u64Hi = u64A;
	uint64_t u64Lo = u64B;

	b69_rotl128(&u64Hi, &u64Lo, uRot);
	return u64Lo;
}

/* ---- key schedule ------------------------------------------------------ */

/*
 * Emit subkeys into k[1..] in encrypt order; set k[0] = grand rounds.
 * Returns number of 64-bit subkeys written (26 or 34).
 */
static unsigned
b69_expand(const unsigned char *pKey, int nBits, uint32_t *pK)
{
	uint64_t u64KlHi, u64KlLo, u64KrHi, u64KrLo;
	uint64_t u64KaHi, u64KaLo, u64KbHi, u64KbLo;
	uint64_t u64D1, u64D2;
	unsigned uOff;
	int fLong;

	/* KL / KR from secret key (RFC 3713 §2.2). */
	u64KlHi = b69_load_be64(pKey);
	u64KlLo = b69_load_be64(pKey + 8);
	if (nBits == 128) {
		u64KrHi = 0;
		u64KrLo = 0;
		fLong = 0;
	} else if (nBits == 192) {
		/* KR = rightmost 64 || ~rightmost 64 */
		u64KrHi = b69_load_be64(pKey + 16);
		u64KrLo = ~u64KrHi;
		fLong = 1;
	} else { /* 256 */
		u64KrHi = b69_load_be64(pKey + 16);
		u64KrLo = b69_load_be64(pKey + 24);
		fLong = 1;
	}

	/* KA, KB via F and Sigma. */
	u64D1 = u64KlHi ^ u64KrHi;
	u64D2 = u64KlLo ^ u64KrLo;
	u64D2 ^= b69_F(u64D1, ((uint64_t)B69_SIG1_H << 32) | B69_SIG1_L);
	u64D1 ^= b69_F(u64D2, ((uint64_t)B69_SIG2_H << 32) | B69_SIG2_L);
	u64D1 ^= u64KlHi;
	u64D2 ^= u64KlLo;
	u64D2 ^= b69_F(u64D1, ((uint64_t)B69_SIG3_H << 32) | B69_SIG3_L);
	u64D1 ^= b69_F(u64D2, ((uint64_t)B69_SIG4_H << 32) | B69_SIG4_L);
	u64KaHi = u64D1;
	u64KaLo = u64D2;

	u64D1 = u64KaHi ^ u64KrHi;
	u64D2 = u64KaLo ^ u64KrLo;
	u64D2 ^= b69_F(u64D1, ((uint64_t)B69_SIG5_H << 32) | B69_SIG5_L);
	u64D1 ^= b69_F(u64D2, ((uint64_t)B69_SIG6_H << 32) | B69_SIG6_L);
	u64KbHi = u64D1;
	u64KbLo = u64D2;

	uOff = 1u; /* first free word after rounds slot */

	if (!fLong) {
		/* 128-bit key subkeys (RFC 3713 §2.2). */
		pK[0] = 18u;
		/* kw1, kw2 */
		b69_put_u64(pK, uOff, b69_half_hi(u64KlHi, u64KlLo, 0));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KlHi, u64KlLo, 0));
		uOff += 2u;
		/* k1, k2 */
		b69_put_u64(pK, uOff, b69_half_hi(u64KaHi, u64KaLo, 0));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KaHi, u64KaLo, 0));
		uOff += 2u;
		/* k3, k4 */
		b69_put_u64(pK, uOff, b69_half_hi(u64KlHi, u64KlLo, 15));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KlHi, u64KlLo, 15));
		uOff += 2u;
		/* k5, k6 */
		b69_put_u64(pK, uOff, b69_half_hi(u64KaHi, u64KaLo, 15));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KaHi, u64KaLo, 15));
		uOff += 2u;
		/* ke1, ke2 */
		b69_put_u64(pK, uOff, b69_half_hi(u64KaHi, u64KaLo, 30));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KaHi, u64KaLo, 30));
		uOff += 2u;
		/* k7, k8 */
		b69_put_u64(pK, uOff, b69_half_hi(u64KlHi, u64KlLo, 45));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KlHi, u64KlLo, 45));
		uOff += 2u;
		/* k9, k10  — note k10 from KL<<<60 lower (RFC asymmetry) */
		b69_put_u64(pK, uOff, b69_half_hi(u64KaHi, u64KaLo, 45));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KlHi, u64KlLo, 60));
		uOff += 2u;
		/* k11, k12 */
		b69_put_u64(pK, uOff, b69_half_hi(u64KaHi, u64KaLo, 60));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KaHi, u64KaLo, 60));
		uOff += 2u;
		/* ke3, ke4 */
		b69_put_u64(pK, uOff, b69_half_hi(u64KlHi, u64KlLo, 77));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KlHi, u64KlLo, 77));
		uOff += 2u;
		/* k13, k14 */
		b69_put_u64(pK, uOff, b69_half_hi(u64KlHi, u64KlLo, 94));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KlHi, u64KlLo, 94));
		uOff += 2u;
		/* k15, k16 */
		b69_put_u64(pK, uOff, b69_half_hi(u64KaHi, u64KaLo, 94));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KaHi, u64KaLo, 94));
		uOff += 2u;
		/* k17, k18 */
		b69_put_u64(pK, uOff, b69_half_hi(u64KlHi, u64KlLo, 111));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KlHi, u64KlLo, 111));
		uOff += 2u;
		/* kw3, kw4 */
		b69_put_u64(pK, uOff, b69_half_hi(u64KaHi, u64KaLo, 111));
		uOff += 2u;
		b69_put_u64(pK, uOff, b69_half_lo(u64KaHi, u64KaLo, 111));
		uOff += 2u;
		return 26u;
	}

	/* 192- and 256-bit key subkeys. */
	pK[0] = 24u;
	/* kw1, kw2 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KlHi, u64KlLo, 0));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KlHi, u64KlLo, 0));
	uOff += 2u;
	/* k1, k2 from KB */
	b69_put_u64(pK, uOff, b69_half_hi(u64KbHi, u64KbLo, 0));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KbHi, u64KbLo, 0));
	uOff += 2u;
	/* k3, k4 from KR<<<15 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KrHi, u64KrLo, 15));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KrHi, u64KrLo, 15));
	uOff += 2u;
	/* k5, k6 from KA<<<15 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KaHi, u64KaLo, 15));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KaHi, u64KaLo, 15));
	uOff += 2u;
	/* ke1, ke2 from KR<<<30 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KrHi, u64KrLo, 30));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KrHi, u64KrLo, 30));
	uOff += 2u;
	/* k7, k8 from KB<<<30 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KbHi, u64KbLo, 30));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KbHi, u64KbLo, 30));
	uOff += 2u;
	/* k9, k10 from KL<<<45 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KlHi, u64KlLo, 45));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KlHi, u64KlLo, 45));
	uOff += 2u;
	/* k11, k12 from KA<<<45 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KaHi, u64KaLo, 45));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KaHi, u64KaLo, 45));
	uOff += 2u;
	/* ke3, ke4 from KL<<<60 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KlHi, u64KlLo, 60));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KlHi, u64KlLo, 60));
	uOff += 2u;
	/* k13, k14 from KR<<<60 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KrHi, u64KrLo, 60));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KrHi, u64KrLo, 60));
	uOff += 2u;
	/* k15, k16 from KB<<<60 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KbHi, u64KbLo, 60));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KbHi, u64KbLo, 60));
	uOff += 2u;
	/* k17, k18 from KL<<<77 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KlHi, u64KlLo, 77));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KlHi, u64KlLo, 77));
	uOff += 2u;
	/* ke5, ke6 from KA<<<77 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KaHi, u64KaLo, 77));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KaHi, u64KaLo, 77));
	uOff += 2u;
	/* k19, k20 from KR<<<94 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KrHi, u64KrLo, 94));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KrHi, u64KrLo, 94));
	uOff += 2u;
	/* k21, k22 from KA<<<94 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KaHi, u64KaLo, 94));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KaHi, u64KaLo, 94));
	uOff += 2u;
	/* k23, k24 from KL<<<111 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KlHi, u64KlLo, 111));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KlHi, u64KlLo, 111));
	uOff += 2u;
	/* kw3, kw4 from KB<<<111 */
	b69_put_u64(pK, uOff, b69_half_hi(u64KbHi, u64KbLo, 111));
	uOff += 2u;
	b69_put_u64(pK, uOff, b69_half_lo(u64KbHi, u64KbLo, 111));
	uOff += 2u;
	return 34u;
}

/* ---- public API -------------------------------------------------------- */

int
Camellia_set_key(const unsigned char *pKey, int nBits, uint32_t *pK)
{
	if (pKey == NULL || pK == NULL) {
		return -1;
	}
	if (nBits != 128 && nBits != 192 && nBits != 256) {
		return -1;
	}
	(void)b69_expand(pKey, nBits, pK);
	return 0;
}

/*
 * Encrypt one 16-byte block.
 * Schedule layout (word index into pK, after rounds at [0]):
 *   pairs at base 1: kw1,kw2, k1..k6, ke1,ke2, k7..k12, ke3,ke4, ...
 * 64-bit subkey index i lives at words 1+2*i .. 2+2*i.
 */
void
Camellia_encrypt(const unsigned char aIn[16], unsigned char aOut[16],
                 const uint32_t *pK)
{
	uint64_t u64D1, u64D2;
	unsigned uRounds;
	unsigned u; /* 64-bit subkey index into schedule after kw1,kw2 */

	if (aIn == NULL || aOut == NULL || pK == NULL) {
		return;
	}

	uRounds = (unsigned)pK[0];
	u64D1 = b69_load_be64(aIn);
	u64D2 = b69_load_be64(aIn + 8);

	/* Prewhitening: kw1, kw2 at subkey indices 0,1 → words 1,3 */
	u64D1 ^= b69_get_u64(pK, 1u);
	u64D2 ^= b69_get_u64(pK, 3u);

	/* Subkey index 2 is first round key k1 (word 5). */
	u = 2u;

	/* Six rounds */
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u++;

	/* FL / FLINV with ke1, ke2 */
	u64D1 = b69_FL(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D2 = b69_FLINV(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u++;

	/* Six rounds */
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u++;

	/* FL / FLINV with ke3, ke4 */
	u64D1 = b69_FL(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D2 = b69_FLINV(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u++;

	/* Six rounds */
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u++;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u++;

	if (uRounds == 24u) {
		/* FL / FLINV with ke5, ke6 */
		u64D1 = b69_FL(u64D1, b69_get_u64(pK, 1u + 2u * u));
		u++;
		u64D2 = b69_FLINV(u64D2, b69_get_u64(pK, 1u + 2u * u));
		u++;

		/* Six more rounds */
		u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
		u++;
		u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
		u++;
		u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
		u++;
		u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
		u++;
		u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
		u++;
		u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
		u++;
	}

	/* Postwhitening: kw3 into D2, kw4 into D1; C = D2 || D1 */
	u64D2 ^= b69_get_u64(pK, 1u + 2u * u);
	u++;
	u64D1 ^= b69_get_u64(pK, 1u + 2u * u);

	b69_store_be64(aOut, u64D2);
	b69_store_be64(aOut + 8, u64D1);
}

/*
 * Decrypt: reverse subkey order (RFC 3713 §2.3.3).
 * For 128-bit: kw1↔kw3, kw2↔kw4, k1↔k18 … k9↔k10, ke1↔ke4, ke2↔ke3.
 * Implemented by walking the encrypt schedule backwards with FL/FLINV swapped.
 */
void
Camellia_decrypt(const unsigned char aIn[16], unsigned char aOut[16],
                 const uint32_t *pK)
{
	uint64_t u64D1, u64D2;
	unsigned uRounds;
	unsigned uLast; /* index of kw3 in 64-bit subkey space */
	unsigned u;

	if (aIn == NULL || aOut == NULL || pK == NULL) {
		return;
	}

	uRounds = (unsigned)pK[0];
	/* Number of 64-bit subkeys: 26 (128) or 34 (192/256). */
	uLast = (uRounds == 18u) ? 25u : 33u;

	/* Ciphertext is D2||D1 from encrypt → load as u64D2, u64D1 then undo. */
	u64D2 = b69_load_be64(aIn);
	u64D1 = b69_load_be64(aIn + 8);

	/* Undo postwhitening: kw4 was applied to D1, kw3 to D2. */
	u64D1 ^= b69_get_u64(pK, 1u + 2u * uLast);
	u64D2 ^= b69_get_u64(pK, 1u + 2u * (uLast - 1u));

	/* Reverse Feistel: start from last round key (k18 or k24). */
	u = uLast - 2u;

	/* Six reverse rounds (even then odd pattern inverted). */
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u--;

	if (uRounds == 24u) {
		/* Inverse FL layer: FLINV with ke6, FL with ke5 (swapped roles). */
		/* After the six reverse rounds above, u points at ke6 (was ke5,ke6). */
		/* Schedule … k18, ke5, ke6, k19… so after consuming k19..k24,
		 * u is at ke6 index. Apply FLINV(D2, ke5) and FL(D1, ke6)?
		 * Encrypt: D1=FL(D1,ke5); D2=FLINV(D2,ke6)
		 * Inverse: D2=FL(D2,ke6); D1=FLINV(D1,ke5)
		 */
		u64D2 = b69_FL(u64D2, b69_get_u64(pK, 1u + 2u * u));
		u--;
		u64D1 = b69_FLINV(u64D1, b69_get_u64(pK, 1u + 2u * u));
		u--;

		u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
		u--;
		u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
		u--;
		u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
		u--;
		u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
		u--;
		u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
		u--;
		u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
		u--;
	}

	/* Inverse FL: encrypt did D1=FL(D1,ke3); D2=FLINV(D2,ke4)
	 * so inverse: D2=FL(D2,ke4); D1=FLINV(D1,ke3)
	 */
	u64D2 = b69_FL(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D1 = b69_FLINV(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u--;

	/* Six reverse rounds */
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u--;

	/* Inverse FL: encrypt D1=FL(D1,ke1); D2=FLINV(D2,ke2) */
	u64D2 = b69_FL(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D1 = b69_FLINV(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u--;

	/* Six reverse rounds */
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D1 ^= b69_F(u64D2, b69_get_u64(pK, 1u + 2u * u));
	u--;
	u64D2 ^= b69_F(u64D1, b69_get_u64(pK, 1u + 2u * u));
	/* u now at k1 index (2); remaining are kw2, kw1 */

	/* Undo prewhitening */
	u64D2 ^= b69_get_u64(pK, 3u); /* kw2 */
	u64D1 ^= b69_get_u64(pK, 1u); /* kw1 */

	b69_store_be64(aOut, u64D1);
	b69_store_be64(aOut + 8, u64D2);
}

int __Camellia_set_key(const unsigned char *pKey, int nBits, uint32_t *pK)
    __attribute__((alias("Camellia_set_key")));
void __Camellia_encrypt(const unsigned char aIn[16], unsigned char aOut[16],
                        const uint32_t *pK)
    __attribute__((alias("Camellia_encrypt")));
void __Camellia_decrypt(const unsigned char aIn[16], unsigned char aOut[16],
                        const uint32_t *pK)
    __attribute__((alias("Camellia_decrypt")));
