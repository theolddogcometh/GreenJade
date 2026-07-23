/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4858: workgroup ceiling unsigned division.
 *
 * Surface (unique symbols):
 *   uint32_t gj_wg_div_ceil_u(uint32_t n, uint32_t d);
 *     - ceil(n / d) as unsigned integer division.
 *       d == 0 → 0. n == 0 → 0 for any d.
 *   uint32_t __gj_wg_div_ceil_u  (alias)
 *   __libcgj_batch4858_marker = "libcgj-batch4858"
 *
 * Exclusive continuum CREATE-ONLY (4851-4860). Unique gj_wg_div_ceil_u
 * surface only; no multi-def. Distinct from gj_div_ceil_u64 (batch380),
 * gj_u64_div_ceil (batch616), and gj_u64_div_ceil_u (batch1067) —
 * uint32 workgroup-named surface. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4858_marker[] = "libcgj-batch4858";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Ceiling divide n / d without overflowing n + d - 1:
 * quotient + (remainder != 0).
 */
static uint32_t
b4858_div_ceil(uint32_t u32N, uint32_t u32D)
{
	if (u32D == 0u) {
		return 0u;
	}
	return (u32N / u32D) + ((u32N % u32D) != 0u ? 1u : 0u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_wg_div_ceil_u - ceiling divide n by d for workgroup launch math.
 *
 * n: numerator (e.g. global extent)
 * d: denominator (e.g. workgroup extent); 0 → 0
 *
 * Returns ceil(n / d). Self-contained; no parent wires.
 */
uint32_t
gj_wg_div_ceil_u(uint32_t u32N, uint32_t u32D)
{
	(void)NULL;
	return b4858_div_ceil(u32N, u32D);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_wg_div_ceil_u(uint32_t u32N, uint32_t u32D)
    __attribute__((alias("gj_wg_div_ceil_u")));
