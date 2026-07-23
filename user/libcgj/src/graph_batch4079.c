/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4079: Q16.16 fractional part extract (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_fract_q16_u(uint32_t x);
 *     - Extract the fractional field of an unsigned Q16.16 fixed-point
 *       value. Returns the low 16 bits of x (x & 0xFFFF), i.e. the
 *       fractional part still in Q16 units on [0, 0xFFFF].
 *   uint32_t __gj_u32_fract_q16_u  (alias)
 *   __libcgj_batch4079_marker = "libcgj-batch4079"
 *
 * Exclusive continuum CREATE-ONLY wave (4071-4080). Unique
 * gj_u32_fract_q16_u surface only; no multi-def. Distinct from
 * gj_u32_lerp_q16 (batch1478) and gj_q16_* (batch172). No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4079_marker[] = "libcgj-batch4079";

/* Q16.16 fractional mask (low 16 bits). */
#define B4079_Q16_FRAC_MASK  ((uint32_t)0x0000FFFFu)

/* ---- freestanding helpers ---------------------------------------------- */

/* Fractional field of unsigned Q16.16. */
static uint32_t
b4079_fract_q16(uint32_t u32X)
{
	return u32X & B4079_Q16_FRAC_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_fract_q16_u - extract Q16.16 fractional bits of x.
 *
 * x: unsigned Q16.16 fixed-point value (integer in high 16, frac in low 16)
 *
 * Returns x & 0xFFFF (fractional part in Q16 units). No parent wires.
 */
uint32_t
gj_u32_fract_q16_u(uint32_t u32X)
{
	(void)NULL;
	return b4079_fract_q16(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_fract_q16_u(uint32_t u32X)
    __attribute__((alias("gj_u32_fract_q16_u")));
