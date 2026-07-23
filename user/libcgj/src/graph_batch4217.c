/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4217: int32_t closed-interval clamp (_s).
 *
 * Surface (unique symbols):
 *   int32_t gj_i32_clamp_range_s(int32_t x, int32_t lo, int32_t hi);
 *     - Confine x to the closed interval [lo, hi]. If lo > hi, the
 *       bounds are swapped so the effective range is always
 *       [min(lo,hi), max(lo,hi)]. Signed two's-complement compare.
 *   int32_t __gj_i32_clamp_range_s  (alias)
 *   __libcgj_batch4217_marker = "libcgj-batch4217"
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

const char __libcgj_batch4217_marker[] = "libcgj-batch4217";

/* ---- freestanding helpers ---------------------------------------------- */

static int32_t
b4217_min(int32_t iA, int32_t iB)
{
	return (iA < iB) ? iA : iB;
}

static int32_t
b4217_max(int32_t iA, int32_t iB)
{
	return (iA > iB) ? iA : iB;
}

/*
 * Clamp x into [lo, hi], swapping lo/hi when inverted.
 */
static int32_t
b4217_clamp_range(int32_t iX, int32_t iLo, int32_t iHi)
{
	int32_t iLow;
	int32_t iHigh;

	iLow = b4217_min(iLo, iHi);
	iHigh = b4217_max(iLo, iHi);

	if (iX < iLow) {
		return iLow;
	}
	if (iX > iHigh) {
		return iHigh;
	}
	return iX;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_clamp_range_s - confine x to the closed interval [lo, hi].
 *
 * x: value to clamp
 * lo, hi: bounds (may be inverted; normalized before clamp)
 *
 * Returns lo' if x < lo', hi' if x > hi', else x, where
 * lo' = min(lo,hi) and hi' = max(lo,hi). Full int32_t range safe.
 * Does not call libc.
 */
int32_t
gj_i32_clamp_range_s(int32_t iX, int32_t iLo, int32_t iHi)
{
	(void)NULL;
	return b4217_clamp_range(iX, iLo, iHi);
}

/* ---- underscored alias ------------------------------------------------- */

int32_t __gj_i32_clamp_range_s(int32_t iX, int32_t iLo, int32_t iHi)
    __attribute__((alias("gj_i32_clamp_range_s")));
