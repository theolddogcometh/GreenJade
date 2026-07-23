/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch560: reverse all 8 bits of a byte.
 *
 * Surface (unique symbols):
 *   uint8_t gj_bitrev8(uint8_t x);
 *     — Bit 0 ↔ bit 7, bit 1 ↔ bit 6, … SWAR parallel reverse;
 *       pure integer, no builtins / lookup tables.
 *   uint8_t __gj_bitrev8  (alias)
 *   __libcgj_batch560_marker = "libcgj-batch560"
 *
 * Does NOT define gj_bitrev32 / gj_bitrev64 (batch181) or
 * gj_bit_reverse32 (batch418) — avoid multi-def.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc, no compiler builtins. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch560_marker[] = "libcgj-batch560";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_bitrev8 — reverse the bit order of an 8-bit byte.
 *
 * x: value whose bits are to be reversed
 *
 * SWAR stages: swap adjacent bits, then 2-bit pairs, then nibbles.
 * Example: 0x01 → 0x80; 0x80 → 0x01; 0xf0 → 0x0f; 0x00 → 0x00.
 */
uint8_t
gj_bitrev8(uint8_t x)
{
	uint8_t u8V;

	u8V = x;
	u8V = (uint8_t)(((u8V >> 1) & 0x55u) | ((u8V & 0x55u) << 1));
	u8V = (uint8_t)(((u8V >> 2) & 0x33u) | ((u8V & 0x33u) << 2));
	u8V = (uint8_t)(((u8V >> 4) & 0x0fu) | ((u8V & 0x0fu) << 4));
	return u8V;
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_bitrev8(uint8_t x)
    __attribute__((alias("gj_bitrev8")));
