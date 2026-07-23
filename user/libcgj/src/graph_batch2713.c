/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2713: saturating uint64_t multiply (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sat_mul_u(uint64_t a, uint64_t b);
 *     - a * b with saturation at UINT64_MAX on overflow (no wrap).
 *       Software path only (no wide mul / __int128 / builtins).
 *   uint64_t __gj_u64_sat_mul_u  (alias)
 *   __libcgj_batch2713_marker = "libcgj-batch2713"
 *
 * Post-2710 continuum exclusive wave (2711-2720). Distinct from
 * gj_sat_mul_u64 (batch567), gj_u64_saturating_mul (batch1374), and
 * gj_u64_mul_sat_soft (batch2033) — different public name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2713_marker[] = "libcgj-batch2713";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Saturating multiply: overflow iff a > floor(UINT64_MAX / b)
 * when both factors are non-zero.
 */
static uint64_t
b2713_sat_mul(uint64_t u64A, uint64_t u64B)
{
	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}
	if (u64A > (UINT64_MAX / u64B)) {
		return UINT64_MAX;
	}
	return u64A * u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sat_mul_u - unsigned 64-bit multiply; clamp to UINT64_MAX.
 *
 * a, b: factors
 *
 * Zero factor never overflows. No __builtin_mul_overflow / wide types.
 */
uint64_t
gj_u64_sat_mul_u(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2713_sat_mul(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sat_mul_u(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_sat_mul_u")));
