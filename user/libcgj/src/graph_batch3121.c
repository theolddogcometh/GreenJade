/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch3121: CRC-8/ATM one-shot (_soft rename).
 *
 * Surface (unique symbols):
 *   uint8_t gj_crc8_atm_u_soft(const void *data, size_t n);
 *     - CRC-8/ATM (ITU-T I.432.1) one-shot over n bytes:
 *       poly 0x07, init 0x00, refin=false, refout=false, xorout 0x55.
 *       Check("123456789") == 0xA1.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0x55.
 *   uint8_t __gj_crc8_atm_u_soft  (alias)
 *   __libcgj_batch3121_marker = "libcgj-batch3121"
 *
 * Exclusive checksum/net CREATE-ONLY wave (3121-3130). Renamed from the
 * planned gj_crc8_atm_u because that symbol is owned by batch2111;
 * _soft suffix keeps the surface unique (no multi-def). No parent wires.
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch3121_marker[] = "libcgj-batch3121";

/* CRC-8/ATM: poly 0x07, xorout 0x55, non-reflected MSB-first. */
#define B3121_POLY   0x07u
#define B3121_INIT   0x00u
#define B3121_XOROUT 0x55u

/* ---- freestanding helpers ---------------------------------------------- */

static uint8_t
b3121_crc8_atm(const unsigned char *pData, size_t cbN)
{
	uint8_t u8Crc;
	size_t iByte;
	unsigned uBit;

	u8Crc = (uint8_t)B3121_INIT;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u8Crc ^= pData[iByte];
		for (uBit = 0u; uBit < 8u; uBit++) {
			if ((u8Crc & 0x80u) != 0u) {
				u8Crc = (uint8_t)((u8Crc << 1) ^ B3121_POLY);
			} else {
				u8Crc = (uint8_t)(u8Crc << 1);
			}
		}
	}
	return (uint8_t)(u8Crc ^ (uint8_t)B3121_XOROUT);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc8_atm_u_soft - one-shot CRC-8/ATM over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 8-bit CRC. NULL data with n != 0 yields 0 (defensive).
 * No parent wires.
 */
uint8_t
gj_crc8_atm_u_soft(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return (uint8_t)(B3121_INIT ^ B3121_XOROUT);
	}
	return b3121_crc8_atm(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint8_t __gj_crc8_atm_u_soft(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc8_atm_u_soft")));
