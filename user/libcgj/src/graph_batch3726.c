/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3726: freestanding uniform map u64 -> [0,bound)
 * (_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_rng_uniform_u64_u(uint64_t r, uint64_t bound);
 *     - Map a full-range uniform draw r on [0, 2^64) into the half-open
 *       interval [0, bound) via Lemire-style multiply-high:
 *         result = high_64( r * bound )   (128-bit product, high half)
 *       Portable split 32-bit multiply — no __int128. bound == 0 -> 0.
 *       Pure mapping only; does not advance a PRNG or reject draws.
 *   uint64_t __gj_rng_uniform_u64_u  (alias)
 *   __libcgj_batch3726_marker = "libcgj-batch3726"
 *
 * CREATE-ONLY exclusive continuum wave (3721-3730). Unique rng_*_u
 * surface; no multi-def. Pairs with gj_rng_uniform_u32_u (batch3725).
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3726_marker[] = "libcgj-batch3726";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * High 64 bits of a 64x64 -> 128 unsigned product.
 * Portable four-way 32-bit split multiply (no __int128 / __uint128_t).
 */
static uint64_t
b3726_mulhi64(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64Alo = u64A & 0xffffffffULL;
	uint64_t u64Ahi = u64A >> 32;
	uint64_t u64Blo = u64B & 0xffffffffULL;
	uint64_t u64Bhi = u64B >> 32;
	uint64_t u64LoLo = u64Alo * u64Blo;
	uint64_t u64HiLo = u64Ahi * u64Blo;
	uint64_t u64LoHi = u64Alo * u64Bhi;
	uint64_t u64HiHi = u64Ahi * u64Bhi;
	uint64_t u64Cross = (u64LoLo >> 32) + (u64HiLo & 0xffffffffULL) +
	                    u64LoHi;
	uint64_t u64Upper = (u64HiLo >> 32) + (u64Cross >> 32) + u64HiHi;

	return u64Upper;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rng_uniform_u64_u — map r into [0, bound) via 64x64 multiply-high.
 *
 * r:     uniform integer on the full uint64 domain [0, 2^64)
 * bound: exclusive upper bound of the result range
 *
 * Returns high_64(r * bound). bound == 0 returns 0 without multiplying.
 */
uint64_t
gj_rng_uniform_u64_u(uint64_t r, uint64_t bound)
{
	(void)NULL;

	if (bound == 0ULL) {
		return 0ULL;
	}
	return b3726_mulhi64(r, bound);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rng_uniform_u64_u(uint64_t r, uint64_t bound)
    __attribute__((alias("gj_rng_uniform_u64_u")));
