/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4338: pack two octets into uint16_t little-endian.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u16_pack_le_u(uint32_t lo, uint32_t hi);
 *     - Form a 16-bit value from two byte values in little-endian order:
 *       lo is LSB, hi is MSB — result = (lo & 0xff) | ((hi & 0xff) << 8).
 *       Argument order is (lo, hi) matching LE wire order.
 *   uint32_t __gj_u16_pack_le_u  (alias)
 *   __libcgj_batch4338_marker = "libcgj-batch4338"
 *
 * Exclusive continuum CREATE-ONLY (4331-4340: u32_pack_be_bytes_u,
 * u32_pack_le_bytes_u, u32_byte0_u, u32_byte1_u, u32_byte2_u, u32_byte3_u,
 * u16_pack_be_u, u16_pack_le_u, u8_nibbles_u, batch_id_4340).
 * Distinct from gj_u16_pack_be_u (batch4337) and gj_pack_u16le buffer
 * store — unique pure value-form 16-bit LE surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4338_marker[] = "libcgj-batch4338";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4338_pack_le(uint32_t u32Lo, uint32_t u32Hi)
{
	return (u32Lo & 0xffu) | ((u32Hi & 0xffu) << 8);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_pack_le_u - pack two octets into a little-endian 16-bit value.
 *
 * lo: least-significant octet (bits 7..0) — first in LE wire order
 * hi: most-significant octet (bits 15..8) — second in LE wire order
 *
 * Returns (lo & 0xff) | ((hi & 0xff) << 8). Self-contained;
 * no parent wires.
 */
uint32_t
gj_u16_pack_le_u(uint32_t u32Lo, uint32_t u32Hi)
{
	(void)NULL;
	return b4338_pack_le(u32Lo, u32Hi);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u16_pack_le_u(uint32_t u32Lo, uint32_t u32Hi)
    __attribute__((alias("gj_u16_pack_le_u")));
