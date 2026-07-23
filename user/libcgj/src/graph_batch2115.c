/*
 * SPDX-License-Identifier: MIT OR Apache-2.0
 * Copyright (c) 2026 Project GreenJade contributors
 *
 * Desktop glibc graph batch2115: gj_crc16_usb_u - CRC-16/USB one-shot.
 *
 * Surface (unique symbols):
 *   uint16_t gj_crc16_usb_u(const void *data, size_t n);
 *     - CRC-16/USB one-shot over n bytes:
 *       poly 0x8005 (reflected 0xA001), init 0xFFFF, refin=true,
 *       refout=true, xorout 0xFFFF.
 *       Check("123456789") == 0xB4C8.
 *       NULL data with n != 0 -> 0. Empty (n == 0) -> 0x0000.
 *   uint16_t __gj_crc16_usb_u  (alias)
 *   __libcgj_batch2115_marker = "libcgj-batch2115"
 *
 * Post-2100 crc exclusive wave. Unique _u surface; distinct from
 * gj_crc16_modbus_u (xorout 0, batch2113).
 *
 * Clean-room freestanding pure C (integer/pointer only). Compiles with
 * -ffreestanding -msse2 -Wall -Wextra -Werror. No malloc, no errno, no
 * libc. No third-party source copied.
 */

#include <stddef.h>
#include <stdint.h>

const char __libcgj_batch2115_marker[] = "libcgj-batch2115";

/* CRC-16/USB reflected poly; final XOR all-ones. */
#define B2115_POLY   0xA001u
#define B2115_INIT   0xFFFFu
#define B2115_XOROUT 0xFFFFu

/* ---- freestanding helpers ---------------------------------------------- */

static uint16_t
b2115_crc16_usb(const unsigned char *pData, size_t cbN)
{
	uint16_t u16Crc;
	size_t iByte;
	unsigned uBit;

	u16Crc = (uint16_t)B2115_INIT;
	for (iByte = 0u; iByte < cbN; iByte++) {
		u16Crc ^= (uint16_t)pData[iByte];
		for (uBit = 0u; uBit < 8u; uBit++) {
			if ((u16Crc & 1u) != 0u) {
				u16Crc = (uint16_t)((u16Crc >> 1) ^ B2115_POLY);
			} else {
				u16Crc = (uint16_t)(u16Crc >> 1);
			}
		}
	}
	return (uint16_t)(u16Crc ^ (uint16_t)B2115_XOROUT);
}

/* ---- public surface ---------------------------------------------------- */

/*
 * gj_crc16_usb_u - one-shot CRC-16/USB over n bytes.
 *
 * data: input bytes (NULL only legal when n == 0)
 * n:    byte count
 *
 * Returns the 16-bit CRC. NULL data with n != 0 yields 0 (defensive).
 */
uint16_t
gj_crc16_usb_u(const void *pData, size_t cbN)
{
	const unsigned char *p;

	(void)NULL;
	p = (const unsigned char *)pData;
	if (p == NULL && cbN != 0u) {
		return 0u;
	}
	if (cbN == 0u) {
		return (uint16_t)(B2115_INIT ^ B2115_XOROUT);
	}
	return b2115_crc16_usb(p, cbN);
}

/* ---- underscored alias ------------------------------------------------- */

uint16_t __gj_crc16_usb_u(const void *pData, size_t cbN)
    __attribute__((alias("gj_crc16_usb_u")));
