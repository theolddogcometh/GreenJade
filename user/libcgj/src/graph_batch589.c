/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch589: uint64 three-way max.
 *
 * Surface (unique symbols):
 *   uint64_t gj_max3_u64(uint64_t a, uint64_t b, uint64_t c);
 *     — Largest of a, b, and c (unsigned compare). Ties keep the
 *       rightmost equal candidate in pairwise fold order.
 *   __gj_max3_u64  (alias)
 *   __libcgj_batch589_marker = "libcgj-batch589"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch589_marker[] = "libcgj-batch589";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_max3_u64 — return the greatest of three uint64_t values.
 *
 * Pairwise: m = max(a, b); return max(m, c). On equality the second
 * operand of each pair is retained (matches gj_max_i64 equal→b style).
 */
uint64_t
gj_max3_u64(uint64_t a, uint64_t b, uint64_t c)
{
	uint64_t m;

	if (a > b) {
		m = a;
	} else {
		m = b;
	}
	if (m > c) {
		return m;
	}
	return c;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_max3_u64(uint64_t a, uint64_t b, uint64_t c)
    __attribute__((alias("gj_max3_u64")));
