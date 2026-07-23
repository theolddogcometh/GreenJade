/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3856: round unsigned 64-bit division (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_round_div_u(uint64_t a, uint64_t b);
 *     - a / b rounded to nearest integer (half rounds up).
 *       b == 0 -> 0.
 *   uint64_t __gj_u64_round_div_u  (alias)
 *   __libcgj_batch3856_marker = "libcgj-batch3856"
 *
 * Exclusive continuum CREATE-ONLY (3851-3860). Distinct from
 * gj_udiv_round (batch429), gj_div_round_nearest_u64 (batch995), and
 * gj_u32_round_div_u (batch3855) — unique gj_u64_round_div_u surface
 * only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3856_marker[] = "libcgj-batch3856";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Nearest half-up: floor(a/b), then +1 when 2*r >= b
 * (overflow-safe remainder compare: r >= b - r). b == 0 yields 0.
 */
static uint64_t
b3856_round_div(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64Q;
	uint64_t u64R;

	if (u64B == 0ull) {
		return 0ull;
	}

	u64Q = u64A / u64B;
	u64R = u64A % u64B;

	if (u64R >= (u64B - u64R)) {
		u64Q += 1ull;
	}

	return u64Q;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_round_div_u - unsigned division rounded to nearest (half up).
 *
 * a: dividend
 * b: divisor (0 -> 0)
 * No parent wires.
 */
uint64_t
gj_u64_round_div_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b3856_round_div(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_round_div_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_round_div_u")));
