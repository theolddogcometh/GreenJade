/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1371: uint64_t closed-interval clamp (range form).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_clamp_range(uint64_t x, uint64_t lo, uint64_t hi);
 *     — Confine x to the closed interval [lo, hi]. If lo > hi, the
 *       bounds are swapped so the effective range is always
 *       [min(lo,hi), max(lo,hi)].
 *   uint64_t __gj_u64_clamp_range  (alias)
 *   __libcgj_batch1371_marker = "libcgj-batch1371"
 *
 * Distinct from gj_clamp_u64 (batch587) and gj_clamp_i64 (batch376).
 * Unique surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch1371_marker[] = "libcgj-batch1371";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b1371_min_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A < u64B) ? u64A : u64B;
}

static uint64_t
b1371_max_u64(uint64_t u64A, uint64_t u64B)
{
	return (u64A > u64B) ? u64A : u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_clamp_range — confine x to the closed interval [lo, hi].
 *
 * lo/hi may be inverted; they are normalized before clamping.
 */
uint64_t
gj_u64_clamp_range(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
{
	uint64_t u64Low;
	uint64_t u64High;

	u64Low = b1371_min_u64(u64Lo, u64Hi);
	u64High = b1371_max_u64(u64Lo, u64Hi);

	if (u64X < u64Low) {
		return u64Low;
	}
	if (u64X > u64High) {
		return u64High;
	}
	return u64X;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_clamp_range(uint64_t u64X, uint64_t u64Lo, uint64_t u64Hi)
    __attribute__((alias("gj_u64_clamp_range")));
