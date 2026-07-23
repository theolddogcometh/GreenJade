/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch588: uint64_t three-way minimum.
 *
 * Surface (unique symbols):
 *   uint64_t gj_min3_u64(uint64_t a, uint64_t b, uint64_t c);
 *     — Least of a, b, and c (ties prefer the later argument in the
 *       pairwise reduction: min(a,b) then min(that, c)).
 *   uint64_t __gj_min3_u64  (alias)
 *   __libcgj_batch588_marker = "libcgj-batch588"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch588_marker[] = "libcgj-batch588";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_min3_u64 — return the least of three uint64_t values.
 *
 * Pairwise: m = min(a, b); return min(m, c). On equality the later
 * operand of each comparison is kept (matches gj_min_i64 style).
 */
uint64_t
gj_min3_u64(uint64_t a, uint64_t b, uint64_t c)
{
	uint64_t m;

	if (a < b) {
		m = a;
	} else {
		m = b;
	}
	if (c < m) {
		return c;
	}
	return m;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_min3_u64(uint64_t a, uint64_t b, uint64_t c)
    __attribute__((alias("gj_min3_u64")));
