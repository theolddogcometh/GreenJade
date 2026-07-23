/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4153: saturating unsigned Q16.16 add (_q16x).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16x_add_sat_u(uint32_t a, uint32_t b);
 *     - a + b as uint32_t with saturation at UINT32_MAX (no wrap).
 *   uint32_t __gj_q16x_add_sat_u  (alias)
 *   __libcgj_batch4153_marker = "libcgj-batch4153"
 *
 * Exclusive continuum CREATE-ONLY (4151-4160). Distinct from
 * gj_i64_add_sat_u / prior sat add surfaces — unique gj_q16x_add_sat_u
 * surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4153_marker[] = "libcgj-batch4153";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4153_add_sat — saturating uint32 add: clamp to UINT32_MAX on overflow.
 */
static uint32_t
b4153_add_sat(uint32_t u32A, uint32_t u32B)
{
	if (u32A > (UINT32_MAX - u32B)) {
		return UINT32_MAX;
	}
	return u32A + u32B;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16x_add_sat_u - saturating unsigned Q16.16 / u32 add.
 *
 * a, b: operands (Q16.16 bit patterns or plain u32)
 *
 * Returns a + b, or UINT32_MAX if the sum would wrap. No parent wires.
 */
uint32_t
gj_q16x_add_sat_u(uint32_t u32A, uint32_t u32B)
{
	(void)NULL;
	return b4153_add_sat(u32A, u32B);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16x_add_sat_u(uint32_t u32A, uint32_t u32B)
    __attribute__((alias("gj_q16x_add_sat_u")));
