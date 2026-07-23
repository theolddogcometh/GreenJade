/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3855: round unsigned 32-bit division (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_round_div_u(uint32_t a, uint32_t b);
 *     - a / b rounded to nearest integer (half rounds up).
 *       b == 0 -> 0.
 *   uint32_t __gj_u32_round_div_u  (alias)
 *   __libcgj_batch3855_marker = "libcgj-batch3855"
 *
 * Exclusive continuum CREATE-ONLY (3851-3860). Distinct from
 * gj_udiv_round (batch429) and gj_div_round_nearest_u64 (batch995) —
 * unique gj_u32_round_div_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3855_marker[] = "libcgj-batch3855";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Nearest half-up: floor(a/b), then +1 when 2*r >= b
 * (overflow-safe remainder compare: r >= b - r). b == 0 yields 0.
 */
static uint32_t
b3855_round_div(uint32_t u32A, uint32_t u32B)
{
	uint32_t u32Q;
	uint32_t u32R;

	if (u32B == 0u) {
		return 0u;
	}

	u32Q = u32A / u32B;
	u32R = u32A % u32B;

	if (u32R >= (u32B - u32R)) {
		u32Q += 1u;
	}

	return u32Q;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_round_div_u - unsigned division rounded to nearest (half up).
 *
 * a: dividend
 * b: divisor (0 -> 0)
 * No parent wires.
 */
uint32_t
gj_u32_round_div_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b3855_round_div(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_round_div_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_round_div_u")));
