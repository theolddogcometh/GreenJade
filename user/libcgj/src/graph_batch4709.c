/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4709: uint32_t absolute difference (abs_diff3).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_abs_diff3_u(uint32_t a, uint32_t b);
 *     - |a - b| as uint32_t (always non-negative; no wrap).
 *   uint32_t __gj_u32_abs_diff3_u  (alias)
 *   __libcgj_batch4709_marker = "libcgj-batch4709"
 *
 * Exclusive continuum CREATE-ONLY (4701-4710). Distinct from
 * gj_u32_abs_diff_u (batch3451), gj_u32_absdiff_u (batch3251), and
 * gj_u32_absdiff_u2 (batch4105) — unique gj_u32_abs_diff3_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4709_marker[] = "libcgj-batch4709";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4709_abs_diff3(uint32_t u32A, uint32_t u32B)
{
	if (u32A >= u32B) {
		return u32A - u32B;
	}
	return u32B - u32A;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_abs_diff3_u - absolute difference of two uint32_t values.
 *
 * a, b: operands
 *
 * Returns |a - b| without signed overflow or wraparound.
 * Wave-unique name (abs_diff3) so it coexists with abs_diff_u /
 * absdiff_u / absdiff_u2. Self-contained; no parent wires.
 */
uint32_t
gj_u32_abs_diff3_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4709_abs_diff3(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_abs_diff3_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_u32_abs_diff3_u")));
