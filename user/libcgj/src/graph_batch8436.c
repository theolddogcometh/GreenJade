/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch8436: CRC update-byte integer stub (_u).
 *
 * Surface (unique symbols):
 *   uint32_t gj_crc_update_byte_u_8436(uint32_t crc, uint32_t byte);
 *     - One MSB-first CRC-32 byte step over crc with low 8 bits of
 *       byte, poly 0x04C11DB7 (normal form). Soft integer helper.
 *   uint32_t __gj_crc_update_byte_u_8436  (alias)
 *   __libcgj_batch8436_marker = "libcgj-batch8436"
 *
 * Exclusive continuum CREATE-ONLY (8431-8440: crc/poly integer stubs).
 * Unique gj_crc_update_byte_u_8436 surface only; no multi-def. Does
 * not replace full crc32 engines. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch8436_marker[] = "libcgj-batch8436";

/* Unreflected IEEE CRC-32 poly for MSB-first step. */
#define B8436_POLY 0x04C11DB7u

/* ---- freestanding helpers ---------------------------------------------- */

/* One-byte MSB-first CRC-32 update (normal poly). */
static uint32_t
b8436_update_byte(uint32_t u32Crc, uint32_t u32Byte)
{
	uint32_t u32C;
	unsigned uBit;

	u32C = u32Crc ^ ((u32Byte & 0xFFu) << 24);
	for (uBit = 0u; uBit < 8u; uBit++) {
		if (u32C & 0x80000000u) {
			u32C = (u32C << 1) ^ B8436_POLY;
		} else {
			u32C = u32C << 1;
		}
	}
	return u32C;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc_update_byte_u_8436 - one MSB-first CRC-32 byte step.
 *
 * crc: running CRC register
 * byte: data byte (only low 8 bits used)
 *
 * Returns updated CRC. Soft integer helper only. No parent wires.
 */
uint32_t
gj_crc_update_byte_u_8436(uint32_t u32Crc, uint32_t u32Byte)
{
	(void)NULL;
	return b8436_update_byte(u32Crc, u32Byte);
}

/* ---- underscored alias ------------------------------------------------- */

uint32_t __gj_crc_update_byte_u_8436(uint32_t u32Crc, uint32_t u32Byte)
    __attribute__((alias("gj_crc_update_byte_u_8436")));
