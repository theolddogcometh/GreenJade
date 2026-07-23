/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4361: gj_crc8_update_u - CRC-8 single-byte update.
 *
 * Surface (unique symbols):
 *   uint8_t gj_crc8_update_u(uint8_t crc, uint8_t byte);
 *     - Fold one byte into a running CRC-8 with poly 0x07 (MSB-first,
 *       non-reflected). Init is caller-chosen (0x00 common for SMBus).
 *   uint8_t __gj_crc8_update_u  (alias)
 *   __libcgj_batch4361_marker = "libcgj-batch4361"
 *
 * Exclusive continuum CREATE-ONLY (4361-4370: crc8_update_u, crc8_buf_u,
 * crc16_ccitt_update_u, crc16_ccitt_buf_u, crc32_update_byte_u, crc32_buf_u,
 * crc32_finalize_u, adler32_update_byte_u, adler32_buf_u, batch_id_4370).
 * Distinct from gj_crc8_atm_u / gj_crc8_poly / gj_crc8_smbus_u — unique
 * gj_crc8_update_u surface only; no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4361_marker[] = "libcgj-batch4361";

/* CRC-8 polynomial 0x07 (x^8 + x^2 + x + 1), MSB-first. */
#define B4361_POLY 0x07u

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b4361_step(uint8_t u8Crc, uint8_t u8Byte)
{
	unsigned uBit;

	u8Crc ^= u8Byte;
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((u8Crc & 0x80u) != 0u) {
			u8Crc = (uint8_t)((u8Crc << 1) ^ (uint8_t)B4361_POLY);
		} else {
			u8Crc = (uint8_t)(u8Crc << 1);
		}
	}
	return u8Crc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc8_update_u - fold one byte into a running CRC-8 (poly 0x07).
 *
 * crc:  running CRC (caller supplies init on first call)
 * byte: input byte
 *
 * Returns the updated CRC. Tableless bit-serial MSB-first. Self-contained;
 * no parent wires.
 */
uint8_t
gj_crc8_update_u(uint8_t u8Crc, uint8_t u8Byte)
{
	(void)NULL;
	return b4361_step(u8Crc, u8Byte);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_crc8_update_u(uint8_t u8Crc, uint8_t u8Byte)
    __attribute__((alias("gj_crc8_update_u")));
