/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch645: int64_t closed-interval clamp with
 * lo/hi reorder when inverted.
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_clamp(int64_t x, int64_t lo, int64_t hi);
 *     — Confine x to the closed interval between lo and hi.
 *       If lo > hi, the bounds are swapped so the effective range is
 *       always [min(lo,hi), max(lo,hi)]:
 *         if x < lo → lo; elif x > hi → hi; else x
 *       (after any needed lo/hi reorder).
 *   int64_t __gj_i64_clamp  (alias)
 *   __libcgj_batch645_marker = "libcgj-batch645"
 *
 * Distinct from gj_clamp_i64 / __gj_clamp_i64 (graph_batch376) — that
 * surface does not remap inverted bounds. Distinct from
 * gj_clamp_u64 / __gj_clamp_u64 (graph_batch587). Do not multi-def
 * those symbols here.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch645_marker[] = "libcgj-batch645";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_clamp — confine x to the closed interval between lo and hi.
 *
 * x:  value to clamp
 * lo: lower bound (may be greater than hi; swapped)
 * hi: upper bound
 *
 * Returns the ordered low bound if x is below it, the ordered high
 * bound if above it, otherwise x. Order of lo/hi does not matter: they
 * are normalized so the interval is never inverted.
 */
int64_t
gj_i64_clamp(int64_t i64X, int64_t i64Lo, int64_t i64Hi)
{
	int64_t i64Tmp;

	if (i64Lo > i64Hi) {
		i64Tmp = i64Lo;
		i64Lo = i64Hi;
		i64Hi = i64Tmp;
	}

	if (i64X < i64Lo) {
		return i64Lo;
	}
	if (i64X > i64Hi) {
		return i64Hi;
	}
	return i64X;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_clamp(int64_t i64X, int64_t i64Lo, int64_t i64Hi)
    __attribute__((alias("gj_i64_clamp")));
