/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4216: uint64_t closed-interval clamp (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_clamp_range_u(uint64_t x, uint64_t lo, uint64_t hi);
 *     - Confine x to the closed interval [lo, hi]. If lo > hi, the
 *       bounds are swapped so the effective range is always
 *       [min(lo,hi), max(lo,hi)].
 *   uint64_t __gj_u64_clamp_range_u  (alias)
 *   __libcgj_batch4216_marker = "libcgj-batch4216"
 *
 * Exclusive continuum CREATE-ONLY (4211-4220: u32_min3_u, u32_max3_u,
 * u64_min3_u, u64_max3_u, u32_clamp_range_u, u64_clamp_range_u,
 * i32_clamp_range_s, i32_abs_s, i32_sign_s, batch_id_4220).
 * Distinct surface in this wave; no parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4216_marker[] = "libcgj-batch4216";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4216_min(uint64_t uA, uint64_t uB)
{
	return (uA < uB) ? uA : uB;
}

static uint64_t
b4216_max(uint64_t uA, uint64_t uB)
{
	return (uA > uB) ? uA : uB;
}

/*
 * Clamp x into [lo, hi], swapping lo/hi when inverted.
 */
static uint64_t
b4216_clamp_range(uint64_t uX, uint64_t uLo, uint64_t uHi)
{
	uint64_t uLow;
	uint64_t uHigh;

	uLow = b4216_min(uLo, uHi);
	uHigh = b4216_max(uLo, uHi);

	if (uX < uLow) {
		return uLow;
	}
	if (uX > uHigh) {
		return uHigh;
	}
	return uX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_clamp_range_u - confine x to the closed interval [lo, hi].
 *
 * x: value to clamp
 * lo, hi: bounds (may be inverted; normalized before clamp)
 *
 * Returns lo' if x < lo', hi' if x > hi', else x, where
 * lo' = min(lo,hi) and hi' = max(lo,hi). Does not call libc.
 */
uint64_t
gj_u64_clamp_range_u(uint64_t uX, uint64_t uLo, uint64_t uHi)
{
	(void)NULL;
	return b4216_clamp_range(uX, uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_clamp_range_u(uint64_t uX, uint64_t uLo, uint64_t uHi)
    __attribute__((alias("gj_u64_clamp_range_u")));
