/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4707: saturating uint64_t decrement.
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_sat_dec_u(uint64_t x);
 *     - x - 1 with saturation at 0 (no wrap).
 *   uint64_t __gj_u64_sat_dec_u  (alias)
 *   __libcgj_batch4707_marker = "libcgj-batch4707"
 *
 * Exclusive continuum CREATE-ONLY (4701-4710). Distinct from
 * gj_u32_sat_dec_u (batch4705) and gj_u64_sat_sub (batch922) —
 * unique gj_u64_sat_dec_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4707_marker[] = "libcgj-batch4707";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating decrement: clamp at 0. */
static uint64_t
b4707_sat_dec(uint64_t u64X)
{
	if (u64X == 0ULL) {
		return 0ULL;
	}
	return u64X - 1ULL;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_sat_dec_u - unsigned 64-bit saturating decrement.
 *
 * x: value to decrement
 *
 * Returns x - 1, or 0 when x is already 0.
 * Self-contained; no parent wires.
 */
uint64_t
gj_u64_sat_dec_u(uint64_t u64X)
{
	(void)NULL;
	return b4707_sat_dec(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_sat_dec_u(uint64_t u64X)
    __attribute__((alias("gj_u64_sat_dec_u")));
