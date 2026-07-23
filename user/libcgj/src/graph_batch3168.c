/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3168: set a single bit in a uint16_t bitset.
 *
 * Surface (unique symbols):
 *   uint16_t gj_bitset16_set_u(uint16_t bits, unsigned b);
 *     - Return bits with bit b set (b in 0..15, LSB = bit 0). If b >= 16,
 *       return bits unchanged (no shift UB). Pure integer OR.
 *   uint16_t __gj_bitset16_set_u  (alias)
 *   __libcgj_batch3168_marker = "libcgj-batch3168"
 *
 * Exclusive continuum CREATE-ONLY (3161-3170). Distinct from
 * gj_bitset8_set_u (batch3165) / gj_bit_set_u32 (batch2722) — unique
 * gj_bitset16_set_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3168_marker[] = "libcgj-batch3168";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b3168_set(uint16_t u16Bits, unsigned uB)
{
	if (uB >= 16u) {
		return u16Bits;
	}
	return (uint16_t)(u16Bits | (uint16_t)(1u << uB));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset16_set_u - set bit b of a 16-bit bitset.
 *
 * bits: value to update
 * b:    bit index; 0 is LSB, 15 is MSB. b >= 16 -> no change.
 *
 * Returns bits | (1u << b) for b in 0..15; otherwise bits.
 * Examples: (0, 0) -> 1; (0, 15) -> 0x8000; (0, 16) -> 0. No parent
 * wires.
 */
uint16_t
gj_bitset16_set_u(uint16_t u16Bits, unsigned uB)
{
	(void)NULL;
	return b3168_set(u16Bits, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_bitset16_set_u(uint16_t u16Bits, unsigned uB)
    __attribute__((alias("gj_bitset16_set_u")));
