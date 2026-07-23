/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch782: CRC-16-CCITT one-shot over n bytes.
 *
 * Surface (unique symbols):
 *   uint16_t gj_crc16_ccitt_n(const void *data, size_t n);
 *     — CRC-16-CCITT over n bytes: poly 0x1021, init 0xFFFF,
 *       refin=false, refout=false, xorout=0.
 *       Check("123456789") == 0x29B1.
 *       NULL data with n != 0 → 0. Empty (n == 0) → 0xFFFF (init).
 *   uint16_t __gj_crc16_ccitt_n  (alias)
 *   __libcgj_batch782_marker = "libcgj-batch782"
 *
 * Distinct from gj_crc16_ccitt (batch452) and crc16_ccitt (batch41) —
 * unique _n name only; no multi-def.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch782_marker[] = "libcgj-batch782";

#define B782_POLY 0x1021u
#define B782_INIT 0xFFFFu

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc16_ccitt_n — one-shot CRC-16/CCITT-FALSE over n bytes.
 *
 * Bit-serial MSB-first; poly 0x1021, init 0xFFFF.
 */
uint16_t
gj_crc16_ccitt_n(const void *pData, size_t cb)
{
	const unsigned char *p;
	uint16_t u16Crc;
	size_t i;
	unsigned uBit;

	if (pData == NULL && cb != 0u) {
		return 0u;
	}

	u16Crc = (uint16_t)B782_INIT;
	if (cb == 0u) {
		return u16Crc;
	}

	p = (const unsigned char *)pData;
	for (i = 0u; i < cb; i++) {
		u16Crc ^= (uint16_t)((uint16_t)p[i] << 8);
		for (uBit = 0u; uBit < 8u; uBit++) {
			if ((u16Crc & 0x8000u) != 0u) {
				u16Crc = (uint16_t)((u16Crc << 1) ^ B782_POLY);
			} else {
				u16Crc = (uint16_t)(u16Crc << 1);
			}
		}
	}
	return u16Crc;
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_crc16_ccitt_n(const void *pData, size_t cb)
    __attribute__((alias("gj_crc16_ccitt_n")));
