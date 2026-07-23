/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4033: freestanding uint64_t bit permute (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_permute_bits_u(uint64_t x, const uint8_t *map);
 *     - Build a new 64-bit word where result bit i is taken from source
 *       bit (map[i] & 63) of x, for i in 0..63. When map is NULL, return
 *       x unchanged (identity). map entries need not form a bijection.
 *   uint64_t __gj_u64_permute_bits_u  (alias)
 *   __libcgj_batch4033_marker = "libcgj-batch4033"
 *
 * Exclusive continuum CREATE-ONLY (4031-4040). Unique
 * gj_u64_permute_bits_u surface only; no multi-def. Distinct from
 * gj_u64_bit_reverse (batch2134), gj_u32_permute_bits_u (batch4032), and
 * gj_u64_unpermute_bits_u (batch4035). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4033_marker[] = "libcgj-batch4033";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * For each dest bit i, copy source bit (map[i] & 63) of x into that
 * position. map == NULL is treated as identity by the public wrapper.
 */
static uint64_t
b4033_permute(uint64_t u64X, const uint8_t *pMap)
{
	uint64_t u64Out;
	unsigned uI;
	unsigned uSrc;

	u64Out = 0ull;
	for (uI = 0u; uI < 64u; uI++) {
		uSrc = (unsigned)pMap[uI] & 63u;
		if (((u64X >> uSrc) & 1ull) != 0ull) {
			u64Out |= (uint64_t)1ull << uI;
		}
	}
	return u64Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_permute_bits_u - permute bits of a 64-bit word via a map.
 *
 * x:   source word
 * map: optional length-64 array; map[i] = source bit index for dest bit i
 *
 * Returns permuted word, or x when map is NULL. No parent wires.
 */
uint64_t
gj_u64_permute_bits_u(uint64_t u64X, const uint8_t *pMap)
{
	(void)NULL;
	if (pMap == NULL) {
		return u64X;
	}
	return b4033_permute(u64X, pMap);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_permute_bits_u(uint64_t u64X, const uint8_t *pMap)
    __attribute__((alias("gj_u64_permute_bits_u")));
