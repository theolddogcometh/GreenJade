/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch717: freestanding uniform map of a uint32_t
 * draw into [0, bound) via multiply-high.
 *
 * Surface (unique symbols):
 *   uint32_t gj_uniform_u32(uint32_t r, uint32_t bound);
 *     — Map a full-range uniform draw r (any uint32_t on [0, 2^32)) into
 *       the half-open interval [0, bound) using Lemire-style multiply-high:
 *         result = high_32( (uint64_t)r * (uint64_t)bound )
 *       bound == 0 → 0 (defensive; empty range). Pure mapping only —
 *       does not advance a PRNG or reject draws.
 *   uint32_t __gj_uniform_u32  (alias)
 *   __libcgj_batch717_marker = "libcgj-batch717"
 *
 * Distinct from arc4random_uniform / __arc4random_uniform (stdlib.c /
 * batch106 rejection sampling) and from private helpers inside
 * gj_shuffle_u32 (batch212) / gj_reservoir_u32 (batch304). Unique
 * gj_uniform_u32 names only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch717_marker[] = "libcgj-batch717";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_uniform_u32 — map r into [0, bound) via 32×32→64 multiply-high.
 *
 * r:     uniform integer on the full uint32 domain [0, 2^32)
 * bound: exclusive upper bound of the result range
 *
 * Computes (r * bound) as a 64-bit product and returns the high 32 bits.
 * When bound is a power of two the map is exactly uniform on [0, bound);
 * otherwise residual bias matches the non-rejecting Lemire shape used
 * elsewhere in the graph (acceptable for bring-up when r is full-range).
 *
 * bound == 0 returns 0 without multiplying.
 */
uint32_t
gj_uniform_u32(uint32_t r, uint32_t bound)
{
	uint64_t u64Prod;

	if (bound == 0u) {
		return 0u;
	}
	u64Prod = (uint64_t)r * (uint64_t)bound;
	return (uint32_t)(u64Prod >> 32);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_uniform_u32(uint32_t r, uint32_t bound)
    __attribute__((alias("gj_uniform_u32")));
