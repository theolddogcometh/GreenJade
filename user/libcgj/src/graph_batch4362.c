/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4362: gj_crc8_buf_u - CRC-8 buffer update.
 *
 * Surface (unique symbols):
 *   uint8_t gj_crc8_buf_u(const void *p, size_t n, uint8_t init);
 *     - Fold n bytes into a CRC-8 starting from init with poly 0x07
 *       (MSB-first, non-reflected). p == NULL → return init.
 *   uint8_t __gj_crc8_buf_u  (alias)
 *   __libcgj_batch4362_marker = "libcgj-batch4362"
 *
 * Exclusive continuum CREATE-ONLY (4361-4370). Companion single-byte
 * update is gj_crc8_update_u (batch4361). Distinct from gj_crc8_atm_u /
 * gj_crc8_poly / gj_crc8_smbus_u — unique gj_crc8_buf_u surface only;
 * no multi-def. No parent wires. No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4362_marker[] = "libcgj-batch4362";

/* CRC-8 polynomial 0x07 (x^8 + x^2 + x + 1), MSB-first. */
#define B4362_POLY 0x07u

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b4362_step(uint8_t u8Crc, uint8_t u8Byte)
{
	unsigned uBit;

	u8Crc ^= u8Byte;
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((u8Crc & 0x80u) != 0u) {
			u8Crc = (uint8_t)((u8Crc << 1) ^ (uint8_t)B4362_POLY);
		} else {
			u8Crc = (uint8_t)(u8Crc << 1);
		}
	}
	return u8Crc;
}

static uint8_t
b4362_buf(const unsigned char *pData, size_t cbN, uint8_t u8Init)
{
	uint8_t u8Crc;
	size_t iByte;

	u8Crc = u8Init;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u8Crc = b4362_step(u8Crc, pData[iByte]);
	}
	return u8Crc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc8_buf_u - fold n bytes into a CRC-8 starting from init (poly 0x07).
 *
 * p:    input bytes (NULL → return init)
 * n:    byte count (0 → return init)
 * init: starting CRC
 *
 * Returns the updated CRC. Self-contained; no parent wires.
 */
uint8_t
gj_crc8_buf_u(const void *pData, size_t cbN, uint8_t u8Init)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL) {
		return u8Init;
	}
	p = (const unsigned char *)pData;
	return b4362_buf(p, cbN, u8Init);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_crc8_buf_u(const void *pData, size_t cbN, uint8_t u8Init)
    __attribute__((alias("gj_crc8_buf_u")));
