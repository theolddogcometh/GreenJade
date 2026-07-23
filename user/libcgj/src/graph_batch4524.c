/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4524: freestanding u32 soft bit-compact via LUT.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_compact_bits_u2(uint32_t x);
 *     - Soft pext of even bits: gather even bits of x into the low 16
 *       bits of the result (bit 2i -> bit i). Odd bits of x are
 *       discarded. Implemented with a 16-entry nibble LUT that packs
 *       even bits of each nibble pair. Inverse of gj_u32_spread_bits_u
 *       for values produced by that expand.
 *       gj_u32_compact_bits_u2(0x55555555) == 0x0000ffff,
 *       gj_u32_compact_bits_u2(0x00000001) == 0x00000001,
 *       gj_u32_compact_bits_u2(0x00000005) == 0x00000003.
 *   uint32_t __gj_u32_compact_bits_u2  (alias)
 *   __libcgj_batch4524_marker = "libcgj-batch4524"
 *
 * Exclusive continuum CREATE-ONLY (4521-4530: table-lookup unique wave).
 * Table-lookup soft pext for even bits -> low 16. Distinct from
 * gj_u32_bit_deinterleave_u (batch3439) and gj_u32_pext_soft_u
 * (batch3421). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4524_marker[] = "libcgj-batch4524";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Pack even-position bits of a 4-bit nibble into the low 2 result bits:
 * nibble bit0 -> out bit0, nibble bit2 -> out bit1. Odd nibble bits
 * ignored. Index 0..15.
 *
 *   n: 0 1 2 3 4 5 6 7 8 9 A B C D E F
 *   r: 0 1 0 1 2 3 2 3 0 1 0 1 2 3 2 3
 */
static const uint8_t b4524_even_nibble[16] = {
	0x0u, 0x1u, 0x0u, 0x1u, 0x2u, 0x3u, 0x2u, 0x3u,
	0x0u, 0x1u, 0x0u, 0x1u, 0x2u, 0x3u, 0x2u, 0x3u
};

/*
 * Compact even bits of one byte into a nibble via two LUT probes.
 */
static uint8_t
b4524_compact_byte(uint8_t u8B)
{
	uint8_t u8Lo;
	uint8_t u8Hi;

	u8Lo = b4524_even_nibble[u8B & 0x0fu];
	u8Hi = b4524_even_nibble[(u8B >> 4) & 0x0fu];
	return (uint8_t)(u8Lo | (uint8_t)(u8Hi << 2));
}

/*
 * Compact even bits of a 32-bit word into the low 16 bits.
 */
static uint32_t
b4524_compact(uint32_t u32X)
{
	uint32_t u32R;

	u32R  = (uint32_t)b4524_compact_byte((uint8_t)(u32X & 0xffu));
	u32R |= (uint32_t)b4524_compact_byte((uint8_t)((u32X >> 8) & 0xffu)) << 4;
	u32R |= (uint32_t)b4524_compact_byte((uint8_t)((u32X >> 16) & 0xffu)) << 8;
	u32R |= (uint32_t)b4524_compact_byte((uint8_t)((u32X >> 24) & 0xffu)) << 12;
	return u32R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_compact_bits_u2 - gather even bits into the low 16 positions (LUT).
 *
 * x: source word (odd bits discarded)
 *
 * Returns compacted low-16 word. No parent wires.
 */
uint32_t
gj_u32_compact_bits_u2(uint32_t u32X)
{
	(void)NULL;
	return b4524_compact(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_compact_bits_u2(uint32_t u32X)
    __attribute__((alias("gj_u32_compact_bits_u2")));
