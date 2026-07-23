/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3865: saturating uint64_t multiply-subtract (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_msub_sat_u(uint64_t a, uint64_t b, uint64_t c);
 *     - Return sat(a * b - c): multiply-subtract with saturation.
 *       Product overflow clamps to UINT64_MAX; underflow of the
 *       subtract clamps to 0. Soft path only (no __int128).
 *       Implemented as sat_sub(sat_mul(a,b), c).
 *   uint64_t __gj_u64_msub_sat_u  (alias)
 *   __libcgj_batch3865_marker = "libcgj-batch3865"
 *
 * Exclusive continuum CREATE-ONLY (3861-3870). Unique
 * gj_u64_msub_sat_u surface only; no multi-def. Distinct from
 * gj_u64_mul_sub_u (batch3861 wrap). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3865_marker[] = "libcgj-batch3865";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft saturating multiply: overflow iff a > floor(UINT64_MAX / b)
 * when both factors are non-zero.
 */
static uint64_t
b3865_mul_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}
	if (u64A > (UINT64_MAX / u64B)) {
		return UINT64_MAX;
	}
	return u64A * u64B;
}

/* Saturating a - b in the uint64_t domain (underflow -> 0). */
static uint64_t
b3865_sub_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64A < u64B) {
		return 0ull;
	}
	return u64A - u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_msub_sat_u - saturating multiply-subtract: sat(sat(a*b) - c).
 *
 * a, b: factors
 * c:    subtrahend
 *
 * Product overflow clamps to UINT64_MAX; if product < c, result is 0.
 * No __int128 / builtins. No parent wires.
 */
uint64_t
gj_u64_msub_sat_u(uint64_t u64A, uint64_t u64B, uint64_t u64C)
{
	(void)NULL;
	return b3865_sub_sat(b3865_mul_sat(u64A, u64B), u64C);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_msub_sat_u(uint64_t u64A, uint64_t u64B, uint64_t u64C)
    __attribute__((alias("gj_u64_msub_sat_u")));
