/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4089: extract Q48.16 fraction bits (u64) (_u).
 *
 * Surface (unique symbols):
 *   uint64_t gj_u64_frac_bits_u(uint64_t q);
 *     - Return the low 16 fractional bits of an unsigned Q48.16 value
 *       (q & 0xFFFF). Raw bitfield extract companion to
 *       gj_u64_fract_q16_u (batch4081).
 *   uint64_t __gj_u64_frac_bits_u  (alias)
 *   __libcgj_batch4089_marker = "libcgj-batch4089"
 *
 * Exclusive continuum CREATE-ONLY (4081-4090). Distinct from
 * gj_u64_fract_q16_u (batch4081) / gj_u32_frac_bits_u (batch4088) —
 * unique gj_u64_frac_bits_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4089_marker[] = "libcgj-batch4089";

/* Low 16 bits of unsigned Q48.16 = fractional bitfield. */
#define B4089_FRAC_MASK  0xFFFFull

/* ---- freestanding helpers ---------------------------------------------- */

static uint64_t
b4089_frac_bits(uint64_t u64Q)
{
	return u64Q & B4089_FRAC_MASK;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u64_frac_bits_u - extract fractional bits of unsigned Q48.16 q.
 *
 * q: unsigned Q48.16 (bits 63..16 integer, 15..0 fraction)
 * Returns bits 15..0 in [0, 0xFFFF]. No parent wires.
 */
uint64_t
gj_u64_frac_bits_u(uint64_t u64Q)
{
	(void)NULL;
	return b4089_frac_bits(u64Q);
}

/* ---- underscored alias ------------------------------------------------- */

uint64_t __gj_u64_frac_bits_u(uint64_t u64Q)
    __attribute__((alias("gj_u64_frac_bits_u")));
