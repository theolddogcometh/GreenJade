/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch380: ceiling unsigned 64-bit division.
 *
 * Surface (unique symbols):
 *   uint64_t gj_div_ceil_u64(uint64_t a, uint64_t b);
 *     — ceil(a / b) as unsigned integer division.
 *       gj_div_ceil_u64(a, 0) == 0  (divide-by-zero → 0).
 *       gj_div_ceil_u64(0, b) == 0  for any b (including 0).
 *       Exact multiples: returns a / b.
 *       Otherwise returns (a / b) + 1.
 *   uint64_t __gj_div_ceil_u64  (alias)
 *   __libcgj_batch380_marker = "libcgj-batch380"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch380_marker[] = "libcgj-batch380";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_div_ceil_u64 — ceiling divide a by b; b == 0 yields 0.
 *
 * Uses quotient + (remainder != 0) so intermediate a + b - 1 never
 * overflows UINT64_MAX (e.g. a == UINT64_MAX, b == 2).
 */
uint64_t
gj_div_ceil_u64(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0u) {
		return 0u;
	}
	return (u64A / u64B) + ((u64A % u64B) != 0u ? 1u : 0u);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_div_ceil_u64(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_div_ceil_u64")));
