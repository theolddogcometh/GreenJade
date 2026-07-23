/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1067: ceiling unsigned 64-bit division (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_div_ceil_u(uint64_t a, uint64_t b);
 *     — ceil(a / b) as unsigned integer division.
 *       b == 0 → 0. a == 0 → 0 for any b.
 *   uint64_t __gj_u64_div_ceil_u  (alias)
 *   __libcgj_batch1067_marker = "libcgj-batch1067"
 *
 * Distinct from gj_div_ceil_u64 (batch380) and gj_u64_div_ceil
 * (batch616) — different symbol names; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1067_marker[] = "libcgj-batch1067";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_div_ceil_u — ceiling divide a by b; b == 0 yields 0.
 *
 * Uses quotient + (remainder != 0) so intermediate a + b - 1 never
 * overflows UINT64_MAX.
 */
uint64_t
gj_u64_div_ceil_u(uint64_t u64A, uint64_t u64B)
{
	if (u64B == 0ull) {
		return 0ull;
	}
	return (u64A / u64B) + ((u64A % u64B) != 0ull ? 1ull : 0ull);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_div_ceil_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_div_ceil_u")));
