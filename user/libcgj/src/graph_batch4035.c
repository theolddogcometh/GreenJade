/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4035: freestanding uint64_t bit unpermute (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_unpermute_bits_u(uint64_t y, const uint8_t *map);
 *     - Inverse of gj_u64_permute_bits_u for the same map: place bit i of
 *       y into result bit (map[i] & 63). When map is a bijection,
 *       unpermute(permute(x, map), map) == x. When map is NULL, return y.
 *       If map is not bijective, later writes to the same dest win.
 *   uint64_t __gj_u64_unpermute_bits_u  (alias)
 *   __libcgj_batch4035_marker = "libcgj-batch4035"
 *
 * Exclusive continuum CREATE-ONLY (4031-4040). Unique
 * gj_u64_unpermute_bits_u surface only; no multi-def. Distinct from
 * gj_u64_permute_bits_u (batch4033), gj_u32_unpermute_bits_u (batch4034),
 * and gj_u64_bit_reverse (batch2134). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4035_marker[] = "libcgj-batch4035";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Inverse map application: result[map[i]] = y[i] for i in 0..63.
 * map == NULL is treated as identity by the public wrapper.
 */
static uint64_t
b4035_unpermute(uint64_t u64Y, const uint8_t *pMap)
{
	uint64_t u64Out;
	unsigned uI;
	unsigned uDst;

	u64Out = 0ull;
	for (uI = 0u; uI < 64u; uI++) {
		uDst = (unsigned)pMap[uI] & 63u;
		if (((u64Y >> uI) & 1ull) != 0ull) {
			u64Out |= (uint64_t)1ull << uDst;
		}
	}
	return u64Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_unpermute_bits_u - inverse bit permute of a 64-bit word.
 *
 * y:   permuted source word
 * map: optional length-64 array matching the map used by permute
 *
 * Returns unpermuted word, or y when map is NULL. No parent wires.
 */
uint64_t
gj_u64_unpermute_bits_u(uint64_t u64Y, const uint8_t *pMap)
{
	(void)NULL;
	if (pMap == NULL) {
		return u64Y;
	}
	return b4035_unpermute(u64Y, pMap);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_unpermute_bits_u(uint64_t u64Y, const uint8_t *pMap)
    __attribute__((alias("gj_u64_unpermute_bits_u")));
