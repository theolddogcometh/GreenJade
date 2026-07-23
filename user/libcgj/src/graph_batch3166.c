/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3166: clear a single bit in a uint8_t bitset.
 *
 * Surface (unique symbols):
 *   uint8_t gj_bitset8_clr_u(uint8_t bits, unsigned b);
 *     - Return bits with bit b cleared (LSB is bit 0). Bits outside
 *       0..7 leave bits unchanged (no shift UB). Pure integer.
 *   uint8_t __gj_bitset8_clr_u  (alias)
 *   __libcgj_batch3166_marker = "libcgj-batch3166"
 *
 * Exclusive continuum CREATE-ONLY (3161-3170). Distinct from
 * gj_bit_clr_u32 (batch2723) / gj_bit_clear_u64 (batch652) — unique
 * gj_bitset8_clr_u surface only; no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3166_marker[] = "libcgj-batch3166";

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b3166_clr(uint8_t u8Bits, unsigned uB)
{
	if (uB >= 8u) {
		return u8Bits;
	}
	return (uint8_t)(u8Bits & (uint8_t)~(1u << uB));
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitset8_clr_u - clear bit b of an 8-bit bitset.
 *
 * bits: value
 * b:    bit index (0 = LSB). If b >= 8, return bits unchanged.
 *
 * Example: gj_bitset8_clr_u(0xFFu, 0) -> 0xFEu;
 *          gj_bitset8_clr_u(0x80u, 7) -> 0. No parent wires.
 */
uint8_t
gj_bitset8_clr_u(uint8_t u8Bits, unsigned uB)
{
	(void)NULL;
	return b3166_clr(u8Bits, uB);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_bitset8_clr_u(uint8_t u8Bits, unsigned uB)
    __attribute__((alias("gj_bitset8_clr_u")));
