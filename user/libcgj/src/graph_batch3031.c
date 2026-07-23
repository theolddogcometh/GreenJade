/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3031: saturating double of uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_twice_sat_u(uint64_t x);
 *     - Return 2*x, saturating at UINT64_MAX on overflow (no wrap).
 *   uint64_t __gj_u64_twice_sat_u  (alias)
 *   __libcgj_batch3031_marker = "libcgj-batch3031"
 *
 * CREATE-ONLY exclusive continuum (3031-3040). Unique gj_u64_twice_sat_u
 * surface only; no multi-def. Distinct from gj_u64_mul_sat_soft /
 * gj_u64_add_sat. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3031_marker[] = "libcgj-batch3031";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating 2*x: overflow iff high bit of x is set. */
static uint64_t
b3031_twice_sat(uint64_t u64X)
{
	if (u64X > (UINT64_MAX / 2ull)) {
		return UINT64_MAX;
	}
	return u64X << 1;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_twice_sat_u - saturating double of a uint64_t.
 *
 * x: value to double
 *
 * Returns min(2*x, UINT64_MAX). Overflow when x > UINT64_MAX/2.
 */
uint64_t
gj_u64_twice_sat_u(uint64_t u64X)
{
	(void)NULL;
	return b3031_twice_sat(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_twice_sat_u(uint64_t u64X)
    __attribute__((alias("gj_u64_twice_sat_u")));
