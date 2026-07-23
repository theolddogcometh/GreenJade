/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3165: set a single bit in a uint8_t bitset.
 *
 * Surface (unique symbols):
 *   uint8_t gj_bitset8_set_u(uint8_t bits, unsigned b);
 *     - Return bits with bit b set (b in 0..7, LSB = bit 0). If b >= 8,
 *       return bits unchanged (no shift UB). Pure integer OR.
 *   uint8_t __gj_bitset8_set_u  (alias)
 *   __libcgj_batch3165_marker = "libcgj-batch3165"
 *
 * Exclusive continuum CREATE-ONLY (3161-3170). Distinct from
 * gj_bit_set_u32 (batch2722) / gj_bit_set_u64 (batch651) — unique
 * gj_bitset8_set_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3165_marker[] = "libcgj-batch3165";

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b3165_set(uint8_t u8Bits, unsigned uB)
{
	if (uB >= 8u) {
		return u8Bits;
	}
	return (uint8_t)(u8Bits | (uint8_t)(1u << uB));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset8_set_u - set bit b of an 8-bit bitset.
 *
 * bits: value to update
 * b:    bit index; 0 is LSB, 7 is MSB. b >= 8 -> no change.
 *
 * Returns bits | (1u << b) for b in 0..7; otherwise bits.
 * Examples: (0, 0) -> 1; (0, 7) -> 0x80; (0, 8) -> 0. No parent wires.
 */
uint8_t
gj_bitset8_set_u(uint8_t u8Bits, unsigned uB)
{
	(void)NULL;
	return b3165_set(u8Bits, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_bitset8_set_u(uint8_t u8Bits, unsigned uB)
    __attribute__((alias("gj_bitset8_set_u")));
