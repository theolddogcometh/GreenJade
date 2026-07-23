/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3169: clear a single bit in a uint16_t bitset.
 *
 * Surface (unique symbols):
 *   uint16_t gj_bitset16_clr_u(uint16_t bits, unsigned b);
 *     - Return bits with bit b cleared (LSB is bit 0). Bits outside
 *       0..15 leave bits unchanged (no shift UB). Pure integer.
 *   uint16_t __gj_bitset16_clr_u  (alias)
 *   __libcgj_batch3169_marker = "libcgj-batch3169"
 *
 * Exclusive continuum CREATE-ONLY (3161-3170). Distinct from
 * gj_bitset8_clr_u (batch3166) / gj_bit_clr_u32 (batch2723) — unique
 * gj_bitset16_clr_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3169_marker[] = "libcgj-batch3169";

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b3169_clr(uint16_t u16Bits, unsigned uB)
{
	if (uB >= 16u) {
		return u16Bits;
	}
	return (uint16_t)(u16Bits & (uint16_t)~(1u << uB));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset16_clr_u - clear bit b of a 16-bit bitset.
 *
 * bits: value
 * b:    bit index (0 = LSB). If b >= 16, return bits unchanged.
 *
 * Example: gj_bitset16_clr_u(0xFFFFu, 0) -> 0xFFFEu;
 *          gj_bitset16_clr_u(0x8000u, 15) -> 0. No parent wires.
 */
uint16_t
gj_bitset16_clr_u(uint16_t u16Bits, unsigned uB)
{
	(void)NULL;
	return b3169_clr(u16Bits, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_bitset16_clr_u(uint16_t u16Bits, unsigned uB)
    __attribute__((alias("gj_bitset16_clr_u")));
