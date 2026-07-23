/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch995: unsigned divide rounded to nearest.
 *
 * Surface (unique symbols):
 *   uint64_t gj_div_round_nearest_u64(uint64_t a, uint64_t b);
 *     — Return a / b rounded to nearest integer (half rounds up).
 *       b == 0 → 0.
 *   uint64_t __gj_div_round_nearest_u64  (alias)
 *   __libcgj_batch995_marker = "libcgj-batch995"
 *
 * Does NOT redefine gj_udiv_round (batch429) — distinct symbol name.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch995_marker[] = "libcgj-batch995";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_div_round_nearest_u64 — unsigned division rounded to nearest (half up).
 *
 * u64A: dividend
 * u64B: divisor (0 → return 0)
 *
 * floor(a/b), then +1 when remainder is at least half of b (2*r >= b).
 */
uint64_t
gj_div_round_nearest_u64(uint64_t u64A, uint64_t u64B)
{
	uint64_t u64Q;
	uint64_t u64R;

	if (u64B == 0u) {
		return 0u;
	}

	u64Q = u64A / u64B;
	u64R = u64A % u64B;

	/* Round up when remainder is >= half of divisor: 2*r >= b. */
	if (u64R >= (u64B - u64R)) {
		u64Q += 1u;
	}

	return u64Q;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_div_round_nearest_u64(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_div_round_nearest_u64")));
