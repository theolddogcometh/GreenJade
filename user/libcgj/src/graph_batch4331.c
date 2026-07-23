/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4331: pack four octets into uint32_t big-endian.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_pack_be_bytes_u(uint32_t b0, uint32_t b1,
 *                                   uint32_t b2, uint32_t b3);
 *     - Form a uint32_t from four byte values in network / big-endian
 *       order: b0 is MSB, b3 is LSB. Each argument is masked to 8 bits.
 *   uint32_t __gj_u32_pack_be_bytes_u  (alias)
 *   __libcgj_batch4331_marker = "libcgj-batch4331"
 *
 * Exclusive continuum CREATE-ONLY (4331-4340: u32_pack_be_bytes_u,
 * u32_pack_le_bytes_u, u32_byte0_u, u32_byte1_u, u32_byte2_u, u32_byte3_u,
 * u16_pack_be_u, u16_pack_le_u, u8_nibbles_u, batch_id_4340).
 * Distinct from gj_pack_u32be (batch513) buffer store — unique pure
 * value-form surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4331_marker[] = "libcgj-batch4331";

/* ---- freestanding helpers ---------------------------------------------- */

static uint32_t
b4331_pack_be(uint32_t u32B0, uint32_t u32B1, uint32_t u32B2, uint32_t u32B3)
{
	return ((u32B0 & 0xffu) << 24) |
	       ((u32B1 & 0xffu) << 16) |
	       ((u32B2 & 0xffu) << 8) |
	       (u32B3 & 0xffu);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_pack_be_bytes_u - pack four octets into a big-endian uint32_t.
 *
 * b0: most-significant octet (bits 31..24)
 * b1: next octet (bits 23..16)
 * b2: next octet (bits 15..8)
 * b3: least-significant octet (bits 7..0)
 *
 * Each argument is reduced modulo 256 before packing. Self-contained;
 * no parent wires.
 */
uint32_t
gj_u32_pack_be_bytes_u(uint32_t u32B0, uint32_t u32B1,
                       uint32_t u32B2, uint32_t u32B3)
{
	(void)NULL;
	return b4331_pack_be(u32B0, u32B1, u32B2, u32B3);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_pack_be_bytes_u(uint32_t u32B0, uint32_t u32B1,
                                  uint32_t u32B2, uint32_t u32B3)
    __attribute__((alias("gj_u32_pack_be_bytes_u")));
