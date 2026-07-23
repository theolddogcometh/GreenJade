/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch616: ceiling unsigned 64-bit division.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_div_ceil(uint64_t a, uint64_t b);
 *     — ceil(a / b) as unsigned integer division.
 *       gj_u64_div_ceil(a, 0) == 0  (divide-by-zero → 0).
 *       gj_u64_div_ceil(0, b) == 0  for any b (including 0).
 *       Exact multiples: returns a / b.
 *       Otherwise equivalent to (a + b - 1) / b with overflow care.
 *   uint64_t __gj_u64_div_ceil  (alias)
 *   __libcgj_batch616_marker = "libcgj-batch616"
 *
 * Distinct from gj_div_ceil_u64 / __gj_div_ceil_u64 (batch380) — different
 * symbol names; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch616_marker[] = "libcgj-batch616";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_div_ceil — ceiling divide a by b; b == 0 yields 0.
 *
 * Naive (a + b - 1) / b overflows when a + b - 1 exceeds UINT64_MAX
 * (e.g. a == UINT64_MAX, b == 2). Compute via quotient + (remainder != 0)
 * instead, which is mathematically equivalent and never overflows.
 */
uint64_t
gj_u64_div_ceil(uint64_t a, uint64_t b)
{
	if (b == 0u) {
		return 0u;
	}
	return (a / b) + ((a % b) != 0u ? 1u : 0u);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_div_ceil(uint64_t a, uint64_t b)
    __attribute__((alias("gj_u64_div_ceil")));
