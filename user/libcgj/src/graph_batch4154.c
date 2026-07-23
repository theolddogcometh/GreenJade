/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4154: saturating unsigned Q16.16 sub (_q16x).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16x_sub_sat_u(uint32_t a, uint32_t b);
 *     - a - b as uint32_t with saturation at 0 (no wrap under zero).
 *   uint32_t __gj_q16x_sub_sat_u  (alias)
 *   __libcgj_batch4154_marker = "libcgj-batch4154"
 *
 * Exclusive continuum CREATE-ONLY (4151-4160). Distinct from
 * gj_i64_sub_sat_u (batch2042) — unique gj_q16x_sub_sat_u surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4154_marker[] = "libcgj-batch4154";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4154_sub_sat — saturating uint32 sub: clamp to 0 on underflow.
 */
static uint32_t
b4154_sub_sat(uint32_t u32A, uint32_t u32B)
{
	if (u32A < u32B) {
		return 0u;
	}
	return u32A - u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16x_sub_sat_u - saturating unsigned Q16.16 / u32 subtract.
 *
 * a, b: operands (Q16.16 bit patterns or plain u32)
 *
 * Returns a - b, or 0 if a < b. No parent wires.
 */
uint32_t
gj_q16x_sub_sat_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4154_sub_sat(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16x_sub_sat_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_q16x_sub_sat_u")));
