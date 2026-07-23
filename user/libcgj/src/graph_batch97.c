/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch97: HighwayHash64 (Google HighwayHash public algo).
 * Clean-room freestanding pure C — integer/pointer only (no libc string).
 * Compiles with -msse2; does not require SIMD intrinsics.
 *
 * Already present elsewhere (NOT redefined — avoid multi-def):
 *   siphash24 / siphash24_buf / __siphash24     → graph_batch36.c
 *   siphash24_with_key / siphash13_with_key     → graph_batch75.c
 *   halfsiphash24                               → graph_batch75.c
 *   cityhash64 / cityhash128                    → graph_batch61.c
 *
 * This TU adds only:
 *   highwayhash64 / __highwayhash64
 *     uint64_t (const unsigned char key[32], const void *data, size_t len)
 *   __libcgj_batch97_marker = "libcgj-batch97"
 *
 * Algorithm (public): HighwayHash — Alakuijala / Cox / Wassenberg.
 * 256-bit key, 32-byte packets, ZipperMerge mixing, 64-bit finalize.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch97_marker[] = "libcgj-batch97";

/* HighwayHash init constants (public algorithm). */
#define B97_M0_0 0xdbe6d5d5fe4cce2full
#define B97_M0_1 0xa4093822299f31d0ull
#define B97_M0_2 0x13198a2e03707344ull
#define B97_M0_3 0x243f6a8885a308d3ull
#define B97_M1_0 0x3bd39e10cb0ef593ull
#define B97_M1_1 0xc0acf169b5f18a8cull
#define B97_M1_2 0xbe5466cf34e90c6cull
#define B97_M1_3 0x452821e638d01377ull

typedef struct {
	uint64_t v0[4];
	uint64_t v1[4];
	uint64_t mul0[4];
	uint64_t mul1[4];
} b97_hh_state;

/* ---- freestanding load helpers ----------------------------------------- */

static uint64_t
b97_load_le64(const unsigned char *p)
{
	return (uint64_t)p[0] | ((uint64_t)p[1] << 8) |
	       ((uint64_t)p[2] << 16) | ((uint64_t)p[3] << 24) |
	       ((uint64_t)p[4] << 32) | ((uint64_t)p[5] << 40) |
	       ((uint64_t)p[6] << 48) | ((uint64_t)p[7] << 56);
}

static uint64_t
b97_load_le64_n(const unsigned char *p, size_t cb)
{
	uint64_t u = 0;
	size_t i;

	for (i = 0; i < cb && i < 8u; i++) {
		u |= (uint64_t)p[i] << (8u * i);
	}
	return u;
}

static uint64_t
b97_rot64_32(uint64_t x)
{
	return (x >> 32) | (x << 32);
}

/* --------------------------------------------------------------------------
 * ZipperMerge: 16-byte permutation of (v1 || v0), added into *add1 / *add0.
 * Public HighwayHash mixing step.
 * -------------------------------------------------------------------------- */

static void
b97_zipper_merge_add(uint64_t v1, uint64_t v0, uint64_t *pAdd1,
		     uint64_t *pAdd0)
{
	*pAdd0 += (((v0 & 0xff000000ull) | (v1 & 0xff00000000ull)) >> 24) |
		  (((v0 & 0xff0000000000ull) | (v1 & 0xff000000000000ull)) >>
		   16) |
		  (v0 & 0xff0000ull) | ((v0 & 0xff00ull) << 32) |
		  ((v1 & 0xff00000000000000ull) >> 8) | (v0 << 56);
	*pAdd1 += (((v1 & 0xff000000ull) | (v0 & 0xff00000000ull)) >> 24) |
		  (v1 & 0xff0000ull) | ((v1 & 0xff0000000000ull) >> 16) |
		  ((v1 & 0xff00ull) << 24) | ((v0 & 0xff000000000000ull) >> 8) |
		  ((v1 & 0xffull) << 48) | (v0 & 0xff00000000000000ull);
}

/* Mix one 32-byte packet (4 little-endian lanes). */

static void
b97_update(b97_hh_state *pSt, const uint64_t aLanes[4])
{
	int i;

	for (i = 0; i < 4; i++) {
		pSt->v1[i] += pSt->mul0[i] + aLanes[i];
		pSt->mul0[i] ^=
		    (pSt->v1[i] & 0xffffffffull) * (pSt->v0[i] >> 32);
		pSt->v0[i] += pSt->mul1[i];
		pSt->mul1[i] ^=
		    (pSt->v0[i] & 0xffffffffull) * (pSt->v1[i] >> 32);
	}
	b97_zipper_merge_add(pSt->v1[1], pSt->v1[0], &pSt->v0[1], &pSt->v0[0]);
	b97_zipper_merge_add(pSt->v1[3], pSt->v1[2], &pSt->v0[3], &pSt->v0[2]);
	b97_zipper_merge_add(pSt->v0[1], pSt->v0[0], &pSt->v1[1], &pSt->v1[0]);
	b97_zipper_merge_add(pSt->v0[3], pSt->v0[2], &pSt->v1[3], &pSt->v1[2]);
}

static void
b97_update_packet(b97_hh_state *pSt, const unsigned char *pPkt)
{
	uint64_t aLanes[4];

	aLanes[0] = b97_load_le64(pPkt + 0);
	aLanes[1] = b97_load_le64(pPkt + 8);
	aLanes[2] = b97_load_le64(pPkt + 16);
	aLanes[3] = b97_load_le64(pPkt + 24);
	b97_update(pSt, aLanes);
}

/* Rotate each 32-bit half of every lane left by nCount (1..31). */

