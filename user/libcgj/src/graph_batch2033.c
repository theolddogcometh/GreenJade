/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2033: soft saturating uint64_t multiply.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_mul_sat_soft(uint64_t a, uint64_t b);
 *     — a * b with saturation at UINT64_MAX on overflow (no wrap).
 *       Software path only (no compiler builtins / wide mul).
 *   uint64_t __gj_u64_mul_sat_soft  (alias)
 *   __libcgj_batch2033_marker = "libcgj-batch2033"
 *
 * Post-2030 u64 arith exclusive wave (2031–2040). Distinct from
 * gj_sat_mul_u64 (batch567) and gj_u64_saturating_mul (batch1374) —
 * different public name; no multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2033_marker[] = "libcgj-batch2033";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft saturating multiply: overflow iff a > floor(UINT64_MAX / b)
 * when both factors are non-zero.
 */
static uint64_t
b2033_mul_sat(uint64_t u64A, uint64_t u64B)
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
 * gj_u64_mul_sat_soft — unsigned multiply; clamp to UINT64_MAX on overflow.
 *
 * a, b: factors
 *
 * Zero factor never overflows. No __builtin_mul_overflow / wide types.
 */
uint64_t
gj_u64_mul_sat_soft(uint64_t u64A, uint64_t u64B)
{
	(void)NULL;
	return b2033_mul_sat(u64A, u64B);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_mul_sat_soft(uint64_t u64A, uint64_t u64B)
    __attribute__((alias("gj_u64_mul_sat_soft")));
