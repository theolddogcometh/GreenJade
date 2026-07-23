/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3122: CRC-8/MAXIM one-shot (_soft rename).
 *
 * Surface (unique symbols):
 *   uint8_t gj_crc8_maxim_u_soft(const void *data, size_t n);
 *     - CRC-8/MAXIM-DOW (1-Wire) one-shot over n bytes:
 *       poly 0x31 (reflected 0x8C), init 0x00, refin=true, refout=true,
 *       xorout 0x00.
 *       Check("123456789") == 0xA1.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0.
 *   uint8_t __gj_crc8_maxim_u_soft  (alias)
 *   __libcgj_batch3122_marker = "libcgj-batch3122"
 *
 * Exclusive checksum/net CREATE-ONLY wave (3121-3130). Renamed from the
 * planned gj_crc8_maxim_u because that symbol is owned by batch2112;
 * _soft suffix keeps the surface unique (no multi-def). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3122_marker[] = "libcgj-batch3122";

/* CRC-8/MAXIM reflected poly (unreflected catalog poly is 0x31). */
#define B3122_POLY 0x8Cu
#define B3122_INIT 0x00u

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b3122_crc8_maxim(const unsigned char *pData, size_t cbN)
{
	uint8_t u8Crc;
	size_t iByte;
	unsigned uBit;

	u8Crc = (uint8_t)B3122_INIT;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u8Crc ^= pData[iByte];
		for (uBit = 0u; uBit < 8u; uBit++) {
			if ((u8Crc & 1u) != 0u) {
				u8Crc = (uint8_t)((u8Crc >> 1) ^ B3122_POLY);
			} else {
				u8Crc = (uint8_t)(u8Crc >> 1);
			}
		}
	}
	return u8Crc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc8_maxim_u_soft - one-shot CRC-8/MAXIM-DOW over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 8-bit CRC. NULL data with n != 0 yields 0 (defensive).
 * No parent wires.
 */
uint8_t
gj_crc8_maxim_u_soft(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return (uint8_t)B3122_INIT;
	}
	return b3122_crc8_maxim(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_crc8_maxim_u_soft(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc8_maxim_u_soft")));
