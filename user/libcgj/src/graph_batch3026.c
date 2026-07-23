/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3026: saturating uint64_t decrement (u-suffix).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_dec_sat_u(uint64_t x);
 *     - Return x - 1, saturating at 0 (no wrap).
 *       gj_u64_dec_sat_u(0) == 0; otherwise returns x - 1.
 *   uint64_t __gj_u64_dec_sat_u  (alias)
 *   __libcgj_batch3026_marker = "libcgj-batch3026"
 *
 * Exclusive continuum CREATE-ONLY wave (3021-3030). Unique
 * gj_u64_dec_sat_u surface only; no multi-def. Distinct from
 * gj_u64_dec_sat (batch656) and gj_u64_clamped_dec (batch928) — different
 * public name. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3026_marker[] = "libcgj-batch3026";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating decrement at 0. */
static uint64_t
b3026_dec_sat(uint64_t u64X)
{
	if (u64X == 0ull) {
		return 0ull;
	}
	return u64X - 1ull;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_dec_sat_u - decrement x with saturation at 0.
 *
 * If x is already 0, return 0 (no wrap-around). Otherwise return x - 1.
 * No parent wires.
 */
uint64_t
gj_u64_dec_sat_u(uint64_t u64X)
{
	(void)NULL;
	return b3026_dec_sat(u64X);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_dec_sat_u(uint64_t u64X)
    __attribute__((alias("gj_u64_dec_sat_u")));
