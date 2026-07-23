/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3822: pack four u8 values little-endian into u32 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u8_pack_le_u(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);
 *     - Form (b3<<24)|(b2<<16)|(b1<<8)|b0.
 *       b0 → bits [7:0] (LSB), b1 → [15:8], b2 → [23:16], b3 → [31:24].
 *   uint32_t __gj_u8_pack_le_u  (alias)
 *   __libcgj_batch3822_marker = "libcgj-batch3822"
 *
 * Exclusive continuum CREATE-ONLY (3821-3830). Distinct from
 * gj_u8_pack4_u (batch3116 same LE layout, different name) and
 * gj_u8_pack_be_u (batch3821) — unique gj_u8_pack_le_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3822_marker[] = "libcgj-batch3822";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pack four bytes little-endian: b0 is the least significant octet. */
static uint32_t
b3822_pack_le(uint8_t u8B0, uint8_t u8B1, uint8_t u8B2, uint8_t u8B3)
{
	return (uint32_t)u8B0
	    | ((uint32_t)u8B1 << 8)
	    | ((uint32_t)u8B2 << 16)
	    | ((uint32_t)u8B3 << 24);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_pack_le_u - pack four uint8 values into one uint32 (little-endian).
 *
 * b0: bits [7:0]   (LSB)
 * b1: bits [15:8]
 * b2: bits [23:16]
 * b3: bits [31:24] (MSB)
 */
uint32_t
gj_u8_pack_le_u(uint8_t u8B0, uint8_t u8B1, uint8_t u8B2, uint8_t u8B3)
{
	(void)NULL;
	return b3822_pack_le(u8B0, u8B1, u8B2, u8B3);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u8_pack_le_u(uint8_t u8B0, uint8_t u8B1, uint8_t u8B2,
    uint8_t u8B3)
    __attribute__((alias("gj_u8_pack_le_u")));
