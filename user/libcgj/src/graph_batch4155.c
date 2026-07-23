/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4155: convert uint32 integer to Q16.16 (_q16x).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16x_from_u32_u(uint32_t x);
 *     - x << 16 when x < 0x10000; else 0xffffffff (saturate).
 *   uint32_t __gj_q16x_from_u32_u  (alias)
 *   __libcgj_batch4155_marker = "libcgj-batch4155"
 *
 * Exclusive continuum CREATE-ONLY (4151-4160). Distinct from
 * gj_q16_from_int (batch172) — unique gj_q16x_from_u32_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4155_marker[] = "libcgj-batch4155";

/* Max integer part that fits in unsigned Q16.16 (shift left by 16). */
#define B4155_MAX_INT  0x10000u

/* Saturated Q16.16 result when x does not fit. */
#define B4155_SAT      0xffffffffu

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4155_from_u32 — integer → Q16.16; saturate if x >= 0x10000.
 */
static uint32_t
b4155_from_u32(uint32_t u32X)
{
	if (u32X < B4155_MAX_INT) {
		return u32X << 16;
	}
	return B4155_SAT;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16x_from_u32_u - convert plain uint32 integer to Q16.16.
 *
 * x: non-negative integer to encode
 *
 * Returns x << 16 when x < 0x10000; otherwise 0xffffffff. No parent wires.
 */
uint32_t
gj_q16x_from_u32_u(uint32_t u32X)
{
	(void)NULL;
	return b4155_from_u32(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16x_from_u32_u(uint32_t u32X)
    __attribute__((alias("gj_q16x_from_u32_u")));
