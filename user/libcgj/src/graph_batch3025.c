/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3025: saturating uint64_t increment (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_inc_sat_u(uint64_t x);
 *     - Return x + 1, saturating at UINT64_MAX (no wrap).
 *       gj_u64_inc_sat_u(UINT64_MAX) == UINT64_MAX.
 *       gj_u64_inc_sat_u(0) == 1.
 *   uint64_t __gj_u64_inc_sat_u  (alias)
 *   __libcgj_batch3025_marker = "libcgj-batch3025"
 *
 * Exclusive continuum CREATE-ONLY wave (3021-3030). Unique
 * gj_u64_inc_sat_u surface only; no multi-def. Distinct from
 * gj_u64_inc_sat (batch649) and gj_u64_clamped_inc (batch927) — different
 * public name. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3025_marker[] = "libcgj-batch3025";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating increment at UINT64_MAX. */
static uint64_t
b3025_inc_sat(uint64_t u64X)
{
	if (u64X == UINT64_MAX) {
		return UINT64_MAX;
	}
	return u64X + 1ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_inc_sat_u - increment x with saturation at UINT64_MAX.
 *
 * If x is already UINT64_MAX, return UINT64_MAX (no wrap-around).
 * Otherwise return x + 1. No parent wires.
 */
uint64_t
gj_u64_inc_sat_u(uint64_t u64X)
{
	(void)NULL;
	return b3025_inc_sat(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_inc_sat_u(uint64_t u64X)
    __attribute__((alias("gj_u64_inc_sat_u")));
