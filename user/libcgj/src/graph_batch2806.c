/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2806: CRC-16/MODBUS (batch-unique _n surface).
 *
 * Surface (unique symbols — gj_crc16_modbus_u is already batch2113):
 *   uint16_t gj_crc16_modbus_n(const void *data, size_t n);
 *     - CRC-16/MODBUS one-shot over n bytes:
 *       poly 0x8005 (reflected 0xA001), init 0xFFFF, refin=true,
 *       refout=true, xorout 0x0000.
 *       Check("123456789") == 0x4B37.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0xFFFF.
 *   uint16_t __gj_crc16_modbus_n  (alias)
 *   __libcgj_batch2806_marker = "libcgj-batch2806"
 *
 * Post-2800 hash exclusive wave. Batch-unique _n surface because
 * gj_crc16_modbus_u / __gj_crc16_modbus_u already live in batch2113.
 * Does NOT redefine crc16_modbus / __crc16_modbus (batch42).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied. No __int128.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2806_marker[] = "libcgj-batch2806";

/* CRC-16/MODBUS reflected poly. */
#define B2806_POLY 0xA001u
#define B2806_INIT 0xFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b2806_crc16_modbus(const unsigned char *pData, size_t cbN)
{
	uint16_t u16Crc;
	size_t iByte;
	unsigned uBit;

	u16Crc = (uint16_t)B2806_INIT;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u16Crc ^= (uint16_t)pData[iByte];
		for (uBit = 0u; uBit < 8u; uBit++) {
			if ((u16Crc & 1u) != 0u) {
				u16Crc = (uint16_t)((u16Crc >> 1) ^ B2806_POLY);
			} else {
				u16Crc = (uint16_t)(u16Crc >> 1);
			}
		}
	}
	return u16Crc;
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc16_modbus_n - one-shot CRC-16/MODBUS over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 16-bit CRC. NULL data with n != 0 yields 0 (defensive).
 * Empty input returns the init value 0xFFFF.
 */
uint16_t
gj_crc16_modbus_n(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return (uint16_t)B2806_INIT;
	}
	return b2806_crc16_modbus(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_crc16_modbus_n(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc16_modbus_n")));
