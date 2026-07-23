/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2805: CRC-16/CCITT-FALSE (exclusive _u surface).
 *
 * Surface (unique symbols only — do not export crc16_ccitt / gj_crc16_ccitt):
 *   uint16_t gj_crc16_ccitt_u(const void *data, size_t n);
 *     - CRC-16/CCITT-FALSE one-shot over n bytes (poly 0x1021, init 0xFFFF,
 *       refin=false, refout=false, xorout=0).
 *       Check("123456789") == 0x29B1.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0xFFFF (init).
 *   uint16_t __gj_crc16_ccitt_u  (alias)
 *   __libcgj_batch2805_marker = "libcgj-batch2805"
 *
 * Distinct from batch41 crc16_ccitt, batch452 gj_crc16_ccitt, batch782
 * gj_crc16_ccitt_n — no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2805_marker[] = "libcgj-batch2805";

/* CRC-16/CCITT-FALSE polynomial (non-reflected, MSB-first). */
#define B2805_POLY 0x1021u
#define B2805_INIT 0xFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b2805_crc16_ccitt(const unsigned char *pData, size_t cbN)
{
	uint16_t u16Crc;
	size_t i;
	unsigned uBit;

	u16Crc = (uint16_t)B2805_INIT;
	for (i = 0u; i < cbN; i++) {
		u16Crc ^= (uint16_t)((uint16_t)pData[i] << 8);
		for (uBit = 0u; uBit < 8u; uBit++) {
			if ((u16Crc & 0x8000u) != 0u) {
				u16Crc = (uint16_t)((u16Crc << 1) ^ B2805_POLY);
			} else {
				u16Crc = (uint16_t)(u16Crc << 1);
			}
		}
	}
	return u16Crc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc16_ccitt_u - one-shot CRC-16/CCITT-FALSE over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 16-bit CRC. NULL data with n != 0 yields 0 (defensive).
 * Empty input returns the init value 0xFFFF.
 */
uint16_t
gj_crc16_ccitt_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return (uint16_t)B2805_INIT;
	}
	return b2805_crc16_ccitt(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_crc16_ccitt_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc16_ccitt_u")));
