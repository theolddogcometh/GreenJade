/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2714: ceiling unsigned 32-bit division (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_div_ceil_u(uint32_t a, uint32_t b);
 *     - ceil(a / b) as unsigned integer division.
 *       b == 0 → 0. a == 0 → 0 for any b.
 *   uint32_t __gj_u32_div_ceil_u  (alias)
 *   __libcgj_batch2714_marker = "libcgj-batch2714"
 *
 * Post-2710 continuum exclusive wave (2711-2720). Distinct from
 * gj_u64_div_ceil / gj_u64_div_ceil_u / gj_div_ceil_u64 — 32-bit
 * u-suffix surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2714_marker[] = "libcgj-batch2714";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * ceil(a / b) via quotient + (remainder != 0) so intermediate
 * a + b - 1 never overflows UINT32_MAX. b == 0 yields 0.
 */
static uint32_t
b2714_div_ceil(uint32_t u32A, uint32_t u32B)
{
	if (u32B == 0u) {
		return 0u;
	}
	return (u32A / u32B) + ((u32A % u32B) != 0u ? 1u : 0u);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_div_ceil_u - ceiling divide a by b; b == 0 yields 0.
 *
 * a: dividend
 * b: divisor (0 → 0)
 */
uint32_t
gj_u32_div_ceil_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b2714_div_ceil(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_div_ceil_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_div_ceil_u")));
