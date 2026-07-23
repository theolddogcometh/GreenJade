/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2608: int64_t closed-interval clamp (exclusive _u).
 *
 * Surface (unique symbols):
 *   int64_t gj_i64_clamp_u(int64_t x, int64_t lo, int64_t hi);
 *     - Confine x to the closed interval between lo and hi.
 *       If lo > hi, bounds are swapped so the effective range is
 *       always [min(lo,hi), max(lo,hi)].
 *   int64_t __gj_i64_clamp_u  (alias)
 *   __libcgj_batch2608_marker = "libcgj-batch2608"
 *
 * Distinct from gj_i64_clamp (batch645) and gj_clamp_i64 (batch376) —
 * exclusive _u name surface only; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2608_marker[] = "libcgj-batch2608";

/* ---- freestanding helpers ---------------------------------------------- */

static int64_t
b2608_clamp(int64_t i64X, int64_t i64Lo, int64_t i64Hi)
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

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_clamp_u — confine x to the closed interval between lo and hi.
 *
 * x:  value to clamp
 * lo: lower bound (may be greater than hi; swapped)
 * hi: upper bound
 *
 * Returns ordered low if x is below it, ordered high if above it,
 * otherwise x. Order of lo/hi does not matter.
 */
int64_t
gj_i64_clamp_u(int64_t i64X, int64_t i64Lo, int64_t i64Hi)
{
	(void)NULL;
	return b2608_clamp(i64X, i64Lo, i64Hi);
}

/* ---- underscored alias ------------------------------------------------- */

int64_t __gj_i64_clamp_u(int64_t i64X, int64_t i64Lo, int64_t i64Hi)
    __attribute__((alias("gj_i64_clamp_u")));
