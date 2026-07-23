/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch1218: CRC-16/IBM (ARC) one-shot.
 *
 * Surface (unique symbols):
 *   uint16_t gj_crc16_ibm(const void *data, size_t n);
 *     — CRC-16/IBM (aka ARC / IBM-SDLC LRC style reflected):
 *       poly 0x8005 (reflected 0xA001), init 0x0000, refin=true,
 *       refout=true, xorout 0x0000.
 *       Check("123456789") == 0xBB3D.
 *       NULL data with n != 0 → 0. Empty (n == 0) → 0 (init).
 *   uint16_t __gj_crc16_ibm  (alias)
 *   __libcgj_batch1218_marker = "libcgj-batch1218"
 *
 * Distinct from crc16_modbus (init 0xFFFF) and gj_crc16_ccitt* —
 * unique name only; avoid multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch1218_marker[] = "libcgj-batch1218";

/* Reflected CRC-16/IBM polynomial. */
#define B1218_POLY 0xA001u
#define B1218_INIT 0x0000u

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc16_ibm — one-shot CRC-16/IBM over n bytes (LSB-first / reflected).
 */
uint16_t
gj_crc16_ibm(const void *pData, size_t cbN)
{
	const unsigned char *p;
	uint16_t u16Crc;
	size_t i;
	unsigned uBit;

	if (pData == NULL && cbN != 0u) {
		return 0u;
	}

	u16Crc = (uint16_t)B1218_INIT;
	if (cbN == 0u) {
		return u16Crc;
	}

	p = (const unsigned char *)pData;
	for (i = 0u; i < cbN; i++) {
		u16Crc ^= (uint16_t)p[i];
		for (uBit = 0u; uBit < 8u; uBit++) {
			if ((u16Crc & 1u) != 0u) {
				u16Crc = (uint16_t)((u16Crc >> 1) ^ B1218_POLY);
			} else {
				u16Crc = (uint16_t)(u16Crc >> 1);
			}
		}
	}
	return u16Crc;
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_crc16_ibm(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc16_ibm")));
