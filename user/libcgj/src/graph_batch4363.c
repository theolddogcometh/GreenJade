/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4363: gj_crc16_ccitt_update_u - CRC-16/CCITT byte.
 *
 * Surface (unique symbols):
 *   uint16_t gj_crc16_ccitt_update_u(uint16_t crc, uint8_t byte);
 *     - Fold one byte into a running CRC-16/CCITT with poly 0x1021
 *       (MSB-first, non-reflected). Init is caller-chosen (0xFFFF for
 *       CCITT-FALSE).
 *   uint16_t __gj_crc16_ccitt_update_u  (alias)
 *   __libcgj_batch4363_marker = "libcgj-batch4363"
 *
 * Exclusive continuum CREATE-ONLY (4361-4370). Distinct from
 * gj_crc16_ccitt / gj_crc16_ccitt_n / gj_crc16_ccitt_u — unique
 * gj_crc16_ccitt_update_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4363_marker[] = "libcgj-batch4363";

/* CRC-16/CCITT polynomial 0x1021, MSB-first non-reflected. */
#define B4363_POLY 0x1021u

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b4363_step(uint16_t u16Crc, uint8_t u8Byte)
{
	unsigned uBit;

	u16Crc ^= (uint16_t)((uint16_t)u8Byte << 8);
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((u16Crc & 0x8000u) != 0u) {
			u16Crc = (uint16_t)((u16Crc << 1) ^ (uint16_t)B4363_POLY);
		} else {
			u16Crc = (uint16_t)(u16Crc << 1);
		}
	}
	return u16Crc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc16_ccitt_update_u - fold one byte into CRC-16/CCITT (poly 0x1021).
 *
 * crc:  running CRC (caller supplies init on first call)
 * byte: input byte
 *
 * Returns the updated CRC. Tableless bit-serial MSB-first. Self-contained;
 * no parent wires.
 */
uint16_t
gj_crc16_ccitt_update_u(uint16_t u16Crc, uint8_t u8Byte)
{
	(void)NULL;
	return b4363_step(u16Crc, u8Byte);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_crc16_ccitt_update_u(uint16_t u16Crc, uint8_t u8Byte)
    __attribute__((alias("gj_crc16_ccitt_update_u")));
