/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4334: extract LE byte 1 of a uint32_t.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_byte1_u(uint32_t x);
 *     - Return the second octet of x under a little-endian byte view
 *       (bits 15..8), as a value in 0..255.
 *   uint32_t __gj_u32_byte1_u  (alias)
 *   __libcgj_batch4334_marker = "libcgj-batch4334"
 *
 * Exclusive continuum CREATE-ONLY (4331-4340: u32_pack_be_bytes_u,
 * u32_pack_le_bytes_u, u32_byte0_u, u32_byte1_u, u32_byte2_u, u32_byte3_u,
 * u16_pack_be_u, u16_pack_le_u, u8_nibbles_u, batch_id_4340).
 * Distinct from gj_u32_byte0_u (batch4333). Unique LE-view byte1 surface
 * only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4334_marker[] = "libcgj-batch4334";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4334_byte1(uint32_t u32X)
{
	return (u32X >> 8) & 0xffu;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_byte1_u - extract little-endian byte 1 of a uint32_t.
 *
 * x: value whose LE-view octet 1 is requested
 *
 * Returns bits 15..8 of x, in the range 0..255. Self-contained;
 * no parent wires.
 */
uint32_t
gj_u32_byte1_u(uint32_t u32X)
{
	(void)NULL;
	return b4334_byte1(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_byte1_u(uint32_t u32X)
    __attribute__((alias("gj_u32_byte1_u")));
