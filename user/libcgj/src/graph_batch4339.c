/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4339: swap nibbles of the low 8 bits of a value.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u8_nibbles_u(uint32_t x);
 *     - Nibble-swap the low octet of x:
 *       ((x & 0xf) << 4) | ((x >> 4) & 0xf). High bits above the low
 *       octet are discarded; result is 0..255.
 *   uint32_t __gj_u8_nibbles_u  (alias)
 *   __libcgj_batch4339_marker = "libcgj-batch4339"
 *
 * Exclusive continuum CREATE-ONLY (4331-4340: u32_pack_be_bytes_u,
 * u32_pack_le_bytes_u, u32_byte0_u, u32_byte1_u, u32_byte2_u, u32_byte3_u,
 * u16_pack_be_u, u16_pack_le_u, u8_nibbles_u, batch_id_4340).
 * Unique low-octet nibble-swap surface only; no multi-def. No parent
 * wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4339_marker[] = "libcgj-batch4339";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4339_nibble_swap(uint32_t u32X)
{
	uint32_t u32Lo = u32X & 0xffu;

	return ((u32Lo & 0x0fu) << 4) | ((u32Lo >> 4) & 0x0fu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u8_nibbles_u - swap the high and low nibbles of the low octet of x.
 *
 * x: input value (only bits 7..0 participate)
 *
 * Returns ((x & 0xf) << 4) | ((x >> 4) & 0xf) over the low 8 bits.
 * Self-contained; no parent wires.
 */
uint32_t
gj_u8_nibbles_u(uint32_t u32X)
{
	(void)NULL;
	return b4339_nibble_swap(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u8_nibbles_u(uint32_t u32X)
    __attribute__((alias("gj_u8_nibbles_u")));
