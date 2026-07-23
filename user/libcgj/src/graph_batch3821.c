/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3821: pack four u8 values big-endian into u32 (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_u8_pack_be_u(uint8_t b0, uint8_t b1, uint8_t b2, uint8_t b3);
 *     - Form (b0<<24)|(b1<<16)|(b2<<8)|b3.
 *       b0 → bits [31:24] (MSB), b1 → [23:16], b2 → [15:8], b3 → [7:0].
 *   uint32_t __gj_u8_pack_be_u  (alias)
 *   __libcgj_batch3821_marker = "libcgj-batch3821"
 *
 * Exclusive continuum CREATE-ONLY (3821-3830). Distinct from
 * gj_u8_pack4_u (batch3116 LE-lane pack) and gj_u64_pack_bytes_be
 * (batch1387 pointer form) — unique gj_u8_pack_be_u surface only;
 * no multi-def. No parent wires.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3821_marker[] = "libcgj-batch3821";

/* ---- freestanding helpers ---------------------------------------------- */

/* Pack four bytes big-endian: b0 is the most significant octet. */
static uint32_t
b3821_pack_be(uint8_t u8B0, uint8_t u8B1, uint8_t u8B2, uint8_t u8B3)
{
	return ((uint32_t)u8B0 << 24)
	    | ((uint32_t)u8B1 << 16)
	    | ((uint32_t)u8B2 << 8)
	    | (uint32_t)u8B3;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_pack_be_u - pack four uint8 values into one uint32 (big-endian).
 *
 * b0: bits [31:24] (MSB)
 * b1: bits [23:16]
 * b2: bits [15:8]
 * b3: bits [7:0]   (LSB)
 */
uint32_t
gj_u8_pack_be_u(uint8_t u8B0, uint8_t u8B1, uint8_t u8B2, uint8_t u8B3)
{
	(void)NULL;
	return b3821_pack_be(u8B0, u8B1, u8B2, u8B3);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u8_pack_be_u(uint8_t u8B0, uint8_t u8B1, uint8_t u8B2,
    uint8_t u8B3)
    __attribute__((alias("gj_u8_pack_be_u")));
