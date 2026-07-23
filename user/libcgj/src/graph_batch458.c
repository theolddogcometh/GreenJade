/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch458: XXH32 one-shot under a unique gj_ name.
 *
 * Surface (unique symbols only — do not export XXH32 / __XXH32):
 *   uint32_t gj_xxh32(const void *data, size_t n, uint32_t seed);
 *     — xxHash XXH32 one-shot over data[0..n). Clean-room integer path.
 *       data may be NULL only when n == 0; otherwise NULL+n>0 → 0.
 *   __gj_xxh32  (alias)
 *   __libcgj_batch458_marker = "libcgj-batch458"
 *
 * Avoid multi-def with graph_batch40.c XXH32/__XXH32: only gj_xxh32 is
 * exported from this TU. Clean-room freestanding pure C (integer/pointer
 * only). Compiles with -ffreestanding -msse2 -Wall -Wextra -Werror.
 * No malloc, no errno. No third-party xxHash source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch458_marker[] = "libcgj-batch458";

/* XXH32 primes (public algorithm constants). */
#define B458_PRIME32_1 0x9E3779B1u
#define B458_PRIME32_2 0x85EBCA77u
#define B458_PRIME32_3 0xC2B2AE3Du
#define B458_PRIME32_4 0x27D4EB2Fu
#define B458_PRIME32_5 0x165667B1u

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b458_rotl32(uint32_t u32X, unsigned uN)
{
	return (u32X << uN) | (u32X >> (32u - uN));
}

static uint32_t
b458_read32le(const unsigned char *p)
{
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) | ((uint32_t)p[2] << 16) |
	       ((uint32_t)p[3] << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_xxh32 — XXH32 one-shot hash.
 *
 * data: input bytes (NULL only legal when n == 0).
 * n:    byte count.
 * seed: 32-bit seed (public xxHash parameter).
 *
 * Returns the 32-bit digest. NULL data with n != 0 yields 0 (defensive).
 * Empty input (n == 0) is valid and seed-dependent (e.g. seed 0 →
 * 0x02CC5D05).
 */
uint32_t
gj_xxh32(const void *data, size_t n, uint32_t seed)
{
	const unsigned char *p = (const unsigned char *)data;
	uint32_t u32H;

	if (p == NULL && n != 0u) {
		return 0u;
	}
	if (n >= 16u) {
		const unsigned char *pLimit = p + n - 16u;
		uint32_t u32V1 = seed + B458_PRIME32_1 + B458_PRIME32_2;
		uint32_t u32V2 = seed + B458_PRIME32_2;
		uint32_t u32V3 = seed + 0u;
		uint32_t u32V4 = seed - B458_PRIME32_1;

		do {
			u32V1 = b458_rotl32(u32V1 + b458_read32le(p) *
			                   B458_PRIME32_2, 13) *
			        B458_PRIME32_1;
			p += 4;
			u32V2 = b458_rotl32(u32V2 + b458_read32le(p) *
			                   B458_PRIME32_2, 13) *
			        B458_PRIME32_1;
			p += 4;
			u32V3 = b458_rotl32(u32V3 + b458_read32le(p) *
			                   B458_PRIME32_2, 13) *
			        B458_PRIME32_1;
			p += 4;
			u32V4 = b458_rotl32(u32V4 + b458_read32le(p) *
			                   B458_PRIME32_2, 13) *
			        B458_PRIME32_1;
			p += 4;
		} while (p <= pLimit);
		u32H = b458_rotl32(u32V1, 1) + b458_rotl32(u32V2, 7) +
		       b458_rotl32(u32V3, 12) + b458_rotl32(u32V4, 18);
	} else {
		u32H = seed + B458_PRIME32_5;
	}
	u32H += (uint32_t)n;
	{
		const unsigned char *pEnd =
		    (const unsigned char *)data + n;

		while (p + 4u <= pEnd) {
			u32H += b458_read32le(p) * B458_PRIME32_3;
			u32H = b458_rotl32(u32H, 17) * B458_PRIME32_4;
			p += 4;
		}
		while (p < pEnd) {
			u32H += (uint32_t)(*p) * B458_PRIME32_5;
			u32H = b458_rotl32(u32H, 11) * B458_PRIME32_1;
			p++;
		}
	}
	u32H ^= u32H >> 15;
	u32H *= B458_PRIME32_2;
	u32H ^= u32H >> 13;
	u32H *= B458_PRIME32_3;
	u32H ^= u32H >> 16;
	return u32H;
}

uint32_t __gj_xxh32(const void *data, size_t n, uint32_t seed)
    __attribute__((alias("gj_xxh32")));
