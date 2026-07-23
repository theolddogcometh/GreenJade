/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3863: saturating uint64_t multiply-add (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_madd_sat_u(uint64_t a, uint64_t b, uint64_t c);
 *     - Return sat(a * b + c): multiply-add with saturation at
 *       UINT64_MAX on overflow (no wrap). Soft path only (no __int128,
 *       no compiler builtins). Implemented as sat_add(sat_mul(a,b), c).
 *   uint64_t __gj_u64_madd_sat_u  (alias)
 *   __libcgj_batch3863_marker = "libcgj-batch3863"
 *
 * Exclusive continuum CREATE-ONLY (3861-3870). Unique
 * gj_u64_madd_sat_u surface only; no multi-def. Distinct from
 * gj_u64_mul_sat_soft (batch2033) and wrap mul surfaces.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3863_marker[] = "libcgj-batch3863";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Soft saturating multiply: overflow iff a > floor(UINT64_MAX / b)
 * when both factors are non-zero.
 */
static uint64_t
b3863_mul_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64A == 0ull || u64B == 0ull) {
		return 0ull;
	}
	if (u64A > (UINT64_MAX / u64B)) {
		return UINT64_MAX;
	}
	return u64A * u64B;
}

/* Saturating a + b in the uint64_t domain. */
static uint64_t
b3863_add_sat(uint64_t u64A, uint64_t u64B)
{
	if (u64B > (UINT64_MAX - u64A)) {
		return UINT64_MAX;
	}
	return u64A + u64B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_madd_sat_u - saturating multiply-add: sat(sat(a*b) + c).
 *
 * a, b: factors
 * c:    addend
 *
 * Product overflow or sum overflow clamps to UINT64_MAX.
 * No __int128 / builtins. No parent wires.
 */
uint64_t
gj_u64_madd_sat_u(uint64_t u64A, uint64_t u64B, uint64_t u64C)
{
	(void)NULL;
	return b3863_add_sat(b3863_mul_sat(u64A, u64B), u64C);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_madd_sat_u(uint64_t u64A, uint64_t u64B, uint64_t u64C)
    __attribute__((alias("gj_u64_madd_sat_u")));
