/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3867: saturating int32_t to uint32_t (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_i32_to_u32_sat_u(int32_t x);
 *     - Map signed x into the uint32_t domain with saturation:
 *       negative x clamps to 0; non-negative x returns (uint32_t)x.
 *   uint32_t __gj_i32_to_u32_sat_u  (alias)
 *   __libcgj_batch3867_marker = "libcgj-batch3867"
 *
 * Exclusive continuum CREATE-ONLY (3861-3870). Unique
 * gj_i32_to_u32_sat_u surface only; no multi-def. Distinct from
 * gj_i64_clamped_to_u64 (batch1739). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3867_marker[] = "libcgj-batch3867";

/* ---- freestanding helpers ---------------------------------------------- */

/* True iff x < 0. */
static int
b3867_is_neg(int32_t i32X)
{
	return (i32X < 0) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i32_to_u32_sat_u - map i32 into [0, INT32_MAX] as uint32_t.
 *
 * Negatives become 0; non-negatives cast without change of magnitude.
 * No parent wires.
 */
uint32_t
gj_i32_to_u32_sat_u(int32_t i32X)
{
	(void)NULL;
	if (b3867_is_neg(i32X) != 0) {
		return 0u;
	}
	return (uint32_t)i32X;
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_i32_to_u32_sat_u(int32_t i32X)
    __attribute__((alias("gj_i32_to_u32_sat_u")));
