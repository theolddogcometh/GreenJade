/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch939: bit-reverse a byte (gj_byte_bitrev).
 *
 * Surface (unique symbols):
 *   uint8_t gj_byte_bitrev(uint8_t x);
 *     — Reverse all 8 bits (bit 0 ↔ bit 7). SWAR parallel reverse.
 *   uint8_t __gj_byte_bitrev  (alias)
 *   __libcgj_batch939_marker = "libcgj-batch939"

 *
 * Unique name gj_byte_bitrev (not gj_byte_mirror, not gj_bitrev8).
 * Does NOT define gj_bitrev8 (batch560), gj_bitrev32 / gj_bitrev64
 * (batch181), or gj_bit_reverse32 (batch418) — avoid multi-def.
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stdint.h>

const char __libcgj_batch939_marker[] = "libcgj-batch939";

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_byte_bitrev — reverse the bit order of an 8-bit byte.
 *
 * SWAR stages: swap adjacent bits, then 2-bit pairs, then nibbles.
 * Example: 0x01 → 0x80; 0x80 → 0x01; 0xf0 → 0x0f; 0x00 → 0x00.
 */
uint8_t
gj_byte_bitrev(uint8_t u8X)
{
	uint8_t u8V;

	u8V = u8X;
	u8V = (uint8_t)(((u8V >> 1) & 0x55u) | ((u8V & 0x55u) << 1));
	u8V = (uint8_t)(((u8V >> 2) & 0x33u) | ((u8V & 0x33u) << 2));
	u8V = (uint8_t)(((u8V >> 4) & 0x0fu) | ((u8V & 0x0fu) << 4));
	return u8V;
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_byte_bitrev(uint8_t u8X)
    __attribute__((alias("gj_byte_bitrev")));
