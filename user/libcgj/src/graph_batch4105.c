/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4105: uint32_t absolute difference (wave rename).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_absdiff_u2(uint32_t a, uint32_t b);
 *     - |a - b| as uint32_t (always non-negative; no wrap).
 *       Renamed from gj_u32_absdiff_u (collision with batch3251).
 *   uint32_t __gj_u32_absdiff_u2  (alias)
 *   __libcgj_batch4105_marker = "libcgj-batch4105"
 *
 * Exclusive continuum CREATE-ONLY (4101-4110). Unique
 * gj_u32_absdiff_u2 surface only; no multi-def. Distinct from
 * gj_u32_absdiff (batch1378), gj_u32_absdiff_u (batch3251), and
 * gj_u32_abs_diff_u (batch3451). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4105_marker[] = "libcgj-batch4105";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4105_absdiff(uint32_t u32A, uint32_t u32B)
{
	if (u32A >= u32B) {
		return u32A - u32B;
	}
	return u32B - u32A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_absdiff_u2 - absolute difference of two uint32_t values.
 *
 * a, b: operands
 *
 * Returns |a - b| without signed overflow or wraparound.
 * Wave-unique rename: gj_u32_absdiff_u is owned by batch3251.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u32_absdiff_u2(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4105_absdiff(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_absdiff_u2(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_absdiff_u2")));
