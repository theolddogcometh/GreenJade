/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch644: int32_t closed-interval clamp.
 *
 * Surface (unique symbols):
 *   int32_t gj_i32_clamp(int32_t x, int32_t lo, int32_t hi);
 *     — Confine x to the closed interval between lo and hi.
 *       If lo > hi, the bounds are swapped so the effective range
 *       is always [min(lo,hi), max(lo,hi)]:
 *         if x < lo → lo; elif x > hi → hi; else x
 *       (after any needed lo/hi reorder).
 *   int32_t __gj_i32_clamp  (alias)
 *   __libcgj_batch644_marker = "libcgj-batch644"
 *
 * Avoid multi-def: gj_clamp_i64 / __gj_clamp_i64 live in graph_batch376.c;
 * gj_clamp_u64 / __gj_clamp_u64 live in graph_batch587.c. This TU exports
 * only the signed int32_t surface (gj_i32_clamp naming).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch644_marker[] = "libcgj-batch644";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_clamp — confine x to the closed interval between lo and hi.
 *
 * x:  value to clamp
 * lo: lower bound (may be greater than hi; swapped)
 * hi: upper bound
 *
 * Returns lo if x is below the ordered low bound, hi if above the ordered
 * high bound, otherwise x. Order of arguments does not matter: lo and hi
 * are normalized so the interval is never inverted.
 */
int32_t
gj_i32_clamp(int32_t x, int32_t lo, int32_t hi)
{
	int32_t tmp;

	if (lo > hi) {
		tmp = lo;
		lo = hi;
		hi = tmp;
	}

	if (x < lo) {
		return lo;
	}
	if (x > hi) {
		return hi;
	}
	return x;
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_i32_clamp(int32_t x, int32_t lo, int32_t hi)
    __attribute__((alias("gj_i32_clamp")));
