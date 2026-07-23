/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4156: floor Q16.16 to uint32 integer (_q16x).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16x_to_u32_floor_u(uint32_t x);
 *     - Floor of unsigned Q16.16 x: x >> 16.
 *   uint32_t __gj_q16x_to_u32_floor_u  (alias)
 *   __libcgj_batch4156_marker = "libcgj-batch4156"
 *
 * Exclusive continuum CREATE-ONLY (4151-4160). Distinct from
 * gj_u32_floor_q16_u (batch4082) and gj_q16_to_int (batch172) —
 * unique gj_q16x_to_u32_floor_u surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4156_marker[] = "libcgj-batch4156";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4156_floor — integer part of unsigned Q16.16 via >> 16.
 */
static uint32_t
b4156_floor(uint32_t u32X)
{
	return u32X >> 16;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16x_to_u32_floor_u - floor unsigned Q16.16 to uint32 integer.
 *
 * x: unsigned Q16.16 value (1.0 == 0x10000)
 *
 * Returns x >> 16. No parent wires.
 */
uint32_t
gj_q16x_to_u32_floor_u(uint32_t u32X)
{
	(void)NULL;
	return b4156_floor(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16x_to_u32_floor_u(uint32_t u32X)
    __attribute__((alias("gj_q16x_to_u32_floor_u")));
