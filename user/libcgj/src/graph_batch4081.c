/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4081: unsigned Q48.16 fractional part (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_fract_q16_u(uint64_t q);
 *     - Fractional part of unsigned Q48.16 value q, as Q16.16 in
 *       [0, 1.0): q - floor(q) == (q & 0xFFFF). Result is still a
 *       Q16-scaled fixed-point quantity (1.0 == 0x10000 would be unit,
 *       so the returned range is [0, 0xFFFF]).
 *   uint64_t __gj_u64_fract_q16_u  (alias)
 *   __libcgj_batch4081_marker = "libcgj-batch4081"
 *
 * Exclusive continuum CREATE-ONLY (4081-4090). Distinct from
 * gj_q16_to_u32_u (batch3063) and gj_u64_frac_bits_u (batch4089) —
 * unique gj_u64_fract_q16_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4081_marker[] = "libcgj-batch4081";

/* Low 16 bits of unsigned Q48.16 = fractional field. */
#define B4081_FRAC_MASK  0xFFFFull

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * fract(q) = q - floor(q) for unsigned Q48.16: mask low 16 bits.
 */
static uint64_t
b4081_fract(uint64_t u64Q)
{
	return u64Q & B4081_FRAC_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_fract_q16_u - fractional part of unsigned Q48.16 q.
 *
 * q: unsigned Q48.16 (bits 63..16 integer, 15..0 fraction; 1.0==0x10000)
 * Returns Q16.16 fraction in [0, 1.0). No parent wires.
 */
uint64_t
gj_u64_fract_q16_u(uint64_t u64Q)
{
	(void)NULL;
	return b4081_fract(u64Q);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_fract_q16_u(uint64_t u64Q)
    __attribute__((alias("gj_u64_fract_q16_u")));
