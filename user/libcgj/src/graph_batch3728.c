/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3728: freestanding map u64 draw into [lo, hi)
 * (_u surface).
 *
 * Surface (unique symbols):
 *   uint64_t gj_rng_range_u64_u(uint64_t r, uint64_t lo, uint64_t hi);
 *     - Map a full-range uniform draw r on [0, 2^64) into the half-open
 *       interval [lo, hi) via high_64(r * (hi - lo)). Portable split
 *       32-bit multiply — no __int128. Empty or inverted range
 *       (hi <= lo) returns lo without multiplying.
 *   uint64_t __gj_rng_range_u64_u  (alias)
 *   __libcgj_batch3728_marker = "libcgj-batch3728"
 *
 * CREATE-ONLY exclusive continuum wave (3721-3730). Distinct from
 * gj_rng_uniform_u64_u (batch3726, bound-from-zero form) — unique
 * range surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3728_marker[] = "libcgj-batch3728";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * High 64 bits of a 64x64 -> 128 unsigned product.
 * Portable four-way 32-bit split multiply (no __int128 / __uint128_t).
 */
static uint64_t
b3728_mulhi64(uint64_t u64A, uint64_t u64B)
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
 * gj_rng_range_u64_u — map r into half-open [lo, hi).
 *
 * r:  uniform integer on the full uint64 domain [0, 2^64)
 * lo: inclusive lower bound of the result range
 * hi: exclusive upper bound of the result range
 *
 * Computes lo + high_64(r * (hi - lo)). When hi <= lo the span is empty
 * or inverted and the function returns lo (defensive clamp).
 */
uint64_t
gj_rng_range_u64_u(uint64_t r, uint64_t lo, uint64_t hi)
{
	uint64_t u64Span;

	(void)NULL;

	if (hi <= lo) {
		return lo;
	}

	u64Span = hi - lo;
	return lo + b3728_mulhi64(r, u64Span);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_rng_range_u64_u(uint64_t r, uint64_t lo, uint64_t hi)
    __attribute__((alias("gj_rng_range_u64_u")));
