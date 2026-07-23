/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2048: int64_t closed-interval clamp (range form).
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_clamp_range(int64_t x, int64_t lo, int64_t hi);
 *     - Confine x to the closed interval [lo, hi]. If lo > hi, the
 *       bounds are swapped so the effective range is always
 *       [min(lo,hi), max(lo,hi)].
 *   int64_t __gj_i64_clamp_range  (alias)
 *   __libcgj_batch2048_marker = "libcgj-batch2048"
 *
 * Post-2000 i64 arith exclusive wave (2041-2050). Distinct from
 * gj_i64_clamp (batch645) and gj_clamp_i64 (batch376) - unique
 * gj_i64_clamp_range surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2048_marker[] = "libcgj-batch2048";

/* ---- freestanding helpers ---------------------------------------------- */

static int64_t
b2048_min(int64_t i64A, int64_t i64B)
{
	return (i64A < i64B) ? i64A : i64B;
}

static int64_t
b2048_max(int64_t i64A, int64_t i64B)
{
	return (i64A > i64B) ? i64A : i64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_clamp_range - confine x to the closed interval [lo, hi].
 *
 * lo/hi may be inverted; they are normalized before clamping.
 */
int64_t
gj_i64_clamp_range(int64_t i64X, int64_t i64Lo, int64_t i64Hi)
{
	int64_t i64Low;
	int64_t i64High;

	(void)NULL;
	i64Low = b2048_min(i64Lo, i64Hi);
	i64High = b2048_max(i64Lo, i64Hi);

	if (i64X < i64Low) {
		return i64Low;
	}
	if (i64X > i64High) {
		return i64High;
	}
	return i64X;
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_clamp_range(int64_t i64X, int64_t i64Lo, int64_t i64Hi)
    __attribute__((alias("gj_i64_clamp_range")));
