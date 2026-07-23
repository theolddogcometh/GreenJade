/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4337: pack two octets into uint16_t big-endian.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u16_pack_be_u(uint32_t hi, uint32_t lo);
 *     - Form a 16-bit value from two byte values in big-endian order:
 *       (hi << 8) | lo, each masked to 8 bits. Result is 0..65535.
 *   uint32_t __gj_u16_pack_be_u  (alias)
 *   __libcgj_batch4337_marker = "libcgj-batch4337"
 *
 * Exclusive continuum CREATE-ONLY (4331-4340: u32_pack_be_bytes_u,
 * u32_pack_le_bytes_u, u32_byte0_u, u32_byte1_u, u32_byte2_u, u32_byte3_u,
 * u16_pack_be_u, u16_pack_le_u, u8_nibbles_u, batch_id_4340).
 * Distinct from gj_pack_u16be buffer store and gj_u32_pack_be_bytes_u —
 * unique pure value-form 16-bit BE surface only; no multi-def.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4337_marker[] = "libcgj-batch4337";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4337_pack_be(uint32_t u32Hi, uint32_t u32Lo)
{
	return ((u32Hi & 0xffu) << 8) | (u32Lo & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u16_pack_be_u - pack two octets into a big-endian 16-bit value.
 *
 * hi: most-significant octet (bits 15..8)
 * lo: least-significant octet (bits 7..0)
 *
 * Returns ((hi & 0xff) << 8) | (lo & 0xff). Self-contained;
 * no parent wires.
 */
uint32_t
gj_u16_pack_be_u(uint32_t u32Hi, uint32_t u32Lo)
{
	(void)NULL;
	return b4337_pack_be(u32Hi, u32Lo);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u16_pack_be_u(uint32_t u32Hi, uint32_t u32Lo)
    __attribute__((alias("gj_u16_pack_be_u")));
