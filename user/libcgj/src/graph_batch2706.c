/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2706: absolute magnitude of int64_t as uint64_t.
 *
 * Surface (unique symbols):
 *   uint64_t gj_i64_abs_mag_u(int64_t x);
 *     - Magnitude of x as an unsigned 64-bit value.
 *       Non-negative x maps to (uint64_t)x.
 *       Negative x maps to the two's-complement magnitude
 *       0u - (uint64_t)x, which for INT64_MIN yields 1ull << 63
 *       (0x8000000000000000), the only representable abs of that
 *       value in uint64_t.
 *   uint64_t __gj_i64_abs_mag_u  (alias)
 *   __libcgj_batch2706_marker = "libcgj-batch2706"
 *
 * Post-2700 product/Steam integer helpers exclusive wave (2701-2710).
 * Distinct from gj_i64_abs_u (batch613), gj_i64_abs_sat_u (batch2045),
 * and gj_abs_i64 (batch378) — unique mag_u surface only; no multi-def.
 * Avoids signed negation of INT64_MIN (undefined behaviour).
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2706_marker[] = "libcgj-batch2706";

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b2706_abs_mag(int64_t i64X)
{
	uint64_t u64X;

	u64X = (uint64_t)i64X;
	if (i64X < 0) {
		return 0u - u64X;
	}
	return u64X;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_abs_mag_u — |x| as uint64_t; INT64_MIN -> 1ull << 63.
 *
 * Unsigned wrap for the negative branch is defined and yields the
 * correct magnitude for every int64_t, including INT64_MIN.
 */
uint64_t
gj_i64_abs_mag_u(int64_t i64X)
{
	(void)NULL;
	return b2706_abs_mag(i64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_i64_abs_mag_u(int64_t i64X)
    __attribute__((alias("gj_i64_abs_mag_u")));
