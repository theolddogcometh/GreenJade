/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4215: uint32_t closed-interval clamp (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_clamp_range_u(uint32_t x, uint32_t lo, uint32_t hi);
 *     - Confine x to the closed interval [lo, hi]. If lo > hi, the
 *       bounds are swapped so the effective range is always
 *       [min(lo,hi), max(lo,hi)].
 *   uint32_t __gj_u32_clamp_range_u  (alias)
 *   __libcgj_batch4215_marker = "libcgj-batch4215"
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

const char __libcgj_batch4215_marker[] = "libcgj-batch4215";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4215_min(uint32_t uA, uint32_t uB)
{
	return (uA < uB) ? uA : uB;
}

static uint32_t
b4215_max(uint32_t uA, uint32_t uB)
{
	return (uA > uB) ? uA : uB;
}

/*
 * Clamp x into [lo, hi], swapping lo/hi when inverted.
 */
static uint32_t
b4215_clamp_range(uint32_t uX, uint32_t uLo, uint32_t uHi)
{
	uint32_t uLow;
	uint32_t uHigh;

	uLow = b4215_min(uLo, uHi);
	uHigh = b4215_max(uLo, uHi);

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
 * gj_u32_clamp_range_u - confine x to the closed interval [lo, hi].
 *
 * x: value to clamp
 * lo, hi: bounds (may be inverted; normalized before clamp)
 *
 * Returns lo' if x < lo', hi' if x > hi', else x, where
 * lo' = min(lo,hi) and hi' = max(lo,hi). Does not call libc.
 */
uint32_t
gj_u32_clamp_range_u(uint32_t uX, uint32_t uLo, uint32_t uHi)
{
	(void)NULL;
	return b4215_clamp_range(uX, uLo, uHi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_clamp_range_u(uint32_t uX, uint32_t uLo, uint32_t uHi)
    __attribute__((alias("gj_u32_clamp_range_u")));
