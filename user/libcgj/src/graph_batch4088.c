/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4088: extract Q16.16 fraction bits (u32) (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_frac_bits_u(uint32_t q);
 *     - Return the low 16 fractional bits of an unsigned Q16.16 value
 *       (q & 0xFFFF). Distinct surface from mathematical fract that is
 *       documented as a Q16 quantity; this is the raw bitfield extract.
 *   uint32_t __gj_u32_frac_bits_u  (alias)
 *   __libcgj_batch4088_marker = "libcgj-batch4088"
 *
 * Exclusive continuum CREATE-ONLY (4081-4090). Distinct from
 * gj_u64_fract_q16_u (batch4081) / gj_u64_frac_bits_u (batch4089) —
 * unique gj_u32_frac_bits_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4088_marker[] = "libcgj-batch4088";

/* Low 16 bits of unsigned Q16.16 = fractional bitfield. */
#define B4088_FRAC_MASK  0xFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4088_frac_bits(uint32_t u32Q)
{
	return u32Q & B4088_FRAC_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_frac_bits_u - extract fractional bits of unsigned Q16.16 q.
 *
 * q: unsigned Q16.16 (bits 31..16 integer, 15..0 fraction)
 * Returns bits 15..0 in [0, 0xFFFF]. No parent wires.
 */
uint32_t
gj_u32_frac_bits_u(uint32_t u32Q)
{
	(void)NULL;
	return b4088_frac_bits(u32Q);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_frac_bits_u(uint32_t u32Q)
    __attribute__((alias("gj_u32_frac_bits_u")));