static void
b97_rotate32_by(uint64_t aLanes[4], unsigned nCount)
{
	int i;

	for (i = 0; i < 4; i++) {
		uint32_t uLo = (uint32_t)aLanes[i];
		uint32_t uHi = (uint32_t)(aLanes[i] >> 32);

		uLo = (uLo << nCount) | (uLo >> (32u - nCount));
		uHi = (uHi << nCount) | (uHi >> (32u - nCount));
		aLanes[i] = (uint64_t)uLo | ((uint64_t)uHi << 32);
	}
}

static void
b97_reset(b97_hh_state *pSt, const uint64_t aKey[4])
{
	pSt->mul0[0] = B97_M0_0;
	pSt->mul0[1] = B97_M0_1;
	pSt->mul0[2] = B97_M0_2;
	pSt->mul0[3] = B97_M0_3;
	pSt->mul1[0] = B97_M1_0;
	pSt->mul1[1] = B97_M1_1;
	pSt->mul1[2] = B97_M1_2;
	pSt->mul1[3] = B97_M1_3;
	pSt->v0[0] = pSt->mul0[0] ^ aKey[0];
	pSt->v0[1] = pSt->mul0[1] ^ aKey[1];
	pSt->v0[2] = pSt->mul0[2] ^ aKey[2];
	pSt->v0[3] = pSt->mul0[3] ^ aKey[3];
	pSt->v1[0] = pSt->mul1[0] ^ b97_rot64_32(aKey[0]);
	pSt->v1[1] = pSt->mul1[1] ^ b97_rot64_32(aKey[1]);
	pSt->v1[2] = pSt->mul1[2] ^ b97_rot64_32(aKey[2]);
	pSt->v1[3] = pSt->mul1[3] ^ b97_rot64_32(aKey[3]);
}

/*
 * Final 1..31-byte packet. Length is mixed into v0; v1 halves are rotated
 * by size_mod32. Padding placement differs for size_mod32 >= 16 vs < 16
 * (public HighwayHash remainder rule).
 */
static void
b97_update_remainder(b97_hh_state *pSt, const unsigned char *pBytes,
		     size_t nMod32)
{
	unsigned char aPkt[32];
	size_t nMod4 = nMod32 & 3u;
	size_t nAligned = nMod32 & ~(size_t)3u;
	const unsigned char *pRem = pBytes + nAligned;
	size_t i;
	uint64_t uPad;
	int j;

	uPad = ((uint64_t)nMod32 << 32) + (uint64_t)nMod32;
	for (j = 0; j < 4; j++) {
		pSt->v0[j] += uPad;
	}
	b97_rotate32_by(pSt->v1, (unsigned)nMod32);

	for (i = 0; i < 32u; i++) {
		aPkt[i] = 0;
	}
	for (i = 0; i < nAligned; i++) {
		aPkt[i] = pBytes[i];
	}

	if ((nMod32 & 16u) != 0u) {
		/* 16..31: last 0..3 + previous 1..4 into packet[28..31]. */
		const unsigned char *pLast4 = pRem + (ptrdiff_t)nMod4 - 4;

		for (i = 0; i < 4u; i++) {
			aPkt[28u + i] = pLast4[i];
		}
	} else if (nMod4 != 0u) {
		/* 1..15: up to 3 padding bytes at packet[16..]. */
		aPkt[16] = pRem[0];
		aPkt[17] = pRem[nMod4 >> 1];
		aPkt[18] = pRem[nMod4 - 1u];
	}

	b97_update_packet(pSt, aPkt);
}

static void
b97_permute_and_update(b97_hh_state *pSt)
{
	uint64_t aPerm[4];

	aPerm[0] = b97_rot64_32(pSt->v0[2]);
	aPerm[1] = b97_rot64_32(pSt->v0[3]);
	aPerm[2] = b97_rot64_32(pSt->v0[0]);
	aPerm[3] = b97_rot64_32(pSt->v0[1]);
	b97_update(pSt, aPerm);
}

static uint64_t
b97_finalize64(b97_hh_state *pSt)
{
	int i;

	for (i = 0; i < 4; i++) {
		b97_permute_and_update(pSt);
	}
	return pSt->v0[0] + pSt->v1[0] + pSt->mul0[0] + pSt->mul1[0];
}

static uint64_t
b97_highwayhash64(const uint64_t aKey[4], const unsigned char *pData,
		  size_t cb)
{
	b97_hh_state st;
	size_t i;

	b97_reset(&st, aKey);
	for (i = 0; i + 32u <= cb; i += 32u) {
		b97_update_packet(&st, pData + i);
	}
	if ((cb & 31u) != 0u) {
		b97_update_remainder(&st, pData + i, cb & 31u);
	}
	return b97_finalize64(&st);
}

/*
 * highwayhash64 — Google HighwayHash 64-bit tag, key first (32 bytes).
 * NULL key → 32 zero bytes. NULL data with len != 0 → 0.
 */
uint64_t
highwayhash64(const unsigned char key[32], const void *data, size_t len)
{
	static const unsigned char aZero[32];
	uint64_t aKey[4];
	const unsigned char *pKey;

	if (key == NULL) {
		pKey = aZero;
	} else {
		pKey = key;
	}
	if (data == NULL && len != 0u) {
		return 0;
	}

	aKey[0] = b97_load_le64_n(pKey + 0, 8u);
	aKey[1] = b97_load_le64_n(pKey + 8, 8u);
	aKey[2] = b97_load_le64_n(pKey + 16, 8u);
	aKey[3] = b97_load_le64_n(pKey + 24, 8u);

	return b97_highwayhash64(aKey, (const unsigned char *)data, len);
}

uint64_t
__highwayhash64(const unsigned char key[32], const void *data, size_t len)
	__attribute__((alias("highwayhash64")));
