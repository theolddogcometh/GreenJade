/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4523: freestanding u32 soft bit-spread via LUT.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_spread_bits_u2(uint32_t x);
 *     - Soft parallel bit deposit: spread the low 16 bits of x into the
 *       even bit positions of a 32-bit word (bit i -> bit 2i). High 16
 *       bits of x are ignored. Implemented with a 16-entry nibble LUT
 *       (each nibble expands to one byte of even-bit pattern).
 *       gj_u32_spread_bits_u2(0x0000ffff) == 0x55555555,
 *       gj_u32_spread_bits_u2(0x00000001) == 0x00000001,
 *       gj_u32_spread_bits_u2(0x00000003) == 0x00000005.
 *   uint32_t __gj_u32_spread_bits_u2  (alias)
 *   __libcgj_batch4523_marker = "libcgj-batch4523"
 *
 * Exclusive continuum CREATE-ONLY (4521-4530: table-lookup unique wave).
 * Table-lookup soft pdep for low-16 -> even bits. Inverse is
 * gj_u32_compact_bits_u. Distinct from gj_u32_bit_interleave_u
 * (batch3438, two-source zip) and gj_u32_pdep_soft_u (batch3418, mask
 * deposit). No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4523_marker[] = "libcgj-batch4523";

/* ---- freestanding helpers ---------------------------------------------- */

/*
 * Expand one nibble (4 bits) into one byte with bits in even positions:
 * input bit k -> output bit 2k. Pattern family 0x00..0x55.
 */
static const uint8_t b4523_spread_nibble[16] = {
	0x00u, /* 0000 -> 00000000 */
	0x01u, /* 0001 -> 00000001 */
	0x04u, /* 0010 -> 00000100 */
	0x05u, /* 0011 -> 00000101 */
	0x10u, /* 0100 -> 00010000 */
	0x11u, /* 0101 -> 00010001 */
	0x14u, /* 0110 -> 00010100 */
	0x15u, /* 0111 -> 00010101 */
	0x40u, /* 1000 -> 01000000 */
	0x41u, /* 1001 -> 01000001 */
	0x44u, /* 1010 -> 01000100 */
	0x45u, /* 1011 -> 01000101 */
	0x50u, /* 1100 -> 01010000 */
	0x51u, /* 1101 -> 01010001 */
	0x54u, /* 1110 -> 01010100 */
	0x55u  /* 1111 -> 01010101 */
};

/*
 * Spread low 16 bits of u32X into even positions via 4 nibble LUTs.
 */
static uint32_t
b4523_spread(uint32_t u32X)
{
	uint32_t u32R;

	u32X &= 0x0000ffffu;
	u32R  = (uint32_t)b4523_spread_nibble[u32X & 0x0fu];
	u32R |= (uint32_t)b4523_spread_nibble[(u32X >> 4) & 0x0fu] << 8;
	u32R |= (uint32_t)b4523_spread_nibble[(u32X >> 8) & 0x0fu] << 16;
	u32R |= (uint32_t)b4523_spread_nibble[(u32X >> 12) & 0x0fu] << 24;
	return u32R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_spread_bits_u2 - spread low 16 bits into even positions (LUT).
 *
 * x: source word (only bits 0..15 matter)
 *
 * Returns expanded word with zeros in odd positions. No parent wires.
 */
uint32_t
gj_u32_spread_bits_u2(uint32_t u32X)
{
	(void)NULL;
	return b4523_spread(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_spread_bits_u2(uint32_t u32X)
    __attribute__((alias("gj_u32_spread_bits_u2")));
