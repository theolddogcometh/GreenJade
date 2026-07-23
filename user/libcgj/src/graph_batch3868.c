/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3868: saturating int64_t to uint64_t (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_i64_to_u64_sat_u(int64_t x);
 *     - Map signed x into the uint64_t domain with saturation:
 *       negative x clamps to 0; non-negative x returns (uint64_t)x.
 *   uint64_t __gj_i64_to_u64_sat_u  (alias)
 *   __libcgj_batch3868_marker = "libcgj-batch3868"
 *
 * Exclusive continuum CREATE-ONLY (3861-3870). Unique
 * gj_i64_to_u64_sat_u surface only; no multi-def. Distinct from
 * gj_i64_clamped_to_u64 (batch1739) — different public name.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3868_marker[] = "libcgj-batch3868";

/* ---- freestanding helpers ---------------------------------------------- */

/* True iff x < 0. */
static int
b3868_is_neg(int64_t i64X)
{
	return (i64X < 0) ? 1 : 0;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_i64_to_u64_sat_u - map i64 into [0, INT64_MAX] as uint64_t.
 *
 * Negatives become 0; non-negatives cast without change of magnitude.
 * No parent wires.
 */
uint64_t
gj_i64_to_u64_sat_u(int64_t i64X)
{
	(void)NULL;
	if (b3868_is_neg(i64X) != 0) {
		return 0ull;
	}
	return (uint64_t)i64X;
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_i64_to_u64_sat_u(int64_t i64X)
    __attribute__((alias("gj_i64_to_u64_sat_u")));
