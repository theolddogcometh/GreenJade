/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch625: MurmurHash3 x86_32 one-shot under a unique
 * gj_ name with explicit seed parameter.
 *
 * Surface (unique symbols only — do not export murmur3_32 / gj_murmur3_32 /
 * gj_murmur3_32_seed0 / MurmurHash3_*):
 *   uint32_t gj_murmur3_32_seed(const void *data, size_t n, uint32_t seed);
 *     — Austin Appleby MurmurHash3 x86_32 one-shot over data[0..n) with
 *       caller-supplied seed. Clean-room integer path. data may be NULL only
 *       when n == 0; otherwise NULL+n>0 → 0.
 *   __gj_murmur3_32_seed  (alias)
 *   __libcgj_batch625_marker = "libcgj-batch625"
 *
 * Avoid multi-def with:
 *   graph_batch41.c  murmur3_32 / __murmur3_32
 *   graph_batch105.c MurmurHash3_x86_32 / __MurmurHash3_x86_32
 *   graph_batch291.c gj_murmur3_32 / __gj_murmur3_32
 *   graph_batch425.c gj_murmur3_32_seed0 / __gj_murmur3_32_seed0
 * Only gj_murmur3_32_seed / __gj_murmur3_32_seed are exported from this TU.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno.
 * No third-party source copied. Algorithm constants match the published
 * SMHasher / Appleby public-domain MurmurHash3 x86_32 reference.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch625_marker[] = "libcgj-batch625";

/* MurmurHash3 x86_32 round constants (public algorithm). */
#define B625_C1 0xcc9e2d51u
#define B625_C2 0x1b873593u
#define B625_M  5u
#define B625_N  0xe6546b64u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b625_rotl32(uint32_t u32X, unsigned uN)
{
	return (u32X << uN) | (u32X >> (32u - uN));
}

static uint32_t
b625_load_le32(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	       ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}

static uint32_t
b625_fmix32(uint32_t u32H)
{
	u32H ^= u32H >> 16;
	u32H *= 0x85ebca6bu;
	u32H ^= u32H >> 13;
	u32H *= 0xc2b2ae35u;
	u32H ^= u32H >> 16;
	return u32H;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_murmur3_32_seed — MurmurHash3 x86_32 one-shot hash with seed.
 *
 * data: input bytes (NULL only legal when n == 0).
 * n:    byte count.
 * seed: 32-bit seed (public MurmurHash3 parameter).
 *
 * Returns the 32-bit digest. NULL data with n != 0 yields 0 (defensive).
 * Empty input (n == 0) is valid and seed-dependent (e.g. seed 0 → 0).
 */
uint32_t
gj_murmur3_32_seed(const void *data, size_t n, uint32_t seed)
{
	const unsigned char *p = (const unsigned char *)data;
	const size_t nBlocks = n / 4u;
	uint32_t u32H1 = seed;
	size_t i;
	const unsigned char *pTail;
	uint32_t u32K1 = 0u;

	if (p == NULL && n != 0u) {
		return 0u;
	}

	for (i = 0u; i < nBlocks; i++) {
		uint32_t u32K = b625_load_le32(p + i * 4u);

		u32K *= B625_C1;
		u32K = b625_rotl32(u32K, 15);
		u32K *= B625_C2;

		u32H1 ^= u32K;
		u32H1 = b625_rotl32(u32H1, 13);
		u32H1 = u32H1 * B625_M + B625_N;
	}

	pTail = (p == NULL) ? NULL : (p + nBlocks * 4u);
	switch (n & 3u) {
	case 3:
		u32K1 ^= (uint32_t)pTail[2] << 16;
		/* fallthrough */
	case 2:
		u32K1 ^= (uint32_t)pTail[1] << 8;
		/* fallthrough */
	case 1:
		u32K1 ^= (uint32_t)pTail[0];
		u32K1 *= B625_C1;
		u32K1 = b625_rotl32(u32K1, 15);
		u32K1 *= B625_C2;
		u32H1 ^= u32K1;
		break;
	default:
		break;
	}

	u32H1 ^= (uint32_t)n;
	return b625_fmix32(u32H1);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_murmur3_32_seed(const void *data, size_t n, uint32_t seed)
    __attribute__((alias("gj_murmur3_32_seed")));
