/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3725: freestanding uniform map u32 -> [0,bound)
 * (_u surface).
 *
 * Surface (unique symbols):
 *   uint32_t gj_rng_uniform_u32_u(uint32_t r, uint32_t bound);
 *     - Map a full-range uniform draw r on [0, 2^32) into the half-open
 *       interval [0, bound) via Lemire-style multiply-high:
 *         result = high_32( (uint64_t)r * (uint64_t)bound )
 *       bound == 0 -> 0 (defensive; empty range). Pure mapping only —
 *       does not advance a PRNG or reject draws.
 *   uint32_t __gj_rng_uniform_u32_u  (alias)
 *   __libcgj_batch3725_marker = "libcgj-batch3725"
 *
 * CREATE-ONLY exclusive continuum wave (3721-3730). Distinct from
 * gj_uniform_u32 (batch717) and gj_uniform_u32_bounded (batch2187) —
 * unique rng_*_u surface; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3725_marker[] = "libcgj-batch3725";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Map r into [0, bound) via 32x32->64 multiply-high.
 * bound must be non-zero on entry.
 */
static uint32_t
b3725_map(uint32_t u32R, uint32_t u32Bound)
{
	uint64_t u64Prod;

	u64Prod = (uint64_t)u32R * (uint64_t)u32Bound;
	return (uint32_t)(u64Prod >> 32);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_rng_uniform_u32_u — map r into [0, bound) via multiply-high.
 *
 * r:     uniform integer on the full uint32 domain [0, 2^32)
 * bound: exclusive upper bound of the result range
 *
 * When bound is a power of two the map is exactly uniform on [0, bound);
 * otherwise residual bias matches the non-rejecting Lemire shape used
 * elsewhere in the graph (acceptable for bring-up when r is full-range).
 *
 * bound == 0 returns 0 without multiplying.
 */
uint32_t
gj_rng_uniform_u32_u(uint32_t r, uint32_t bound)
{
	(void)NULL;

	if (bound == 0u) {
		return 0u;
	}
	return b3725_map(r, bound);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_rng_uniform_u32_u(uint32_t r, uint32_t bound)
    __attribute__((alias("gj_rng_uniform_u32_u")));
