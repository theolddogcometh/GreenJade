/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch429: unsigned divide rounded to nearest.
 *
 * Surface (unique symbols):
 *   uint64_t gj_udiv_round(uint64_t a, uint64_t b);
 *     — Return a / b rounded to nearest integer (half rounds up).
 *       b == 0 → 0.
 *   uint64_t __gj_udiv_round  (alias)
 *   __libcgj_batch429_marker = "libcgj-batch429"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch429_marker[] = "libcgj-batch429";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_udiv_round — unsigned division rounded to nearest (half up).
 *
 * a: dividend
 * b: divisor (0 → return 0)
 *
 * Computes floor(a / b), then rounds up when the remainder is at least
 * half of b (2 * r >= b), using an overflow-safe remainder compare.
 * Exact halfway cases (even b, r == b/2) round away from zero (up).
 */
uint64_t
gj_udiv_round(uint64_t a, uint64_t b)
{
	uint64_t u64Q;
	uint64_t u64R;

	if (b == 0u) {
		return 0u;
	}

	u64Q = a / b;
	u64R = a % b;

	/* Round up when remainder is >= half of divisor: 2*r >= b. */
	if (u64R >= (b - u64R)) {
		u64Q += 1u;
	}

	return u64Q;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_udiv_round(uint64_t a, uint64_t b)
    __attribute__((alias("gj_udiv_round")));
