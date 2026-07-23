/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4032: freestanding uint32_t bit permute (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_permute_bits_u(uint32_t x, const uint8_t *map);
 *     - Build a new 32-bit word where result bit i is taken from source
 *       bit (map[i] & 31) of x, for i in 0..31. When map is NULL, return
 *       x unchanged (identity). map entries need not form a bijection.
 *   uint32_t __gj_u32_permute_bits_u  (alias)
 *   __libcgj_batch4032_marker = "libcgj-batch4032"
 *
 * Exclusive continuum CREATE-ONLY (4031-4040). Unique
 * gj_u32_permute_bits_u surface only; no multi-def. Distinct from
 * gj_u32_bit_reverse (batch2133), DES-style internal permutes, and
 * gj_u32_unpermute_bits_u (batch4034). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4032_marker[] = "libcgj-batch4032";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * For each dest bit i, copy source bit (map[i] & 31) of x into that
 * position. map == NULL is treated as identity by the public wrapper.
 */
static uint32_t
b4032_permute(uint32_t u32X, const uint8_t *pMap)
{
	uint32_t u32Out;
	unsigned uI;
	unsigned uSrc;

	u32Out = 0u;
	for (uI = 0u; uI < 32u; uI++) {
		uSrc = (unsigned)pMap[uI] & 31u;
		if (((u32X >> uSrc) & 1u) != 0u) {
			u32Out |= (uint32_t)1u << uI;
		}
	}
	return u32Out;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_permute_bits_u - permute bits of a 32-bit word via a map.
 *
 * x:   source word
 * map: optional length-32 array; map[i] = source bit index for dest bit i
 *
 * Returns permuted word, or x when map is NULL. No parent wires.
 */
uint32_t
gj_u32_permute_bits_u(uint32_t u32X, const uint8_t *pMap)
{
	(void)NULL;
	if (pMap == NULL) {
		return u32X;
	}
	return b4032_permute(u32X, pMap);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_permute_bits_u(uint32_t u32X, const uint8_t *pMap)
    __attribute__((alias("gj_u32_permute_bits_u")));
