/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4706: saturating uint64_t increment.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sat_inc_u(uint64_t x);
 *     - x + 1 with saturation at UINT64_MAX (no wrap).
 *   uint64_t __gj_u64_sat_inc_u  (alias)
 *   __libcgj_batch4706_marker = "libcgj-batch4706"
 *
 * Exclusive continuum CREATE-ONLY (4701-4710). Distinct from
 * gj_u32_sat_inc_u (batch4704) and gj_u64_sat_add (batch921) —
 * unique gj_u64_sat_inc_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4706_marker[] = "libcgj-batch4706";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating increment: clamp at UINT64_MAX. */
static uint64_t
b4706_sat_inc(uint64_t u64X)
{
	if (u64X == UINT64_MAX) {
		return UINT64_MAX;
	}
	return u64X + 1ULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sat_inc_u - unsigned 64-bit saturating increment.
 *
 * x: value to increment
 *
 * Returns x + 1, or UINT64_MAX when x is already UINT64_MAX.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_sat_inc_u(uint64_t u64X)
{
	(void)NULL;
	return b4706_sat_inc(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sat_inc_u(uint64_t u64X)
    __attribute__((alias("gj_u64_sat_inc_u")));
