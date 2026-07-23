/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch376: int64_t min / max / clamp.
 *
 * Surface (unique symbols):
 *   int64_t gj_min_i64(int64_t a, int64_t b);
 *     — Smaller of a and b (equal → b).
 *   int64_t gj_max_i64(int64_t a, int64_t b);
 *     — Larger of a and b (equal → b).
 *   int64_t gj_clamp_i64(int64_t x, int64_t lo, int64_t hi);
 *     — Clamp x into [lo, hi]: max(lo, min(hi, x)).
 *       Caller should pass lo <= hi; if lo > hi the composition
 *       still follows the max/min form (not remapped).
 *   int64_t __gj_min_i64    (alias)
 *   int64_t __gj_max_i64    (alias)
 *   int64_t __gj_clamp_i64  (alias)
 *   __libcgj_batch376_marker = "libcgj-batch376"
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch376_marker[] = "libcgj-batch376";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_min_i64 — return the lesser of two int64_t values.
 */
int64_t
gj_min_i64(int64_t i64A, int64_t i64B)
{
	if (i64A < i64B) {
		return i64A;
	}
	return i64B;
}

/*
 * gj_max_i64 — return the greater of two int64_t values.
 */
int64_t
gj_max_i64(int64_t i64A, int64_t i64B)
{
	if (i64A > i64B) {
		return i64A;
	}
	return i64B;
}

/*
 * gj_clamp_i64 — confine x to the closed interval [lo, hi].
 *
 * Equivalent to gj_max_i64(lo, gj_min_i64(hi, x)). Branch form avoids
 * nested calls so the TU stays self-contained and optimizable.
 */
int64_t
gj_clamp_i64(int64_t i64X, int64_t i64Lo, int64_t i64Hi)
{
	if (i64X < i64Lo) {
		return i64Lo;
	}
	if (i64X > i64Hi) {
		return i64Hi;
	}
	return i64X;
}

/* ---- underscored aliases ----------------------------------------------- */

int64_t __gj_min_i64(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_min_i64")));

int64_t __gj_max_i64(int64_t i64A, int64_t i64B)
    __attribute__((alias("gj_max_i64")));

int64_t __gj_clamp_i64(int64_t i64X, int64_t i64Lo, int64_t i64Hi)
    __attribute__((alias("gj_clamp_i64")));
