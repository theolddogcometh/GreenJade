/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4526: freestanding u32 nibble unzip-lo via LUT.
 *
 * Surface (unique symbols):
 *   uint32_t gj_u32_unzip_lo_u(uint32_t x);
 *     - Extract even nibbles of x (positions 0,2,4,6) and pack them into
 *       the low 16 bits of the result (nibble i of result = nibble 2i of
 *       x). High 16 bits of the result are zero. Uses a 16-entry
 *       identity nibble LUT. Inverse half of gj_u32_zip_nibbles_u
 *       (recovers a).
 *   uint32_t __gj_u32_unzip_lo_u  (alias)
 *   __libcgj_batch4526_marker = "libcgj-batch4526"
 *
 * Exclusive continuum CREATE-ONLY (4521-4530: table-lookup unique wave).
 * Unique gj_u32_unzip_lo_u surface only; no multi-def. Distinct from
 * gj_u32_compact_bits_u (bit-level) and gj_u32_bit_deinterleave_u.
 * No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4526_marker[] = "libcgj-batch4526";

/* ---- freestanding helpers ---------------------------------------------- */

/* Identity nibble LUT: index n -> n (table-lookup theme). */
static const uint8_t b4526_nibble_id[16] = {
	0x0u, 0x1u, 0x2u, 0x3u, 0x4u, 0x5u, 0x6u, 0x7u,
	0x8u, 0x9u, 0xau, 0xbu, 0xcu, 0xdu, 0xeu, 0xfu
};

/*
 * Pack even nibbles of u32X into low 16 bits.
 */
static uint32_t
b4526_unzip_lo(uint32_t u32X)
{
	uint32_t u32R;
	unsigned uI;
	uint8_t u8N;

	u32R = 0u;
	for (uI = 0u; uI < 4u; uI++) {
		u8N = (uint8_t)((u32X >> (8u * uI)) & 0x0fu);
		u32R |= (uint32_t)b4526_nibble_id[u8N] << (4u * uI);
	}
	return u32R;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_u32_unzip_lo_u - pack even nibbles of x into the low 16 bits.
 *
 * x: nibble-zipped source (e.g. from gj_u32_zip_nibbles_u)
 *
 * Returns the even-nibble stream in bits 0..15. No parent wires.
 */
uint32_t
gj_u32_unzip_lo_u(uint32_t u32X)
{
	(void)NULL;
	return b4526_unzip_lo(u32X);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_u32_unzip_lo_u(uint32_t u32X)
    __attribute__((alias("gj_u32_unzip_lo_u")));
