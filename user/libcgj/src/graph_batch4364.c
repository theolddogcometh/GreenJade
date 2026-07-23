/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch4364: gj_crc16_ccitt_buf_u - CRC-16/CCITT buffer.
 *
 * Surface (unique symbols):
 *   uint16_t gj_crc16_ccitt_buf_u(const void *p, size_t n, uint16_t init);
 *     - Fold n bytes into a CRC-16/CCITT starting from init with poly
 *       0x1021 (MSB-first, non-reflected). p == NULL → return init.
 *   uint16_t __gj_crc16_ccitt_buf_u  (alias)
 *   __libcgj_batch4364_marker = "libcgj-batch4364"
 *
 * Exclusive continuum CREATE-ONLY (4361-4370). Companion single-byte
 * update is gj_crc16_ccitt_update_u (batch4363). Distinct from
 * gj_crc16_ccitt / gj_crc16_ccitt_n / gj_crc16_ccitt_u — unique
 * gj_crc16_ccitt_buf_u surface only; no multi-def. No parent wires.
 * No __int128.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch4364_marker[] = "libcgj-batch4364";

/* CRC-16/CCITT polynomial 0x1021, MSB-first non-reflected. */
#define B4364_POLY 0x1021u

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b4364_step(uint16_t u16Crc, uint8_t u8Byte)
{
	unsigned uBit;

	u16Crc ^= (uint16_t)((uint16_t)u8Byte << 8);
	for (uBit = 0u; uBit < 8u; uBit++) {
		if ((u16Crc & 0x8000u) != 0u) {
			u16Crc = (uint16_t)((u16Crc << 1) ^ (uint16_t)B4364_POLY);
		} else {
			u16Crc = (uint16_t)(u16Crc << 1);
		}
	}
	return u16Crc;
}

static uint16_t
b4364_buf(const unsigned char *pData, size_t cbN, uint16_t u16Init)
{
	uint16_t u16Crc;
	size_t iByte;

	u16Crc = u16Init;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u16Crc = b4364_step(u16Crc, pData[iByte]);
	}
	return u16Crc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc16_ccitt_buf_u - fold n bytes into CRC-16/CCITT from init (0x1021).
 *
 * p:    input bytes (NULL → return init)
 * n:    byte count (0 → return init)
 * init: starting CRC
 *
 * Returns the updated CRC. Self-contained; no parent wires.
 */
uint16_t
gj_crc16_ccitt_buf_u(const void *pData, size_t cbN, uint16_t u16Init)
{
	const unsigned char *p;

	(void)NULL;
	if (pData == NULL) {
		return u16Init;
	}
	p = (const unsigned char *)pData;
	return b4364_buf(p, cbN, u16Init);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_crc16_ccitt_buf_u(const void *pData, size_t cbN,
    uint16_t u16Init)
    __attribute__((alias("gj_crc16_ccitt_buf_u")));
