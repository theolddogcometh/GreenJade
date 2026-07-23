/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4157: fractional field of Q16.16 (_q16x).
 *
 * Surface (unique symbols):
 *   uint32_t gj_q16x_frac_u(uint32_t x);
 *     - Fractional bits of unsigned Q16.16 x: x & 0xffff.
 *   uint32_t __gj_q16x_frac_u  (alias)
 *   __libcgj_batch4157_marker = "libcgj-batch4157"
 *
 * Exclusive continuum CREATE-ONLY (4151-4160). Distinct from
 * gj_u64_fract_q16_u (batch4081) and gj_u32_frac_bits_u (batch4088) —
 * unique gj_q16x_frac_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4157_marker[] = "libcgj-batch4157";

/* Low 16 bits of unsigned Q16.16 = fractional field. */
#define B4157_FRAC_MASK  0xffffu

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * b4157_frac — fractional part of unsigned Q16.16: mask low 16 bits.
 */
static uint32_t
b4157_frac(uint32_t u32X)
{
	return u32X & B4157_FRAC_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_q16x_frac_u - fractional bits of unsigned Q16.16 x.
 *
 * x: unsigned Q16.16 value (1.0 == 0x10000)
 *
 * Returns x & 0xffff (range [0, 0xffff]). No parent wires.
 */
uint32_t
gj_q16x_frac_u(uint32_t u32X)
{
	(void)NULL;
	return b4157_frac(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_q16x_frac_u(uint32_t u32X)
    __attribute__((alias("gj_q16x_frac_u")));
