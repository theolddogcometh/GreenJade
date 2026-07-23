/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4082: floor unsigned Q16.16 → integer (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_floor_q16_u(uint32_t q);
 *     - Floor of unsigned Q16.16 q as a plain integer (q >> 16).
 *       Equivalent to discard-fraction for non-negative fixed-point.
 *   uint32_t __gj_u32_floor_q16_u  (alias)
 *   __libcgj_batch4082_marker = "libcgj-batch4082"
 *
 * Exclusive continuum CREATE-ONLY (4081-4090). Distinct from
 * gj_q16_to_u32_u (batch3063) / gj_q16_to_int_floor (batch1474) —
 * unique gj_u32_floor_q16_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4082_marker[] = "libcgj-batch4082";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * floor(q) for unsigned Q16.16: logical right shift by 16.
 */
static uint32_t
b4082_floor(uint32_t u32Q)
{
	return u32Q >> 16;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_floor_q16_u - floor of unsigned Q16.16 q to integer.
 *
 * q: unsigned Q16.16 (bits 31..16 integer, 15..0 fraction; 1.0==0x10000)
 * Returns floor(q) in [0, 0xFFFF]. No parent wires.
 */
uint32_t
gj_u32_floor_q16_u(uint32_t u32Q)
{
	(void)NULL;
	return b4082_floor(u32Q);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_floor_q16_u(uint32_t u32Q)
    __attribute__((alias("gj_u32_floor_q16_u")));
