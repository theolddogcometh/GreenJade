/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch587: uint64_t closed-interval clamp.
 *
 * Surface (unique symbols):
 *   uint64_t gj_clamp_u64(uint64_t x, uint64_t lo, uint64_t hi);
 *     — Confine x to the closed interval between lo and hi.
 *       If lo > hi, the bounds are swapped conceptually so the
 *       effective range is always [min(lo,hi), max(lo,hi)]:
 *         if x < lo → lo; elif x > hi → hi; else x
 *       (after any needed lo/hi reorder).
 *   uint64_t __gj_clamp_u64  (alias)
 *   __libcgj_batch587_marker = "libcgj-batch587"
 *
 * Avoid multi-def: gj_clamp_i64 / __gj_clamp_i64 live in graph_batch376.c;
 * this TU exports only the unsigned _u64 surface. Clean-room freestanding
 * pure C (integer only). Compiles with -ffreestanding -msse2 -Wall
 * -Wextra -Werror. No malloc, no errno, no libc. No third-party source
 * copied.
 */

#include <stdint.h>

const char __libcgj_batch587_marker[] = "libcgj-batch587";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_clamp_u64 — confine x to the closed interval between lo and hi.
 *
 * x:  value to clamp
 * lo: lower bound (may be greater than hi; swapped conceptually)
 * hi: upper bound
 *
 * Returns lo if x is below the ordered low bound, hi if above the ordered
 * high bound, otherwise x. Order of arguments does not matter: lo and hi
 * are normalized so the interval is never inverted.
 */
uint64_t
gj_clamp_u64(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	uint64_t u64Tmp;

	if (u64Lo > u64Hi) {
		u64Tmp = u64Lo;
		u64Lo = u64Hi;
		u64Hi = u64Tmp;
	}

	if (u64X < u64Lo) {
		return u64Lo;
	}
	if (u64X > u64Hi) {
		return u64Hi;
	}
	return u64X;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_clamp_u64(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
    __attribute__((alias("gj_clamp_u64")));
