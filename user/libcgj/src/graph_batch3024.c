/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3024: saturating uint32_t decrement (u-suffix).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_dec_sat_u(uint32_t x);
 *     - Return x - 1, saturating at 0 (no wrap).
 *       gj_u32_dec_sat_u(0) == 0; otherwise returns x - 1.
 *   uint32_t __gj_u32_dec_sat_u  (alias)
 *   __libcgj_batch3024_marker = "libcgj-batch3024"
 *
 * Exclusive continuum CREATE-ONLY wave (3021-3030). Unique
 * gj_u32_dec_sat_u surface only; no multi-def. Distinct from
 * gj_u64_dec_sat (batch656) and gj_u64_clamped_dec (batch928). No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3024_marker[] = "libcgj-batch3024";

/* ---- freestanding helpers ---------------------------------------------- */

/* Saturating decrement at 0. */
static uint32_t
b3024_dec_sat(uint32_t u32X)
{
	if (u32X == 0u) {
		return 0u;
	}
	return u32X - 1u;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_dec_sat_u - decrement x with saturation at 0.
 *
 * If x is already 0, return 0 (no wrap-around). Otherwise return x - 1.
 * No parent wires.
 */
uint32_t
gj_u32_dec_sat_u(uint32_t u32X)
{
	(void)NULL;
	return b3024_dec_sat(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_dec_sat_u(uint32_t u32X)
    __attribute__((alias("gj_u32_dec_sat_u")));
