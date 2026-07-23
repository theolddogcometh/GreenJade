/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4034: freestanding uint32_t bit unpermute (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_unpermute_bits_u(uint32_t y, const uint8_t *map);
 *     - Inverse of gj_u32_permute_bits_u for the same map: place bit i of
 *       y into result bit (map[i] & 31). When map is a bijection,
 *       unpermute(permute(x, map), map) == x. When map is NULL, return y.
 *       If map is not bijective, later writes to the same dest win.
 *   uint32_t __gj_u32_unpermute_bits_u  (alias)
 *   __libcgj_batch4034_marker = "libcgj-batch4034"
 *
 * Exclusive continuum CREATE-ONLY (4031-4040). Unique
 * gj_u32_unpermute_bits_u surface only; no multi-def. Distinct from
 * gj_u32_permute_bits_u (batch4032) and gj_u32_bit_reverse (batch2133).
 * No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4034_marker[] = "libcgj-batch4034";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Inverse map application: result[map[i]] = y[i] for i in 0..31.
 * map == NULL is treated as identity by the public wrapper.
 */
static uint32_t
b4034_unpermute(uint32_t u32Y, const uint8_t *pMap)
{
	uint32_t u32Out;
	unsigned uI;
	unsigned uDst;

	u32Out = 0u;
	for (uI = 0u; uI < 32u; uI++) {
		uDst = (unsigned)pMap[uI] & 31u;
		if (((u32Y >> uI) & 1u) != 0u) {
			u32Out |= (uint32_t)1u << uDst;
		}
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_unpermute_bits_u - inverse bit permute of a 32-bit word.
 *
 * y:   permuted source word
 * map: optional length-32 array matching the map used by permute
 *
 * Returns unpermuted word, or y when map is NULL. No parent wires.
 */
uint32_t
gj_u32_unpermute_bits_u(uint32_t u32Y, const uint8_t *pMap)
{
	(void)NULL;
	if (pMap == NULL) {
		return u32Y;
	}
	return b4034_unpermute(u32Y, pMap);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_unpermute_bits_u(uint32_t u32Y, const uint8_t *pMap)
    __attribute__((alias("gj_u32_unpermute_bits_u")));
